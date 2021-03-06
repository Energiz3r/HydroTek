EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title "HyroTek"
Date ""
Rev "6"
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L ESP32_mini:mini_esp32 U?
U 1 1 604D9195
P 4450 1600
F 0 "U?" H 4475 1725 50  0000 C CNN
F 1 "mini_esp32" H 4475 1634 50  0000 C CNN
F 2 "" H 4600 1700 50  0001 C CNN
F 3 "" H 4600 1700 50  0001 C CNN
	1    4450 1600
	1    0    0    -1  
$EndComp
$Comp
L power:VCC #PWR?
U 1 1 604DC9E1
P 5250 1700
F 0 "#PWR?" H 5250 1550 50  0001 C CNN
F 1 "VCC" H 5265 1873 50  0000 C CNN
F 2 "" H 5250 1700 50  0001 C CNN
F 3 "" H 5250 1700 50  0001 C CNN
	1    5250 1700
	1    0    0    -1  
$EndComp
$Comp
L power:+3V3 #PWR?
U 1 1 604E0F87
P 5500 1800
F 0 "#PWR?" H 5500 1650 50  0001 C CNN
F 1 "+3V3" H 5515 1973 50  0000 C CNN
F 2 "" H 5500 1800 50  0001 C CNN
F 3 "" H 5500 1800 50  0001 C CNN
	1    5500 1800
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR?
U 1 1 604E22D7
P 5750 1900
F 0 "#PWR?" H 5750 1650 50  0001 C CNN
F 1 "GND" H 5755 1727 50  0000 C CNN
F 2 "" H 5750 1900 50  0001 C CNN
F 3 "" H 5750 1900 50  0001 C CNN
	1    5750 1900
	1    0    0    -1  
$EndComp
Wire Wire Line
	5000 1700 5250 1700
Wire Wire Line
	5000 1800 5500 1800
Wire Wire Line
	5750 1900 5300 1900
Wire Wire Line
	5000 2000 5300 2000
Wire Wire Line
	5300 2000 5300 1900
Connection ~ 5300 1900
Wire Wire Line
	5300 1900 5000 1900
Wire Wire Line
	5000 2100 5300 2100
Wire Wire Line
	5300 2100 5300 2000
Connection ~ 5300 2000
NoConn ~ 5000 2200
NoConn ~ 3950 3400
NoConn ~ 5000 2350
NoConn ~ 5000 2450
NoConn ~ 3950 3300
NoConn ~ 3950 3200
NoConn ~ 3950 2100
NoConn ~ 5000 2700
NoConn ~ 3950 2200
NoConn ~ 5000 2800
NoConn ~ 5000 2900
NoConn ~ 3950 2700
NoConn ~ 5000 3100
NoConn ~ 5000 3200
NoConn ~ 5000 3000
Text Label 3000 3100 0    50   ~ 0
LED
Wire Wire Line
	3000 3100 3950 3100
Text Label 3000 2500 0    50   ~ 0
BUZZER
Text Label 3000 4000 0    50   ~ 0
PUMP
Text Label 3000 2800 0    50   ~ 0
LAMP
Text Label 3000 2900 0    50   ~ 0
AUX1
Text Label 3000 3800 0    50   ~ 0
FLOAT1
Text Label 3000 2000 0    50   ~ 0
FLOAT2
Text Label 3000 2400 0    50   ~ 0
BTN_UP
Text Label 3000 3700 0    50   ~ 0
SCL
Text Label 3000 3600 0    50   ~ 0
SDA
Text Label 3000 2600 0    50   ~ 0
TEMP
Text Label 5500 3300 0    50   ~ 0
AUX3
Text Label 3000 4100 0    50   ~ 0
BTN_BK
Text Label 3000 3900 0    50   ~ 0
FLOW
Text Label 3000 3000 0    50   ~ 0
BTN_OK
Text Label 3000 2300 0    50   ~ 0
AUX4
Text Label 3000 1900 0    50   ~ 0
BTN_DN
Text Label 3000 1800 0    50   ~ 0
AUX2
Wire Wire Line
	3000 1800 3950 1800
