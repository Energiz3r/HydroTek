# HydroTek
## An IoT Controller for Aquaponics, Hydroponics, and Aeroponics

*Current revision: rev5*
(rev5 pictured below)

![alt-text](https://i.imgur.com/Od8aEW4l.jpg "Image")

(rev4 pictured below)

![alt-text](https://i.imgur.com/qcLx93il.jpg "Image")

(rev3 pictured below)

![alt-text](https://i.imgur.com/RJYCEXgl.jpg "Image")

---
### Status Update:

This device is still under active development. See notes in below sections about how the device works and how much of the software is complete.

Early revisions used an ESP8266 and an Arduino Nano, which communicated with each other over serial. Since discovering the ESP32 I've changed the design to simplify it greatly, however I've found a consistent issue with the gerber files produced by PCBWeb which has been ruining my manufactured PCBs. The tool is excellent otherwise but it looks like I will need to change to Eagle or KiCad or somesuch unless the vendor can release a patch.

In the meantime, I've fixed the rev5 gerber files using a gerber editor such that they can be successfully manufactured. There are still issues with this board - the LED and buzzer are both connected to pins on the ESP32 which cannot serve as outputs. I have assembled a rev5 for testing and will soon be working on a 6th revision using the new PCB design tool.

---

### Overview:

There are four components to this repository:
* PCB cad file (created in PCBWeb)
* Firmware for the ESP32
* STL files for a case
* The cloud app for configuration and monitoring of the device

The device has been designed for use with a run-to-waste hydroponic system, however could be applied to just about any setup where the sensors and control outputs are relevant, such as airponics, aquaponics, and even a simple vegge patch.

---

### Features:

* Uploads sensor data to cloud via WiFi
* 128x64 OLED display
* RTC with battery backup for accurate control of grow light times
* Can monitor:
  * DHT22 temperature / humidity sensor
  * Liquid flow sensor (YF-S201 or any 5V based hall-effect sensor)
  * Float switch to detect either empty nutrient tank or full run-off tank
* Can control:
  * Grow light
  * Nutrient pump
* Buzzer / LED to alert user to lost WiFi signal, temperature extremes, nutrient pump faults, or full / empty nutrient / run-off tanks
* The board can be powered either through the barrel jack as designed or using a USB power source plugged into the ESP32 (Note: the USB power source must be sufficient to run both the HydroTek and any connected relays)

I2C pH sensors *could* be connected using a kit [such as this](https://www.sparkfun.com/products/10972), however pH meters are supposed to be stored in a pH-neutral storage solution. Since this means they aren't suitable for a constant-monitoring setup, pH monitoring was excluded from the feature set.

---

### Design considerations:

* The device must be maintenance-free
* Once assembled, flashed, and configured, the device must be easy to set up and use by non-technical people
* Compact size desirable. Other off-the-shelf units or open-source setups seem to be quite large

---

### To Do (not started):

* fetch the configuration for lamp / pump timings from online so the sketch doesn't need to be altered to make changes
* update the RTC time from online
* WiFi configuration on the device, using buttons and a menu instead of by editing the sketch
* Calculate actual flow from the flow sensors

---

### Build instructions:

The software for this is a bit incomplete. The hardware is good to go, so go ahead with that, but if you want to build this and use the online component as I've designed it, shoot me a message at cl@noobs.wtf and I'll sort out the things still needing to be done manually, such as creating an account and preparing the database for your hardware.

Note: The capacitors, barrel jack, and LM7805 can be omitted if the intention is to only run HydroTek from a USB power source. 

*(rev5)*

* To manufacture the PCB, upload the gerber files for the current PCB revision to a service such as [DirtyPCBs](http://dirtypcbs.com/store/pcbs)
* Purchase the components, as desired, outlined in the BOM file
* Solder the components to the PCB, ensuring the following order is observed (*B/S = BOTTOM SIDE*):
  * (B/S) Solder the LM7805 linear regulator to the UNDERSIDE of the board, ensuring the tab is soldered to the large square pad for heat dissipation
  * Solder in the 4x capacitors
  * Solder in the piezo buzzer
  * Solder in the IDC headers for the sensor inputs (to the TOP side of the board)
  * Solder in the IDC sockets for the LCD (or alternatively, solder the LCD direct to the board, if clearance above capacitors allows)
  * Solder in the ESP32
  * Solder remaining components
  * Install IDC jumper shunts between the GND and DATA pins for flow sensors that will not be used, else the pin will float and detect erroneous readings
  * If the float sensor logic is to be inverted (normally-closed), a jumper shunt on those pins will be handy to silence the alarm if the power is on and the float sensors are not connected
* Configure your Arduino IDE according to the section below
* Edit the WiFi configuration and web username / password combination in 'esp32.ino' to suit your needs, then upload to the ESP32
* Power on your HydroTek and access the web app to confirm it works
* Connect your (safe, low-voltage) lamps / pumps to the relay module and connect the relay module inputs to the header pins on the PCB
  * NOTE: If in any doubt at all, use safe, low-voltage LED grow lights and nutrient pumps. Switching of mains voltages may not be at all legal where you live, your relays may not be sufficient, I advise you not to try it, and I accept no responsibility if you do.

---

### Arduino IDE Configuration:
*(rev5)*

* Go to 'File' > 'Preferences', then click the icon to the right of the text input for "Additional board manager URLs"
* Paste the following text into the field (separate existing entries by a comma and a new line):
> https://dl.espressif.com/dl/package_esp32_index.json
* Go to 'Tools' > 'Board...' > 'Boards Manager'
* Search for ESP32, and install the 'esp32 by Espressif Systems' package (version 1.0.2 at the time of writing)
* Go to 'Tools' > 'Board...' and select 'ESP32 Dev Module'
* Go to 'Sketch' > 'Include Library' > 'Manage Libraries'
* Search for 'Adafruit GFX' and install the 'Adafruit GFX Library' package by Adafruit
* Search for 'Adafruit 1306' and install the 'Adafruit SSD1306' package by Adafruit
* Search for 'dht22' and install the 'DHT sensor library' package by Adafruit
* Search for 'rtclib' and install the 'RTClib' package by Adafruit

* Go to 'Tools' > 'Port' and select the port for your connected ESP32
* Once you have edited the .ino file to suit your desired configuration, click upload to flash the ESP.

---

### Known issues:
*(rev5)*

* Have not yet designed a 3D printable case for rev 4 / 5
* The react app is included in the repo for now but will be removed in future. Makers will be able to use my hosted platform or build their own API
* The nutrient flow values are raw 'ticks' from the flow sensor, not a calculated litres or gallons value

---

### API:
*(rev5)*

The sketch sends a POST request to the URL supplied in the variable 'webEndpoint'. The POST values are as follows:

| POST variable name | description | data type the ESP will supply |
| --- | --- | --- |
| user | username for API | string |
| password | password for API | string |
| deviceID | the ID of the device* | int |
| t1 | temperature 1 | int or float |
| t2 | temperature 2 | int or float |
| h1 | humidity 1 | int or float |
| h2 | humidity 2 | int or float |
| f1 | float 1 | int |
| f2 | float 2 | int |
| l1 | flow 1 | int or float |
| l2 | flow 2 | int or float |

*The deviceID was only intended to be used if you wanted to have multiple devices use your endpoint.*

In future, makers will be able to use my hosted platform once it is complete instead of building their own.
