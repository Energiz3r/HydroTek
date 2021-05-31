/*
 * 
 * HydroTek hydroponics controller - tim eastwood
 * 
 * See https://github.com/Energiz3r/HydroTek for information and instructions
 *
 * A lot of this code could be external or at least classes but really only the button showed a need to be classed at this time
 * Device may be released as 'single plant' further negating the need to worry about it
 * 
 * Target: ESP32 Dev Module
 * 
 * Pin assignment notes: 
 * 
 *   This sketch to suit ESP32 "D1" WeMos D1 Mini compatible
 * 
 *  4,13,18,19,23,25,26,27,32,33 - OK **19 grounded accidentally
 *  2 - OK, connected to onboard LED
 *  5,14,15 - OK, PWM at boot
 *  34,35,36,39 - input only, no pull-up/down
 *  12 - output OK, boot fail if pulled HIGH
 *  21, 22 - SDA, SCL
 *  16,17 - OK, but RX, TX UART 2
 *  
 *  
 *  
 */

#define LOOP_SPEED 50 // frequency with which to update sensors and so on (every x ms). default 50. do not change
#define HEATER_SPEED_RATIO 4 // how often the PID loop will cycle the grow lamp output when it is used as a heater.
                             // 1 = 12 seconds, the shortest time. (minimum setting)
                             // 2 = 24 seconds (12 x 2)
                             // 3 = 36 seconds... and so on
                             // increase this number to slow down the frequency at which your heater/lamp will cycle to
                             // prevent rapid cooling / heating of the element

//#define PREFERENCES_CLEAR //uncomment to erase settings from flash
//#define SET_RTC_MANUAL // uncomment to force setting the RTC time to the sketch compile time
//#define offlineMode //deprecated... there is now a WIFI enable/disable setting in the menu
//#define buttonDisable

#include <pitches.h>
#include <Tone32.h>

// PIN ASSIGNMENTS
#define floatSw1Pin 23 // OK
#define floatSw2Pin 5 // OK
#define dhtPin 16 // OK (dht2: 17)
#define flowPin 25 // OK (flow2: 32)
#define lampPin 18 // OK - untested (lamp2: 18) 19 originally - bridged to ground on current PCB
#define pumpPin 26 // OK - untested (pump2: 36)

#define ledPin 33 // not working (pin is not an output)
#define ledBrightness 20 // 0 - 255
#define buzzPin 14 // not tested
#define buttonPinOK 32 //
#define buttonPinBK 27 //
#define buttonPinUP 13 // TCK
#define buttonPinDN 4 // IO4
#define buttonDurationLong 

#define aux1 19
#define aux2 2
#define aux3 15 //(TDO)
#define aux4 12 //(TDI)
// END PIN ASSIGNMENTS

String devicePassword = "";
String deviceID = "1";
char* wifiSSID = "";
char* wifiPassphrase =  "";

#define webEndpoint "https://noobs.wtf/plants/device/upload.php" //the php script that the ESP8266 will connect to for uploading data
#define lampHeaterTempVariance 1.5 // how much to allow the temp to vary from the desired setting before switching the state of the heater (lamp)
#define buttonShortTime 30 //ms
#define buttonLongTime 250 //ms
//#define flowPulsesPerMl 0.45 //flow sensor pulses per mL of liquid (YF-S201 sensor = 0.45) //now a config item configurable on the device
#define buzzerChannel 1
#define screenSleepTime 1 // how long to keep screen on for before sleeping
#define buttonDebounceTime 10
#define numMenuOptions 33

// ESP WiFi Includes
#include <WiFi.h>
#include <HTTPClient.h>

// OLED display includes
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1); // -1 is the reset pin, or 4 // Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)

// DHT sensor includes
#include "DHT.h"
DHT dht(dhtPin, DHT22); //pin, type

// RTC includes
#include <RTClib.h>
RTC_DS1307 RTC;

// Preferences (EEPROM) includes
#include <Preferences.h>
Preferences preferences;

// JSON includes
#include <ArduinoJson.h>

class MenuOption {
  public:
    void Init(int pos, String title, String type, float maxVal, float minVal); //bool, int, or float
    void adjustVal(bool increment);
    void setBoolVal(bool value);
    void setIntVal(int value);
    void setFloatVal(float value);
    bool boolVal();
    int intVal();
    float floatVal();
    String title();
    int pos();
  private:
    int _pos;
    String _title;
    String _type;
    bool _boolVal;
    int _intVal;
    float _floatVal;
    float _maxVal;
    float _minVal;
};


void MenuOption::Init(int pos, String title, String type, float maxVal, float minVal) {
  _pos = pos;
  _title = title;
  _type = type;
  _maxVal = maxVal;
  _minVal = minVal;
}

void MenuOption::adjustVal(bool increment) {
  if (_type == "bool") {
    _boolVal = !_boolVal;
  }
  else if (_type == "int") {
    if (increment) {
      _intVal++;
      if (_intVal > int(_maxVal)) {
        _intVal = int(_minVal);
      }
    } else {
      _intVal--;
      if (_intVal < int(_minVal)) {
        _intVal = int(_maxVal);
      }
    }
  }
  else if (_type == "float") {
    if (increment) {
      _floatVal = _floatVal + 0.5;
      if (_floatVal > _maxVal) {
        _floatVal = _minVal;
      }
    } else {
      _floatVal = _floatVal - 0.5;
      if (_floatVal < _minVal) {
        _floatVal = _maxVal;
      }
    }
  }
}

void MenuOption::setBoolVal(bool value) {
  _boolVal = value;
}
void MenuOption::setIntVal(int value) {
  _intVal = value;
}
void MenuOption::setFloatVal(float value) {
  _floatVal = value;
}
bool MenuOption::boolVal() {
  return _boolVal;
}
int MenuOption::intVal() {
  return _intVal;
}
float MenuOption::floatVal() {
  return _floatVal;
}
String MenuOption::title() {
  return _title;
}
int MenuOption::pos() {
  return _pos;
}

MenuOption configUploadFrequencyMins;
MenuOption configTempEnable;
MenuOption configTempLampShutoff;
MenuOption configTempLampShutoffTemp;
MenuOption configTempHighTempAlarm;
MenuOption configTempHighTemp;
MenuOption configTempLowTempAlarm;
MenuOption configTempLowTemp;
MenuOption configLampEnable;
MenuOption configLampHeaterMode;
MenuOption configLampHeaterTemp;
MenuOption configLampStartHour;
MenuOption configLampEndHour;
MenuOption configLampInvertLogic;
MenuOption configPumpEnable;
MenuOption configPumpFlowMode;
MenuOption configPumpFlowMl;
MenuOption configPumpFlowAlarm;
MenuOption configPumpDurationSec;
MenuOption configPumpFrequencyMin;
MenuOption configPumpInvertLogic;
MenuOption configFloat1Enable;
MenuOption configFloat1Alarm;
MenuOption configFloat1PumpShutoff;
MenuOption configFloat1InvertLogic;
MenuOption configFloat2Enable;
MenuOption configFloat2Alarm;
MenuOption configFloat2PumpShutoff;
MenuOption configFloat2InvertLogic;
MenuOption configFlowEnable;
MenuOption configFlowPulsesPerMl;
MenuOption configDisplayType;
MenuOption configEnableWifi;

