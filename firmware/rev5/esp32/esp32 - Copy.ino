/*
 * 
 * HydroTek hydroponics controller - tim eastwood
 * 
 * See https://github.com/Energiz3r/HydroTek for information and instructions
 * 
 */

// PIN ASSIGNMENTS - to suit ESP32 "D1" WeMos D1 Mini compatible
#define floatSw1Pin 5
#define floatSw2Pin 23
#define dht1Pin 16
#define dht2Pin 17
#define flow1Pin 25
#define flow2Pin 32
#define lamp1Pin 19
#define lamp2Pin 18
#define pump1Pin 26
#define pump2Pin 36

#define ledPin 33
#define buzzPin 35
#define buttonPin 39
// END PIN ASSIGNMENTS

#define webEndpoint "http://noobs.wtf/plants/endpoint.php" //the php script that the ESP8266 will connect to for uploading data
#define lampHeaterTempVariance 1.5 // how much to allow the temp to vary from the desired setting before switching the state of the heater (lamp)
#define postIdentifierString "user=" webUsername "&password=" webPassword "&device=" deviceId "&"
#define buttonShortTime 30 //ms
#define buttonLongTime 250 //ms
#define flowPulsesPerMl 0.45 //flow sensor pulses per mL of liquid (YF-S201 sensor)
// #define SET_RTC_MANUAL // uncomment to force setting the RTC time to the sketch compile time

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
DHT dht1(dht1Pin, DHT22); //pin, type
DHT dht2(dht2Pin, DHT22);

// RTC includes
#include <RTClib.h>
RTC_DS1307 RTC;

// Preferences (EEPROM) includes
#include <Preferences.h>
Preferences preferences;

// JSON includes
#include <ArduinoJson.h>

String webUsername = "";
String webPassword = "";
String webDeviceID = "";
char* wifiSSID = "";
char* wifiPassphrase =  "";

//the below values are loaded from the web portal
int configUploadFrequencySec = 0;
//temp 1
bool configTemp1Enable = false;
bool configTemp1LampShutoff = false;
float configTemp1LampShutoffTemp = 0.0;
bool configTemp1HighTempAlarm = false;
float configTemp1HighTemp = 0.0;
bool configTemp1LowTempAlarm = false;
float configTemp1LowTemp = 0.0;
//temp 2
bool configTemp2Enable = false;
bool configTemp2LampShutoff = false;
float configTemp2LampShutoffTemp = 0.0;
bool configTemp2HighTempAlarm = false;
float configTemp2HighTemp = 0.0;
bool configTemp2LowTempAlarm = false;
float configTemp2LowTemp = 0.0;
//lamp 1
bool configLamp1Enable = false;
bool configLamp1HeaterMode = false;
float configLamp1HeaterTemp = 0.0;
int configLamp1StartHour = 0;
int configLamp1EndHour = 0;
bool configLamp1InvertLogic = false;
//lamp 2
bool configLamp2Enable = false;
bool configLamp2HeaterMode = false;
float configLamp2HeaterTemp = 0.0;
int configLamp2StartHour = 0;
int configLamp2EndHour = 0;
bool configLamp2InvertLogic = false;
//pump 1
bool configPump1Enable = false;
bool configPump1FlowMode = false;
int configPump1FlowMl = 0;
int configPump1FlowAlarm = false;
int configPump1DurationSec = 0;
int configPump1FrequencyMin = 0;
bool configPump1InvertLogic = false;
//pump 2
bool configPump2Enable = false;
bool configPump2FlowMode = false;
int configPump2FlowMl = 0;
int configPump1FlowAlarm = true;
int configPump2DurationSec = 0;
int configPump2FrequencyMin = 0;
bool configPump2InvertLogic = false;
//float 1
bool configFloat1Enable = false;
bool configFloat1Alarm = false;
bool configFloat1PumpShutoff = false;
bool configFloat1InvertLogic = false;
//float 2
bool configFloat2Enable = false;
bool configFloat2Alarm = false;
bool configFloat2PumpShutoff = false;
bool configFloat2InvertLogic = false;
//flow 1
bool configFlow1Enable = false;
//flow 2
bool configFlow2Enable = false;

void webConfigRead() {
  webUsername = preferences.getString("webUser", "");
  webPassword = preferences.getString("webPass", "");
  webDeviceID = preferences.getString("webDevice", "");
  wifiSSID = preferences.getChar("wifiSSID", "");
  wifiPassphrase =  preferences.getChar("wifiPass", "");
}
void webConfigWrite() {
  preferences.putString("webUser", webUsername);
  preferences.putString("webPass", webPassword);
  preferences.putString("webDevice", webDeviceID);
  preferences.putChar("wifiSSID", wifiSSID);
  preferences.putChar("wifiPass", wifiPassphrase);
}

