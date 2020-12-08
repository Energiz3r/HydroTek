/*
 * 
 * plant controller - tim eastwood
 * 
 */

#define SERIAL_DEBUG //print output from BOTH the ESP8266 and the Pro Micro to serial
//#define RTC_SET //sets the time on the RTC to the time the sketch is compiled - upload the sketch, then ensure the sketch is re-uploaded with this commented-out afterwards otherwise the RTC will reset when the unit is powered on

#define lamp1OnHour 5
#define lamp1OffHour 23

#define pump1Enable //run the pump every 4 hours
#define pump1Duration 30 //seconds

#define enableFlow1 // uncomment to make a ticking sounds when activity from the flow sensors is detected
//#define flowTicks
//#define drawBorder // uncomment to draw a white border and title background on the OLED (looks nicer)
//#define invertFloatSensorLogic // uncomment for use with a normally-closed float sensor, or to detect emptyness with float sensors designed for detecting fullness. Note that installing a jumper is a good idea if the float alarm is enabled and the unit will be powered on without the float sensor connected
//#define enableFloatAlarm // uncomment to sound an alarm when the float sensor is triggered

// PIN ASSIGNMENTS
#define flow1Pin 4
#define dht1Pin 19
#define ledPin 6
#define buzzPin 7
#define buttonPin 18
#define floatSw1Pin 9
#define lamp1Pin 10
#define pump1Pin 14
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

//RTC
#include <RTClib.h>
RTC_DS1307 RTC;

//draws the border and title
void drawInitial() {
  display.clearDisplay();
    display.setCursor(30, 2);
    display.setTextColor(WHITE, BLACK);
    display.print("HydroTek v1");
}

void setup() {

  delay(2000);
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(floatSw1Pin, INPUT_PULLUP);
  pinMode(lamp1Pin, OUTPUT); //digitalWrite( lamp1Pin, HIGH); //default the relay to OFF
  pinMode(pump1Pin, OUTPUT); //digitalWrite( pump1Pin, HIGH);
  pinMode(flow1Pin, INPUT_PULLUP);
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
  RTC.begin();

  #ifdef RTC_SET
    //if (! RTC.isrunning()) {
      //#ifdef SERIAL_DEBUG
        //Serial.println("RTC is NOT running!");
      //#endif
      // following line sets the RTC to the date & time this sketch was compiled
  RTC.adjust(DateTime(__DATE__, __TIME__));
    //}
  #endif

}