String getConfigItemDescription(int menuPos) {
  if (configUploadFrequencyMins.pos() == menuPos) { return configUploadFrequencyMins.title(); }
  else if (configTempEnable.pos() == menuPos) { return configTempEnable.title(); }
  else if (configTempLampShutoff.pos() == menuPos) { return configTempLampShutoff.title(); }
  else if (configTempLampShutoffTemp.pos() == menuPos) { return configTempLampShutoffTemp.title(); }
  else if (configTempHighTempAlarm.pos() == menuPos) { return configTempHighTempAlarm.title(); }
  else if (configTempHighTemp.pos() == menuPos) { return configTempHighTemp.title(); }
  else if (configTempLowTempAlarm.pos() == menuPos) { return configTempLowTempAlarm.title(); }
  else if (configTempLowTemp.pos() == menuPos) { return configTempLowTemp.title(); }
  else if (configLampEnable.pos() == menuPos) { return configLampEnable.title(); }
  else if (configLampHeaterMode.pos() == menuPos) { return configLampHeaterMode.title(); }
  else if (configLampHeaterTemp.pos() == menuPos) { return configLampHeaterTemp.title(); }
  else if (configLampStartHour.pos() == menuPos) { return configLampStartHour.title(); }
  else if (configLampEndHour.pos() == menuPos) { return configLampEndHour.title(); }
  else if (configLampInvertLogic.pos() == menuPos) { return configLampInvertLogic.title(); }
  else if (configPumpEnable.pos() == menuPos) { return configPumpEnable.title(); }
  else if (configPumpFlowMode.pos() == menuPos) { return configPumpFlowMode.title(); }
  else if (configPumpFlowMl.pos() == menuPos) { return configPumpFlowMl.title(); }
  else if (configPumpFlowAlarm.pos() == menuPos) { return configPumpFlowAlarm.title(); }
  else if (configPumpDurationSec.pos() == menuPos) { return configPumpDurationSec.title(); }
  else if (configPumpFrequencyMin.pos() == menuPos) { return configPumpFrequencyMin.title(); }
  else if (configPumpInvertLogic.pos() == menuPos) { return configPumpInvertLogic.title(); }
  else if (configFloat1Enable.pos() == menuPos) { return configFloat1Enable.title(); }
  else if (configFloat1Alarm.pos() == menuPos) { return configFloat1Alarm.title(); }
  else if (configFloat1PumpShutoff.pos() == menuPos) { return configFloat1PumpShutoff.title(); }
  else if (configFloat1InvertLogic.pos() == menuPos) { return configFloat1InvertLogic.title(); }
  else if (configFloat2Enable.pos() == menuPos) { return configFloat2Enable.title(); }
  else if (configFloat2Alarm.pos() == menuPos) { return configFloat2Alarm.title(); }
  else if (configFloat2PumpShutoff.pos() == menuPos) { return configFloat2PumpShutoff.title(); }
  else if (configFloat2InvertLogic.pos() == menuPos) { return configFloat2InvertLogic.title(); }
  else if (configFlowEnable.pos() == menuPos) { return configFlowEnable.title(); }
  else if (configFlowPulsesPerMl.pos() == menuPos) { return configFlowPulsesPerMl.title(); }
  else if (configDisplayType.pos() == menuPos) { return configDisplayType.title(); }
  else if (configEnableWifi.pos() == menuPos) { return configEnableWifi.title(); }
  else { return "Invalid"; }
}

void incrementConfigItem(int menuPos, bool increment) {
  if (configUploadFrequencyMins.pos() == menuPos) { configUploadFrequencyMins.adjustVal(increment); }
  else if (configTempEnable.pos() == menuPos) { configTempEnable.adjustVal(increment); }
  else if (configTempLampShutoff.pos() == menuPos) { configTempLampShutoff.adjustVal(increment); }
  else if (configTempLampShutoffTemp.pos() == menuPos) { configTempLampShutoffTemp.adjustVal(increment); }
  else if (configTempHighTempAlarm.pos() == menuPos) { configTempHighTempAlarm.adjustVal(increment); }
  else if (configTempHighTemp.pos() == menuPos) { configTempHighTemp.adjustVal(increment); }
  else if (configTempLowTempAlarm.pos() == menuPos) { configTempLowTempAlarm.adjustVal(increment); }
  else if (configTempLowTemp.pos() == menuPos) { configTempLowTemp.adjustVal(increment); }
  else if (configLampEnable.pos() == menuPos) { configLampEnable.adjustVal(increment); }
  else if (configLampHeaterMode.pos() == menuPos) { configLampHeaterMode.adjustVal(increment); }
  else if (configLampHeaterTemp.pos() == menuPos) { configLampHeaterTemp.adjustVal(increment); }
  else if (configLampStartHour.pos() == menuPos) { configLampStartHour.adjustVal(increment); }
  else if (configLampEndHour.pos() == menuPos) { configLampEndHour.adjustVal(increment); }
  else if (configLampInvertLogic.pos() == menuPos) { configLampInvertLogic.adjustVal(increment); }
  else if (configPumpEnable.pos() == menuPos) { configPumpEnable.adjustVal(increment); }
  else if (configPumpFlowMode.pos() == menuPos) { configPumpFlowMode.adjustVal(increment); }
  else if (configPumpFlowMl.pos() == menuPos) { configPumpFlowMl.adjustVal(increment); }
  else if (configPumpFlowAlarm.pos() == menuPos) { configPumpFlowAlarm.adjustVal(increment); }
  else if (configPumpDurationSec.pos() == menuPos) { configPumpDurationSec.adjustVal(increment); }
  else if (configPumpFrequencyMin.pos() == menuPos) { configPumpFrequencyMin.adjustVal(increment); }
  else if (configPumpInvertLogic.pos() == menuPos) { configPumpInvertLogic.adjustVal(increment); }
  else if (configFloat1Enable.pos() == menuPos) { configFloat1Enable.adjustVal(increment); }
  else if (configFloat1Alarm.pos() == menuPos) { configFloat1Alarm.adjustVal(increment); }
  else if (configFloat1PumpShutoff.pos() == menuPos) { configFloat1PumpShutoff.adjustVal(increment); }
  else if (configFloat1InvertLogic.pos() == menuPos) { configFloat1InvertLogic.adjustVal(increment); }
  else if (configFloat2Enable.pos() == menuPos) { configFloat2Enable.adjustVal(increment); }
  else if (configFloat2Alarm.pos() == menuPos) { configFloat2Alarm.adjustVal(increment); }
  else if (configFloat2PumpShutoff.pos() == menuPos) { configFloat2PumpShutoff.adjustVal(increment); }
  else if (configFloat2InvertLogic.pos() == menuPos) { configFloat2InvertLogic.adjustVal(increment); }
  else if (configFlowEnable.pos() == menuPos) { configFlowEnable.adjustVal(increment); }
  else if (configFlowPulsesPerMl.pos() == menuPos) { configFlowPulsesPerMl.adjustVal(increment); }
  else if (configDisplayType.pos() == menuPos) { configDisplayType.adjustVal(increment); }
  else if (configEnableWifi.pos() == menuPos) { configEnableWifi.adjustVal(increment); }
}