void flashRead() {
  configUploadFrequencySec            = preferences.getUInt("uploadFreq", 0);
  //temp 1
  configTemp1Enable             = preferences.getBool("t1enable", false);
  configTemp1LampShutoff        = preferences.getBool("t1lampshutoff", false);
  configTemp1LampShutoffTemp    = preferences.getFloat("t1shutofftemp", 0.0);
  configTemp1HighTempAlarm      = preferences.getBool("t1highalarm", false);
  configTemp1HighTemp           = preferences.getFloat("t1hightemp", 0.0);
  configTemp1LowTempAlarm       = preferences.getBool("t1lowalarm", false);
  configTemp1LowTemp            = preferences.getFloat("t1lowtemp", 0.0);
  //temp 2
  configTemp2Enable             = preferences.getBool("t2enable", false);
  configTemp2LampShutoff        = preferences.getBool("t2lampshutoff", false);
  configTemp2LampShutoffTemp    = preferences.getFloat("t2shutofftemp", 0.0);
  configTemp2HighTempAlarm      = preferences.getBool("t2highalarm", false);
  configTemp2HighTemp           = preferences.getFloat("t2hightemp", 0.0);
  configTemp2LowTempAlarm       = preferences.getBool("t2lowalarm", false);
  configTemp2LowTemp            = preferences.getFloat("t2lowtemp", 0.0);
  //lamp 1
  configLamp1Enable             = preferences.getBool("l1enable", false);
  configLamp1HeaterMode         = preferences.getBool("l1heater", false);
  configLamp1HeaterTemp         = preferences.getFloat("l1temp", 0.0);
  configLamp1StartHour          = preferences.getUInt("l1starthour", 0);
  configLamp1EndHour            = preferences.getUInt("l1endhour", 0);
  configLamp1InvertLogic        = preferences.getBool("l1invert", false);
  //lamp 2
  configLamp2Enable             = preferences.getBool("l2enable", false);
  configLamp2HeaterMode         = preferences.getBool("l2heater", false);
  configLamp2HeaterTemp         = preferences.getFloat("l2temp", 0.0);
  configLamp2StartHour          = preferences.getUInt("l2starthour", 0);
  configLamp2EndHour            = preferences.getUInt("l2endhour", 0);
  configLamp2InvertLogic        = preferences.getBool("l2invert", false);
  //pump 1
  configPump1Enable             = preferences.getBool("p1enable", false);
  configPump1FlowMode           = preferences.getBool("p1flow", false);
  configPump1FlowMl             = preferences.getUInt("p1flowml", 0);
  configPump1FlowAlarm          = preferences.getBool("p1flowalarm", false);
  configPump1DurationSec        = preferences.getUInt("p1duration", 0);
  configPump1FrequencyMin       = preferences.getUInt("p1frequency", 0);
  configPump1InvertLogic        = preferences.getBool("p1invert", false);
  //pump 2
  configPump2Enable             = preferences.getBool("p2enable", false);
  configPump2FlowMode           = preferences.getBool("p2flow", false);
  configPump2FlowMl             = preferences.getUInt("p2flowml", 0);
  configPump2FlowAlarm          = preferences.getBool("p2flowalarm", false);
  configPump2DurationSec        = preferences.getUInt("p2duration", 0);
  configPump2FrequencyMin       = preferences.getUInt("p2frequency", 0);
  configPump2InvertLogic        = preferences.getBool("p2invert", false);
  //float 1
  configFloat1Enable            = preferences.getBool("flt1enable", false);
  configFloat1Alarm             = preferences.getBool("flt1alarm", false);
  configFloat1PumpShutoff       = preferences.getBool("flt1shutoff", false);
  configFloat1InvertLogic       = preferences.getBool("flt1invert", false);
  //float 2
  configFloat2Enable            = preferences.getBool("flt2enable", false);
  configFloat2Alarm             = preferences.getBool("flt2alarm", false);
  configFloat2PumpShutoff       = preferences.getBool("flt2shutoff", false);
  configFloat2InvertLogic       = preferences.getBool("flt2invert", false);
  //flow 1
  configFlow1Enable             = preferences.getBool("flw1enable", false);
  //flow 2
  configFlow2Enable             = preferences.getBool("flw2enable", false);
}

