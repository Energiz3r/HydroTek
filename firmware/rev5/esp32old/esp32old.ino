/*
 * 
 * HydroTek hydroponics controller - tim eastwood
 * 
 * See https://github.com/Energiz3r/HydroTek for information and instructions
 * 
 */

// PIN ASSIGNMENTS - ESP32
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

#define SERIAL_DEBUG //print output to serial
//#define RTC_SET //sets the time on the RTC to the time the sketch is compiled - upload the sketch, then ensure the sketch is re-uploaded with this commented-out afterwards otherwise the RTC will reset when the unit is powered on

//Ensure your username/password are alphanumeric and don't contain special characters.
#define webUsername "Tangles"
#define webPassword "jiblet123"
#define deviceId "1"
#define webEndpoint "http://noobs.wtf/plants/endpoint.php" //the php script that the ESP8266 will connect to for uploading data
//#define dummyUpload //uncomment to attempt to upload dummy data every 10 seconds for testing purposes
const char* ssid = "TanglesS8";
const char* passphrase =  "jiblet123";

#define uploadFrequency 60 //how often to upload data, in seconds

#define lamp1OnHour 7
#define lamp1OffHour 21
#define lamp2OnHour 10
#define lamp2OffHour 11

#define pump1Enable //run the pump every 4 hours
#define pump1Duration 15 //seconds
//#define pump2Enable
#define pump2Duration 10
//#define INVERT_RELAY_LOGIC //invert the logic of the relay outputs

#define enableFlow1 // uncomment to enable the flow sensor
#define enableFlow2
#define flowTicks // uncomment to make ticking sounds when activity from the flow sensors is detected
#define drawBorder // uncomment to draw a white border and title background on the OLED (looks nicer)
//#define invertFloatSensorLogic // uncomment for use with a normally-closed float sensor, or to detect emptyness with float sensors designed for detecting fullness. Note that installing a jumper is a good idea if the float alarm is enabled and the unit will be powered on without the float sensor connected
#define enableFloatAlarm // uncomment to sound an alarm when the float sensor is triggered


//ESP Includes
#include <WiFi.h>
#include <HTTPClient.h>
#define postIdentifierString "user=" webUsername "&password=" webPassword "&device=" deviceId "&"

//display includes
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1); // -1 is the reset pin, or 4 // Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)

//DHT sensor includes
#include "DHT.h"
DHT dht1(dht1Pin, DHT22); //pin, type
DHT dht2(dht2Pin, DHT22);

//RTC
#include <RTClib.h>
RTC_DS1307 RTC;

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

//draws the status on the LCD. pass true to draw instantly
String displayMessage = "Booting...";
void drawDisplayMessage(bool redraw = true) {
  display.setCursor(2, 12);
  display.print(displayMessage); // line 1
  if (redraw) {
    display.display();
  }
}

#ifdef dummyUpload
  String dummyData;
#endif
bool wifiConnected = false;
void setup() {

  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(floatSw1Pin, INPUT_PULLUP);
  pinMode(floatSw2Pin, INPUT_PULLUP);
  //pinMode(ledPin, OUTPUT); //using analogWrite for this so that it isn't at full brightness
  //pinMode(buzzPin, OUTPUT);
  pinMode(lamp1Pin, OUTPUT); //digitalWrite( lamp1Pin, HIGH); //default the relay to OFF
  pinMode(lamp2Pin, OUTPUT); //digitalWrite( lamp2Pin, HIGH);
  pinMode(pump1Pin, OUTPUT); //digitalWrite( pump1Pin, HIGH);
  pinMode(pump2Pin, OUTPUT); //digitalWrite( pump2Pin, HIGH);
  pinMode(flow1Pin, INPUT_PULLUP);
  pinMode(flow2Pin, INPUT_PULLUP);
  //analogWrite(ledPin, 5);
  
  //tone(buzzPin, 800, 200);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.display();

  Serial.begin(115200); //pour a bowl of Serial
  dht1.begin();
  dht2.begin(); //initilize the DHT22 sensors
  RTC.begin();

  #ifdef RTC_SET
    if (! RTC.isrunning()) {
      #ifdef SERIAL_DEBUG
        Serial.println("RTC is NOT running!");
      #endif
      // following line sets the RTC to the date & time this sketch was compiled
      RTC.adjust(DateTime(__DATE__, __TIME__));
    }
  #endif

  delay(4000);   //Delay needed before calling the WiFi.begin
  WiFi.begin(ssid, passphrase); 

  #ifdef dummyUpload
    dummyData = "t1="; dummyData += "24.2"; //temp 1
    dummyData += "&t2="; dummyData += "25.3"; //temp 2
    dummyData += "&h1="; dummyData += "64.5"; //humidity 1
    dummyData += "&h2="; dummyData += "72.1"; //humidity 2
    dummyData += "&f1="; dummyData += "1"; //float switch status 1
    dummyData += "&f2="; dummyData += "0"; //float switch status 2
    dummyData += "&l1="; dummyData += "450"; //flow 1 pulse count
    dummyData += "&l2="; dummyData += "275"; //flow 2 pulse count
  #endif
}

