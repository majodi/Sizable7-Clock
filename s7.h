#include "colorutils.h"
#include "Arduino.h"
#include <sys/_stdint.h>
#include "FastLED.h"

void putStatistics() {
  preferences.begin("statistics", false);
  preferences.putULong("cUptime", cUptime);
  preferences.putULong("cInteractions", cInteractions);
  preferences.end();
}

void getStatistics() {
  preferences.begin("statistics", false);
  lUptime = preferences.getULong("cUptime", 0); // last saved current is now last
  cUptime = 0;                                  // reset current uptime (we just restarted)
  preferences.putULong("cUptime", cUptime);     // overwrite previous current (will be done once a day at midnight)
  hUptime = preferences.getULong("hUptime", 0); // get last highest uptime
  if (lUptime > hUptime) {
    hUptime = lUptime;
    preferences.putULong("hUptime", hUptime);   // if last exceeds highest, overwrite
  }
  lInteractions = preferences.getULong("cInteractions"); // lased saved interactions
  cInteractions = 0; // start interactions counter
  preferences.putULong("cInteractions", cInteractions);  // overwrite previous current (will be done once a day)
  preferences.end();
}

void getConfig() {
  preferences.begin("config", false);
  digitHue = preferences.getUChar("digitHue", 0);
  baseBrightness = preferences.getInt("brightness", 150);
  brightness = brightnessBaseToAdj( baseBrightness );
  pacifica = preferences.getBool("pacifica", false);
  smoothTransition = preferences.getBool("smoothTransition", true);
  clapDetection = preferences.getBool("clapDetection", false);
  clapHelper = preferences.getBool("clapHelper", false);
  smoothSpeed = preferences.getInt("smoothSpeed", 10);
  preferences.end();
}

void initS7() {
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, S7_MODULES*36);
  FastLED.setMaxPowerInVoltsAndMilliamps(5,750*S7_MODULES);
  FastLED.clear(true);
  s7State = AnimateInit;
}

void setSegment(uint8_t module, uint8_t segment, uint8_t h, uint8_t s, uint8_t v) {
  for (uint8_t led = 0; led < 5; led++) {
    leds[(segmentLed[segment][led])+(module*36)].setHSV(h, s, v);
  }
}

void setDigit(CRGB leds[], uint8_t module, uint8_t digit, uint8_t h, uint8_t s, uint8_t v) {
  if (module > S7_MODULES-1) return;
  fill_solid(leds+(module*36), 36, 0x000000);
  for(uint8_t segment = 0; segment < 7; segment++) {
    uint8_t onSegment = digitSegment[digit][segment];
    if (onSegment > 6) break;
    for(uint8_t led = 0; led < 5; led++) {
      leds[(segmentLed[onSegment][led])+(module*36)].setHSV(h, s, v);
    }
  }
}

void setDigitInv(CRGB leds[], uint8_t module, uint8_t digit, uint8_t h, uint8_t s, uint8_t v) {
  if (module > S7_MODULES-1) return;
  for(uint8_t segment = 0; segment < 7; segment++) {
    uint8_t offSegment = digitSegmentInv[digit][segment];
    if (offSegment > 6) break;
    for(uint8_t led = 0; led < 5; led++) {
      leds[(segmentLed[offSegment][led])+(module*36)] = CRGB::Black;
      ledsOnOff[(segmentLed[offSegment][led])+(module*36)] = 0;
    }
  }
}

void s7_showTime() {
  FastLED.setBrightness(brightness);
  setDigit(leds, S7_MODULES-3, hourDigit1, digitHue, digitSat, digitVal);
  setDigit(leds, S7_MODULES-4, hourDigit0, digitHue, digitSat, digitVal);
  setDigit(leds, S7_MODULES-1, minuteDigit1, digitHue, digitSat, digitVal);
  setDigit(leds, S7_MODULES-2, minuteDigit0, digitHue, digitSat, digitVal);
  FastLED.show();
}

void s7_showTimeSlow() {
  setDigit(ledsNew, S7_MODULES-3, hourDigit1, digitHue, digitSat, digitVal);
  setDigit(ledsNew, S7_MODULES-4, hourDigit0, digitHue, digitSat, digitVal);
  setDigit(ledsNew, S7_MODULES-1, minuteDigit1, digitHue, digitSat, digitVal);
  setDigit(ledsNew, S7_MODULES-2, minuteDigit0, digitHue, digitSat, digitVal);
  for (uint8_t i = 0; i < S7_MODULES*36; i++) {
    leds[i] = ledsNew[i];
    FastLED.show();
    delay(60 - smoothSpeed);
  }
}

void s7_showTimeInv() {
  FastLED.setBrightness(brightness);
  memset(ledsOnOff, 1, sizeof(ledsOnOff));
  for (uint8_t m = 0; m < S7_MODULES; m++) {
    ledsOnOff[35 + (m*36)] = 0;
  }
  setDigitInv(leds, S7_MODULES-3, hourDigit1, digitHue, digitSat, digitVal);
  setDigitInv(leds, S7_MODULES-4, hourDigit0, digitHue, digitSat, digitVal);
  setDigitInv(leds, S7_MODULES-1, minuteDigit1, digitHue, digitSat, digitVal);
  setDigitInv(leds, S7_MODULES-2, minuteDigit0, digitHue, digitSat, digitVal);
  FastLED.show();
}