void flashWrite() {
  preferences.putUInt("uploadFreq", configUploadFrequencySec);
  //temp 1
  preferences.putBool("t1enable", configTemp1Enable);
  preferences.putBool("t1lampshutoff", configTemp1LampShutoff);
  preferences.putFloat("t1shutofftemp", configTemp1LampShutoffTemp);
  preferences.putBool("t1highalarm", configTemp1HighTempAlarm);
  preferences.putFloat("t1hightemp", configTemp1HighTemp);
  preferences.putBool("t1lowalarm", configTemp1LowTempAlarm);
  preferences.putFloat("t1lowtemp", configTemp1LowTemp);
  //temp 2
  preferences.putBool("t2enable", configTemp2Enable);
  preferences.putBool("t2lampshutoff", configTemp2LampShutoff);
  preferences.putFloat("t2shutofftemp", configTemp2LampShutoffTemp);
  preferences.putBool("t2highalarm", configTemp2HighTempAlarm);
  preferences.putFloat("t2hightemp", configTemp2HighTemp);
  preferences.putBool("t2lowalarm", configTemp2LowTempAlarm);
  preferences.putFloat("t2lowtemp", configTemp2LowTemp);
  //lamp 1
  preferences.putBool("l1enable", configLamp1Enable);
  preferences.putBool("l1heater", configLamp1HeaterMode);
  preferences.putFloat("l1temp", configLamp1HeaterTemp);
  preferences.putUInt("l1starthour", configLamp1StartHour);
  preferences.putUInt("l1endhour", configLamp1EndHour);
  preferences.putBool("l1invert", configLamp1InvertLogic);
  //lamp 2
  preferences.putBool("l2enable", configLamp2Enable);
  preferences.putBool("l2heater", configLamp2HeaterMode);
  preferences.putFloat("l2temp", configLamp2HeaterTemp);
  preferences.putUInt("l2starthour", configLamp2StartHour);
  preferences.putUInt("l2endhour", configLamp2EndHour);
  preferences.putBool("l2invert", configLamp2InvertLogic);
  //pump 1
  preferences.putBool("p1enable", configPump1Enable);
  preferences.putBool("p1flow", configPump1FlowMode);
  preferences.putUInt("p1flowml", configPump1FlowMl);
  preferences.putBool("p1flowalarm", configPump1FlowAlarm);
  preferences.putUInt("p1duration", configPump1DurationSec);
  preferences.putUInt("p1frequency", configPump1FrequencyMin);
  preferences.putBool("p1invert", configPump1InvertLogic);
  //pump 2
  preferences.putBool("p2enable", configPump2Enable);
  preferences.putBool("p2flow", configPump2FlowMode);
  preferences.putUInt("p2flowml", configPump2FlowMl);
  preferences.putBool("p2flowalarm", configPump2FlowAlarm);
  preferences.putUInt("p2duration", configPump2DurationSec);
  preferences.putUInt("p2frequency", configPump2FrequencyMin);
  preferences.putBool("p2invert", configPump2InvertLogic);
  //float 1
  preferences.putBool("flt1enable", configFloat1Enable);
  preferences.putBool("flt1alarm", configFloat1Alarm);
  preferences.putBool("flt1shutoff", configFloat1PumpShutoff);
  preferences.putBool("flt1invert", configFloat1InvertLogic);
  //float 2
  preferences.putBool("flt2enable", configFloat2Enable);
  preferences.putBool("flt2alarm", configFloat2Alarm);
  preferences.putBool("flt2shutoff", configFloat2PumpShutoff);
  preferences.putBool("flt2invert", configFloat2InvertLogic);
  //flow 1
  preferences.putBool("flw1enable", configFlow1Enable);
  //flow 2
  preferences.putBool("flw2enable", configFlow2Enable);
}