const String stringEnabled = "Enabled";
const String stringDisabled = "Disabled";
String getConfigItemValue(int menuPos) {
  if (configUploadFrequencyMins.pos() == menuPos) { return String(configUploadFrequencyMins.intVal()); }
  else if (configTempEnable.pos() == menuPos) { return configTempEnable.boolVal() ? stringEnabled : stringDisabled; }
  else if (configTempLampShutoff.pos() == menuPos) { return configTempLampShutoff.boolVal() ? stringEnabled : stringDisabled; }
  else if (configTempLampShutoffTemp.pos() == menuPos) { return String(configTempLampShutoffTemp.floatVal()); }
  else if (configTempHighTempAlarm.pos() == menuPos) { return configTempHighTempAlarm.boolVal() ? stringEnabled : stringDisabled; }
  else if (configTempHighTemp.pos() == menuPos) { return String(configTempHighTemp.floatVal()); }
  else if (configTempLowTempAlarm.pos() == menuPos) { return configTempLowTempAlarm.boolVal() ? stringEnabled : stringDisabled; }
  else if (configTempLowTemp.pos() == menuPos) { return String(configTempLowTemp.floatVal()); }
  else if (configLampEnable.pos() == menuPos) { return configLampEnable.boolVal() ? stringEnabled : stringDisabled; }
  else if (configLampHeaterMode.pos() == menuPos) { return configLampHeaterMode.boolVal() ? stringEnabled : stringDisabled; }
  else if (configLampHeaterTemp.pos() == menuPos) { return String(configLampHeaterTemp.floatVal()); }
  else if (configLampStartHour.pos() == menuPos) { return String(configLampStartHour.intVal()); }
  else if (configLampEndHour.pos() == menuPos) { return String(configLampEndHour.intVal()); }
  else if (configLampInvertLogic.pos() == menuPos) { return configLampInvertLogic.boolVal() ? stringEnabled : stringDisabled; }
  else if (configPumpEnable.pos() == menuPos) { return configPumpEnable.boolVal() ? stringEnabled : stringDisabled; }
  else if (configPumpFlowMode.pos() == menuPos) { return configPumpFlowMode.boolVal() ? stringEnabled : stringDisabled; }
  else if (configPumpFlowMl.pos() == menuPos) { return String(configPumpFlowMl.intVal()); }
  else if (configPumpFlowAlarm.pos() == menuPos) { return configPumpFlowAlarm.boolVal() ? stringEnabled : stringDisabled; }
  else if (configPumpDurationSec.pos() == menuPos) { return String(configPumpDurationSec.intVal()); }
  else if (configPumpFrequencyMin.pos() == menuPos) { return String(configPumpFrequencyMin.intVal()); }
  else if (configPumpInvertLogic.pos() == menuPos) { return configPumpInvertLogic.boolVal() ? stringEnabled : stringDisabled; }
  else if (configFloat1Enable.pos() == menuPos) { return configFloat1Enable.boolVal() ? stringEnabled : stringDisabled; }
  else if (configFloat1Alarm.pos() == menuPos) { return configFloat1Alarm.boolVal() ? stringEnabled : stringDisabled; }
  else if (configFloat1PumpShutoff.pos() == menuPos) { return configFloat1PumpShutoff.boolVal() ? stringEnabled : stringDisabled; }
  else if (configFloat1InvertLogic.pos() == menuPos) { return configFloat1InvertLogic.boolVal() ? stringEnabled : stringDisabled; }
  else if (configFloat2Enable.pos() == menuPos) { return configFloat2Enable.boolVal() ? stringEnabled : stringDisabled; }
  else if (configFloat2Alarm.pos() == menuPos) { return configFloat2Alarm.boolVal() ? stringEnabled : stringDisabled; }
  else if (configFloat2PumpShutoff.pos() == menuPos) { return configFloat2PumpShutoff.boolVal() ? stringEnabled : stringDisabled; }
  else if (configFloat2InvertLogic.pos() == menuPos) { return configFloat2InvertLogic.boolVal() ? stringEnabled : stringDisabled; }
  else if (configFlowEnable.pos() == menuPos) { return configFlowEnable.boolVal() ? stringEnabled : stringDisabled; }
  else if (configFlowPulsesPerMl.pos() == menuPos) { return String(configFlowPulsesPerMl.floatVal()); }
  else if (configDisplayType.pos() == menuPos) { return String(configDisplayType.intVal()); }
  else if (configEnableWifi.pos() == menuPos) { return configEnableWifi.boolVal() ? stringEnabled : stringDisabled; }
  else { return "Invalid"; }
}

void onButtonPress(String btnType, bool longPress);
class Button {
  public:
    int Init(int pin, String type);
    void checkState(unsigned int long curMs);
  private:
    int _pin;
    String _btnType;
    unsigned int long _startTime;
    bool _lastState;
};

int Button::Init(int pin, String type) {
  _pin = pin;
  _btnType = type;
  _startTime = 0;
  _lastState = false;
  pinMode(pin, INPUT_PULLUP);
}

void Button::checkState(unsigned int long curMs) {
  if (digitalRead(_pin) == LOW && !_lastState) {
    _startTime = curMs;
    _lastState = true;
  }
  //button released
  if (digitalRead(_pin) == HIGH && _lastState) {
    if (curMs - _startTime > buttonShortTime) {
//      if (curMs - _startTime > buttonLongTime) {
//        tone(buzzPin, 800, 250);
//        onButtonPress(_btnType, true);
//      } else {
        tone(buzzPin, NOTE_C5, 10, buzzerChannel);
        noTone(buzzPin, buzzerChannel);
        onButtonPress(_btnType, false);
      //}
    }
    _lastState = false;
  }
}

Button buttonOK;
Button buttonBK;
Button buttonUP;
Button buttonDN;

void webConfigRead() {
//  webUsername = preferences.getString("webUser", "");
//  webPassword = preferences.getString("webPass", "");
//  webDeviceID = preferences.getString("webDevice", "");
//  wifiSSID = preferences.getChar("wifiSSID", "");
//  wifiPassphrase =  preferences.getChar("wifiPass", "");
}
void webConfigWrite() {
//  preferences.putString("webUser", webUsername);
//  preferences.putString("webPass", webPassword);
//  preferences.putString("webDevice", webDeviceID);
//  preferences.putChar("wifiSSID", wifiSSID);
//  preferences.putChar("wifiPass", wifiPassphrase);
}

void flashRead() {
  Serial.println("Reading flash...");
  configUploadFrequencyMins.setIntVal(     preferences.getUInt("uploadFreq", 30));
  //temp
  configTempEnable.setBoolVal(             preferences.getBool("tenable", true));
  configTempLampShutoff.setBoolVal(        preferences.getBool("tlampshutoff", false));
  configTempLampShutoffTemp.setFloatVal(   preferences.getFloat("tshutofftemp", 45.0));
  configTempHighTempAlarm.setBoolVal(      preferences.getBool("thighalarm", true));
  configTempHighTemp.setFloatVal(          preferences.getFloat("thightemp", 45.0));
  configTempLowTempAlarm.setBoolVal(       preferences.getBool("tlowalarm", true));
  configTempLowTemp.setFloatVal(           preferences.getFloat("tlowtemp", 10.0));
  //lamp
  configLampEnable.setBoolVal(             preferences.getBool("lenable", true));
  configLampHeaterMode.setBoolVal(         preferences.getBool("lheater", false));
  configLampHeaterTemp.setFloatVal(        preferences.getFloat("ltemp", 26.0));
  configLampStartHour.setIntVal(           preferences.getUInt("lstarthour", 2));
  configLampEndHour.setIntVal(             preferences.getUInt("lendhour", 22));
  configLampInvertLogic.setBoolVal(        preferences.getBool("linvert", false));
  //pump
  configPumpEnable.setBoolVal(             preferences.getBool("penable", true));
  configPumpFlowMode.setBoolVal(           preferences.getBool("pflow", false));
  configPumpFlowMl.setIntVal(              preferences.getUInt("pflowml", 0));
  configPumpFlowAlarm.setBoolVal(          preferences.getBool("pflowalarm", false));
  configPumpDurationSec.setIntVal(         preferences.getUInt("pduration", 30));
  configPumpFrequencyMin.setIntVal(        preferences.getUInt("pfrequency", 15));
  configPumpInvertLogic.setBoolVal(        preferences.getBool("pinvert", false));
  //float 1
  configFloat1Enable.setBoolVal(           preferences.getBool("flt1enable", true));
  configFloat1Alarm.setBoolVal(            preferences.getBool("flt1alarm", false));
  configFloat1PumpShutoff.setBoolVal(      preferences.getBool("flt1shutoff", false));
  configFloat1InvertLogic.setBoolVal(      preferences.getBool("flt1invert", false));
  //float 2
  configFloat2Enable.setBoolVal(           preferences.getBool("flt2enable", true));
  configFloat2Alarm.setBoolVal(            preferences.getBool("flt2alarm", false));
  configFloat2PumpShutoff.setBoolVal(      preferences.getBool("flt2shutoff", false));
  configFloat2InvertLogic.setBoolVal(      preferences.getBool("flt2invert", false));
  //flow
  configFlowEnable.setBoolVal(             preferences.getBool("flwenable", true));

  configFlowPulsesPerMl.setFloatVal(       preferences.getFloat("flowpulsesml", 0.45));

  configEnableWifi.setBoolVal(             preferences.getBool("enablewifi", true));

  Serial.println("Display type: " + preferences.getUInt("displaytype", 0));
  configDisplayType.setIntVal(             preferences.getUInt("displaytype", 0));
  Serial.println("Loaded settings from flash!");
}

