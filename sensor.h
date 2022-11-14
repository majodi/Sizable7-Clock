#include <sys/_stdint.h>
#include "FastLED.h"

void initSensors() {
  pinMode(LIGHTSENSORPIN,  INPUT);
  DHT.begin();                                                    // ESP32 default pins 21 (SDA) 22 (SCL)
  Wire.setClock(400000);
  pinMode(soundPin, INPUT);
}

void getAmbientLight() {
    ambientLight = analogRead(LIGHTSENSORPIN);                    //Read light level
    if (ambientLight == 0 && lastAmbientLight == 0) {
      return;
    }
    lastAmbientLight = lastAmbientLight == 0 ? 1 : lastAmbientLight; // avoid divide by zero 
    if ((int)(((float)abs(ambientLight-lastAmbientLight)/lastAmbientLight)*100) > 25) {
      lastAmbientLight = ambientLight;
      brightness = brightnessBaseToAdj( baseBrightness );
      events.send(String(ambientLight).c_str(),"ambientLight",millis());
      events.send(String(brightness).c_str(),"brightness",millis());
    }
}

void getTempHum() {
  int status = DHT.read();
  temperature = DHT.getTemperature() - 2.35;                          // correction -2.35
  events.send(String(temperature,1).c_str(),"temperature",millis());
  humidity = DHT.getHumidity() + 6;                                   // correction + 6
  events.send(String(humidity,1).c_str(),"humidity",millis());
}

void clapTrigger() {
  FastLED.clear(true);
  int tempInt = 10 * temperature;
  uint8_t dig1 = tempInt / 100;
  uint8_t dig2 = (tempInt - (dig1 * 100)) / 10;
  uint8_t dig3 = (tempInt - (dig1 * 100)) % 10;
  uint8_t contrastColor = 0; //((digitHue/32)*32) + 32;
  for (int i = 0; i < 5; i++) {
    setDigit(leds, S7_MODULES-4, dig1, contrastColor, digitSat, digitVal);
    setDigit(leds, S7_MODULES-3, dig2, contrastColor, digitSat, digitVal);
    setDigit(leds, S7_MODULES-2, dig3, contrastColor, digitSat, digitVal);
    leds[71].setHSV(contrastColor, digitSat, digitVal);  // dot
    setSegment(3, 0, contrastColor, digitSat, digitVal); // degree symbol
    setSegment(3, 1, contrastColor, digitSat, digitVal); // degree symbol
    setSegment(3, 5, contrastColor, digitSat, digitVal); // degree symbol
    setSegment(3, 6, contrastColor, digitSat, digitVal); // degree symbol
    FastLED.show();
    delay(1000);
    FastLED.clear(true);
    delay(250);
  }
  int humInt = 10 * humidity;
  dig1 = humInt / 100;
  dig2 = (humInt - (dig1 * 100)) / 10;
  dig3 = (humInt - (dig1 * 100)) % 10;
  contrastColor = 160; //+= 32;
  for (int i = 0; i < 5; i++) {
    setDigit(leds, S7_MODULES-4, dig1, contrastColor, digitSat, digitVal);
    setDigit(leds, S7_MODULES-3, dig2, contrastColor, digitSat, digitVal);
    setDigit(leds, S7_MODULES-2, dig3, contrastColor, digitSat, digitVal);
    leds[71].setHSV(contrastColor, digitSat, digitVal);  // dot
    setSegment(3, 0, contrastColor, digitSat, digitVal); // P symbol
    setSegment(3, 1, contrastColor, digitSat, digitVal); // P symbol
    setSegment(3, 4, contrastColor, digitSat, digitVal); // P symbol
    setSegment(3, 5, contrastColor, digitSat, digitVal); // P symbol
    setSegment(3, 6, contrastColor, digitSat, digitVal); // P symbol
    FastLED.show();
    delay(1000);
    FastLED.clear(true);
    delay(250);
  }
  s7_showTime();
  s7State = Time;
}

void setFsmState(uint8_t state) {
  fsmState = state;
  if (clapDetection && clapHelper) {
    if (state == 0) {
      leds[35] = 0;
      leds[71] = 0;
      leds[107] = 0;
    }
    else if (state == 1) {
      leds[35].setHue(0);
    }
    else if (state == 2) {
      leds[35].setHue(0);
      leds[71].setHue(0);
    }
    else if (state == 3) {
      leds[35].setHue(0);
      leds[71].setHue(0);
      leds[107].setHue(0);
    }
    FastLED.show();
  }
}