//update config from online
String webError = '';
void handleReceiveConfig(String webResponse) {

  const size_t capacity = JSON_OBJECT_SIZE(51) + 1210;
  DynamicJsonDocument doc(capacity);

  deserializeJson(doc, json);

  //check if parsing the JSON failed
  JsonObject object = doc.as<JsonObject>();
  if (object.isNull()) {
    webError = 'json:';
    webError += webResponse;
    return
  }

  configUploadFrequencySec = doc["configUploadFrequencySec"];

  configTemp1Enable = doc["configTemp1Enable"];
  configTemp1LampShutoff = doc["configTemp1LampShutoff"];
  configTemp1LampShutoffTemp = doc["configTemp1LampShutoffTemp"];
  configTemp1HighTempAlarm = doc["configTemp1HighTempAlarm"];
  configTemp1HighTemp = doc["configTemp1HighTemp"];
  configTemp1LowTempAlarm = doc["configTemp1LowTempAlarm"];
  configTemp1LowTemp = doc["configTemp1LowTemp"];

  configTemp2Enable = doc["configTemp2Enable"];
  configTemp2LampShutoff = doc["configTemp2LampShutoff"];
  configTemp2LampShutoffTemp = doc["configTemp2LampShutoffTemp"];
  configTemp2HighTempAlarm = doc["configTemp2HighTempAlarm"];
  configTemp2HighTemp = doc["configTemp2HighTemp"];
  configTemp2LowTempAlarm = doc["configTemp2LowTempAlarm"];
  configTemp2LowTemp = doc["configTemp2LowTemp"];

  configLamp1Enable = doc["configLamp1Enable"];
  configLamp1HeaterMode = doc["configLamp1HeaterMode"];
  configLamp1HeaterTemp = doc["configLamp1HeaterTemp"];
  configLamp1StartHour = doc["configLamp1StartHour"];
  configLamp1EndHour = doc["configLamp1EndHour"];
  configLamp1InvertLogic = doc["configLamp1InvertLogic"];

  configLamp2Enable = doc["configLamp2Enable"];
  configLamp2HeaterMode = doc["configLamp2HeaterMode"];
  configLamp2HeaterTemp = doc["configLamp2HeaterTemp"];
  configLamp2StartHour = doc["configLamp2StartHour"];
  configLamp2EndHour = doc["configLamp2EndHour"];
  configLamp2InvertLogic = doc["configLamp2InvertLogic"];

  configPump1Enable = doc["configPump1Enable"];
  configPump1FlowMode = doc["configPump1FlowMode"];
  configPump1FlowMl = doc["configPump1FlowMl"];
  configPump1FlowAlarm = doc["configPump1FlowAlarm"];
  configPump1DurationSec = doc["configPump1DurationSec"];
  configPump1FrequencyMin = doc["configPump1FrequencyMin"];
  configPump1InvertLogic = doc["configPump1InvertLogic"];

  configPump2Enable = doc["configPump2Enable"];
  configPump2FlowMode = doc["configPump2FlowMode"];
  configPump2FlowMl = doc["configPump2FlowMl"];
  configPump2FlowAlarm = doc["configPump2FlowAlarm"];
  configPump2DurationSec = doc["configPump2DurationSec"];
  configPump2FrequencyMin = doc["configPump2FrequencyMin"];
  configPump2InvertLogic = doc["configPump2InvertLogic"];

  configFloat1Enable = doc["configFloat1Enable"];
  configFloat1Alarm = doc["configFloat1Alarm"];
  configFloat1PumpShutoff = doc["configFloat1PumpShutoff"];
  configFloat1InvertLogic = doc["configFloat1InvertLogic"];

  configFloat2Enable = doc["configFloat2Enable"];
  configFloat2Alarm = doc["configFloat2Alarm"];
  configFloat2PumpShutoff = doc["configFloat2PumpShutoff"];
  configFloat2InvertLogic = doc["configFloat2InvertLogic"];

  configFlow1Enable = doc["configFlow1Enable"];
  configFlow2Enable = doc["configFlow2Enable"];

  flashWrite();
}

#define firstCoordinate 0
#define charHeight 5
#define titleBarHeight 12
#define lineX 1
int lineY(int line) { 
  if (line == 1) { return firstCoordinate + titleBarHeight + 1 }
  if (line == 2) { return lineY(1) + charHeight }
  if (line > 2) { return lineY(line - 1) + charHeight }
}

//draws the border and title etc
bool wifiConnected = false;
void drawInitial() {
  display.clearDisplay();
  display.drawRoundRect(0,0,display.width(),display.height(), 2, WHITE); //x, y, x, y, radius, color
  display.fillRoundRect(0,0,display.width(),10, 2, WHITE); //x, y, x, y, radius, color
  display.setTextColor(BLACK); // Draw black text (background transparent by default)
  display.setCursor(30, 2); //title line
  display.print("HydroTek rev5");
  display.setCursor(30, 118); //title line
  if (wifiConnected) { display.print("+"); } else { display.print("-"); }
  display.setTextColor(WHITE, BLACK); // Draw white text with a black background 
}

