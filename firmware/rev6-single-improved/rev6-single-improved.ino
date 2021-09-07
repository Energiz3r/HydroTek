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

//#define PREFERENCES_CLEAR //deprecated. can be reset from the settings menu. uncomment to erase settings from flash
//#define SET_RTC_MANUAL // uncomment to force setting the RTC time to the sketch compile time
//#define offlineMode // deprecated. wifi can be disabled from the settings menu
//#define buttonDisable

#include <pitches.h>
#include <Tone32.h>

#include "menu-option.h"

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
#define lampHeaterTempVariance 0.5 // how much to allow the temp to vary from the desired setting before switching the state of the heater (lamp)
#define buttonShortTime 30 //ms
#define buttonLongTime 250 //ms
//#define flowPulsesPerMl 0.45 //flow sensor pulses per mL of liquid (YF-S201 sensor = 0.45) //deprecated. now an item in the settings menu
#define buzzerChannel 1 // specify the PWM channel to use for the buzzer
#define screenSleepTime 1 // how long to keep screen on for before sleeping (mins)
#define buttonDebounceTime 10 //ms

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

// JSON includes
#include <ArduinoJson.h>

// declare an array of menu option objects
#define NUM_MENU_OPTIONS 34
MenuOption menuOptions[NUM_MENU_OPTIONS] = {
  //(int pos (menu position), String title (description), String systemName (used for the 'preferences' library), String type (bool, int, or float), float defaultVal, float maxVal, float minVal, bool showInMenu, bool saveInFlash
  
  (1, "Upload frequency", "uploadFreq", "int", 30, 1, true, true),
  //temp
  (2, "Temp enable", "tenable", "bool", 0, 0.0, true, true),
  (3, "HI Temp lamp off", "tlampshutoff", "bool", 0, 0, true, true),
  (4, "Shutoff Temp degC", "tshutofftemp", "float", 60, 5, true, true),
  (5, "HI temp alarm", "thighalarm", "bool", 0, 0, true, true),
  (6, "HI Temp degC", "thightemp", "float", 60, 5, true, true),
  (7, "LO temp alarm", "tlowalarm", "bool", 0, 0, true, true),
  (8, "HI Temp degC", "tlowtemp", "float", 60, 5, true, true),
  //lamp
  (9, "Lamp enable", "lenable", "bool", 0, 0, true, true),
  (10, "Lamp is heater", "lheater", "bool", 0, 0, true, true),
  (11, "Heater temp degC", "ltemp", "float", 60, 5, true, true),
  (12, "Lamp on hour", "lstarthour", "int", 23, 0, true, true),
  (13, "Lamp off hour", "lendhour", "int", 23, 0, true, true),
  (14, "Lamp invert", "linvert", "bool", 0, 0, true, true),
  //pump
  (15, "Pump enable", "penable", "bool", 0, 0, true, true),
  (16, "Pump flow mode", "pflow", "bool", 0, 0, true, true),
  (17, "Pump flow Ml", "pflowml", "int", 5000, 0, true, true),
  (18, "Pump alarm", "pflowalarm", "bool", 0, 0, true, true),
  (19, "Pump duration sec", "pduration", "int", 240, 5, true, true),
  (20, "Pump freq. min", "pfrequency", "int", 240, 2, true, true),
  (21, "Pump invert", "pinvert", "bool", 0, 0, true, true),
  //float 1
  (22, "Float1 enable", "flt1enable", "bool", 0, 0, true, true),
  (23, "Float1 alarm", "flt1alarm", "bool", 0, 0, true, true),
  (24, "Float1 pump off", "flt1shutoff", "bool", 0, 0, true, true),
  (25, "Float1 invert", "flt1invert", "bool", 0, 0, true, true),
  //float 2
  (26, "Float2 enable", "flt2enable", "bool", 0, 0, true, true),
  (27, "Float2 alarm", "flt2alarm", "bool", 0, 0, true, true),
  (28, "Float2 pump off", "flt2shutoff", "bool", 0, 0, true, true),
  (29, "Float2 invert", "flt2invert", "bool", 0, 0, true, true),
  //flow
  (30, "Flow enable", "flwenable", "bool", 0, 0, true, true),
  (31, "Flow pulses per Ml", "flowpulsesml", "float", 1000, 0, true, true),
  //system
  (32, "Select display type", "displaytype", "int", 1, 0, true, true),
  (33, "Enable WiFi", "enablewifi", "bool", 0, 0, true, true),
  (34, "Reset Settings", "reset", "bool", 0, 0, true, false)
}