void showDayInfo() {
  if (!forceDayInfo) {
    for (int i = 0; i < 5; i++) {
      s7_showTime();
      delay(500);
      FastLED.clear(true);
      delay(500);  
    }
  }
  forceDayInfo = false; // reset if that's why we came here

  setDigit(leds, S7_MODULES-4, timeinfo.tm_mday / 10, digitHue, digitSat, digitVal);
  setDigit(leds, S7_MODULES-3, timeinfo.tm_mday % 10, digitHue, digitSat, digitVal);
  setDigit(leds, S7_MODULES-2, (timeinfo.tm_mon + 1) / 10, digitHue, digitSat, digitVal);
  setDigit(leds, S7_MODULES-1, (timeinfo.tm_mon + 1) % 10, digitHue, digitSat, digitVal);
  FastLED.show();
  for (int i = 0; i < 5; i++) {
    fadeToBlackBy(leds, NUM_LEDS, 55);
    FastLED.show();
    delay(1000);
  }

  sunrise = d2d_Berghem.sunrise(timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday, timeinfo.tm_isdst);
  sunriseHour = sunrise / 60;
  sunriseMinute = sunrise % 60;
  for (int i = 0; i < 256; i++) {
    setDigit(leds, S7_MODULES-4, sunriseHour / 10, HUE_ORANGE, digitSat, i);
    setDigit(leds, S7_MODULES-3, sunriseHour % 10, HUE_ORANGE, digitSat, i);
    setDigit(leds, S7_MODULES-2, sunriseMinute / 10, HUE_ORANGE, digitSat, i);
    setDigit(leds, S7_MODULES-1, sunriseMinute % 10, HUE_ORANGE, digitSat, i);
    FastLED.show();
    delay(25);
  }

  sunset = d2d_Berghem.sunset(timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday, timeinfo.tm_isdst);
  sunsetHour = sunset / 60;
  sunsetMinute = sunset % 60;
  for (int i = 255; i > 0; i--) {
    setDigit(leds, S7_MODULES-4, sunsetHour / 10, HUE_BLUE, digitSat, i);
    setDigit(leds, S7_MODULES-3, sunsetHour % 10, HUE_BLUE, digitSat, i);
    setDigit(leds, S7_MODULES-2, sunsetMinute / 10, HUE_BLUE, digitSat, i);
    setDigit(leds, S7_MODULES-1, sunsetMinute % 10, HUE_BLUE, digitSat, i);
    FastLED.show();
    delay(25);
  }

  s7_showTime();
}

void animateInit() {
  static uint8_t dotPos = 0;
  for (uint8_t module = 0; module < S7_MODULES; module++) {
      leds[dotPos+(module*36)].setHue(0);
  }
  for (uint8_t module = 0; module < S7_MODULES; module++) {
    fadeToBlackBy(leds+(module*36), 30, 64);
  }
  fadeToBlackBy(leds, 30, 64);
  FastLED.show();
  dotPos++;
  if (dotPos > 29) {
    dotPos = 0;
  }
}

void say_AP_() {
  static bool isOn = 0;
  isOn = !isOn;
  if (isOn) {
    setSegment(0, 0, 0, 255, 255); // A
    setSegment(0, 1, 0, 255, 255);
    setSegment(0, 2, 0, 255, 255);
    setSegment(0, 4, 0, 255, 255);
    setSegment(0, 5, 0, 255, 255);
    setSegment(0, 6, 0, 255, 255);

    setSegment(1, 0, 0, 255, 255); // P
    setSegment(1, 1, 0, 255, 255);
    setSegment(1, 4, 0, 255, 255);
    setSegment(1, 5, 0, 255, 255);
    setSegment(1, 6, 0, 255, 255);
  } else {
    FastLED.clear();
  }
  FastLED.show();
}

void beatStrokes() {
  FastLED.clear();
  for (int p = 5; p < 15; p++) {
    for (int m = 0; m < 4; m++) {
      leds[ (m*36) + p ].setHue(16 + (m*16));
      leds[ (m*36) + p+15 ].setHue(16 + (m*16));
    }
    FastLED.show();
  }
}

void beatLines(uint8_t pixels) {
  FastLED.clear();
  for (int p = 0; p < pixels; p++) {
    uint8_t m = p / 5;
    leds[ (m*36) + (p-(m*5)) ].setHue(HUE_BLUE);
    leds[ (m*36) + 30 + (p-(m*5)) ].setHue(HUE_PURPLE);
    leds[ (m*36) + 19 - (p-(m*5)) ].setHue(HUE_AQUA);
    FastLED.show();
  }
}

//===== Fire =====
#define COOLING  75
#define SPARKING 100
#define FIRE_LEDS 15

