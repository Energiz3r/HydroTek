# HydroTek
## An IoT Hydroponics Controller

*Current revision: rev4 - see [Known Issues](#known-issues)*

(rev3 pictured)
![alt-text](https://i.imgur.com/i79Ttwah.jpg "Image")

---

### Overview:

There are four components to this repository:
* PCB cad file (created in PCBWeb)
* Firmware (sketches) for the Sparkfun Pro Micro and for the ESP8266
* React/PHP web app for displaying data captured by the device
* STL files for a 3D printed case to house the controller

The PHP / React combination was selected due to me already having a CPanel-based web hosting platform that I was familiar with. Better solutions exist, and a nodeJS-based platform would be preferred, however this was available and ready with minimal effort.

---

### To Do (not started):

* user management interface for the online component
* fetching configuration for lamp / pump timings from online so the sketch doesn't need to be altered
* alerts
* safe switching of mains, maybe through the use of an off the shelf 'switcher' of some kind
* WiFi configuration on the device instead of through the sketch

---

### Features:

* Uploads sensor data to a web server
* WiFi enabled
* 128x64 OLED display
* RTC with battery backup for control of grow light on/off periods
* Can monitor:
  * 2x DHT22 temperature / humidity sensors
  * 2x YF-S201 hall-effect liquid flow sensors, to detect nutrient flow and operation of nutrient pumps
  * 2x float switches to detect either empty nutrient tank or full run-off tank
* Can control:
  * 2x grow lights
  * 2x nutrient pumps - *(not implemented yet)*
* Buzzer / LED to alert lost WiFi signal, temperature extremes, nutrient pump faults, or full / empty nutrient tanks - *(partially implemented)*
* Online / email alerts for temperature extremes, nutrient pump faults, or full / empty nutrient tanks - *(not implemented yet)*
* I2C pH sensors *can* be connected using a kit [such as this](https://www.sparkfun.com/products/10972). pH meters are supposed to be kept in a storage solution and aren't suitable for constant monitoring, therefore this was excluded from the feature set.
* The board can be powered either through the barrel jack as designed or using a USB power source capable of 1.5A plugged into the Pro Micro.

---

### Design considerations:

* Must be maintenance-free (the device, not the hydro setup necessarily)
* Once assembled, configured, and flashed, the device must be easy to set up and use by non-technical people
* Compact size desirable. Other off-the-shelf units or open-source setups seem to be quite large
* Data easily accessible from phone / PC anywhere (rather than using ESP in built web server)

---

### Build instructions:
*(rev4)*

* To manufacture the PCB, upload the gerber files for the current PCB revision to a service such as [DirtyPCBs](http://dirtypcbs.com/store/pcbs)
* Purchase the components, as desired, that are outlined in the BOM file
* Solder the components, ensuring the following order is observed:
  * Solder in Pro Micro on the reverse side of the board
  * Solder in ESP8266 on the reverse side of the board
  * Solder in RTC ensuring clearance between battery and the header pins of the ESP8266
  * Solder in OLED using IDC headers ensuring clearance between battery and the header pins of the Pro Micro
  * Bridge J2 with solder if you need to bypass the in-built regulator on the Pro Micro (bridges RAW to VCC)
  * Bridge J3 with solder if LM7805 will not be installed (ensure regulated +5v is supplied to the PCB)
  * Bridge either side of J4 with solder, depending whether you'd like an always-on LED or the LED to be controlled by the Pro Micro
  * Solder in remaining components and headers
  * Install IDC jumper shunts between the GND and DATA pins for flow sensors that will not be used, else the pin will float and detect erroneous readings
  * If the float sensor logic is to be inverted (normally-closed), a jumper shunt to close the connection may be required to silence the alarm, if enabled, while the power is on 
* Edit the WiFi configuration and username / password combination in 'esp8266.ino' to suit your needs, then upload to the ESP8266
* Edit the configuration in 'pro-micro.ino' to suit your needs, then upload to the Pro Micro ensuring you select "5V, 16Mhz" variant
* Deploy the files found in /hydrotek-react/server to your PHP/MYSQL capable web server (see known issues)
* Edit the configuration to suit your needs in /config.php (see known issues)
* Execute the installation script found in /install.php, then delete the file (see known issues)
* Power on your HydroTek and access the web app to confirm it all works
* Connect your lamps / pumps to the relay module and connect the relay module to the relay header pins on the PCB
  * NOTE: If in any doubt at all, use safe, low-voltage LED grow lights and nutrient pumps. Switching of mains voltages may not be at all legal where you live, your relays may not be sufficient, I advise you not to try it, and I accept no responsibility if you do.

---

### Known issues:
*(rev4)*

* I have observed some Pro Micros seem to have a fault with the onboard regulator or fuse (haven't been able to track down the cause). If the Pro Micro will not power on when the PCB is supplied with power, or if the other components don't get power when the Pro Micro is supplied by USB, run a hookup wire from **J1 on the Pro Micro** to the nearby J1 pad on the PCB. J2 may need to be bridged as well.
* Have not yet designed a case for the relay module - may incorporate this into the main unit in a future revision, or offer both an integrated and remote unit.
* The PHP script is missing from the repo completely as it's hacky and not suitable for inclusion just yet.
* The react app is included, but is just as hacky and not suitable for use.
* Hardware UARTS are used to communicate between the Pro Micro and the 8266. For this reason, there is no serial output from the 8266 while communicating. Observe the output from the Pro Micro or enable dummyUpload mode on the 8266 sketch.


