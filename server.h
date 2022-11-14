int brightnessAdjToBase(uint8_t adjusted) {               // base (value for highest level) will be saved in settings. This can be >255 but real brightness will be capped to 255 for each level.
  if (ambientLight < 200) {
    return (int)((adjusted / 20.0) * 100);
  }
  else if (ambientLight >= 200 && ambientLight < 500) {
    return (int)((adjusted / 40.0) * 100);
  }
  else if (ambientLight >= 500 && ambientLight < 1000) {
    return (int)((adjusted / 60.0) * 100);
  }
  else if (ambientLight >= 1000 && ambientLight < 3000) {
    return (int)((adjusted / 80.0) * 100);
  }
  else {
    return (int)adjusted;
  }
}

uint8_t brightnessBaseToAdj(int base) {                 // adjusted will be used and will vary with ambient light
  if (ambientLight < 200) {
    uint8_t adj = 0.2 * base;
    return constrain(adj, 0, 255);
  }
  else if (ambientLight >= 200 && ambientLight < 500) {
    uint8_t adj = 0.4 * base;
    return constrain(adj, 0, 255);
  }
  else if (ambientLight >= 500 && ambientLight < 1000) {
    uint8_t adj = 0.6 * base;
    return constrain(adj, 0, 255);
  }
  else if (ambientLight >= 1000 && ambientLight < 3000) {
    uint8_t adj = 0.8 * base;
    return constrain(adj, 0, 255);
  }
  else {
    return constrain(base, 0, 255);
  }
}

void getCredentials() {
  preferences.begin("credentials", false);
  ssid = preferences.getString("ssid", ""); 
  password = preferences.getString("password", "");
  preferences.end();
}

void setRetryWifi(bool retryFlag) {
  preferences.begin("wifi", false);
  preferences.putBool("retryFlag", retryFlag);
  preferences.end();
}

bool isRetryWifi() {
  preferences.begin("wifi", false);
  return preferences.getBool("retryFlag", false);
  preferences.end();
}

bool initWiFi() {
  if (ssid == "" || password == ""){
    printf("No values saved for ssid or password");
    return false;
  }
  else {
    WiFi.mode(WIFI_STA);
    WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE);
    WiFi.setHostname(hostname.c_str());  
    WiFi.begin(ssid.c_str(), password.c_str());
    printf("Connecting to wifi\n");
    unsigned long currentMillis = millis();
    previousMillis = currentMillis;
    while(WiFi.status() != WL_CONNECTED) {
      currentMillis = millis();
      if (currentMillis - previousMillis >= interval) {
        printf("Failed to connect.\n");
        if (isRetryWifi()) {
          setRetryWifi(false);  // reset wifi retry flag for next time
          return false;         // if this is already a retry then return false to ask for wifi credentials
        } else {
          setRetryWifi(true);   // if first time fail then set retry flag and restart to retry
          ESP.restart();
        }
      }
    }
    printf("wifi connected.\nIP address: %s\n",WiFi.localIP().toString());
    setRetryWifi(false);        // reset wifi retry flag    
    return true;
  }
}

bool getNTPtime() {
  time(&now);
  localtime_r(&now, &timeinfo);
  hour = timeinfo.tm_hour;
  hourDigit0 = hour / 10;
  hourDigit1 = hour % 10; 
  minute = timeinfo.tm_min;
  minuteDigit0 = minute / 10;
  minuteDigit1 = minute % 10;
  if (minute != lastMinute) {
    timeChanged = true;
    lastMinute = minute;
    char message[10];
    snprintf(message, 10, "%d%d:%d%d", hourDigit0, hourDigit1, minuteDigit0, minuteDigit1);
    events.send(String(message).c_str(),"time",millis());
  } else {
    timeChanged = false;
  }
  return true;
}

String processor(const String& var){
  if(var == "TIME"){
    char message[10];
    snprintf(message, 10, "%d%d:%d%d", hourDigit0, hourDigit1, minuteDigit0, minuteDigit1);
    return String(message);
  }
  else if(var == "AMBIENTLIGHT"){
    return String(ambientLight);
  }
  else if(var == "TEMPERATURE"){
    return String(temperature,1);
  }
  else if(var == "HUMIDITY"){
    return String(humidity,1);
  }
  else if(var == "DIGITHUE"){
    return String(digitHue);
  }
  else if(var == "BRIGHTNESS"){
    return String(brightness);
  }
  else if(var == "SMOOTHTRANSITION"){
    return String(smoothTransition == true ? "true" : "false");
  }
  else if(var == "PACIFICA"){
    return String(pacifica == true ? "true" : "false");
  }
  else if(var == "FIRE"){
    return String(s7State == AnimateFire ? "true" : "false");
  }
  else if(var == "CLAPDETECTION"){
    return String(clapDetection == true ? "true" : "false");
  }
  else if(var == "CLAPHELPER"){
    return String(clapHelper == true ? "true" : "false");
  }
  else if(var == "SMOOTHSPEED"){
    return String(smoothSpeed);
  }
  else if(var == "C_UPTIME"){
    return String(cUptime);
  }
  else if(var == "L_UPTIME"){
    return String(lUptime);
  }
  else if(var == "H_UPTIME"){
    return String(hUptime);
  }
  else if(var == "C_INTERACTIONS"){
    return String(cInteractions);
  }
  else if(var == "L_INTERACTIONS"){
    return String(lInteractions);
  }
  return String();
}