void flashWrite() {

  Serial.println("Writing flash...");
  preferences.putUInt("uploadFreq", configUploadFrequencyMins.intVal());
  //temp 1
  preferences.putBool("tenable", configTempEnable.boolVal());
  preferences.putBool("tlampshutoff", configTempLampShutoff.boolVal());
  preferences.putFloat("tshutofftemp", configTempLampShutoffTemp.floatVal());
  preferences.putBool("thighalarm", configTempHighTempAlarm.boolVal());
  preferences.putFloat("thightemp", configTempHighTemp.floatVal());
  preferences.putBool("tlowalarm", configTempLowTempAlarm.boolVal());
  preferences.putFloat("tlowtemp", configTempLowTemp.floatVal());
  //lamp 1
  preferences.putBool("lenable", configLampEnable.boolVal());
  preferences.putBool("lheater", configLampHeaterMode.boolVal());
  preferences.putFloat("ltemp", configLampHeaterTemp.floatVal());
  preferences.putUInt("lstarthour", configLampStartHour.intVal());
  preferences.putUInt("lendhour", configLampEndHour.intVal());
  preferences.putBool("linvert", configLampInvertLogic.boolVal());
  //pump 1
  preferences.putBool("penable", configPumpEnable.boolVal());
  preferences.putBool("pflow", configPumpFlowMode.boolVal());
  preferences.putUInt("pflowml", configPumpFlowMl.intVal());
  preferences.putBool("pflowalarm", configPumpFlowAlarm.boolVal());
  preferences.putUInt("pduration", configPumpDurationSec.intVal());
  preferences.putUInt("pfrequency", configPumpFrequencyMin.intVal());
  preferences.putBool("pinvert", configPumpInvertLogic.boolVal());
  //float 1
  preferences.putBool("flt1enable", configFloat1Enable.boolVal());
  preferences.putBool("flt1alarm", configFloat1Alarm.boolVal());
  preferences.putBool("flt1shutoff", configFloat1PumpShutoff.boolVal());
  preferences.putBool("flt1invert", configFloat1InvertLogic.boolVal());
  //float 2
  preferences.putBool("flt2enable", configFloat2Enable.boolVal());
  preferences.putBool("flt2alarm", configFloat2Alarm.boolVal());
  preferences.putBool("flt2shutoff", configFloat2PumpShutoff.boolVal());
  preferences.putBool("flt2invert", configFloat2InvertLogic.boolVal());
  //flow
  preferences.putBool("flwenable", configFlowEnable.boolVal());

  preferences.putFloat("flowpulsesml", configFlowPulsesPerMl.floatVal());

  Serial.println("Writing display type: "+ String(configDisplayType.intVal()));
  preferences.putUInt("displaytype", configDisplayType.intVal());

  preferences.putBool("enablewifi", configEnableWifi.boolVal());

  
}

//update config from online
String webError = "";
bool configDownloaded = false;
void handleReceiveConfig(String webResponse) {

  const size_t capacity = JSON_OBJECT_SIZE(51) + 1210;
  DynamicJsonDocument doc(capacity);

  deserializeJson(doc, webResponse);

  //check if parsing the JSON failed
  JsonObject object = doc.as<JsonObject>();
  if (object.isNull()) {
    webError = "Error: ";
    webError += webResponse;
    Serial.println(webError);
    configDownloaded = false;
    return;
  }

  configUploadFrequencyMins.setIntVal( doc["configUploadFrequencyMins"]);

  configTempEnable.setBoolVal( doc["configTempEnable"]);
  configTempLampShutoff.setBoolVal( doc["configTempLampShutoff"]);
  configTempLampShutoffTemp.setFloatVal( doc["configTempLampShutoffTemp"]);
  configTempHighTempAlarm.setBoolVal( doc["configTempHighTempAlarm"]);
  configTempHighTemp.setFloatVal( doc["configTempHighTemp"]);
  configTempLowTempAlarm.setBoolVal( doc["configTempLowTempAlarm"]);
  configTempLowTemp.setFloatVal( doc["configTempLowTemp"]);

  configLampEnable.setBoolVal( doc["configLampEnable"]);
  configLampHeaterMode.setBoolVal( doc["configLampHeaterMode"]);
  configLampHeaterTemp.setFloatVal( doc["configLampHeaterTemp"]);
  configLampStartHour.setIntVal( doc["configLampStartHour"]);
  configLampEndHour.setIntVal( doc["configLampEndHour"]);
  configLampInvertLogic.setBoolVal( doc["configLampInvertLogic"]);

  configPumpEnable.setBoolVal( doc["configPumpEnable"]);
  configPumpFlowMode.setBoolVal( doc["configPumpFlowMode"]);
  configPumpFlowMl.setIntVal( doc["configPumpFlowMl"]);
  configPumpFlowAlarm.setBoolVal( doc["configPumpFlowAlarm"]);
  configPumpDurationSec.setIntVal( doc["configPumpDurationSec"]);
  configPumpFrequencyMin.setIntVal( doc["configPumpFrequencyMin"]);
  configPumpInvertLogic.setBoolVal( doc["configPumpInvertLogic"]);

  configFloat1Enable.setBoolVal( doc["configFloat1Enable"]);
  configFloat1Alarm.setBoolVal( doc["configFloat1Alarm"]);
  configFloat1PumpShutoff.setBoolVal( doc["configFloat1PumpShutoff"]);
  configFloat1InvertLogic.setBoolVal( doc["configFloat1InvertLogic"]);

  configFloat2Enable.setBoolVal( doc["configFloat2Enable"]);
  configFloat2Alarm.setBoolVal( doc["configFloat2Alarm"]);
  configFloat2PumpShutoff.setBoolVal( doc["configFloat2PumpShutoff"]);
  configFloat2InvertLogic.setBoolVal( doc["configFloat2InvertLogic"]);

  configFlowEnable.setBoolVal( doc["configFlowEnable"]);

  configFlowPulsesPerMl.setFloatVal( doc["configFlowPulsesPerMl"]);

  configDisplayType.setIntVal( doc["configDisplayType"]);

  // configEnableWifi ... it would be a bit silly to set this parameter via an online app...

  Serial.println("Updated config:");
  Serial.println(webResponse);

  configDownloaded = true;
  webError = "";

  flashWrite();
}

#define firstCoordinate 0
#define charHeight 8
#define titleBarHeight 11
#define lineX 2
int lineY(int line) { 
  if (line == 1) { return firstCoordinate + titleBarHeight + 1; }
  if (line == 2) { return lineY(1) + charHeight; }
  if (line > 2) { return lineY(line - 1) + charHeight; }
}

//draws the border and title etc
bool wifiConnected = false;
bool inMenu = false;
String menuTitle = "";
void drawInitial() {
  display.clearDisplay();
  display.drawRoundRect(0,0,display.width(),display.height(), 2, WHITE); //x, y, x, y, radius, color
  display.fillRoundRect(0,0,display.width(),10, 2, WHITE); //x, y, x, y, radius, color
  display.setTextColor(BLACK); // Draw black text (background transparent by default)
  display.setCursor(27, 2); //title line
  String title = inMenu ? menuTitle : "HydroTek rev5";
  display.print(title);
  display.setCursor(3, 2); //title line
  if (wifiConnected) { display.print("+"); } else { display.print("-"); }
  if (configDownloaded) { display.print("+"); } else { display.print("-"); }
  display.setTextColor(WHITE, BLACK); // Draw white text with a black background 
}

