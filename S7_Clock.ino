/*

S7 Clock 221016

*/

#include <ESPmDNS.h>
#include <Preferences.h>
#include <WiFi.h>
#include <DNSServer.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>
#include <FastLED.h>
#include "DHT20.h"
#include <Dusk2Dawn.h>

#include "wm_html.h"
#include "index_html.h"
#include "stats_html.h"
#include "globals.h"
#include "server.h"
#include "s7.h"
#include "sensor.h"

void IRAM_ATTR soundISR() {
  portENTER_CRITICAL_ISR(&mux);
    newIntTime = esp_timer_get_time();
  portEXIT_CRITICAL_ISR(&mux);
}

void setup() {
  getStatistics();
  Serial.begin(115200);
  printf("\n\nInitializing\n");
  initS7();
  xTaskCreatePinnedToCore(fastLedTask, "fastLedTask", 2048, NULL, 1, NULL, 0);
  initSensors();
  attachInterrupt(digitalPinToInterrupt(soundPin), soundISR, CHANGE);
  xTaskCreatePinnedToCore(soundStateMachineTask, "soundStateMachineTask", 2048, NULL, 2, NULL, 1);
  getCredentials();
  if (initWiFi()) {
    //Handle the Web Server in Station Mode
    setSTA_routes();
    AsyncElegantOTA.begin(&server);
    server.begin();
    configTime(0, 0, NTP_SERVER);
    setenv("TZ", TZ_INFO, 1);
    getNTPtime();
    if(!MDNS.begin("s7")) {
      printf("Error starting mDNS\n");
    } else {
      if (!MDNS.addService("http", "tcp", 80)) {
        printf("Adding mDNS service failed\n");
      }
    }
    getConfig();
    s7State = Time;
    getTempHum();
    s7_showTime();
  } else {
    // else initialize the ESP32 in Access Point mode
    s7State = AP;
    printf("Setting AP (Access Point)\n");
    WiFi.mode(WIFI_AP);
    WiFi.softAP(hostname.c_str(), NULL);
    IPAddress IP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(IP); 
    setAP_routes();
    dnsServer.start(53, "*", WiFi.softAPIP());
    server.addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER); //only when requested from AP
    server.begin();
  }

  printf("\nRunning...\n");
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    // other things
    vTaskDelay(1);
  } else {
    dnsServer.processNextRequest();
  }
}

void fastLedTask(void * parameter) {
  while(true) {
    switch (s7State)
    {
      case Time:
        EVERY_N_SECONDS(1){
          getAmbientLight();
          getNTPtime();
          if (forceDayInfo) {
            showDayInfo();            
          }
          if (timeChanged) {
            if ((hour == 0) && (minute == 0)) {
              putStatistics();
              showDayInfo();
            }
            if (!forceDayInfo) {
              if (!pacifica) {
                if (smoothTransition) {
                  s7_showTimeSlow();
                } else {
                  s7_showTime();
                }
              } else {
                s7_showTimeInv();
              }
            }
          }
        }

        EVERY_N_MINUTES(1){
          cUptime++;
          getTempHum();
        }

        if (pacifica) {
          EVERY_N_MILLISECONDS(20) {
            pacifica_loop();
            FastLED.show();
          }
        }
        break;
      case Temp:
        clapTrigger();
        break;
      case AnimateInit:
        EVERY_N_MILLISECONDS(50) {
          animateInit();
        }
        break;
      case AnimateFire:
        EVERY_N_MILLISECONDS(65) {
          fire2012();
          FastLED.show();
        }
        break;
      case AnimateBeats:
        EVERY_N_MILLISECONDS(1) {
          if (currSilence < 1000) {
            beatStrokes();
          }
        }
        EVERY_N_MILLISECONDS(5) {
          if (currSilence <= 100000) {
            beatLines(20 - (currSilence / 5000));
          } else {
            FastLED.clear(true);
          }
        }
        break;
      case AP:
        EVERY_N_MILLISECONDS(500) {
          say_AP_();
        }
        break;
      default:
        FastLED.clear();
        FastLED.show();
        break;
    }
    vTaskDelay(1);
  }
}

void soundStateMachineTask(void * parameter) {
  while(true) {

    portENTER_CRITICAL(&mux);                                                   // get safe copy of current interrupt time
      currentIntTime = newIntTime;
    portEXIT_CRITICAL(&mux);

    if (currentIntTime > lastIntTime) {                                         // on new interrupt (time current int is higher than last int)
      lastIntTime = currentIntTime;                                             // save current interrupt time to last interrupt time    
      lastSilence = currSilence;                                                // save current silence duration to last silence period duration
      currSilence = 0;                                                          // reset current silence duration to zero
      sTimerStart = currentIntTime;                                             // new silence starts right after the last trigger moment
    }
    else {
      currSilence = esp_timer_get_time() - sTimerStart;                         // no new interrupt --> update current silence duration
    }

    if (!clapDetection || (s7State == AnimateBeats)) {
      vTaskDelay(1);
      continue;
    }

    switch (fsmState) {
      case 0: // look for start of burst (leadInTime long enough)
        if ((currSilence == 0) && (lastSilence > leadInTime)) {                 // if interrupt just happened (silence == 0) and it was silent before long enough
          clapStart = currentIntTime;                                           // set clap start time to current interrupt time
          setFsmState(1);                                                       // move to state 1 (clap start assumed)
        }
        break;
      case 1: // look for silence (leadOutTime long enough) and check if burst of first clap was in range
        if (currSilence > leadOutTime) {                                           // when current silence long enough to take a measurement
          uint64_t clapDuration = esp_timer_get_time() - leadOutTime - clapStart;  // calculate clap time
          if ((clapDuration > clapMinTime) && (clapDuration < clapMaxTime)) {      // if in range to qualify
            setFsmState(2);                                                        // move to state 2 (first clap okay, waiting for next)
          } else {
            setFsmState(0);                                                        // first clap failed so back to state 0
          }
        }
        break;
      case 2: // look for next clap detection and see if pause was in range
        if (currSilence == 0) {                                                 // did we just have an interrupt?
          if (lastSilence > pauseMinTime) {                                     // was the previous pause long enogh?
            clapStart = currentIntTime;                                         // set clap start to this (current) interrupt time
            setFsmState(3);                                                     // advance state
          } else {
            setFsmState(0);                                                     // else it was a pause fail, start over
          }
        } else {                                                                // no interrupt
          if (currSilence > pauseMaxTime) {                                     // pause too long?
            setFsmState(0);                                                     // pause fail, start over
          }
        }
        break;
      case 3: // look for silence (leadOutTime long enough) and check if burst of second clap was in range
        if (currSilence > leadOutTime) {                                        // when current silence long enough to take a measurement
          uint64_t clapDuration = esp_timer_get_time() - leadOutTime - clapStart;      // calculate clap time
          if ((clapDuration > clapMinTime) && (clapDuration < clapMaxTime)) {   // if in range to qualify
            s7State = Temp;                                                     // trigger!
          }
          setFsmState(0);                                                       // start over
        }
        break;
      default:
        break;
    }
    vTaskDelay(1);
  }
}

/*

     00 01 02 03 04 05 06

13      00 01 02 03 04
14   29                05
15   28                06
16   27                07
17   26                08
18   25                09
19      30 31 32 33 34
20   24                10
21   23                11
22   22                12
23   21                13
24   20                14
25      19 18 17 16 15

*/