void setSTA_routes() {
  server.onNotFound([](AsyncWebServerRequest *request) {
    if (request->method() == HTTP_OPTIONS) {
      request->send(200);
    } else {
      request->send(404);
    }
  });

void s7_showTime(); // forward declarations
void s7_showTimeInv();
void showDayInfo();
void clapTrigger();

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });    
  server.on("/stats", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", stats_html, processor);
  });    
  server.on("/toggle", HTTP_GET, [](AsyncWebServerRequest *request){
    if (s7State == Off) {
      s7State = Time;
    } else {
      s7State = Off;
    }
    request->send(200, "text/plain", "toggle done!");
  });
  server.on("/wifimanager", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", wifimanager_html);
  });
  server.on("/set", HTTP_POST, [](AsyncWebServerRequest *request) {
    preferences.begin("config", false);
    int params = request->params();
    for(int i=0;i<params;i++){
      AsyncWebParameter* p = request->getParam(i);
      if(p->isPost()){
        cInteractions++;
        if (p->name() == "digitHueVal") {
          String digitHueVal = p->value();
          digitHue = digitHueVal.toInt();
          preferences.putUChar("digitHue", digitHue);
        }
        if (p->name() == "brightnessVal") {
          String brightnessVal = p->value();
          brightness = brightnessVal.toInt();
          baseBrightness = brightnessAdjToBase( brightness );
          preferences.putInt("brightness", baseBrightness);
        }
        if (p->name() == "pacificaVal") {
          String pacificaVal = p->value();
          pacifica = pacificaVal == "true";
          if (pacifica) {  // don't wait for time change
            s7_showTimeInv();
          } else {
            s7_showTime();          
          }
          preferences.putBool("pacifica", pacifica);
        }
        if (p->name() == "smoothTransitionVal") {
          String smoothTransitionVal = p->value();
          smoothTransition = smoothTransitionVal == "true";
          preferences.putBool("smoothTransition", smoothTransition);          
        }
        if (p->name() == "fireVal") {
          String fireVal = p->value();
          s7State = fireVal == "true" ? AnimateFire : Time;
          if (s7State == Time) {
            s7_showTime(); // don't wait for time change and show full before slow (smooth)
          }
        }
        if (p->name() == "clapDetectionVal") {
          String clapDetectionVal = p->value();
          clapDetection = clapDetectionVal == "true";
          preferences.putBool("clapDetection", clapDetection);          
        }
        if (p->name() == "clapHelperVal") {
          String clapHelperVal = p->value();
          clapHelper = clapHelperVal == "true";
          preferences.putBool("clapHelper", clapHelper);          
        }
        if (p->name() == "smoothSpeedVal") {
          String smoothSpeedVal = p->value();
          smoothSpeed = smoothSpeedVal.toInt();
          preferences.putInt("smoothSpeed", smoothSpeed);
        }
        if (p->name() == "actionVal") {
          String actionVal = p->value();
          action = actionVal.toInt();
          if (action == 1) {
            forceDayInfo = true;
          }
          else if (action == 2) {
            s7State = Temp;
          }
          else if (action == 3) {
            s7State = AnimateBeats;
          }
          else if (action == 4) {
            s7State = Time;
            s7_showTime(); // don't wait for time change
          }
        }
        printf("POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
      }
    }
    request->send(200, "text/plain", "okay");
    preferences.end();
  });
  server.on("/", HTTP_POST, [](AsyncWebServerRequest *request) {
    preferences.begin("credentials", false);
    int params = request->params();
    for(int i=0;i<params;i++){
      AsyncWebParameter* p = request->getParam(i);
      if(p->isPost()){
        if (p->name() == "ssid") {
          ssid = p->value();
          printf("SSID set to: %s\n", ssid.c_str());
          preferences.putString("ssid", ssid);
        }
        if (p->name() == "pass") {
          password = p->value();
          printf("Password set to: %s\n", password.c_str());
          preferences.putString("password", password);
        }
        printf("POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
      }
    }
    preferences.end();
    request->send(200, "text/plain", "Config of network settings done. Sizable7 Clock will restart");
    delay(3000);
    ESP.restart();
  });

  events.onConnect([](AsyncEventSourceClient *client){
    if(client->lastId()){
      printf("Client reconnected! Last message ID that it got is: %u\n", client->lastId());
    }
    client->send("hello!", NULL, millis(), 10000);
  });
  server.addHandler(&events);

}

void setAP_routes() {
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", wifimanager_html);
  });
  server.on("/", HTTP_POST, [](AsyncWebServerRequest *request) {
    int params = request->params();
    for(int i=0;i<params;i++){
      AsyncWebParameter* p = request->getParam(i);
      if(p->isPost()){
        if (p->name() == "ssid") {
          ssid = p->value();
          printf("SSID set to: %s\n", ssid.c_str());
          preferences.putString("ssid", ssid);
        }
        if (p->name() == "pass") {
          password = p->value();
          printf("Password set to: %s\n", password.c_str());
          preferences.putString("password", password);
        }
        printf("POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
      }
    }
    request->send(200, "text/plain", "Config of network settings done. Sizable7 Clock will restart");
    delay(3000);
    ESP.restart();
  });
}