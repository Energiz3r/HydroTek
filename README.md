# HydroTek
## An IoT Hydroponics Controller

*Current revision: rev3 - see [Known Issues](#known-issues)*

![alt-text](https://i.imgur.com/i79Ttwah.jpg "Image")

---

### Overview:

There are five components to this repository:
* Firmware (sketch) for the Sparkfun Pro Micro and for the ESP8266
* PCB cad file (created in PCBWeb)
* React web app for displaying data client-side
* PHP web app for fetching data and passing to react app server-side
* STL files for a 3D printed case to house the controller

The PHP / React combination was selected due to me already having a CPanel-based web hosting platform that I was familiar with. Better solutions are likely to exist, in particular a nodeJS-based platform would be preferred, however for rapid deployment this was what was available and ready.

---

### To Do (apart from things that are in-progress):

* pH sensor inputs
* user management for the online component
* getting configuration for lamp / pump timings from online so the sketch doesn't need to be uploaded to do it (pump control not currently implemented, lamp timing currently hardcoded in sketch)
* online / email alerts
* safe switching of mains, maybe through the use of an off the shelf 'switcher' of some kind?
* WiFi configuration on the device instead of hardcoded to the sketch... Not sure if possible without some work to shrink the code, unless we do it directly on the ESP as there's only a few % space left in the sketch on the pro micro. 

---

### Features:

* Uploads sensor data to a web server to enable online monitoring and control of hydroponics *(control not yet implemented)*
* WiFi enabled
* 128x64 OLED display
* RTC with battery backup for control of grow light on/off periods
* Can monitor:
  * 2x DHT22 temperature / humidity sensors
  * 2x YF-S201 hall-effect liquid flow sensors, to detect nutrient flow and operation of nutrient pumps
  * 2x float sensors to detect either empty nutrient tank or full run-off tank
* Can control:
  * 2x grow lights
  * 2x nutrient pumps *(not yet implemented)*
* Buzzer to alert lost WiFi signal, temperature extremes, nutrient pump faults, or full / empty nutrient tanks *(partially implemented)*
* Online alerts for temperature extremes, nutrient pump faults, or full / empty nutrient tanks *(not yet implemented)*
* Email alerts for temperature extremes, nutrient pump faults, or full / empty nutrient tanks *(not yet implemented)*

---

### Build instructions:
*(rev3)*

* To manufacture the PCB, upload the gerber files for the current PCB revision to a service such as [DirtyPCBs](http://dirtypcbs.com/store/pcbs)
* Purchase the components, as desired, that are outlined in the BOM file
* Solder the components, ensuring the following order is observed :
  * Using a dremel or other cutting tool, cut traces on underside of PCB for the data (middle) pins for both flow sensors
  * Solder in Pro Micro
  * Solder in LED1
  * Solder in R1 (if reqd)
  * Solder in C12 (if reqd)
  * Solder in ESP8266 on reverse side of PCB
  * Run hookup wires from RX0 -> TXD2(D8) and TX0 -> RXD2(D7) of Pro Micro and ESP8266, respectively
  * Run hookup wires from pins 19(A1) and 21(A3) on the Pro Micro to the flow sensor 1 and 2 data pin headers on the PCB, respectively
  * If required / desired, install the LC Filter using IDC header pins, otherwise bridge the pads using hookup wire
  * Solder in RTC using IDC headers
  * Solder in OLED using IDC headers NOTE: ensure a safe gap is left between underside of OLED and the ESP8266 headers. Trim ESP8266 headers if required
  * Solder in barrel jack if desired
  * Solder in LM7805 if desired
  * Solder in piezo buzzer
  * Bridge J1 with solder if LM7805 will not be installed - ensure regulated +5v is supplied to the PCB
  * Install IDC jumpers between the GND and DATA pins for any flow sensors that will not be used, else pin will float and detect erroneous readings
  * If the float sensor logic is to be inverted (normally-closed), a jumper to close the connection may be required to silence the alarm while the unit is powered on  
* Edit the WiFi configuration and username / password combination in 'esp8266.ino' to suit your needs, then upload to the ESP8266 ensuring you select "NodeMCU v1.0" variant
* Edit the configuration in 'pro-micro.ino' to suit your needs, then upload to the Pro Micro ensuring you select "5V, 16Mhz" variant
* Deploy the files found in /hydrotek-react/server to your PHP/MYSQL capable web server (not yet possible - see known issues)
* Edit the configuration to suit your needs in /config.php (not yet possible - see known issues)
* Execute the installation script found in /install.php, then delete the file (not yet possible - see known issues)
* Power on your HydroTek and access the web app to confirm it all works
* Connect your lamps / pumps to the relay module and connect the relay module to the relay header pins on the PCB
  * NOTE: If in any doubt at all, use safe, low-voltage LED grow lights and nutrient pumps. Switching of mains voltages may not be at all legal where you live, I advise you not to try it, and I accept no responsibility if you do.

---

### Known issues:
*(rev3)*

* Have not yet designed a case for the relay module - may incorporate this into the main unit in a future revision, or offer both an integrated and remote unit.
* The PHP script is missing from the repo completely as it's hacky and not suitable for inclusion just yet.
* The react app is included, but is just as hacky and not suitable for use.
* PCB traces for serial communication between the Pro Micro and the ESP8266 is wrong as the method used was changed; hardware UART is used instead of software serial as initially planned. Hookup wires are required.
* Due to the above, the traces for the flow sensors need to be cut and hookup wires ran to the new pins.
* The LC filter used is an inadequate replacement for the proper decoupling capacitors the LM7805 requires. Unless a known clean power supply is used, dangerous oscillation and component damage is likely to occur. The next PCB revision will include these capacitors.
* Work-arounds for the above include omission of the LM7805 and LC filter, and use of a regulated 5V supply instead, or you can simply use a micro USB cable plugged into the Pro Micro and USB power source capable of at least 1.5A (such as a phone charger).
* The LM7805 *just* provides the necessary current for the relatively power-hungry ESP8266, however runs quite hot at 12V input. The next PCB revision will provide a ground pad for soldering the LM's tab onto for heat dissipation. In testing this has proven effective enough not to warrant investigation into a beefier regulator.
* I have observed some Pro Micros seem to have a fault with the onboard regulator or fuse (haven't been able to track down the cause). If the Pro Micro will not power on when the PCB is supplied with power, or if the other components don't get power when the Pro Micro is supplied by USB, run a hookup wire from **J1 on the Pro Micro** to the nearby (square) +5V pad on the ESP8266. The next PCB revision will supply power to the Pro Micro via the RAW pin, with a pad available to run a hookup to J1 for this issue, and a jumper to solder to bridge RAW to VCC.