float temp1;
float temp2;
float humidity1;
float humidity2;
void tempCheckState() {
  if (configTemp1Enable) {
    temp1 = dht1.readTemperature(); //read temperature as Celsius (the default)
    humidity1 = dht1.readHumidity(); //read humidity
  }
  if (configTemp2Enable) {
    temp2 = dht2.readTemperature();
    humidity2 = dht2.readHumidity();
  }
}

//turn lamp relay(s) on / off
void lampSetState (int lampNum, String stateStr) {
  bool state = stateStr == "ON";
  int lampPin = (lampNum == 1 ? lamp1Pin : lamp2Pin);
  bool invertLogic = (lampNum == 1 ? configLamp1InvertLogic : configLamp2InvertLogic);
  bool tempOverride = false;
  if (!configLamp1HeaterMode && configTemp1Enable && configTemp1LampShutoff && temp1 > configTemp1LampShutoffTemp) {
    if (lampNum == 1) {
      tempOverride = true;
    }
  }
  if (!configLamp2HeaterMode && configTemp2Enable && configTemp2LampShutoff && temp2 > configTemp2LampShutoffTemp) {
    if (lampNum == 2) {
      tempOverride = true;
    }
  }
  if (!invertLogic) {
    if (state && !tempOverride) {
      digitalWrite(lampPin, LOW);
    } else {
      digitalWrite(lampPin, HIGH);
    }
  } else {
    if (state && !tempOverride) {
      digitalWrite(lampPin, LOW);
    } else {
      digitalWrite(lampPin, HIGH);
    }
  }
}

void lampCheckState (int currentHour) {
  // lamp 1
  if (configLamp1Enable) {
    if (configLamp1HeaterMode) {
      if (configTemp1Enable) {
        if (temp1 > configLamp1HeaterTemp + lampHeaterTempVariance) {
          lampSetState(1, "OFF");
        } else if (temp1 < configLamp1HeaterTemp - lampHeaterTempVariance) {
          lampSetState(1, "ON");
        }
      }
    } else {
      if (configLamp1EndHour > configLamp1StartHour) {
        if (true || currentHour < configLamp1EndHour && currentHour >= configLamp1StartHour) {
          lampSetState(1, "ON");
        } else {
          lampSetState(1, "OFF");
        }
      }
      else if (configLamp1EndHour < configLamp1StartHour) {
        if (currentHour >= configLamp1StartHour || currentHour < configLamp1EndHour) {
          lampSetState(1, "ON");
        } else {
          lampSetState(1, "OFF");
        }
      }
    }
  }
  // lamp 2
  if (configLamp2Enable) {
    if (configLamp2HeaterMode) {
      if (configTemp2Enable) {
        if (temp2 > configLamp2HeaterTemp + lampHeaterTempVariance) {
          lampSetState(2, "OFF");
        } else if (temp2 < configLamp2HeaterTemp - lampHeaterTempVariance) {
          lampSetState(2, "ON");
        }
      }
    } else {
      if (configLamp2EndHour > configLamp2StartHour) {
        if (currentHour < configLamp2EndHour && currentHour >= configLamp2StartHour) {
          lampSetState(2, "ON");
        } else {
          lampSetState(2, "OFF");
        }
      }
      else if (configLamp2EndHour < configLamp2StartHour) {
        if (currentHour >= configLamp2StartHour || currentHour < configLamp2EndHour) {
          lampSetState(2, "ON");
        } else {
          lampSetState(2, "OFF");
        }
      }
    }
  }
}


//sounds alarm if conditions are met
bool pump1MissedFlowTarget = false;
bool pump2MissedFlowTarget = false;
String alarmReason = '';
String alarmCheckState(){
  bool shouldSoundAlarm = false;
  alarmReason = '';
  if (configTemp1Enable && configTemp1HighTempAlarm && temp1 > configTemp1HighTemp) {
    shouldSoundAlarm = true;
    alarmReason = "Temp 1 HIGH";
  }
  if (configTemp2Enable && configTemp2HighTempAlarm && temp2 > configTemp2HighTemp) {
    shouldSoundAlarm = true;
    alarmReason = "Temp 2 HIGH";
  }
  if (configPump1Enable && configFlow1Enable && configPump1FlowMode && flow1LastVolume < configPump1FlowMl) {
    shouldSoundAlarm = true;
    alarmReason = "Pump 1 NO FLOW";
  }
  if (configPump2Enable && configFlow2Enable && configPump2FlowMode && flow2LastVolume < configPump2FlowMl) {
    shouldSoundAlarm = true;
    alarmReason = "Pump 2 NO FLOW";
  }
  if (configFloat1Enable && configFloat1Alarm) {
    shouldSoundAlarm = true;
    alarmReason = "Tank 1 FULL/EMPTY";
  }
  if (configFloat2Enable && configFloat2Alarm) {
    shouldSoundAlarm = true;
    alarmReason = "Tank 2 FULL/EMPTY";
  }
  if (pump1MissedFlowTarget) {
    shouldSoundAlarm = true;
    alarmReason = "Pump 1 LOW FLOW";
  }
  if (pump2MissedFlowTarget) {
    shouldSoundAlarm = true;
    alarmReason = "Pump 2 LOW FLOW";
  }
  if (shouldSoundAlarm) {
    tone(buzzPin, 800, 500);
  }
}

