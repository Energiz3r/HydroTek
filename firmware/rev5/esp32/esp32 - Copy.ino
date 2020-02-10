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

String webUsername = "";
String webPassword = "";
String webDeviceID = "";
char* wifiSSID = "";
char* wifiPassphrase =  "";

//the below values are the configuration for the device which is loaded from the web portal

int uploadFrequencySec = 0;
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
  uploadFrequencySec            = preferences.getUInt("uploadFreq", 0);
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
  preferences.putUInt("uploadFreq", uploadFrequencySec);
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

//draws the border and title
void drawInitial() {
  display.clearDisplay();
  display.drawRoundRect(0,0,display.width(),display.height(), 2, WHITE); //x, y, x, y, radius, color
  display.fillRoundRect(0,0,display.width(),10, 2, WHITE); //x, y, x, y, radius, color
  display.setTextColor(BLACK); // Draw black text (background transparent by default)
  display.setCursor(30, 2); //title line
  display.print("HydroTek rev5");
  display.setTextColor(WHITE, BLACK); // Draw white text with a black background 
}

//turn lamp relay(s) on / off
void lampSetState (int lampNum, String stateStr) {
  bool state = stateStr == "ON";
  int lampPin = (lampNum == 1 ? lamp1Pin : lamp2Pin);
  bool invertLogic = (lampNum == 1 ? configLamp1InvertLogic : configLamp2InvertLogic);
  bool tempOverride = false;
  if (configTemp1Enable && configTemp1LampShutoff && TEMP1 > configTemp1LampShutoffTemp) {
    if (lampNum == 1) {
      tempOverride = true;
    }
  }
  if (configTemp2Enable && configTemp2LampShutoff && TEMP2 > configTemp2LampShutoffTemp) {
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
        if (TEMP1 > configLamp1HeaterTemp + lampHeaterTempVariance) {
          lampSetState(1, "OFF");
        } else if (TEMP1 < configLamp1HeaterTemp - lampHeaterTempVariance) {
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
        if (TEMP2 > configLamp2HeaterTemp + lampHeaterTempVariance) {
          lampSetState(2, "OFF");
        } else if (TEMP2 < configLamp2HeaterTemp - lampHeaterTempVariance) {
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

bool wifiConnected = false;
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

  #ifdef RTC_SET
    if (! RTC.isrunning()) {
      // following line sets the RTC to the date & time this sketch was compiled
      RTC.adjust(DateTime(__DATE__, __TIME__));
    }
  #endif

  delay(4000);   //Delay needed before calling WiFi.begin
  WiFi.begin(wifiSSID, wifiPassphrase); 

}

bool flow1LastState = false;
bool flow2LastState = false;
int flow1PulseCount = 0;
int flow2PulseCount = 0;

void loop () {

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


  // flowLitresPerHour = (flow_frequency * 60 / 7.5); //the LPH calculation for flow sensors
}