float temp;
float humidity;
void tempCheckState() {
  if (configTempEnable.boolVal()) {
    temp = dht.readTemperature(); //read temperature as Celsius (the default)
    humidity = dht.readHumidity(); //read humidity
    if (isnan(temp)) { temp = 0.0; }
    if (isnan(humidity)) { humidity = 0.0; }
  }
}

//PID variables
float temperature_read = 0;
float set_temperature = 0;
float PID_error = 0;
float previous_error = 0;
float elapsedTime, Time, timePrev;
int PID_value = 0;

//PID constants
int kp = 9.1;   int ki = 0.1;   int kd = 0.1;
int PID_p = 0;    int PID_i = 0;    int PID_d = 0;

//the current duty cycle calculated by the PID loop
int lampDutyCycle = 0;

//temperature PID control for when Lamp is used as Heater
unsigned int long lastSerialUpdate = 0;
void heaterPIDUpdate() {
  // First we read the real value of temperature
  temperature_read = temp;
  set_temperature = configLampHeaterTemp.floatVal();
  //Next we calculate the error between the setpoint and the real value
  PID_error = set_temperature - temperature_read;
  //Calculate the P value
  PID_p = kp * PID_error;
  //Calculate the I value in a range on +-3
  if(-3 < PID_error <3)
  {
    PID_i = PID_i + (ki * PID_error);
  }

  //For derivative we need real time to calculate speed change rate
  timePrev = Time;                            // the previous time is stored before the actual time read
  Time = millis();                            // actual time read
  elapsedTime = (Time - timePrev) / 1000;
  //Now we can calculate the D calue
  PID_d = kd*((PID_error - previous_error)/elapsedTime);
  //Final total PID value is the sum of P + I + D
  PID_value = PID_p + PID_i + PID_d;

  //We define PWM range between 0 and 255
  if(PID_value < 0)
  {    PID_value = 0;    }
  if(PID_value > 255)  
  {    PID_value = 255;  }
  //Now we can write the PWM signal to the mosfet on digital pin D3
  //analogWrite(lampPin,PID_value);
  lampDutyCycle = PID_value;
  previous_error = PID_error;     //Remember to store the previous error for next loop.

  String PIDOutput = 
  "Set temp: " + String(set_temperature) + 
  " Real temp: " + String(temperature_read) + 
  " PID_d: " + String(PID_d) + 
  " PID_value: " + String(PID_value) + 
  " Duty cycle: " + String(PID_value) +
  " Elapsed time: " + String(elapsedTime);
  if (millis() - lastSerialUpdate > 5000) {
    Serial.println(PIDOutput);
    lastSerialUpdate = millis();
  }
  
}

//turn lamp relay(s) on / off
bool lampLastState = false; // used to know the status of the lamp elsewhere

//unsigned int long lastLampDutyCycleUpdate = 0; // a timer for turning the lamp on / off
int lampDutyCycleCount = 0;
void lampSetState (String stateStr) {
  
  bool hourState = stateStr == "ON"; //the lamp state, requested by the normal lamp operation (hours based)
  bool invertLogic = configLampInvertLogic.boolVal();
  bool heaterMode = configLampHeaterMode.boolVal();
  bool tempSensorEnabled = configTempEnable.boolVal();
  bool heaterUsesHours = false; //add an option for this!
  bool actualState = hourState;
  bool heaterState = false;
  
  if (heaterMode) {
    if (!tempSensorEnabled) {
      Serial.println("Heater mode is enabled but temp sensor is not!");
      return;
    }
    if (!heaterUsesHours || (heaterUsesHours && hourState)) {
      heaterPIDUpdate();
        if (lampDutyCycle > 1 && lampDutyCycle * HEATER_SPEED_RATIO >= lampDutyCycleCount) { //lamp ON (duty cycle > 1 to prevent short flashes)
          heaterState = true;
        } else {
          heaterState = false;
        }
        lampDutyCycleCount++;
        if (lampDutyCycleCount > 255 * HEATER_SPEED_RATIO) {
          lampDutyCycleCount = 0;
        }
        actualState = heaterState; 
    }
  }
  if (!invertLogic) {
    if (actualState) {
      digitalWrite(lampPin, LOW);
      lampLastState = true;
    } else {
      digitalWrite(lampPin, HIGH);
      lampLastState = false;
    }
  } else {
    if (actualState) {
      digitalWrite(lampPin, HIGH);
      lampLastState = true;
    } else {
      digitalWrite(lampPin, LOW);
      lampLastState = false;
    }
  }
  
}

//check whether the current time is during the lamp ON hours
void lampCheckState (int currentHour) {
  if (configLampEnable.boolVal()) {
    if (configLampEndHour.intVal() > configLampStartHour.intVal()) { //if the shutoff hour is later in the day than the turn on hour
      if (currentHour < configLampEndHour.intVal() && currentHour >= configLampStartHour.intVal()) {
        lampSetState("ON");
      } else {
        lampSetState("OFF");
      }
    }
    else if (configLampEndHour.intVal() < configLampStartHour.intVal()) { // if the shutoff hour is earlier than the turn on hour
      if (currentHour >= configLampStartHour.intVal() || currentHour < configLampEndHour.intVal()) {
        lampSetState("ON");
      } else {
        lampSetState("OFF");
      }
    } else if (configLampEndHour.intVal() == configLampStartHour.intVal()) { //if both hours are the same, assume on all the time
      lampSetState("ON");
    }
  } else {
    lampSetState("OFF");
  }
}

//float sensor variables
bool float1triggered = false;
bool float2triggered = false;

//sounds alarm if conditions are met
bool pumpMissedFlowTarget = false;
String alarmReason = "";
void alarmCheckState(){
  bool shouldSoundAlarm = false;
  alarmReason = "";
  if (!wifiConnected && configEnableWifi.boolVal()) {
    alarmReason = "WiFi Disconnected";
  } else if (configEnableWifi.boolVal()) {
    if (configDownloaded) {
      alarmReason = "Sync OK";
    } else {
      #ifdef offlineMode
        alarmReason = "Offline Mode";
      #else
        alarmReason = "Sync Error";
      #endif
    }
  }
  if (configTempEnable.boolVal() && configTempHighTempAlarm.boolVal() && temp > configTempHighTemp.floatVal()) {
    shouldSoundAlarm = true;
    alarmReason = "Temp HIGH";
  }
  if (configFloat1Enable.boolVal() && float1triggered) {
    if (configFloat1Alarm.boolVal()) {
      shouldSoundAlarm = true;
    }    
    alarmReason = "Tank 1 FULL/EMPTY";
  }
  if (configFloat2Enable.boolVal() && float2triggered) {
    if (configFloat2Alarm.boolVal()) {
      shouldSoundAlarm = true; 
    }
    alarmReason = "Tank 2 FULL/EMPTY";
  }
  if (pumpMissedFlowTarget) {
    shouldSoundAlarm = true;
    alarmReason = "Pump LOW FLOW";
  }
  if (shouldSoundAlarm) {
    tone(buzzPin, NOTE_A4, 50, buzzerChannel);
    noTone(buzzPin, buzzerChannel);
  }
}

bool pumpLastState = false;
void pumpSetState(String state) {
  if (!configPumpInvertLogic.boolVal() && state == "ON") {
    digitalWrite(pumpPin, LOW);
    pumpLastState = true;
  } else if (!configPumpInvertLogic.boolVal() && state == "OFF") {
    digitalWrite(pumpPin, HIGH);
    pumpLastState = false;
  } else if (configPumpInvertLogic.boolVal() && state == "ON") {
    digitalWrite(pumpPin, HIGH);
    pumpLastState = true;
  } else if (configPumpInvertLogic.boolVal() && state == "OFF") {
    digitalWrite(pumpPin, LOW);
    pumpLastState = false;
  }
}