void pumpSetState(int pump, String state) {
  if (pump == 1) {
    if (!configPump1InvertLogic && state == "ON") {
      digitalWrite(pump1Pin, LOW);
    } else if (!configPump1InvertLogic && state == "OFF") {
      digitalWrite(pump1Pin, HIGH);
    } else if (configPump1InvertLogic && state == "ON") {
      digitalWrite(pump1Pin, HIGH);
    } else if (configPump1InvertLogic && state == "OFF") {
      digitalWrite(pump1Pin, LOW);
    }
  } else if (pump == 2) {
    if (!configPump2InvertLogic && state == "ON") {
      digitalWrite(pump2Pin, LOW);
    } else if (!configPump2InvertLogic && state == "OFF") {
      digitalWrite(pump2Pin, HIGH);
    } else if (configPump2InvertLogic && state == "ON") {
      digitalWrite(pump2Pin, HIGH);
    } else if (configPump2InvertLogic && state == "OFF") {
      digitalWrite(pump2Pin, LOW);
    }
  }
}

unsigned int pump1MeasuredFlowMl = 0;
int pump1MissedTargetByMl = 0;
unsigned int pump1MeasuredFlowMl = 0;
int pump2MissedTargetByMl = 0;
void pumpCheckState(DateTime now) {
  currentSecond = now.hour() * 60 * 60 + now.minute() * 60 + now.second();
  //pump1
  if (configPump1Enable) {
    bool pumpShouldRun = false;
    for (int i = 0; i <= 86400; i = i + configPump1FrequencyMin * 60) {
      if (currentSecond >= i && currentSecond <= i + configPump1DurationSec) {
        if (configPump1FlowMode) {
          if (pump1MeasuredFlowMl < configPump1FlowMl) {
            pumpShouldRun = true;
          }
          //if it's within the last second of run time and the flow hasn't met the target by more than 5ml, consider the flow target not met
          if (currentSecond > i + configPump1DurationSec - 2 && configPump1FlowMl - pump1MeasuredFlowMl > 5) {
            pump1MissedFlowTarget = true;
            pump1MissedTargetByMl = configPump1FlowMl - pump1MeasuredFlowMl;
          }
        } else {
          pumpShouldRun = true;
        }
      }
    }
    if (pumpShouldRun) {
      pumpSetState(1, "ON");
    } else {
      pumpSetState(1, "OFF");
      pump1MeasuredFlowMl = 0;
    }
  }
  //pump2
  if (configPump2Enable) {
    bool pumpShouldRun = false;
    for (int i = 0; i <= 86400; i = i + configPump2FrequencyMin * 60) {
      if (currentSecond >= i && currentSecond <= i + configPump2DurationSec) {
        if (configPump2FlowMode) {
          if (pump2MeasuredFlowMl < configPump2FlowMl) {
            pumpShouldRun = true;
          }
          //if it's within the last second of run time and the flow hasn't met the target by more than 5ml, consider the flow target not met
          if (currentSecond > i + configPump2DurationSec - 2 && configPump2FlowMl - pump2MeasuredFlowMl > 5) {
            pump2MissedFlowTarget = true;
            pump2MissedTargetByMl = configPump2FlowMl - pump2MeasuredFlowMl;
          }
        } else {
          pumpShouldRun = true;
        }
      }
    }
    if (pumpShouldRun) {
      pumpSetState(1, "ON");
    } else {
      pumpSetState(1, "OFF");
      pump2MeasuredFlowMl = 0;
    }
  }
}

