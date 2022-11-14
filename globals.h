#include <sys/_types.h>
#include <sys/_stdint.h>
//general
unsigned long cUptime; // %C_UPTIME% cUptime is minutes, written once a day. On new startup this is saved to last (so current and last can be up to 24 hours less when a crash/reset happens)
unsigned long lUptime; // %L_UPTIME%
unsigned long hUptime; // %H_UPTIME% 
unsigned long cInteractions; // %C_INTERACTIONS%
unsigned long lInteractions; // %L_INTERACTIONS%

//wifi
String ssid;
String password;
const String hostname = "Sizable7 Clock";
unsigned long previousMillis = 0;
const long interval = 10000;                                              // interval to wait for Wi-Fi connection (milliseconds)

//NTP
const char* NTP_SERVER = "pool.ntp.org";
const char* TZ_INFO    = "CET-1CEST-2,M3.5.0/02:00:00,M10.5.0/03:00:00";  // enter your time zone (https://remotemonitoringsystems.ca/time-zone-abbreviations.php)
tm timeinfo;
time_t now;
int hour;
int minute;
int lastMinute;
uint8_t hourDigit0;
uint8_t hourDigit1;
uint8_t minuteDigit0;
uint8_t minuteDigit1;
bool timeChanged = false;
uint8_t smoothSpeed = 50;

Dusk2Dawn d2d_Berghem(51.7621, 5.5815, 1);
int sunrise, sunset;
int sunriseHour = 0;
int sunriseMinute = 0;
int sunsetHour = 0;
int sunsetMinute = 0;

//webserver
class CaptiveRequestHandler : public AsyncWebHandler {
public:
  CaptiveRequestHandler() {}
  virtual ~CaptiveRequestHandler() {}

  bool canHandle(AsyncWebServerRequest *request){
    return true;
  }

  void handleRequest(AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", wifimanager_html); 
  }
};

AsyncWebServer server(80);
AsyncEventSource events("/events");
Preferences preferences;
DNSServer dnsServer;

//sound
#define soundPin 4

#define leadInTime 1000000                                        // required silence time before a valid clap sequence
#define leadOutTime 50000                                         // required silence before starting a measurement of the previous clap
#define clapMinTime 55000                                         // min burst time to qualify as clap
#define clapMaxTime 275000                                        // max burst time to qualify as clap
#define pauseMinTime 70000                                        // min time between two claps
#define pauseMaxTime 340000                                       // max time between two claps
uint64_t volatile newIntTime;                                     // current interrupt time (passed by ISR to frontend)
uint64_t currentIntTime;                                          // copy of current interrupt time for frontend to work with
uint64_t lastIntTime = 0;                                         // saved time of last interrupt
uint64_t sTimerStart;
uint64_t currSilence;
uint64_t lastSilence;
uint64_t clapStart;

uint8_t fsmState = 0;
bool clapDetection = false;
bool clapHelper = false;
portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;

//sensor
#define LIGHTSENSORPIN 35
int ambientLight;
int lastAmbientLight;

DHT20 DHT;
float temperature;
float humidity;

//led
#define DATA_PIN 16                                               // GPIO
#define S7_MODULES 4                                              // number of Sizable7 modules in chain
#define NUM_LEDS S7_MODULES*36

uint8_t brightness = 150;
int baseBrightness = 255;                                         // can be >255 but calculated brightness for level will be capped
uint8_t digitHue = HUE_RED;
uint8_t digitSat = 255;
uint8_t digitVal = 255;
bool pacifica = false;
bool smoothTransition = true;
enum s7States{Off, Time, Temp, AnimateInit, AnimateFire, AnimateBeats, AP} s7State = Off;
uint8_t action;
bool forceDayInfo = false;

const uint8_t segmentLed[7][5] = {
  {0,1,2,3,4},
  {5,6,7,8,9},
  {10,11,12,13,14},
  {15,16,17,18,19},
  {20,21,22,23,24},
  {25,26,27,28,29},
  {30,31,32,33,34}
};
const uint8_t digitSegment[10][7] = {
  {0,1,2,3,4,5,99}, //0
  {1,2,99},         //1
  {0,1,6,4,3,99},   //2
  {0,1,6,2,3,99},   //3
  {5,6,1,2,99},     //4
  {0,5,6,2,3,99},   //5
  {0,5,6,4,2,3,99}, //6
  {0,1,2,99},       //7
  {0,1,2,3,4,5,6},  //8
  {0,1,5,6,2,3,99}  //9
};
const uint8_t digitSegmentInv[10][7] = {
  {6,99},           //0
  {0,3,4,5,6,99},   //1
  {2,5,99},         //2
  {4,5,99},         //3
  {0,3,4,99},       //4
  {1,4,99},         //5
  {1,99},           //6
  {3,4,5,6,99},     //7
  {99},             //8
  {4,99}            //9
};
CRGBPalette16 pacifica_palette_1 = 
    { 0x000507, 0x000409, 0x00030B, 0x00030D, 0x000210, 0x000212, 0x000114, 0x000117, 
      0x000019, 0x00001C, 0x000026, 0x000031, 0x00003B, 0x000046, 0x14554B, 0x28AA50 };
CRGBPalette16 pacifica_palette_2 = 
    { 0x000507, 0x000409, 0x00030B, 0x00030D, 0x000210, 0x000212, 0x000114, 0x000117, 
      0x000019, 0x00001C, 0x000026, 0x000031, 0x00003B, 0x000046, 0x0C5F52, 0x19BE5F };
CRGBPalette16 pacifica_palette_3 = 
    { 0x000208, 0x00030E, 0x000514, 0x00061A, 0x000820, 0x000927, 0x000B2D, 0x000C33, 
      0x000E39, 0x001040, 0x001450, 0x001860, 0x001C70, 0x002080, 0x1040BF, 0x2060FF };

CRGB leds[S7_MODULES*36];
CRGB ledsNew[S7_MODULES*36];
uint8_t ledsOnOff[S7_MODULES*36];