unsigned int pumpMeasuredFlowMl = 0;
int pumpMissedTargetByMl = 0;
void pumpCheckState(DateTime now) {
  if (inMenu) { return; }
  int currentSecond = now.hour() * 60 * 60 + now.minute() * 60 + now.second();
  if (configPumpEnable.boolVal()) {
    bool pumpShouldRun = false;
    for (int i = 0; i <= 86400; i = i + configPumpFrequencyMin.intVal() * 60) {
      if (currentSecond >= i && currentSecond <= i + configPumpDurationSec.intVal()) {
        if (configPumpFlowMode.boolVal()) {
          if (pumpMeasuredFlowMl < configPumpFlowMl.intVal()) {
            pumpShouldRun = true;
          }
          //if it's within the last second of run time and the flow hasn't met the target by more than 5ml, consider the flow target not met
          if (currentSecond > i + configPumpDurationSec.intVal() - 2 && configPumpFlowMl.intVal() - pumpMeasuredFlowMl > 5) {
            pumpMissedFlowTarget = true;
            pumpMissedTargetByMl = configPumpFlowMl.intVal() - pumpMeasuredFlowMl;
          }
        } else {
          pumpShouldRun = true;
        }
      }
    }
    if (pumpShouldRun) {
      pumpSetState("ON");
    } else {
      pumpSetState("OFF");
      pumpMeasuredFlowMl = 0;
    }
  }
}

unsigned int flowPulseCount = 0;
unsigned int flowMlSinceLastUpload = 0;
void flowCheckState(){
  if (configFlowEnable.boolVal()) {
    int flowInMl = flowPulseCount / configFlowPulsesPerMl.floatVal();
    pumpMeasuredFlowMl = pumpMeasuredFlowMl + flowInMl;
    flowMlSinceLastUpload = flowMlSinceLastUpload + flowInMl;
    flowPulseCount = 0;
  }
  //(pulsesOver1000ms * 60 / 7.5); //the LPH calculation for flow sensors
}

/*
 * 
 * //the below values are loaded from the web portal
    int configUploadFrequencyMins = 0;
    //temp
    bool configTempEnable;
    bool configTempLampShutoff;
    float configTempLampShutoffTemp;
    bool configTempHighTempAlarm;
    float configTempHighTemp;
    bool configTempLowTempAlarm;
    float configTempLowTemp;
    //lamp
    bool configLampEnable;
    bool configLampHeaterMode;
    float configLampHeaterTemp;
    int configLampStartHour;
    int configLampEndHour;
    bool configLampInvertLogic;
    //pump
    bool configPumpEnable;
    bool configPumpFlowMode;
    int configPumpFlowMl;
    int configPumpFlowAlarm;
    int configPumpDurationSec;
    int configPumpFrequencyMin;
    bool configPumpInvertLogic;
    //float 1
    bool configFloat1Enable;
    bool configFloat1Alarm;
    bool configFloat1PumpShutoff;
    bool configFloat1InvertLogic;
    //float 2
    bool configFloat2Enable;
    bool configFloat2Alarm;
    bool configFloat2PumpShutoff;
    bool configFloat2InvertLogic;
    //flow
    bool configFlowEnable;
 * 
 */

 

    
//bool inMenu = false; relocated to the drawInitial function
//String menuTitle = ""; relocated to the drawInitial function
bool screenIsSleeping = false;
int unsigned long screenSleepLastAction = 0;
int menuSelection = 1;
bool editingAnOption = false;
int unsigned long buttonDebounceTimer = 0;
bool calibratingFlowSensor = false;
int long unsigned flowSensorCalibrationPulses = 0;
void onButtonPress(String btnType, bool longPress) { //OK BK UP DN

  #ifdef buttonDisable
  return;
  #endif
  
  if (millis() - buttonDebounceTimer < buttonDebounceTime) {
    return;
  } else {
    buttonDebounceTimer = millis();
  }
  
  Serial.println("Button pressed: " + btnType + " " + (longPress ? "long" : "short"));

  screenSleepLastAction = millis();

  //wake the screen up and do nothing else
  if (screenIsSleeping) {
    screenIsSleeping = false;
    return;
  }  
  
  if (!inMenu && btnType == "OK") {
    inMenu = true;
    menuTitle = "Menu";
  }
  else if (inMenu) {

    if (btnType == "OK") {
      if (editingAnOption){
        if (menuSelection == 31) {
          calibratingFlowSensor = !calibratingFlowSensor;
          if (calibratingFlowSensor) {
            configFlowPulsesPerMl.setFloatVal(flowSensorCalibrationPulses / 5.0 / 1000.0);
            flashWrite();
            flowSensorCalibrationPulses = 0;
          }
        } else {
          editingAnOption = !editingAnOption;
        }
      } else {
        editingAnOption = !editingAnOption;
      }      
    }
    
    if (btnType == "BK") {
      if (!editingAnOption) {
        inMenu = false;
        menuTitle = "";
        drawInitial();
      } else {
        if (menuSelection == 31) {
          if (calibratingFlowSensor) {
            calibratingFlowSensor = !calibratingFlowSensor;
            configFlowPulsesPerMl.setFloatVal(flowSensorCalibrationPulses / 5.0 / 1000.0);
            flashWrite();
            flowSensorCalibrationPulses = 0;
          } else {
            editingAnOption = false;
          }
        } else {
          editingAnOption = false;
        }
      }
    }

    if (btnType == "UP") {
      if (!editingAnOption){
        if (menuSelection == 1) {
          menuSelection = numMenuOptions;
        } else {
          menuSelection--;
        }
      } else {
        incrementConfigItem(menuSelection, true);
        flashWrite();
      }
    }

    if (btnType == "DN") {
      if (!editingAnOption) {
        if (menuSelection == numMenuOptions) {
          menuSelection = 1;
        } else {
          menuSelection++;
        }
      } else {
        incrementConfigItem(menuSelection, false);
        flashWrite();
      }
    }
    
  }
}

//set line position and font
void drawMenuSet(int menuPos, int offset) {
  if (menuPos > numMenuOptions) { return; }
  int displayLine = menuPos - offset; //offset corrects the number representing overall menu position for display on a display with room for only 6 lines
  display.setCursor(lineX, lineY(displayLine));
  if (menuSelection == menuPos) {
    display.setTextColor(BLACK, WHITE);
  }
  else {
    display.setTextColor(WHITE, BLACK);
  }
  display.print(getConfigItemDescription(menuPos));
}

void menuDraw() {
  if (!editingAnOption) {
    int iMin = 1;
    int iMax = 7;
    int offset = 0;
      
    for(int i = 1; i < numMenuOptions + 1; i = i + 6) {
      int numPages = ceil(numMenuOptions / 6.0);
      if (menuSelection >= i && menuSelection < i + 7) {
        menuTitle = "Menu - " + String(int(floor(i / 6) + 1)) + "/" + String(numPages);
        iMin = i;
        iMax = i + 6;
        offset = i - 1;
      }
    }
    
    drawInitial(); //the code is structured this way so the menuTitle could be set before the drawInitial, but not before actually writing the menu items to the display, as drawInitial writes a black background
    for (int i = iMin; i < iMax; i++) { drawMenuSet(i, offset); }
  }
  else if (editingAnOption) {
    menuTitle = "Menu - " + String(menuSelection) + "/" + String(numMenuOptions);
    drawInitial();
    display.setCursor(lineX, lineY(1));
    display.print(getConfigItemDescription(menuSelection));
    display.setCursor(lineX + 15, lineY(3));
    display.setTextSize(2);
    if (calibratingFlowSensor && menuSelection == 31) {
      display.print(flowSensorCalibrationPulses);
      display.setTextSize(1);
      display.setCursor(lineX, lineY(5));
      display.print("Pump 5L then hit OK");
    } else {
      if (menuSelection == 32) { // selecting display type
        if (getConfigItemValue(menuSelection) == "0") { //getConfigItemValue returns a String for display friendliness
          display.print("All Stats");
        } else if (getConfigItemValue(menuSelection) == "1") {
          display.print("Temp Only");
        }
      } else {
        display.print(getConfigItemValue(menuSelection)); //print the current setting
        if (menuSelection == 31) { // if 
          display.setTextSize(1);
          display.setCursor(lineX, lineY(2));
          display.print("Hit OK to calibrate");
        }
      }      
    }
    display.setTextSize(1);
  }
  display.display();
}