unsigned int flow1PulseCount = 0;
unsigned int flow1MlSinceLastUpload = 0;
unsigned int flow2PulseCount = 0;
unsigned int flow2MlSinceLastUpload = 0;
void flowCheckState(){
  if (configFlow1Enable) {
    int flowInMl = flow1PulseCount / flowPulsesPerMl;
    pump1MeasuredFlowMl = pump1MeasuredFlowMl + flowInMl;
    flow1MlSinceLastUpload = flow1MlSinceLastUpload + flowInMl;
    flow1PulseCount = 0;
  }
  if (configFlow2Enable) {
    int flowInMl = flow2PulseCount / flowPulsesPerMl;
    pump2MeasuredFlowMl = pump2MeasuredFlowMl + flowInMl;
    flow2MlSinceLastUpload = flow2MlSinceLastUpload + flowInMl;
    flow2PulseCount = 0;
  }
  //(pulsesOver1000ms * 60 / 7.5); //the LPH calculation for flow sensors
}

bool inMenu = false;
int menuPosition = 0;
int subMenuPosition = 0;
void onButtonPress(bool longPress) {
  if (!inMenu && longPress) {
    inMenu = true;
    menuPosition = 0;
    subMenuPosition = 0;
  }
  if (inMenu && longPress) {
    inMenu = false;
  }
}

bool float1triggered = false;
bool float2triggered = false;
void floatCheckState() {
  //get float switch statuses
  if (configFloat1Enable) {
    if (configFloat1InvertLogic) {
      float1triggered = digitalRead(floatSw1Pin);
    } else {
      float1triggered = !digitalRead(floatSw1Pin);
    }
  }
  if (configFloat2Enable) {
    if (configFloat2InvertLogic) {
      float2triggered = digitalRead(floatSw2Pin);
    } else {
      float2triggered = !digitalRead(floatSw2Pin);
    }
  }
}

#define numPages 2
byte page = 1; //which page the loop is on
#define updateFrequency 3000 //seconds to display each page before moving to the next
unsigned int long timeSinceLastPage = 0; //track when page was last changed
void displayUpdate(DateTime now, String alarmReason, int curMillis) {
  if (!timeSinceLastPage == 0 && curMillis - timeSinceLastPage < updateFrequency ) {
    return
  } else {
    timeSinceLastPage = curMillis;
  }
  //update the display
  drawInitial();
  //display.setTextSize(2);
  display.setCursor(lineX, lineY(1));
  if (page == 1) {
    String line = "Temp 1: ";
    line += temp1;
    line += (char)247;
    line += "C";
    display.print(line); 
    display.setCursor(lineX, lineY(2));
    line = "Temp 2: ";
    line += temp2;
    line += (char)247;
    line += "C";
    display.print(line); 
    display.setCursor(lineX, lineY(3));
    String line = "Hum. 1: ";
    line += humidity1;
    line += "%";
    display.print(line); 
    display.setCursor(lineX, lineY(4));
    line = "Hum. 2: ";
    line += humidity2;
    line += "%";
    display.print(line); 
  }
  else if (page == 2) {
    String date; date += now.day(); date += '/'; date += now.month(); date += '/'; date += now.year();
    display.print(date);
    display.setCursor(lineX, lineY(2));
    String curTime; curTime += now.hour(); curTime += ':'; curTime += now.minute();
    display.print(curTime);
    display.setCursor(lineX, lineY(3));
    String line = "Flow 1: ";
    line += flow1MlSinceLastUpload;
    line += "mL";
    display.print(line); 
    display.setCursor(lineX, lineY(4));
    line = "Flow 2: ";
    line += flow2MlSinceLastUpload;
    line += "mL";
    display.print(line); 
  }
  display.setCursor(lineX, lineY(5));
  display.print(alarmReason); 
  display.setCursor(lineX, lineY(6));
  display.print(webError); 
  display.display();
  page++;
  if (page > numPages) {
    page = 1;
  }
  webError = '';
}

