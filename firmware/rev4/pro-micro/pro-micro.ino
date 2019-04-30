/*
 * 
 * plant controller - tim eastwood
 * 
 */

//#define SERIAL_DEBUG //print output from BOTH the ESP8266 and the Pro Micro to serial
//#define RTC_SET //sets the time on the RTC to the time the sketch is compiled - upload the sketch, then ensure the sketch is re-uploaded with this commented-out afterwards otherwise the RTC will reset when the unit is powered on

#define lamp1OnHour 7
#define lamp1OffHour 19
#define lamp2OnHour 10
#define lamp2OffHour 11

#define enableFlowTicks // uncomment to make a ticking sounds when activity from the flow sensors is detected
#define drawBorder // uncomment to draw a white border and title background on the OLED (looks nicer)
#define invertFloatSensorLogic // uncomment for use with a normally-closed float sensor, or to detect emptyness with float sensors designed for detecting fullness. Note that installing a jumper is a good idea if the float alarm is enabled and the unit will be powered on without the float sensor connected
#define enableFloatAlarm // uncomment to sound an alarm when the float sensor is triggered

// PIN ASSIGNMENTS
#define RXPin 8 //used to talk to the ESP
#define TXPin 19 //used to talk to the ESP
#define flow1Pin 19
#define flow2Pin 21
#define dht1Pin 4
#define dht2Pin 5
#define ledPin 6
#define buzzPin 7
#define buttonPin 8
#define floatSw1Pin 9
#define floatSw2Pin 20
#define lamp1Pin 10
#define lamp2Pin 16
#define pump1Pin 14
#define pump2Pin 15
// END PIN ASSIGNMENTS

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
  #ifdef drawBorder
    display.drawRoundRect(0,0,display.width(),display.height(), 2, WHITE); //x, y, x, y, radius, color
    display.fillRoundRect(0,0,display.width(),10, 2, WHITE); //x, y, x, y, radius, color
    display.setTextColor(BLACK); // Draw black text (background transparent by default)
    display.setCursor(30, 2); //title line
    display.print("HydroTek v1");
    display.setTextColor(WHITE, BLACK); // Draw white text with a black background 
  #else
    display.setCursor(30, 2);
    display.setTextColor(WHITE, BLACK);
    display.print("HydroTek v1");
  #endif
}

//draws the messages from the ESP on the screen
String displayMessage = "Booting...";
void drawDisplayMessage(bool redraw = true) {
  display.setCursor(2, 12);
  display.print(displayMessage); // line 1
  if (redraw) {
    display.display();
  }
}

void setup() {

  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(floatSw1Pin, INPUT_PULLUP);
  pinMode(floatSw2Pin, INPUT_PULLUP);
  //pinMode(ledPin, OUTPUT); //using analogWrite for this so that it isn't at full brightness
  //pinMode(buzzPin, OUTPUT);
  pinMode(lamp1Pin, OUTPUT); digitalWrite( lamp1Pin, HIGH); //default the relay to OFF
  pinMode(lamp2Pin, OUTPUT); digitalWrite( lamp2Pin, HIGH);
  pinMode(pump1Pin, OUTPUT); digitalWrite( pump1Pin, HIGH);
  pinMode(pump2Pin, OUTPUT); digitalWrite( pump2Pin, HIGH);
  pinMode(flow1Pin, INPUT);
  pinMode(flow2Pin, INPUT);
  analogWrite(ledPin, 5);
  
  tone(buzzPin, 800, 200);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.display();

  #ifdef SERIAL_DEBUG
    Serial.begin(115200); //pour a bowl of Serial
  #else
    pinMode(SS, OUTPUT); delay(500); digitalWrite(SS, LOW); //turn off the RX_LED
    TXLED0; //set mode 0 on the TX_LED - it is already set as an output
  #endif
  Serial1.begin(9600); //the RX/TX pins are on a separate hardware UART! Hurrah!
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

}

bool waitingForESP = true;
bool ESPWaitingForAVR = true;
void handleSWSerialInput(String input) {
  #ifdef SERIAL_DEBUG
    Serial.print("in: ");
    Serial.println(input);
  #endif
  if (waitingForESP && input == "ready") {
    waitingForESP = false;
    #ifdef SERIAL_DEBUG
      Serial.println("saw ESP");
    #endif
    Serial1.println("seen");
  } else if (!waitingForESP && input == "ready") {
    #ifdef SERIAL_DEBUG
      Serial.println("saw ESP again");
    #endif
    Serial1.println("seen");
  } else if (ESPWaitingForAVR && input == "seen") {
    ESPWaitingForAVR = false;
    waitingForESP = false;
    #ifdef SERIAL_DEBUG
      Serial.println("been seen");
    #endif
  } else if (!ESPWaitingForAVR && input == "seen") {
    #ifdef SERIAL_DEBUG
      Serial.println("been seen again");
    #endif
  } else {
    displayMessage = input;
    drawDisplayMessage();
  }
}