//checkstate variables moved to above the alarm check function
void floatCheckState() {
  //get float switch statuses
  if (configFloat1Enable.boolVal()) {
    if (configFloat1InvertLogic.boolVal()) {
      float1triggered = digitalRead(floatSw1Pin);
    } else {
      float1triggered = !digitalRead(floatSw1Pin);
    }
  }
  if (configFloat2Enable.boolVal()) {
    if (configFloat2InvertLogic.boolVal()) {
      float2triggered = digitalRead(floatSw2Pin);
    } else {
      float2triggered = !digitalRead(floatSw2Pin);
    }
  }
}

int unsigned long lastTestCircle = 0;
void testdrawcircle(void) {
  if (millis() - lastTestCircle > 1000) {
    display.clearDisplay();
    int x = random(15, display.width() - 15);
    int y = random(15, display.height() - 15);
    int radius = random(display.height()/2-15);
    display.drawCircle(x, y, radius, WHITE);
    display.display();
    lastTestCircle = millis();
  }  
}

#define numPages 2
byte page = 1; //which page the loop is on
#define updateFrequency 250 //how often to update contents of display
#define pageFrequency 3000 //seconds to display each page before moving to the next
unsigned int long timeSinceLastPage = 0; //track when page was last changed
unsigned int long timeSinceLastUpdate = 0;
void displayUpdate(DateTime now, int curMillis) {

  if (screenIsSleeping) {
    //display.clearDisplay();
    testdrawcircle();
    //display.display();
    return;
  }
  
  if (inMenu) {
    //Serial.println("IN menu");
    menuDraw();
    return;
  }
  if (!timeSinceLastUpdate == 0 && curMillis - timeSinceLastUpdate < updateFrequency ) {
    return;
  } else {
    timeSinceLastUpdate = curMillis;
  }
  //update the display
  drawInitial();
  //display.setTextSize(2);
  display.setCursor(lineX, lineY(1));

  int displayMode = configDisplayType.intVal();

  if (displayMode == 0) {
    if (page == 1) {
      String line = "Temp: ";
      if (configTempEnable.boolVal()) {
        line += temp;
        line += (char)247;
        line += "C";
      } else { line += "disabled"; }
      display.print(line); 
      display.setCursor(lineX, lineY(2));
      line = "Hum.: ";
      if (configTempEnable.boolVal()) {
        line += humidity;
        line += "%";
      } else { line += "disabled"; }
      display.print(line); 
      display.setCursor(lineX, lineY(3));
      String curDate; curDate += now.day(); curDate += '/'; curDate += now.month(); curDate += '/'; curDate += now.year();
      String curTime; curTime += now.hour(); curTime += ':'; curTime += now.minute(); curTime += ':'; curTime += now.second();
      line = curDate + " - " + curTime;
      display.print(line);
      display.setCursor(lineX, lineY(4));
      line = "Flow: ";
      if (configFlowEnable.boolVal()) {
        line += flowMlSinceLastUpload;
        line += "mL";
      } else { line += "disabled"; }
      display.print(line);
    }
    else if (page == 2) {
      String line = "Float 1: ";
      if (configFloat1Enable.boolVal()) {
        line += float1triggered ? "X" : "-";
      } else { line += "disabled"; }
      display.print(line);
      display.setCursor(lineX, lineY(2));
      line = "Float 2: ";
      if (configFloat2Enable.boolVal()) {
        line += float2triggered ? "X" : "-";
      } else { line += "disabled"; }
      display.print(line);
      display.setCursor(lineX, lineY(3));
      line = "Lamp: ";
      if (configLampEnable.boolVal()) {
        line += lampLastState ? "ON" : "OFF";
      } else { line += "disabled"; }
      display.print(line);
      display.setCursor(lineX, lineY(4));
      line = "Pump: ";
      if (configPumpEnable.boolVal()) {
        line += pumpLastState ? "ON" : "OFF";
      } else { line += "disabled"; }
      display.print(line);
    }
  } else if (displayMode == 1) {
    display.setCursor(lineX + 30, lineY(1));
    display.setTextSize(2);
    display.print("Temp"); 
    display.setCursor(lineX + 15, lineY(3));
    String line = "";
    if (configTempEnable.boolVal()) {
      line += temp;
      line += (char)247;
      line += "C";
    } else { line += "disabled"; }
    display.print(line);
    display.setCursor(lineX + 3, lineY(6));
    line = "Lamp: ";
    if (configLampEnable.boolVal()) {
      line += lampLastState ? "ON" : "OFF";
    } else { line += "disabled"; }
    display.setTextSize(1);
    display.print(line);    
  }
  display.setCursor(lineX, lineY(5));
  //Serial.println(alarmReason);
  display.print(alarmReason); 
  display.setCursor(lineX, lineY(6));
  display.print(""); //used to be webError
  display.display();

  if (!timeSinceLastPage == 0 && curMillis - timeSinceLastPage < pageFrequency ) {
    return;
  } else {
    timeSinceLastPage = curMillis;
  }
  
  page++;
  if (page > numPages) {
    page = 1;
  }
}

unsigned int long lastDataUpload = 0;
unsigned int long lastSuccessfulDataUpload = 0;
bool firstUpload = false;
void uploadData(int curMillis) {

  #ifdef offlineMode
    return;
  #endif

  if (wifiConnected) {
    if (curMillis - lastDataUpload > configUploadFrequencyMins.intVal() * 1000 * 60 || !firstUpload) {
      Serial.print("Uploading data. Last attempt ");
      Serial.print((curMillis - lastDataUpload) / 1000);
      Serial.println("sec ago.");
      Serial.print("Last successful attempt ");
      Serial.print((curMillis - lastSuccessfulDataUpload) / 1000);
      Serial.println("sec ago.");
      lastDataUpload = curMillis;      
      
      String uploadData = "t="; uploadData += temp;
      uploadData += "&h="; uploadData += humidity;
      uploadData += "&f1="; uploadData += float1triggered ? "1" : "0";
      uploadData += "&f2="; uploadData += float2triggered ? "1" : "0";
      uploadData += "&l="; uploadData += flowMlSinceLastUpload;

      if (!firstUpload) {
        firstUpload = true;
      }
  
      flowMlSinceLastUpload = 0; //reset vars

      Serial.println(uploadData);

      String webResponse = "";
      String response = "";
      HTTPClient http;
      if(http.begin(webEndpoint)){
        http.addHeader("Content-Type", "application/x-www-form-urlencoded", false, true);
        
        String postString = "&password=" + devicePassword + "&deviceid=" + deviceID + "&" + uploadData;
        int httpCode = http.POST(postString);
        if (httpCode > 0) { // httpCode will be negative on error
          response = "OK";
          response += (String)httpCode;

          webResponse = http.getString();
          webResponse.replace('\n', ' ');
        } else {
          response = "ERROR";
          response += (String)httpCode;

          webResponse = http.errorToString(httpCode).c_str();
          webResponse.replace('\n', ' ');
        }
        http.end();
      } else {
        webResponse = "Connection Error";
      }
  
      handleReceiveConfig(webResponse);
      alarmReason = "";
      
    } 
  } else if (firstUpload) {
    Serial.println("Set to online mode, but not on WiFi");
  }
  
}