// return title
String getMenuOptionTitle(int menuPos) {
  for (int i = 0; i < NUM_MENU_OPTIONS; i ++) {
    if (menuOptions[i].pos() == menuPos) {
      return menuOptions[i].title();
    }
  }
  return "Menu Error";
}

// increment or decrement a menu option
void incrementMenuOption(int menuPos, bool increment) {
  for (int i = 0; i < NUM_MENU_OPTIONS; i ++) {
    if (menuOptions[i].pos() == menuPos) {
      if (menuPos == 34) { // reset all settings
        Serial.println("Erasing flash and rebooting..."); preferences.clear(); ESP.restart(); // delete everything in the flash and reboot the ESP to recreate the structure
      } else {
        menuOptions[i].adjustVal(increment);
      }
    }
  }
}

// returns value as a String (for use on the display)
const String stringEnabled = "Enabled";
const String stringDisabled = "Disabled"; // used this instead of a #define because I'm pretty sure this takes up less space in the sketch
String getMenuOptionValue(int menuPos) {
  for (int i = 0; i < NUM_MENU_OPTIONS; i ++) {
    if (menuOptions[i].pos() == menuPos) {
      if (menuPos == 34) { // reset all settings
        return "";
      } else {
        String type = menuOptions[i].getType();
        if (type == "bool") {
          return menuOptions[i].boolVal() ? stringEnabled : stringDisabled;
        } else if (type == "int") {
          return String(menuOptions[i].intVal());
        } else if (type == "float" ) {
          return String(menuOptions[i].floatVal());
        }
      }
    }
  }
  return "Menu Error";
}

// read settings from flash memory using 'preferences' library
void flashRead() {
  Serial.println("Reading flash...");
  for (int i = 0; i < NUM_MENU_OPTIONS; i ++) {
    if (menuOptions[i].saveInFlash()) {
      String type = menuOptions[i].getType();
      String systemName = menuOptions[i].getSysName();
      if (type == "bool") {
        menuOptions[i].setBoolVal(preferences.getBool(systemName, menuOptions[i].defaultVal() == 1 ? true : false;));
      } else if (type == "int") {
        menuOptions[i].setIntVal(preferences.getUInt(systemName, Int(menuOptions[i].defaultVal())));
      } else if (type == "float" ) {
        menuOptions[i].setIntVal(preferences.getFloat(systemName, menuOptions[i].defaultVal()));
      }
    }
  }
  Serial.println("Loaded settings from flash!");
}

// write settings to flash memory
void flashWrite() {
  Serial.println("Writing flash...");
  for (int i = 0; i < NUM_MENU_OPTIONS; i ++) {
    if (menuOptions[i].saveInFlash()) {
      String type = menuOptions[i].getType();
      String systemName = menuOptions[i].getSysName();
      if (type == "bool") {
        preferences.putBool(systemName, menuOptions[i].boolVal())
      } else if (type == "int") {
        preferences.putUInt(systemName, menuOptions[i].intVal())
      } else if (type == "float" ) {
        preferences.putFloat(systemName, menuOptions[i].floatVal())
      }
    }
  }
  Serial.println("Wrote settings to flash!");
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
  // etc...

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

//turn lamp relay(s) on / off
bool lampLastState = false;
void lampSetState (String stateStr) {
  bool state = stateStr == "ON";
  bool invertLogic = configLampInvertLogic.boolVal();
  //Serial.println("Lamp invert: " + String(invertLogic));
  bool tempOverride = false;
  if (!configLampHeaterMode.boolVal() && configTempEnable.boolVal() && configTempLampShutoff.boolVal() && temp > configTempLampShutoffTemp.floatVal()) {
    tempOverride = true;
  }
  if (!invertLogic) {
    if (state && !tempOverride) {
      digitalWrite(lampPin, LOW);
      lampLastState = true;
    } else {
      digitalWrite(lampPin, HIGH);
      lampLastState = false;
    }
  } else {
    if (state && !tempOverride) {
      digitalWrite(lampPin, HIGH);
      lampLastState = true;
    } else {
      digitalWrite(lampPin, LOW);
      lampLastState = false;
    }
  }
}

void lampCheckState (int currentHour) {
  if (configLampEnable.boolVal()) {
    if (configLampHeaterMode.boolVal()) {
      if (configTempEnable.boolVal()) {
        if (temp > configLampHeaterTemp.floatVal() + lampHeaterTempVariance) {
          lampSetState("OFF");
        } else if (temp < configLampHeaterTemp.floatVal() - lampHeaterTempVariance) {
          lampSetState("ON");
        }
      }
    } else {
      if (configLampEndHour.intVal() > configLampStartHour.intVal()) {
        if (currentHour < configLampEndHour.intVal() && currentHour >= configLampStartHour.intVal()) {
          lampSetState("ON");
        } else {
          lampSetState("OFF");
        }
      }
      else if (configLampEndHour.intVal() < configLampStartHour.intVal()) {
        if (currentHour >= configLampStartHour.intVal() || currentHour < configLampEndHour.intVal()) {
          lampSetState("ON");
        } else {
          lampSetState("OFF");
        }
      }
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
          menuSelection = NUM_MENU_OPTIONS;
        } else {
          menuSelection--;
        }
      } else {
        incrementMenuOption(menuSelection, true);
        flashWrite();
      }
    }

    if (btnType == "DN") {
      if (!editingAnOption) {
        if (menuSelection == NUM_MENU_OPTIONS) {
          menuSelection = 1;
        } else {
          menuSelection++;
        }
      } else {
        incrementMenuOption(menuSelection, false);
        flashWrite();
      }
    }
    
  }
}