String inputString = ""; // a String to hold incoming data
void serialEventCapture() {
  while (Serial1.available()) {
    char inChar = (char)Serial1.read();
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

#define numPages 4
#define updateFrequency 4000 //seconds to display each page before moving to the next
byte page = 1; //which page the loop ison
unsigned int long lastSendHandshake = 0;
unsigned int long lastDataUpload = 0;
unsigned int long lastUpdateTime = 0; //track when page was last changed
bool flow1LastState = false; //track the last known state of the flow sensor
unsigned int flow1PulseCount = 0; //track the pulse count from the flow sensor
unsigned int flow1SinceLastUpdate = 0; //track how many flow pulses there have been since the last upload
bool flow2LastState = false;
unsigned int flow2PulseCount = 0;
unsigned int flow2SinceLastUpdate = 0;
void loop() {

  //capture serial incoming
  if (Serial1.available()) {
    serialEventCapture();
  }

  //monitor for changes to state on the flow sensor inputs
  if (digitalRead(flow1Pin) != flow1LastState) {
    flow1LastState = !flow1LastState;
    flow1PulseCount++;
    #ifdef enableFlowTicks
      tone(buzzPin, 800, 5);
    #endif
  } if (digitalRead(flow2Pin) != flow2LastState) {
    flow2LastState = !flow2LastState;
    flow2PulseCount++;
    #ifdef enableFlowTicks
      tone(buzzPin, 600, 5);
    #endif
  }

  //tell the ESP the AVR is ready
  if (ESPWaitingForAVR && millis() - lastSendHandshake > 1000) {
    Serial1.println("ready");
    #ifdef SERIAL_DEBUG
      Serial.println("Sent 'ready'");
    #endif
    lastSendHandshake = millis();
  }

  //advance the page and update sensors etc every x ms
  if (millis() - lastUpdateTime > updateFrequency || lastUpdateTime == 0) {
    lastUpdateTime = millis();

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

    //update the values on the display
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
    #ifndef RTC_SET
      else if (page == 3) {
        Serial.print("hour: ");
        Serial.println(now.hour());
        String date; date += now.day(); date += '/'; date += now.month(); date += '/'; date += now.year();
        display.print(date); // line 2
        display.setCursor(2, 36);
        String curTime; curTime += now.hour(); curTime += ':'; curTime += now.minute();
        display.print(curTime); //line 4
      }
    #endif
    else if (page == 4) {
      display.print("Nute Flow"); // line 2
      display.setCursor(2, 36);
      unsigned int flow = flow1PulseCount + flow2PulseCount;
      display.print(flow); //line 4
      flow1SinceLastUpdate += flow1PulseCount;
      flow1PulseCount = 0; //reset the flow totals since the last update
      flow2SinceLastUpdate += flow2PulseCount;
      flow2PulseCount = 0;
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

    //sound an alarm if either of the float switches are triggered
    #ifdef enableFloatAlarm
      if (floatSw1 || floatSw2) {
        tone(buzzPin, 400, 800);
      }
    #endif

    //compile the data into a string of POST values to send to the HTTP connection on the ESP
    //I've shortened these to a few characters to keep the RAM impact minimal... not sure if neccessary but the ESP's serial is buggy as shit so less is more (apparently)
    String data = "t1="; data += temp1;
    data += "&t2="; data += temp2;
    data += "&h1="; data += humidity1;
    data += "&h2="; data += humidity2;
    data += "&f1=";
    if (floatSw1) { data += "1"; } else { data += "0"; }
    data += "&f2=";
    if (floatSw2) { data += "1"; } else { data += "0"; }
    data += "&l1="; data += flow1SinceLastUpdate;
    data += "&l2="; data += flow2SinceLastUpdate;

    //set the state of the lamp outputs
    #if lamp1OffHour > lamp1OnHour
      if (now.hour() < lamp1OffHour && now.hour() >= lamp1OnHour) {
        digitalWrite(lamp1Pin, LOW); //fucking relay is inverted... LOW is ON
      } else {
        digitalWrite(lamp1Pin, HIGH);
      }
    #else if lamp1OffHour < lamp1OnHour
      if (now.hour() >= lamp1OnHour || now.hour() < lamp1OffHour) {
        digitalWrite(lamp1Pin, LOW); //fucking relay is inverted... LOW is ON
      } else {
        digitalWrite(lamp1Pin, HIGH);
      }
    #endif
    #if lamp2OffHour > lamp2OnHour
      if (now.hour() < lamp2OffHour && now.hour() >= lamp2OnHour) {
        digitalWrite(lamp2Pin, LOW); //fucking relay is inverted... LOW is ON
      } else {
        digitalWrite(lamp2Pin, HIGH);
      }
    #else if lamp2OffHour < lamp2OnHour
      if (now.hour() >= lamp1OnHour || now.hour() < lamp1OffHour) {
        digitalWrite(lamp1Pin, LOW); //fucking relay is inverted... LOW is ON
      } else {
        digitalWrite(lamp1Pin, HIGH);
      }
    #endif

    //do the data upload to the ESP
    if (!ESPWaitingForAVR && !waitingForESP && millis() - lastDataUpload > 10000) {
      if (temp1 > 0 || temp2 > 0) {
        #ifdef SERIAL_DEBUG
          Serial.println("send data");
        #endif
        Serial1.println(data);
      }
      #ifdef SERIAL_DEBUG
        else {
         Serial.println("nothing to send");
        }
      #endif
      //reset these values
      flow1SinceLastUpdate = 0;
      flow2SinceLastUpdate = 0;
      lastDataUpload = millis();
    }

    page++;
    if (page > numPages) {
      page = 1;
    }    
  }
}