void setup() {

  Serial.begin(115200); //pour a bowl of Serial

  buttonOK.Init(buttonPinOK, "OK");
  buttonBK.Init(buttonPinBK, "BK");
  buttonUP.Init(buttonPinUP, "UP");
  buttonDN.Init(buttonPinDN, "DN");

  //void Init(int pos, String title, String type, float maxVal, float minVal); //bool, int, or float
  
  configUploadFrequencyMins.Init(1, "Upload frequency", "int", 30, 1);
  //temp
  configTempEnable.Init(2, "Temp enable", "bool", 0, 0.0);
  configTempLampShutoff.Init(3, "HI Temp lamp off", "bool", 0, 0);
  configTempLampShutoffTemp.Init(4, "Shutoff Temp degC", "float", 60, 5);
  configTempHighTempAlarm.Init(5, "HI temp alarm", "bool", 0, 0);
  configTempHighTemp.Init(6, "HI Temp degC", "float", 60, 5);
  configTempLowTempAlarm.Init(7, "LO temp alarm", "bool", 0, 0);
  configTempLowTemp.Init(8, "HI Temp degC", "float", 60, 5);
  //lamp
  configLampEnable.Init(9, "Lamp enable", "bool", 0, 0);
  configLampHeaterMode.Init(10, "Lamp is heater", "bool", 0, 0);
  configLampHeaterTemp.Init(11, "Heater temp degC", "float", 60, 5);
  configLampStartHour.Init(12, "Lamp on hour", "int", 23, 0);
  configLampEndHour.Init(13, "Lamp off hour", "int", 23, 0);
  configLampInvertLogic.Init(14, "Lamp invert", "bool", 0, 0);
  //pump
  configPumpEnable.Init(15, "Pump enable", "bool", 0, 0);
  configPumpFlowMode.Init(16, "Pump flow mode", "bool", 0, 0);
  configPumpFlowMl.Init(17, "Pump flow Ml", "int", 5000, 0);
  configPumpFlowAlarm.Init(18, "Pump alarm", "bool", 0, 0);
  configPumpDurationSec.Init(19, "Pump duration sec", "int", 240, 5);
  configPumpFrequencyMin.Init(20, "Pump freq. min", "int", 240, 2);
  configPumpInvertLogic.Init(21, "Pump invert", "bool", 0, 0);
  //float 1
  configFloat1Enable.Init(22, "Float1 enable", "bool", 0, 0);
  configFloat1Alarm.Init(23, "Float1 alarm", "bool", 0, 0);
  configFloat1PumpShutoff.Init(24, "Float1 pump off", "bool", 0, 0);
  configFloat1InvertLogic.Init(25, "Float1 invert", "bool", 0, 0);
  //float 2
  configFloat2Enable.Init(26, "Float2 enable", "bool", 0, 0);
  configFloat2Alarm.Init(27, "Float2 alarm", "bool", 0, 0);
  configFloat2PumpShutoff.Init(28, "Float2 pump off", "bool", 0, 0);
  configFloat2InvertLogic.Init(29, "Float2 invert", "bool", 0, 0);
  //flow
  configFlowEnable.Init(30, "Flow enable", "bool", 0, 0);

  configFlowPulsesPerMl.Init(31, "Flow pulses per Ml", "float", 1000, 0);

  configDisplayType.Init(32, "Select display type", "int", 1, 0);

  configEnableWifi.Init(33, "Enable WiFi", "bool", 0, 0);

  pinMode(floatSw1Pin, INPUT_PULLUP);
  pinMode(floatSw2Pin, INPUT_PULLUP);
  pinMode(lampPin, OUTPUT);
  pinMode(pumpPin, OUTPUT);
  pinMode(flowPin, INPUT_PULLUP);

  //pinMode(ledPin, OUTPUT);
  ledcSetup(15, 5000, 8);
  ledcAttachPin(ledPin, 15);
  ledcWrite(15, ledBrightness);
  
  tone(buzzPin, NOTE_C4, 60, buzzerChannel);
  noTone(buzzPin, buzzerChannel);
  tone(buzzPin, NOTE_E4, 60, buzzerChannel);
  noTone(buzzPin, buzzerChannel);
  tone(buzzPin, NOTE_G4, 60, buzzerChannel);
  noTone(buzzPin, buzzerChannel);
  tone(buzzPin, NOTE_C5, 250, buzzerChannel);
  noTone(buzzPin, buzzerChannel);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.display();
  
  dht.begin();
  RTC.begin();

  // load last config from flash
  preferences.begin("hydrotek", false);
  #ifdef PREFERENCES_CLEAR
    preferences.clear();
  #endif
  flashRead();

  if (! RTC.isrunning()) {
    // following line sets the RTC to the date & time this sketch was compiled
    RTC.adjust(DateTime(__DATE__, __TIME__));
  }
  #ifdef SET_RTC_MANUAL
    RTC.adjust(DateTime(__DATE__, __TIME__));
  #endif

}

bool flowLastState = false;
int long unsigned lastSensorUpdate = 0;
bool wifiConnOnce = false; // track whether wifi ever connected
bool calledWifiBegin = false; //track whether WiFi.begin() was called (only do this once, but we don't do this in setup() as we need to wait a while for the stack to be ready
void loop () {

  unsigned int long curMillis = millis();

  if (curMillis - screenSleepLastAction > screenSleepTime * 60 * 1000) {
    screenIsSleeping = true;
  }

  if (configEnableWifi.boolVal()) {
    if (!wifiConnOnce) {
      if (curMillis > 1000 * 60 * 2) { // if WiFi hasn't connected after 2 minutes, hard-reset the ESP
          ESP.restart();
      }
    }

    if (!calledWifiBegin && millis() > 4000) {
      WiFi.begin(wifiSSID, wifiPassphrase);
      calledWifiBegin = true;
    }
  }

  //monitor for changes to state on the flow sensor inputs
  // this *should* be an interrupt but the loop runs fast enough that it isn't really an issue, and in this case the accuracy of the flow sensor was already poor
  if (configFlowEnable.boolVal()) {
    if (digitalRead(flowPin) != flowLastState) {
      flowLastState = !flowLastState;
      if (flowLastState) { //only increment on a HIGH reading
        if (calibratingFlowSensor) {
          flowSensorCalibrationPulses++;
        }
        flowPulseCount++;
      }
    }
  }

  //OK button down
  buttonOK.checkState(curMillis);
  buttonBK.checkState(curMillis);
  buttonUP.checkState(curMillis);
  buttonDN.checkState(curMillis);

  //update sensors and display every x ms
  if (curMillis - lastSensorUpdate > LOOP_SPEED || lastSensorUpdate == 0) {
    lastSensorUpdate = curMillis;

    //check the WiFi status
    if (configEnableWifi.boolVal()) {
      if (calledWifiBegin && WiFi.status() == WL_CONNECTED) {
        wifiConnected = true;
        if (!wifiConnOnce) {
          wifiConnOnce = true;
        }
      } else {
        wifiConnected = false;
      }
    }

    DateTime now = RTC.now(); 

    //get temp and humidity readings
    tempCheckState();

    //check float sensor state
    floatCheckState();

    //update lamp states
    lampCheckState(now.hour());

    //update flow readings
    flowCheckState();

    //update pump states
    pumpCheckState(now);

    //sound any alarms
    alarmCheckState();

    //write to the display
    displayUpdate(now, curMillis);

    //upload to server
    if (configEnableWifi.boolVal()){
      uploadData(curMillis);
    }

  }

}