unsigned int long lastDataUpload = 0;
String uploadData(int curMillis) {

  String webResponse = '';
  if (wifiConnected && curMillis - lastDataUpload > configUploadFrequencySec * 1000) {

    String uploadData = "t1="; uploadData += temp1;
    uploadData += "&t2="; uploadData += temp2;
    uploadData += "&h1="; uploadData += humidity1;
    uploadData += "&h2="; uploadData += humidity2;
    uploadData += "&f1="; uploadData += float1triggered ? "1" : "0";
    uploadData += "&f2="; uploadData += float2triggered ? "1" : "0";
    uploadData += "&l1="; uploadData += flow1MlSinceLastUpload;
    uploadData += "&l2="; uploadData += flow2MlSinceLastUpload;
    uploadData += "&a="; uploadData += alarmReason;

    flow1MlSinceLastUpload = 0; //reset vars
    flow2MlSinceLastUpload = 0;

    HTTPClient http;
    if(http.begin(webEndpoint)){
      http.addHeader("Content-Type", "application/x-www-form-urlencoded", false, true);
      
      String postString = postIdentifierString + uploadData;
      int httpCode = http.POST(postString);
      if (httpCode > 0) { // httpCode will be negative on error
        String response = "OK";
        response += (String)httpCode;
        response += ", ";
        response += http.getString();
        response.replace('\n', ' ');
        webResponse = response;
      } else {
        String response = "ERROR";
        response += (String)httpCode;
        response += ", ";
        response += http.errorToString(httpCode).c_str();
        response.replace('\n', ' ');
        webResponse = response;
      }
      http.end();
    } else {
      webResponse = "Connection Error";
    }
    //update sensors
    flow1SinceLastUpdate = 0;
    flow2SinceLastUpdate = 0;
    lastDataUpload = curMillis;
  }
  handleReceiveConfig(webResponse);
  alarmReason = '';
}

void setup() {

  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(floatSw1Pin, INPUT_PULLUP);
  pinMode(floatSw2Pin, INPUT_PULLUP);
  pinMode(lamp1Pin, OUTPUT);
  pinMode(lamp2Pin, OUTPUT);
  pinMode(pump1Pin, OUTPUT);
  pinMode(pump2Pin, OUTPUT);
  pinMode(flow1Pin, INPUT_PULLUP);
  pinMode(flow2Pin, INPUT_PULLUP);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.display();

  Serial.begin(115200); //pour a bowl of Serial
  dht1.begin();
  dht2.begin(); //initilize the DHT22 sensors
  RTC.begin();

  // load last config from flash
  preferences.begin("hydrotek", false);
  flashRead();

  if (! RTC.isrunning()) {
    // following line sets the RTC to the date & time this sketch was compiled
    RTC.adjust(DateTime(__DATE__, __TIME__));
  }
  #ifdef SET_RTC_MANUAL
    RTC.adjust(DateTime(__DATE__, __TIME__));
  #endif

  delay(4000);   //Delay needed before calling WiFi.begin
  WiFi.begin(wifiSSID, wifiPassphrase);

  tone(buzzPin, 600, 100);
  delay(100);
  tone(buzzPin, 800, 100);
  delay(100);
  tone(buzzPin, 1000, 100);

}

bool flow1LastState = false;
bool flow2LastState = false;
bool buttonLastState = false;
int long unsigned buttonStartTime = 0;
int long unsigned lastSensorUpdate = 0;
void loop () {

  curMillis = millis();

  //monitor for changes to state on the flow sensor inputs
  // this *should* be an interrupt but the loop runs fast enough that it isn't really an issue, and in this case the accuracy of the flow sensor was already poor
  if (configFlow1Enable) {
    if (digitalRead(flow1Pin) != flow1LastState) {
      flow1LastState = !flow1LastState;
      flow1PulseCount++;
    }
  }
  if (configFlow2Enable) {
    if (digitalRead(flow2Pin) != flow2LastState) {
      flow2LastState = !flow2LastState;
      flow2PulseCount++;
    }
  }

  //button down
  if (digitalRead(buttonPin) == LOW && !buttonLastState) {
    buttonStartTime = curMillis;
    buttonLastState = true;
  }
  //button released
  if (digitalRead(buttonPin) == HIGH && buttonLastState) {
    if (curMillis - buttonStartTime > buttonShortTime) {
      if (curMillis - buttonStartTime > buttonLongTime) {
        tone(buzzPin, 800, 250);
        onButtonPress(true);
      } else {
        tone(buzzPin, 800, 50);
        onButtonPress(false);
      }
    }
    buttonLastState = false;
  }

  //update sensors and display every x ms
  if (curMillis - lastSensorUpdate > 1000 || lastSensorUpdate == 0) {
    lastSensorUpdate = curMillis;

    //check the WiFi status
    if (WiFi.status() == WL_CONNECTED) {
      wifiConnected = true;
    } else {
      wifiConnected = false;
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
    String alarmReason = alarmCheckState();

    //write to the display
    displayUpdate(now, alarmReason, curMillis);

    //upload to server
    uploadData(now, curMillis);

  }
}