#define numPages 4
#define updateFrequency 3000 //seconds to display each page before moving to the next
byte page = 1; //which page the loop ison
unsigned int long lastUpdateTime = 0; //track when page was last changed
bool flow1LastState = false; //track the last known state of the flow sensor
unsigned int flow1PulseCount = 0; //track the pulse count from the flow sensor
unsigned int long pumpLastCheck = 0;
bool lamp1on = false;
void loop() {

  //monitor for changes to state on the flow sensor inputs
  #ifdef enableFlow1
    if (digitalRead(flow1Pin) != flow1LastState) {
      flow1LastState = !flow1LastState;
        flow1PulseCount++;
        #ifdef flowTicks
          tone(buzzPin, 800, 5);
        #endif
    }
  #endif
  
  //run the pump. The pump is hardcoded to run at these times:
  /*
    3
    7
    11
    15
    19
    23
   */
  if (millis() - pumpLastCheck > 500) {
    pumpLastCheck = millis();
    DateTime now = RTC.now(); 
    byte H = now.hour();
    byte M = now.minute();
    byte S = now.second();
    if ((H == 3 || H == 7 || H == 11 || H == 15 || H == 19 || H == 23) && M == 1) { //only run the pumps at the start of these hours and during the first minute of the hour 
      #ifdef pump1Enable
        if (S <= pump1Duration) { //only run for the duration of the pump
          digitalWrite(pump1Pin, LOW);
        } else {
          digitalWrite(pump1Pin, HIGH);
        }
      #endif
    }
    else {
      digitalWrite(pump1Pin, HIGH);
    }
  }

  //advance the page and update sensors etc every x ms
  if (millis() - lastUpdateTime > updateFrequency || lastUpdateTime == 0) {
    lastUpdateTime = millis();

    float humidity1 = dht1.readHumidity(); //read humidity
    float temp1 = dht1.readTemperature(); //read temperature as Celsius (the default)
    
    if (isnan(humidity1)) { humidity1 = 0; } //check for valid reading
    float averageHumidity = humidity1;
  
    if (isnan(temp1)) { temp1 = 0; }
    float averageTemp = temp1;

    //grab the time from the RTC
    DateTime now = RTC.now(); 

    //update the values on the display
    drawInitial();
    display.setTextSize(2); //use larger font
    display.setCursor(2, 20);
    if (page == 1) {
      display.print("Temp"); // line 2
      display.setCursor(2, 36);
      display.print(averageTemp);
      display.print("C"); // line 4
    } else if (page == 2) {
      display.print("Humidity"); // line 2
      display.setCursor(2, 36);
      display.print(averageHumidity);
      display.print("%"); // line 4
    }
    #ifndef RTC_SET
      else if (page == 3) {
        String date; date += now.day(); date += '/'; date += now.month(); date += '/'; date += now.year();
        display.print(date); // line 2
        display.setCursor(2, 36);
        String curTime;
        curTime += now.hour() > 12 ? now.hour() - 12 : now.hour();
        curTime += ':';
        curTime += now.minute() < 10 ? "0" : "";
        curTime += now.minute();
        curTime += now.hour() > 12 ? "pm" : "am";
        display.print(curTime); //line 4
      }
    #endif
    else if (page == 4) {
      display.print("Nute Flow"); // line 2
      display.setCursor(2, 36);
      unsigned int flow = flow1PulseCount;
      display.print(flow); //line 4
      flow1PulseCount = 0; //reset the flow totals since the last update
    }
    display.setTextSize(1); //set back to size 1
    display.setCursor(2, 52); //line 6
    display.print("Lamp ");
    display.println(lamp1on ? "on" : "off");
    display.display();

    //get float switch statuses
    #ifdef invertFloatSensorLogic //reads HIGH if 'triggered' (NC)
      bool floatSw1 = digitalRead(floatSw1Pin);
    #else                         //reads LOW if 'triggered' (NO)
      bool floatSw1 = !digitalRead(floatSw1Pin);
    #endif

    //sound an alarm if either of the float switches are triggered
    #ifdef enableFloatAlarm
      if (floatSw1) {
        tone(buzzPin, 400, 800);
      }
    #endif

    //set the state of the lamp outputs
    #if lamp1OffHour > lamp1OnHour
      if (true || now.hour() < lamp1OffHour && now.hour() >= lamp1OnHour) {
        #ifdef SERIAL_DEBUG
          Serial.println("Lamp 1 ON");
        #endif
        lamp1on = true;
        digitalWrite(lamp1Pin, LOW); //fucking relay is inverted... LOW is ON
      } else {
        #ifdef SERIAL_DEBUG
          Serial.println("Lamp 1 OFF");
        #endif
        lamp1on = false;
        digitalWrite(lamp1Pin, HIGH);
      }
    #else if lamp1OffHour < lamp1OnHour
      if (now.hour() >= lamp1OnHour || now.hour() < lamp1OffHour) {
        #ifdef SERIAL_DEBUG
          Serial.println("Lamp 1 ON");
        #endif
        lamp1on = true;
        digitalWrite(lamp1Pin, LOW); //fucking relay is inverted... LOW is ON
      } else {
        #ifdef SERIAL_DEBUG
          Serial.println("Lamp 1 OFF");
        #endif
        lamp1on = false;
        digitalWrite(lamp1Pin, HIGH);
      }
    #endif

    page++;
    if (page > numPages) {
      page = 1;
    }    
  }
}