Wire Wire Line
	3000 1900 3950 1900
Wire Wire Line
	3000 2000 3950 2000
Wire Wire Line
	3000 2300 3950 2300
Wire Wire Line
	3000 2400 3950 2400
Wire Wire Line
	3000 2500 3950 2500
Wire Wire Line
	3000 2600 3950 2600
Wire Wire Line
	3000 2800 3950 2800
Wire Wire Line
	3000 2900 3950 2900
Wire Wire Line
	3000 3000 3950 3000
Wire Wire Line
	3000 3600 3950 3600
Wire Wire Line
	3000 3700 3950 3700
Wire Wire Line
	3000 3800 3950 3800
Wire Wire Line
	3000 3900 3950 3900
Wire Wire Line
	3000 4000 3950 4000
Wire Wire Line
	3000 4100 3950 4100
Wire Wire Line
	5500 3300 5000 3300
NoConn ~ 3950 3500
NoConn ~ 3950 1700
$Comp
L power:PWR_FLAG #FLG?
U 1 1 604EB4B9
P 6500 1500
F 0 "#FLG?" H 6500 1575 50  0001 C CNN
F 1 "PWR_FLAG" H 6500 1673 50  0000 C CNN
F 2 "" H 6500 1500 50  0001 C CNN
F 3 "~" H 6500 1500 50  0001 C CNN
	1    6500 1500
	1    0    0    -1  
$EndComp
$Comp
L power:PWR_FLAG #FLG?
U 1 1 604EC45E
P 7000 1500
F 0 "#FLG?" H 7000 1575 50  0001 C CNN
F 1 "PWR_FLAG" H 7000 1673 50  0000 C CNN
F 2 "" H 7000 1500 50  0001 C CNN
F 3 "~" H 7000 1500 50  0001 C CNN
	1    7000 1500
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR?
U 1 1 604ECE9D
P 7000 1700
F 0 "#PWR?" H 7000 1450 50  0001 C CNN
F 1 "GND" H 7005 1527 50  0000 C CNN
F 2 "" H 7000 1700 50  0001 C CNN
F 3 "" H 7000 1700 50  0001 C CNN
	1    7000 1700
	1    0    0    -1  
$EndComp
$Comp
L power:VCC #PWR?
U 1 1 604EE080
P 6500 1900
F 0 "#PWR?" H 6500 1750 50  0001 C CNN
F 1 "VCC" H 6515 2073 50  0000 C CNN
F 2 "" H 6500 1900 50  0001 C CNN
F 3 "" H 6500 1900 50  0001 C CNN
	1    6500 1900
	1    0    0    -1  
$EndComp
Wire Wire Line
	7000 1500 7000 1700
Wire Wire Line
	6500 1500 6500 1900
$Comp
L power:+3V3 #PWR?
U 1 1 604EFDCF
P 7500 1900
F 0 "#PWR?" H 7500 1750 50  0001 C CNN
F 1 "+3V3" H 7515 2073 50  0000 C CNN
F 2 "" H 7500 1900 50  0001 C CNN
F 3 "" H 7500 1900 50  0001 C CNN
	1    7500 1900
	1    0    0    -1  
$EndComp
$Comp
L power:PWR_FLAG #FLG?
U 1 1 604F1105
P 7500 1500
F 0 "#FLG?" H 7500 1575 50  0001 C CNN
F 1 "PWR_FLAG" H 7500 1673 50  0000 C CNN
F 2 "" H 7500 1500 50  0001 C CNN
F 3 "~" H 7500 1500 50  0001 C CNN
	1    7500 1500
	1    0    0    -1  
$EndComp
Wire Wire Line
	7500 1500 7500 1900
$EndSCHEMATC