#define numPages 4
byte page = 1; //which page the loop is on
#define updateFrequency 3000 //seconds to display each page before moving to the next
unsigned int long lastDataUpload = 0;
unsigned int long lastUpdateLoop = 0; //track when page was last changed
bool flow1LastState = false; //track the last known state of the flow sensor
bool flow2LastState = false;
unsigned int flow1PulseCount = 0; //track pulse count
unsigned int flow2PulseCount = 0;
unsigned int flow1SinceLastUpdate = 0; //track pulse count between successful data uploads
unsigned int flow2SinceLastUpdate = 0;
unsigned int long sensorLastCheck = 0;
void loop() {

  //monitor for changes to state on the flow sensor inputs
  // this probably should be an interrupt but the state doesn't seem to change fast enough that the loop will miss it. Worst-case a few ticks from the flow sensors are missed
  #ifdef enableFlow1
    if (digitalRead(flow1Pin) != flow1LastState) {
      flow1LastState = !flow1LastState;
      flow1PulseCount++;
    }
  #endif
  #ifdef enableFlow2
    if (digitalRead(flow2Pin) != flow2LastState) {
      flow2LastState = !flow2LastState;
      flow2PulseCount++;
    }
  #endif  

  //run a check on status / sensors every x ms
  if (millis() - sensorLastCheck > 500) {
    sensorLastCheck = millis();

    //check the WiFi status
    if (WiFi.status() == WL_CONNECTED) {
      if (!wifiConnected) {
        wifiConnected = true;
        Serial.println("WiFi Connected!");
      }      
    } else {
      if (wifiConnected) {
        wifiConnected = false;
        Serial.println("WiFi Disonnected...");
      }
    }

    //run the pump. The pump is hardcoded to run every 4 hours, at these hours:
    // 3, 7, 11, 15, 19, 23
    DateTime now = RTC.now(); 
    byte H = now.hour();
    byte M = now.minute();
    byte S = now.second();
    if ((H == 3 || H == 7 || H == 11 || H == 15 || H == 19 || H == 23) && M == 1) { //only run the pumps at the start of these hours and during the first minute of the hour 
      #ifdef pump1Enable
        if (S <= pump1Duration) { //only run for the pump for the specified duration
          #ifndef INVERT_RELAY_LOGIC
            digitalWrite(pump1Pin, LOW);
          #else
            digitalWrite(pump1Pin, HIGH);
          #endif
        } else {
          #ifndef INVERT_RELAY_LOGIC
            digitalWrite(pump1Pin, HIGH);
          #else
            digitalWrite(pump1Pin, LOW);
          #endif
        }
      #endif
      #ifdef pump2Enable
        if (S <= pump2Duration) {
          #ifndef INVERT_RELAY_LOGIC
            digitalWrite(pump2Pin, LOW);
          #else
            digitalWrite(pump2Pin, HIGH);
          #endif
        } else {
          #ifndef INVERT_RELAY_LOGIC
            digitalWrite(pump2Pin, HIGH);
          #else
            digitalWrite(pump2Pin, LOW);
          #endif
        }
      #endif
    }
    else {
      #ifndef INVERT_RELAY_LOGIC
        digitalWrite(pump1Pin, HIGH);
        digitalWrite(pump2Pin, HIGH);
      #else
        digitalWrite(pump1Pin, LOW);
        digitalWrite(pump2Pin, LOW);
      #endif
    }
  }

  //advance the page and update sensors etc every x ms
  if (millis() - lastUpdateLoop > updateFrequency || lastUpdateLoop == 0) {
    lastUpdateLoop = millis();

    float humidity1 = dht1.readHumidity(); //read humidity
    float temp1 = dht1.readTemperature(); //read temperature as Celsius (the default)
    float humidity2 = dht2.readHumidity();
    float temp2 = dht2.readTemperature();
    
    float workingRHSensors = 2.0; //used for working out the average humidity
    if (isnan(humidity1)) { humidity1 = 0; workingRHSensors -= 1.0; } //check for valid reading
    if (isnan(humidity2)) { humidity2 = 0; workingRHSensors -= 1.0; }
    float averageHumidity = (humidity1 + humidity2) / workingRHSensors;
  
    float workingTempSensors = 2.0;
    if (isnan(temp1)) { temp1 = 0; workingTempSensors -= 1; }
    if (isnan(temp2)) { temp2 = 0; workingTempSensors -= 1; }
    float averageTemp = (temp1 + temp2) / workingTempSensors;

    //grab the time from the RTC
    DateTime now = RTC.now(); 

    //update flow sensor readings
    flow1SinceLastUpdate += flow1PulseCount;
    flow1PulseCount = 0; //reset the flow totals since the last update
    flow2SinceLastUpdate += flow2PulseCount;
    flow2PulseCount = 0;

    //update the display
    drawInitial();
    drawDisplayMessage(false);
    display.setTextSize(2); //use larger font
    display.setCursor(2, 20);
    if (page == 1) {
      display.print("Avg Temp"); // line 2
      display.setCursor(2, 36);
      if (workingTempSensors > 0) {
        display.print(averageTemp);
        display.print("C"); // line 4
      } else { display.print("N/A"); }
    } else if (page == 2) {
      display.print("Avg Humid."); // line 2
      display.setCursor(2, 36);
      if (workingRHSensors > 0) {
        display.print(averageHumidity);
        display.print("%"); // line 4
      } else { display.print("N/A"); }
    }
    else if (page == 3) {
      String date; date += now.day(); date += '/'; date += now.month(); date += '/'; date += now.year();
      display.print(date); // line 2
      display.setCursor(2, 36);
      String curTime; curTime += now.hour(); curTime += ':'; curTime += now.minute();
      display.print(curTime); //line 4
    }
    else if (page == 4) {
      display.print("Nute Flow"); // line 2
      display.setCursor(2, 36);
      unsigned int flow = flow1SinceLastUpdate + flow2SinceLastUpdate;
      display.print(flow); //line 4
    }
    display.setTextSize(1); //set back to size 1
    display.setCursor(2, 52); //line 6
    display.print("H "); display.print(int(workingRHSensors)); display.print(" T "); display.println(int(workingTempSensors));
    display.display();

    //get float switch statuses
    #ifdef invertFloatSensorLogic //reads HIGH if 'triggered' (NC)
      bool floatSw1 = digitalRead(floatSw1Pin);
      bool floatSw2 = digitalRead(floatSw2Pin);
    #else                         //reads LOW if 'triggered' (NO)
      bool floatSw1 = !digitalRead(floatSw1Pin);
      bool floatSw2 = !digitalRead(floatSw2Pin);
    #endif

    //compile the data into a string of POST values to send to the HTTP connection on the ESP
    String uploadData = "t1="; uploadData += temp1;
    uploadData += "&t2="; uploadData += temp2;
    uploadData += "&h1="; uploadData += humidity1;
    uploadData += "&h2="; uploadData += humidity2;
    uploadData += "&f1=";
    if (floatSw1) { uploadData += "1"; } else { uploadData += "0"; }
    uploadData += "&f2=";
    if (floatSw2) { uploadData += "1"; } else { uploadData += "0"; }
    uploadData += "&l1="; uploadData += flow1SinceLastUpdate;
    uploadData += "&l2="; uploadData += flow2SinceLastUpdate;

    //set the state of the lamp outputs
    #if lamp1OffHour > lamp1OnHour
      if (true || now.hour() < lamp1OffHour && now.hour() >= lamp1OnHour) {
        #ifdef SERIAL_DEBUG
          Serial.println("Lamp 1 ON");
        #endif
        #ifndef INVERT_RELAY_LOGIC
          digitalWrite(lamp1Pin, LOW); //fucking relay is inverted... LOW is ON
        #else
          digitalWrite(lamp1Pin, HIGH);
        #endif
      } else {
        #ifdef SERIAL_DEBUG
          Serial.println("Lamp 1 OFF");
        #endif
        #ifndef INVERT_RELAY_LOGIC
          digitalWrite(lamp1Pin, HIGH);
        #else
          digitalWrite(lamp1Pin, LOW);
        #endif
      }
    #else if lamp1OffHour < lamp1OnHour
      if (now.hour() >= lamp1OnHour || now.hour() < lamp1OffHour) {
        #ifdef SERIAL_DEBUG
          Serial.println("Lamp 1 ON");
        #endif
        #ifndef INVERT_RELAY_LOGIC
          digitalWrite(lamp1Pin, LOW);
        #else
          digitalWrite(lamp1Pin, HIGH);
        #endif
      } else {
        #ifdef SERIAL_DEBUG
          Serial.println("Lamp 1 OFF");
        #endif
        #ifndef INVERT_RELAY_LOGIC
          digitalWrite(lamp1Pin, HIGH);
        #else
          digitalWrite(lamp1Pin, LOW);
        #endif
      }
    #endif
    #if lamp2OffHour > lamp2OnHour
      if (now.hour() < lamp2OffHour && now.hour() >= lamp2OnHour) {
        #ifndef INVERT_RELAY_LOGIC
          digitalWrite(lamp2Pin, LOW);
        #else
          digitalWrite(lamp2Pin, HIGH);
        #endif
      } else {
        #ifndef INVERT_RELAY_LOGIC
          digitalWrite(lamp2Pin, HIGH);
        #else
          digitalWrite(lamp2Pin, LOW);
        #endif
      }
    #else if lamp2OffHour < lamp2OnHour
      if (now.hour() >= lamp2OnHour || now.hour() < lamp2OffHour) {
        #ifndef INVERT_RELAY_LOGIC
          digitalWrite(lamp2Pin, LOW);
        #else
          digitalWrite(lamp2Pin, HIGH);
        #endif
      } else {
        #ifndef INVERT_RELAY_LOGIC
          digitalWrite(lamp2Pin, HIGH);
        #else
          digitalWrite(lamp2Pin, LOW);
        #endif
      }
    #endif

    // DIAGNOSTIC
    #ifdef SERIAL_DEBUG
      Serial.print("Working TH Sensors: ");
      Serial.println(workingRHSensors);
      if (workingRHSensors > 0) {
        Serial.print("Temp 1: ");
        Serial.println(temp1);
        Serial.print("Temp 2: ");
        Serial.println(temp2);
      }
      Serial.print("Float1: ");
      Serial.println(floatSw1);
      Serial.print("Float2: ");
      Serial.println(floatSw2);
    #endif
    // END DIAGNOSTIC

    //upload data to web
    #ifndef dummyUpload
    if (wifiConnected && workingRHSensors > 0 && millis() - lastDataUpload > uploadFrequency * 1000) {
    #else
    if (wifiConnected && millis() - lastDataUpload > 10000) {
    #endif
      HTTPClient http;
      if(http.begin(webEndpoint)){
        http.addHeader("Content-Type", "application/x-www-form-urlencoded", false, true);
        
        #ifndef dummyUpload
          String postString = postIdentifierString + uploadData;
          #ifdef SERIAL_DEBUG
            Serial.println("Sending POST request:");
            Serial.println(postString);
            Serial.print("POST response: ");
          #endif
          int httpCode = http.POST(postString);
        #else
          String postString = postIdentifierString + dummyData;
          #ifdef SERIAL_DEBUG
            Serial.println("Sending dummy POST request:");
            Serial.println(dummyData);
            Serial.print("POST response: ");
          #endif
          int httpCode = http.POST(postString);
        #endif
        if (httpCode > 0) { // httpCode will be negative on error
          String response = "OK";
          response += (String)httpCode;
          response += ", ";
          response += http.getString();
          response.replace('\n', ' ');
          displayMessage = response;
          #ifdef SERIAL_DEBUG
            Serial.println(response);
          #endif
        } else {
          String response = "ER";
          response += (String)httpCode;
          response += ", ";
          response += http.errorToString(httpCode).c_str();
          response.replace('\n', ' ');
          displayMessage = response;
          #ifdef SERIAL_DEBUG
            Serial.println(response);
          #endif
        }
        http.end();
      } else {
        #ifdef SERIAL_DEBUG
          Serial.println("Could not connect to server");
        #endif
        displayMessage = "Connection error";
      }
      //update sensors
      flow1SinceLastUpdate = 0;
      flow2SinceLastUpdate = 0;
      lastDataUpload = millis();
    } else if (millis() - lastDataUpload > uploadFrequency * 1000) {
      if (wifiConnected) {
        #ifdef SERIAL_DEBUG
          Serial.println("Skipped upload - no sensor data");
        #endif
        displayMessage = "No sensors";
      } else {
        #ifdef SERIAL_DEBUG
          Serial.println("Skipped upload - WiFi not connected");
        #endif
        displayMessage = "Not on WiFi";
      }
    }

    //advance the page
    page++;
    if (page > numPages) {
      page = 1;
    }
  }
}
