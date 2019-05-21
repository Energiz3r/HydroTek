/*
 * 
 * HydroTek
 * firmware 0.5 / rev5
 * Tim Eastwood
 *  
 */

/*

Pinouts for ESP:

Simple digital input
Flow 1: 
Flow 2:

Requires pin-change interrupt


 */

/* START CONFIG */

#define dataString "user=USERNAME&password=PASSWORD&device=1&" //set the username, password, and ID of the device here. Ensure your username/password are alphanumeric and don't contain special characters. Don't alter the other characters as this forms part of the GET request.
#define endpoint "http://MYDOMAIN.com/endpoint.php" //the php script that the ESP8266 will connect to for uploading data
//#define dummyUpload //uncomment to attempt a dummy upload every 10 seconds, with the results output to Serial (Pro Micro not required)

#define SERIAL_DEBUG //print output from BOTH the ESP8266 and the Pro Micro to serial
//#define RTC_SET //sets the time on the RTC to the time the sketch is compiled - upload the sketch, then ensure the sketch is re-uploaded with this commented-out afterwards otherwise the RTC will reset when the unit is powered on

#define lamp1OnHour 7
#define lamp1OffHour 21
#define lamp2OnHour 10
#define lamp2OffHour 11

#define pump1Enable //run the pump every 4 hours
#define pump1Duration 10 //seconds
//#define pump2Enable
#define pump2Duration 10

#define enableFlow1 // uncomment to make a ticking sounds when activity from the flow sensors is detected
#define enableFlow2 // uncomment to make a ticking sounds when activity from the flow sensors is detected
#define flowTicks
//#define drawBorder // uncomment to draw a white border and title background on the OLED (looks nicer)
//#define invertFloatSensorLogic // uncomment for use with a normally-closed float sensor
#define enableFloatAlarm // uncomment to sound an alarm when the float sensor is triggered

// PIN ASSIGNMENTS
#define flow1Pin 4
#define flow2Pin 5
#define dht1Pin 19
#define dht2Pin 21
#define ledPin 6
#define buzzPin 7
#define buttonPin 18
#define floatSw1Pin 9
#define floatSw2Pin 20
#define lamp1Pin 10
#define lamp2Pin 16
#define pump1Pin 14
#define pump2Pin 15

// See section below for adding WiFi access points to the configuration

/* END CONFIG */

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

ESP8266WiFiMulti WiFiMulti;

String displayMessage = "ESP Booting...";
void setup() {
  #ifndef dummyUpload
    Serial.begin(9600);
    Serial.swap();
  #else
    Serial.begin(115200);
  #endif
  delay(3000);
  Serial.println(displayMessage);  
  WiFi.mode(WIFI_STA);

  /* START WIFI CONFIG */
  
  WiFiMulti.addAP("AccessPoint", "passphrase");
  WiFiMulti.addAP("Another", "passphrase");

  /* START WIFI CONFIG */
}

bool wifiIsConnected = false;
bool shouldUpload = true; //prevent trying to upload while a HTTP connection is already in progress
void uploadData(String message) {
  if (wifiIsConnected) {

    String data = dataString;
    data += message;
    
    WiFiClient client;
    HTTPClient http;
   
    if (http.begin(client, endpoint)) {      
      
      http.addHeader("Content-Type", "application/x-www-form-urlencoded", false, true);
      int httpCode = http.POST(data);
      
      if (httpCode > 0) { // httpCode will be negative on error
        String response = "OK";
        response += (String)httpCode;
        response += ", ";
        response += http.getString();
        response.replace('\n', ' ');
        displayMessage = response;
      } else {
        String response = "ER";
        response += (String)httpCode;
        response += ", ";
        response += http.errorToString(httpCode).c_str();
        response.replace('\n', ' ');
        displayMessage = response;
      }
      http.end();
    } else {
      displayMessage = "Couldn't connect";
    }
  } else {
    displayMessage = "Not on WiFi";
  }
  #ifdef dummyUpload
    Serial.println(displayMessage);
  #endif
  shouldUpload = true;
}

int unsigned long lastUploadTime = 0;
bool waitingForAVR = true;
bool AVRWaitingForESP = true;
void handleSWSerialInput(String input) {
  if (waitingForAVR && input == "ready") {
    waitingForAVR = false;
    Serial.println("seen");
  } else if (!waitingForAVR && input == "ready") {
    Serial.println("seen");
  } else if (AVRWaitingForESP && input == "seen") {
    AVRWaitingForESP = false;
    waitingForAVR = false;
    displayMessage = "ESP Running";
  } else if (!AVRWaitingForESP && input == "seen") {
    Serial.println("seen again"); //not really neccessary
  } else {
    //check if another upload should be done
    //if (shouldUpload && millis() - lastUploadTime > uploadFrequency){
      lastUploadTime = millis();
      shouldUpload = false;
      uploadData(input); //upload the received data to the server - we'll let the server check for errors
    //}
  }
}

String inputString = ""; // a String to hold incoming data
void serialEventCapture() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    if (inChar == '\n') {
      handleSWSerialInput(inputString);
      inputString = "";
    } else {
      if ((' ' <= inChar) && (inChar <= '~')) { //filter out garbage characters
        inputString += inChar;
      }
    }
  }
}

String postMsg = "";
unsigned int long lastDisplayUpdateTime = 0;
unsigned int long lastSendHandshake = 0;
void loop() {

  //watch for input on software serial
  if (Serial.available()) {
    serialEventCapture();
  }

  //send dummy data every 10 seconds. Use the lastSendHandshake int since it's not used otherwise
  #ifdef dummyUpload
    if (millis() - lastSendHandshake > 10000) {
      String data = "t1="; data += "24.2"; //temp 1
      data += "&t2="; data += "25.3"; //temp 2
      data += "&h1="; data += "64.5"; //humidity 1
      data += "&h2="; data += "72.1"; //humidity 2
      data += "&f1="; data += "1"; //float switch status 1
      data += "&f2="; data += "0"; //float switch status 2
      data += "&l1="; data += "450"; //flow 1 pulse count
      data += "&l2="; data += "275"; //flow 2 pulse count
      uploadData(data);
    }
  #endif

  //tell the AVR when ready
  #ifndef dummyUpload
    if (AVRWaitingForESP && millis() - lastSendHandshake > 850) {
      Serial.println("ready");
      lastSendHandshake = millis();
    }
  #endif

  //keep an eye on the WiFi status
  if ((WiFiMulti.run() == WL_CONNECTED)) {
    if (!wifiIsConnected) { Serial.println("WiFi Connect"); }
    wifiIsConnected = true;
  } else {
    if (wifiIsConnected) { Serial.println("WiFi Disconnect"); }
    wifiIsConnected = false;
  }

  //update the AVR display
  #ifndef dummyUpload
    if (!waitingForAVR && !AVRWaitingForESP && millis() - lastDisplayUpdateTime > 11000) {
      Serial.println(displayMessage); //still print this despite if the following is printed; will allow diagnosis via the AVR serial console
      if (millis() - lastUploadTime > 30000) {
        Serial.println("Idle - not uploading");
      }    
      lastDisplayUpdateTime = millis();
    }
  #endif

  delay(5);
}