//set line position and font
void drawMenuSet(int menuPos, int offset) {
  if (menuPos > NUM_MENU_OPTIONS) { return; }
  int displayLine = menuPos - offset; //offset corrects the number representing overall menu position for display on a display with room for only 6 lines
  display.setCursor(lineX, lineY(displayLine));
  if (menuSelection == menuPos) {
    display.setTextColor(BLACK, WHITE);
  }
  else {
    display.setTextColor(WHITE, BLACK);
  }
  display.print(getMenuOptionTitle(menuPos));
}

void menuDraw() {
  if (!editingAnOption) {
    int iMin = 1;
    int iMax = 7;
    int offset = 0;
      
    for(int i = 1; i < NUM_MENU_OPTIONS + 1; i = i + 6) {
      int numPages = ceil(NUM_MENU_OPTIONS / 6.0);
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
    menuTitle = "Menu - " + String(menuSelection) + "/" + String(NUM_MENU_OPTIONS);
    drawInitial();
    display.setCursor(lineX, lineY(1));
    display.print(getMenuOptionTitle(menuSelection));
    display.setCursor(lineX + 15, lineY(3));
    display.setTextSize(2);
    if (calibratingFlowSensor && menuSelection == 31) {
      display.print(flowSensorCalibrationPulses);
      display.setTextSize(1);
      display.setCursor(lineX, lineY(5));
      display.print("Pump 5L then hit OK");
    } else {
      if (menuSelection == 32) { // selecting display type
        if (getMenuOptionValue(menuSelection) == "0") { //getMenuOptionValue returns a String for display friendliness
          display.print("All Stats");
        } else if (getMenuOptionValue(menuSelection) == "1") {
          display.print("Temp Only");
        }
      } else if (menuSelection == 34) {
        display.setCursor(lineX + 3, lineY(2));
        display.print("R U sure?");
        display.setTextSize(1);
        display.setCursor(lineX + 3, lineY(4));
        display.print("Press Up or Down to");
        display.setCursor(lineX + 3, lineY(5));
        display.print("confirm");
      } else {
        display.print(getMenuOptionValue(menuSelection)); //print the current setting
        if (menuSelection == 31) { // if adjusting the flow sensor ticks / mL setting
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
  configResetSettings.Init(34, "Reset Settings", "bool", 0, 0);

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
int long unsigned lastSerialUpdate = 0;
bool wifiConnOnce = false; // track whether wifi ever connected
bool calledWifiBegin = false; //track whether WiFi.begin() was called (only do this once, but we don't do this in setup() as we need to wait a while for the stack to be ready
void loop () {

  unsigned int long curMillis = millis();

  if (millis() - lastSerialUpdate > 60000) {
    String upd = "";
    upd = "Temp: " + String(temp) + " Target: " + String(configLampHeaterTemp.floatVal());
    Serial.println(upd);
    lastSerialUpdate = millis();
  }

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