void fire2012()
{
  for( int s7Module = 0; s7Module < S7_MODULES; s7Module++) {
    static uint8_t heat[S7_MODULES][FIRE_LEDS];
    for( int i = 0; i < FIRE_LEDS; i++) {
      heat[s7Module][i] = qsub8( heat[s7Module][i],  random8(0, ((COOLING * 10) / FIRE_LEDS) + 2));
    }
    for( int k= FIRE_LEDS - 1; k >= 2; k--) {
      heat[s7Module][k] = (heat[s7Module][k - 1] + heat[s7Module][k - 2] + heat[s7Module][k - 2] ) / 3;
    }
    if( random8() < SPARKING ) {
      int y = random8(3);
      heat[s7Module][y] = qadd8( heat[s7Module][y], random8(160,255) );
    }
    for( int j = 0; j < FIRE_LEDS; j++) {
      CRGB color = HeatColor( heat[s7Module][j]);
      int pixelnumber;
      pixelnumber = j;
      pixelnumber = pixelnumber < 10 ? pixelnumber + 20 : pixelnumber - 10; // adjust for S7
      leds[(s7Module * 36) + pixelnumber] = color;
      if (pixelnumber > 24 && pixelnumber < 30) {
        leds[5 + (s7Module * 36) + pixelnumber] = color;
      }
      pixelnumber = (FIRE_LEDS-1) - j;
      pixelnumber = pixelnumber + 5; // adjust for S7
      leds[(((S7_MODULES-1) - s7Module) * 36) + pixelnumber] = color;
    }
  }
}

//===== Pacifica with time (see s7_showTimeInv())=====

void pacifica_one_layer( CRGBPalette16& p, uint16_t cistart, uint16_t wavescale, uint8_t bri, uint16_t ioff)
{
  uint16_t ci = cistart;
  uint16_t waveangle = ioff;
  uint16_t wavescale_half = (wavescale / 2) + 20;
  for( uint16_t i = 0; i < NUM_LEDS; i++) {
    waveangle += 250;
    uint16_t s16 = sin16( waveangle ) + 32768;
    uint16_t cs = scale16( s16 , wavescale_half ) + wavescale_half;
    ci += cs;
    uint16_t sindex16 = sin16( ci) + 32768;
    uint8_t sindex8 = scale16( sindex16, 240);
    CRGB c = ColorFromPalette( p, sindex8, bri, LINEARBLEND);
    if (ledsOnOff[i]) {
      leds[i] += c;
    }
  }
}

void pacifica_add_whitecaps()
{
  uint8_t basethreshold = beatsin8( 9, 55, 65);
  uint8_t wave = beat8( 7 );
    for( uint16_t i = 0; i < NUM_LEDS; i++) {
    uint8_t threshold = scale8( sin8( wave), 20) + basethreshold;
    wave += 7;
    uint8_t l = leds[i].getAverageLight();
    if( l > threshold) {
      uint8_t overage = l - threshold;
      uint8_t overage2 = qadd8( overage, overage);
      if (ledsOnOff[i]){
        leds[i] += CRGB( overage, overage2, qadd8( overage2, overage2));
      }
    }
  }
}

void pacifica_deepen_colors()
{
  for( uint16_t i = 0; i < NUM_LEDS; i++) {
    if (ledsOnOff[i]) {
      leds[i].blue = scale8( leds[i].blue,  145); 
      leds[i].green= scale8( leds[i].green, 200); 
      leds[i] |= CRGB( 2, 5, 7);
    }
  }
}

void pacifica_loop()
{
  static uint16_t sCIStart1, sCIStart2, sCIStart3, sCIStart4;
  static uint32_t sLastms = 0;
  uint32_t ms = GET_MILLIS();
  uint32_t deltams = ms - sLastms;
  sLastms = ms;
  uint16_t speedfactor1 = beatsin16(3, 179, 269);
  uint16_t speedfactor2 = beatsin16(4, 179, 269);
  uint32_t deltams1 = (deltams * speedfactor1) / 256;
  uint32_t deltams2 = (deltams * speedfactor2) / 256;
  uint32_t deltams21 = (deltams1 + deltams2) / 2;
  sCIStart1 += (deltams1 * beatsin88(1011,10,13));
  sCIStart2 -= (deltams21 * beatsin88(777,8,11));
  sCIStart3 -= (deltams1 * beatsin88(501,5,7));
  sCIStart4 -= (deltams2 * beatsin88(257,4,6));
  fill_solid( leds, NUM_LEDS, CRGB( 0, 0, 0));
  pacifica_one_layer( pacifica_palette_1, sCIStart1, beatsin16( 3, 11 * 256, 14 * 256), beatsin8( 10, 70, 130), 0-beat16( 301) );
  pacifica_one_layer( pacifica_palette_2, sCIStart2, beatsin16( 4,  6 * 256,  9 * 256), beatsin8( 17, 40,  80), beat16( 401) );
  pacifica_one_layer( pacifica_palette_3, sCIStart3, 6 * 256, beatsin8( 9, 10,38), 0-beat16(503));
  pacifica_one_layer( pacifica_palette_3, sCIStart4, 5 * 256, beatsin8( 8, 10,28), beat16(601));
  pacifica_add_whitecaps();
  pacifica_deepen_colors();
}
