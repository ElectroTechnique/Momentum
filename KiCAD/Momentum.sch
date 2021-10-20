EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A3 16535 11693
encoding utf-8
Sheet 1 2
Title "Momentum"
Date "2021-08-27"
Rev "1.0"
Comp "ElectroTechnique"
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L power:GND #PWR03
U 1 1 5F9D0123
P 2090 2135
F 0 "#PWR03" H 2090 1885 50  0001 C CNN
F 1 "GND" H 2095 1962 50  0000 C CNN
F 2 "" H 2090 2135 50  0001 C CNN
F 3 "" H 2090 2135 50  0001 C CNN
	1    2090 2135
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR04
U 1 1 5F9D01B3
P 3340 2135
F 0 "#PWR04" H 3340 1885 50  0001 C CNN
F 1 "GND" H 3345 1962 50  0000 C CNN
F 2 "" H 3340 2135 50  0001 C CNN
F 3 "" H 3340 2135 50  0001 C CNN
	1    3340 2135
	1    0    0    -1  
$EndComp
Text Label 11285 1720 0    50   ~ 0
Display_SCK
Text Label 11285 1820 0    50   ~ 0
Display_SDO
Text Label 11285 2720 0    50   ~ 0
Display_DC
Text Label 2390 1535 0    50   ~ 0
VBUS
Text Label 3640 1535 0    50   ~ 0
VBUS_HOST
Text Notes 1890 1135 0    50   ~ 0
USB Client Power
Text Notes 3190 1135 0    50   ~ 0
USB Host MIDI
Text Label 2390 1735 0    50   ~ 0
USB_D+
Text Label 9485 1920 2    50   ~ 0
USB_OUT_D+
Text Label 9485 1820 2    50   ~ 0
USB_OUT_D-
Text Label 2390 1835 0    50   ~ 0
USB_D-
NoConn ~ 2390 1935
Text Label 3640 1735 0    50   ~ 0
USB_MIDI_D+
Text Label 3640 1835 0    50   ~ 0
USB_MIDI_D-
Text Label 9485 2120 2    50   ~ 0
USB_MIDI_D-
Text Label 9485 2220 2    50   ~ 0
USB_MIDI_D+
Wire Wire Line
	1440 9685 1540 9685
Wire Wire Line
	1540 9685 1540 9335
Wire Wire Line
	1540 9335 1590 9335
Wire Wire Line
	1790 9035 2190 9035
Wire Wire Line
	2190 9035 2190 9485
Wire Wire Line
	1790 9635 1790 9885
Wire Wire Line
	1790 9885 2240 9885
Wire Wire Line
	2040 10085 2240 10085
Wire Wire Line
	2240 10085 2240 9885
Connection ~ 2240 9885
Wire Wire Line
	2240 9885 2390 9885
Wire Wire Line
	2040 9485 2190 9485
Connection ~ 2190 9485
Wire Wire Line
	2190 9485 2190 9685
Wire Wire Line
	1840 9785 1440 9785
Text Notes 1440 10285 0    50   ~ 0
MIDI In TRS protection\nfacebook.com/NeutronSound/
$Comp
L power:GND #PWR010
U 1 1 5FA46560
P 3240 9885
F 0 "#PWR010" H 3240 9635 50  0001 C CNN
F 1 "GND" H 3245 9712 50  0000 C CNN
F 2 "" H 3240 9885 50  0001 C CNN
F 3 "" H 3240 9885 50  0001 C CNN
	1    3240 9885
	1    0    0    -1  
$EndComp
$Comp
L power:+3V3 #PWR0101
U 1 1 5FA4B2A7
P 3240 9485
F 0 "#PWR0101" H 3240 9335 50  0001 C CNN
F 1 "+3V3" H 3255 9658 50  0000 C CNN
F 2 "" H 3240 9485 50  0001 C CNN
F 3 "" H 3240 9485 50  0001 C CNN
	1    3240 9485
	1    0    0    -1  
$EndComp
Text Label 3440 9685 0    50   ~ 0
MIDI_IN
Text Label 11285 3020 0    50   ~ 0
MIDI_IN
Text Label 11285 2920 0    50   ~ 0
MIDI_OUT
NoConn ~ 13420 5390
$Comp
L power:GND #PWR0102
U 1 1 5FA2EA63
P 13120 4690
F 0 "#PWR0102" H 13120 4440 50  0001 C CNN
F 1 "GND" H 12970 4615 50  0000 C CNN
F 2 "" H 13120 4690 50  0001 C CNN
F 3 "" H 13120 4690 50  0001 C CNN
	1    13120 4690
	1    0    0    -1  
$EndComp
Wire Wire Line
	13120 4690 13420 4690
Wire Wire Line
	13420 4790 13270 4790
$Comp
L power:+3V3 #PWR0103
U 1 1 5FA34005
P 13270 4790
F 0 "#PWR0103" H 13270 4640 50  0001 C CNN
F 1 "+3V3" H 13345 4840 50  0000 C CNN
F 2 "" H 13270 4790 50  0001 C CNN
F 3 "" H 13270 4790 50  0001 C CNN
	1    13270 4790
	1    0    0    -1  
$EndComp
Text Label 11285 1020 0    50   ~ 0
SDIO_CLK
Text Label 11285 1120 0    50   ~ 0
SDIO_CMD
Text Label 11285 1220 0    50   ~ 0
SDIO_DATA0
Text Label 11285 1520 0    50   ~ 0
SDIO_DATA3
$Comp
L power:+3V3 #PWR015
U 1 1 5FA42FC8
P 9485 870
F 0 "#PWR015" H 9485 720 50  0001 C CNN
F 1 "+3V3" H 9485 1020 50  0000 C CNN
F 2 "" H 9485 870 50  0001 C CNN
F 3 "" H 9485 870 50  0001 C CNN
	1    9485 870 
	1    0    0    -1  
$EndComp
Text Notes 2440 1985 0    50   ~ 0
ID: Client
$Comp
L power:GND #PWR0104
U 1 1 5FA2C183
P 9535 5220
F 0 "#PWR0104" H 9535 4970 50  0001 C CNN
F 1 "GND" H 9540 5047 50  0000 C CNN
F 2 "" H 9535 5220 50  0001 C CNN
F 3 "" H 9535 5220 50  0001 C CNN
	1    9535 5220
	1    0    0    -1  
$EndComp
Text Notes 9985 4920 0    50   Italic 0
1.5mm space underneath
Text Notes 13225 4500 0    50   Italic 0
3mm space underneath
Text Label 13420 4890 2    50   ~ 0
Display_SCK
Text Label 13420 4990 2    50   ~ 0
Display_SDO
$Comp
L power:GND #PWR017
U 1 1 5FA5120D
P 1760 6705
F 0 "#PWR017" H 1760 6455 50  0001 C CNN
F 1 "GND" H 1765 6532 50  0000 C CNN
F 2 "" H 1760 6705 50  0001 C CNN
F 3 "" H 1760 6705 50  0001 C CNN
	1    1760 6705
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR020
U 1 1 5FA8E6A7
P 2290 8185
F 0 "#PWR020" H 2290 7935 50  0001 C CNN
F 1 "GND" H 2295 8012 50  0000 C CNN
F 2 "" H 2290 8185 50  0001 C CNN
F 3 "" H 2290 8185 50  0001 C CNN
	1    2290 8185
	1    0    0    -1  
$EndComp
Wire Wire Line
	2290 3135 2290 3785
Connection ~ 2290 3785
Wire Wire Line
	2290 3785 2290 4435
Connection ~ 2290 4435
Wire Wire Line
	2290 4435 2290 5085
Connection ~ 2290 5085
Wire Wire Line
	2290 5085 2290 5685
Connection ~ 2290 5685
Wire Wire Line
	2290 5685 2290 6385
Connection ~ 2290 6385
Wire Wire Line
	2290 6385 2290 7035
Connection ~ 2290 7035
Wire Wire Line
	2290 7035 2290 7735
Connection ~ 2290 7735
Wire Wire Line
	2290 7735 2290 8185
$Comp
L power:GND #PWR018
U 1 1 5FA93218
P 13540 9435
F 0 "#PWR018" H 13540 9185 50  0001 C CNN
F 1 "GND" H 13545 9262 50  0000 C CNN
F 2 "" H 13540 9435 50  0001 C CNN
F 3 "" H 13540 9435 50  0001 C CNN
	1    13540 9435
	1    0    0    -1  
$EndComp
Connection ~ 13540 9435
Text Notes 14140 9635 0    50   ~ 0
Mounting holes for front panel
Wire Wire Line
	13540 9435 13840 9435
Connection ~ 13840 9435
Wire Wire Line
	13840 9435 14140 9435
Connection ~ 14140 9435
Wire Wire Line
	14140 9435 14440 9435
Connection ~ 14440 9435
Wire Wire Line
	14440 9435 14740 9435
$Comp
L power:GND #PWR021
U 1 1 5FA8F446
P 5875 3475
F 0 "#PWR021" H 5875 3225 50  0001 C CNN
F 1 "GND" H 5880 3302 50  0000 C CNN
F 2 "" H 5875 3475 50  0001 C CNN
F 3 "" H 5875 3475 50  0001 C CNN
	1    5875 3475
	1    0    0    -1  
$EndComp
Wire Wire Line
	5875 3325 5875 3475
Wire Wire Line
	5275 3225 5175 3225
Wire Wire Line
	5175 3225 5175 3475
Wire Wire Line
	5175 3475 5875 3475
Connection ~ 5875 3475
$Comp
L power:GND #PWR023
U 1 1 5FAA3CC9
P 5940 6945
F 0 "#PWR023" H 5940 6695 50  0001 C CNN
F 1 "GND" H 5945 6772 50  0000 C CNN
F 2 "" H 5940 6945 50  0001 C CNN
F 3 "" H 5940 6945 50  0001 C CNN
	1    5940 6945
	1    0    0    -1  
$EndComp
Wire Wire Line
	5940 6795 5940 6945
Wire Wire Line
	5340 6695 5240 6695
Wire Wire Line
	5240 6695 5240 6945
Wire Wire Line
	5240 6945 5940 6945
Connection ~ 5940 6945
$Comp
L power:GND #PWR024
U 1 1 5FAA8883
P 5980 8590
F 0 "#PWR024" H 5980 8340 50  0001 C CNN
F 1 "GND" H 5985 8417 50  0000 C CNN
F 2 "" H 5980 8590 50  0001 C CNN
F 3 "" H 5980 8590 50  0001 C CNN
	1    5980 8590
	1    0    0    -1  
$EndComp
Wire Wire Line
	5980 8440 5980 8590
Wire Wire Line
	5380 8340 5280 8340
Wire Wire Line
	5280 8340 5280 8590
Wire Wire Line
	5280 8590 5980 8590
Connection ~ 5980 8590
$Comp
L power:GND #PWR022
U 1 1 5FAAFB4B
P 5875 5105
F 0 "#PWR022" H 5875 4855 50  0001 C CNN
F 1 "GND" H 5880 4932 50  0000 C CNN
F 2 "" H 5875 5105 50  0001 C CNN
F 3 "" H 5875 5105 50  0001 C CNN
	1    5875 5105
	1    0    0    -1  
$EndComp
Wire Wire Line
	5875 4955 5875 5105
Wire Wire Line
	5275 4855 5175 4855
Wire Wire Line
	5175 4855 5175 5105
Wire Wire Line
	5175 5105 5875 5105
Connection ~ 5875 5105
Wire Wire Line
	15040 9435 14740 9435
Connection ~ 14740 9435
Text Label 3090 3035 0    50   ~ 0
LED1R
Text Label 3090 3235 0    50   ~ 0
LED1G
Text Label 5020 3125 2    50   ~ 0
Encoder1A
Text Label 5020 3325 2    50   ~ 0
Encoder1B
Text Label 6175 3125 0    50   ~ 0
Encoder1Sw
NoConn ~ 1440 9585
$Comp
L power:GND #PWR033
U 1 1 5FABA527
P 5270 9740
F 0 "#PWR033" H 5270 9490 50  0001 C CNN
F 1 "GND" H 5275 9567 50  0000 C CNN
F 2 "" H 5270 9740 50  0001 C CNN
F 3 "" H 5270 9740 50  0001 C CNN
	1    5270 9740
	1    0    0    -1  
$EndComp
$Comp
L power:+3V3 #PWR032
U 1 1 5FAE9A83
P 4620 9630
F 0 "#PWR032" H 4620 9480 50  0001 C CNN
F 1 "+3V3" H 4635 9803 50  0000 C CNN
F 2 "" H 4620 9630 50  0001 C CNN
F 3 "" H 4620 9630 50  0001 C CNN
	1    4620 9630
	1    0    0    -1  
$EndComp
Text Label 5070 9540 2    50   ~ 0
MIDI_OUT
$Comp
L power:GND #PWR028
U 1 1 5FB34526
P 9095 6555
F 0 "#PWR028" H 9095 6305 50  0001 C CNN
F 1 "GND" H 9245 6505 50  0000 C CNN
F 2 "" H 9095 6555 50  0001 C CNN
F 3 "" H 9095 6555 50  0001 C CNN
	1    9095 6555
	1    0    0    -1  
$EndComp
Wire Wire Line
	8845 6355 8845 6505
Connection ~ 8845 6355
Wire Wire Line
	9095 6355 8845 6355
Wire Wire Line
	8845 6305 8845 6355
$Comp
L power:+3V3 #PWR09
U 1 1 5FB29EAE
P 8845 6305
F 0 "#PWR09" H 8845 6155 50  0001 C CNN
F 1 "+3V3" H 8860 6478 50  0000 C CNN
F 2 "" H 8845 6305 50  0001 C CNN
F 3 "" H 8845 6305 50  0001 C CNN
	1    8845 6305
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR029
U 1 1 5FB2917F
P 8845 8405
F 0 "#PWR029" H 8845 8155 50  0001 C CNN
F 1 "GND" H 8850 8232 50  0000 C CNN
F 2 "" H 8845 8405 50  0001 C CNN
F 3 "" H 8845 8405 50  0001 C CNN
	1    8845 8405
	1    0    0    -1  
$EndComp
Wire Wire Line
	8845 8405 8345 8405
Wire Wire Line
	8345 8405 8345 8105
Connection ~ 8845 8405
NoConn ~ 9345 6905
Text Label 8345 6905 2    50   ~ 0
Encoder1A
Text Label 5015 4750 2    50   ~ 0
Encoder2A
Text Label 5015 4950 2    50   ~ 0
Encoder2B
Text Label 5080 6590 2    50   ~ 0
Encoder3A
Text Label 5080 6790 2    50   ~ 0
Encoder3B
Text Label 5120 8235 2    50   ~ 0
Encoder4A
Text Label 5120 8435 2    50   ~ 0
Encoder4B
Text Label 6175 4755 0    50   ~ 0
Encoder2Sw
Text Label 6240 6595 0    50   ~ 0
Encoder3Sw
Text Label 6280 8240 0    50   ~ 0
Encoder4Sw
Text Label 8345 7005 2    50   ~ 0
Encoder2A
Text Label 8345 7105 2    50   ~ 0
Encoder3A
Text Label 8345 7205 2    50   ~ 0
Encoder4A
$Comp
L power:GND #PWR040
U 1 1 5FBCB076
P 11300 6635
F 0 "#PWR040" H 11300 6385 50  0001 C CNN
F 1 "GND" H 11450 6585 50  0000 C CNN
F 2 "" H 11300 6635 50  0001 C CNN
F 3 "" H 11300 6635 50  0001 C CNN
	1    11300 6635
	1    0    0    -1  
$EndComp
Wire Wire Line
	11050 6435 11050 6585
Connection ~ 11050 6435
Wire Wire Line
	11300 6435 11050 6435
Wire Wire Line
	11050 6385 11050 6435
$Comp
L power:+3V3 #PWR036
U 1 1 5FBCBD54
P 11050 6385
F 0 "#PWR036" H 11050 6235 50  0001 C CNN
F 1 "+3V3" H 11065 6558 50  0000 C CNN
F 2 "" H 11050 6385 50  0001 C CNN
F 3 "" H 11050 6385 50  0001 C CNN
	1    11050 6385
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR049
U 1 1 5FBCBD5E
P 11050 8485
F 0 "#PWR049" H 11050 8235 50  0001 C CNN
F 1 "GND" H 11055 8312 50  0000 C CNN
F 2 "" H 11050 8485 50  0001 C CNN
F 3 "" H 11050 8485 50  0001 C CNN
	1    11050 8485
	1    0    0    -1  
$EndComp
Wire Wire Line
	11050 8485 10550 8485
Wire Wire Line
	10550 8485 10550 8185
Connection ~ 11050 8485
NoConn ~ 11550 6985
Text Label 10550 6985 2    50   ~ 0
Encoder1B
Text Label 10550 7085 2    50   ~ 0
Encoder2B
Text Label 10550 7185 2    50   ~ 0
Encoder3B
Text Label 10550 7285 2    50   ~ 0
Encoder4B
$Comp
L power:GND #PWR041
U 1 1 5FBD6534
P 8865 9410
F 0 "#PWR041" H 8865 9160 50  0001 C CNN
F 1 "GND" H 9015 9360 50  0000 C CNN
F 2 "" H 8865 9410 50  0001 C CNN
F 3 "" H 8865 9410 50  0001 C CNN
	1    8865 9410
	1    0    0    -1  
$EndComp
$Comp
L power:+3V3 #PWR037
U 1 1 5FBD7298
P 8365 9135
F 0 "#PWR037" H 8365 8985 50  0001 C CNN
F 1 "+3V3" H 8380 9308 50  0000 C CNN
F 2 "" H 8365 9135 50  0001 C CNN
F 3 "" H 8365 9135 50  0001 C CNN
	1    8365 9135
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR050
U 1 1 5FBD72A2
P 8365 10685
F 0 "#PWR050" H 8365 10435 50  0001 C CNN
F 1 "GND" H 8370 10512 50  0000 C CNN
F 2 "" H 8365 10685 50  0001 C CNN
F 3 "" H 8365 10685 50  0001 C CNN
	1    8365 10685
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR042
U 1 1 5FBE808B
P 13835 6500
F 0 "#PWR042" H 13835 6250 50  0001 C CNN
F 1 "GND" H 13985 6450 50  0000 C CNN
F 2 "" H 13835 6500 50  0001 C CNN
F 3 "" H 13835 6500 50  0001 C CNN
	1    13835 6500
	1    0    0    -1  
$EndComp
Wire Wire Line
	13585 6300 13585 6450
Connection ~ 13585 6300
Wire Wire Line
	13835 6300 13585 6300
Wire Wire Line
	13585 6250 13585 6300
$Comp
L power:+3V3 #PWR038
U 1 1 5FBE8E75
P 13585 6250
F 0 "#PWR038" H 13585 6100 50  0001 C CNN
F 1 "+3V3" H 13600 6423 50  0000 C CNN
F 2 "" H 13585 6250 50  0001 C CNN
F 3 "" H 13585 6250 50  0001 C CNN
	1    13585 6250
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR051
U 1 1 5FBE8E7F
P 13585 8350
F 0 "#PWR051" H 13585 8100 50  0001 C CNN
F 1 "GND" H 13590 8177 50  0000 C CNN
F 2 "" H 13585 8350 50  0001 C CNN
F 3 "" H 13585 8350 50  0001 C CNN
	1    13585 8350
	1    0    0    -1  
$EndComp
Wire Wire Line
	13585 8350 13085 8350
Wire Wire Line
	13085 8350 13085 8050
Connection ~ 13585 8350
NoConn ~ 14085 6850
Text Label 13085 6850 2    50   ~ 0
Encoder1Sw
Text Label 13085 6950 2    50   ~ 0
Encoder2Sw
Text Label 13085 7050 2    50   ~ 0
Encoder3Sw
Text Label 13085 7150 2    50   ~ 0
Encoder4Sw
Text Label 8345 7305 2    50   ~ 0
Button1
Text Label 10550 7385 2    50   ~ 0
Button2
Text Label 13085 7250 2    50   ~ 0
Button3
Text Label 8345 7405 2    50   ~ 0
Button4
Text Label 10550 7485 2    50   ~ 0
Button5
Text Label 13085 7350 2    50   ~ 0
Button6
Text Label 8345 7505 2    50   ~ 0
Button7
Text Label 10550 7585 2    50   ~ 0
Button8
Text Label 8345 7605 2    50   ~ 0
ButtonUp
Text Label 10550 7685 2    50   ~ 0
ButtonDown
Text Label 3090 3685 0    50   ~ 0
LED2R
Text Label 3090 3885 0    50   ~ 0
LED2G
Text Label 3090 4335 0    50   ~ 0
LED3R
Text Label 3090 4535 0    50   ~ 0
LED3G
Text Label 3090 4985 0    50   ~ 0
LED4R
Text Label 3090 5185 0    50   ~ 0
LED4G
Text Label 3090 5585 0    50   ~ 0
LED5R
Text Label 3090 5785 0    50   ~ 0
LED5G
Text Label 3090 6285 0    50   ~ 0
LED6R
Text Label 3090 6485 0    50   ~ 0
LED6G
Text Label 3090 6935 0    50   ~ 0
LED7R
Text Label 3090 7135 0    50   ~ 0
LED7G
Text Label 3090 7635 0    50   ~ 0
LED8R
Text Label 3090 7835 0    50   ~ 0
LED8G
$Comp
L power:GND #PWR056
U 1 1 5FCCAE47
P 9485 4720
F 0 "#PWR056" H 9485 4470 50  0001 C CNN
F 1 "GND" H 9490 4547 50  0000 C CNN
F 2 "" H 9485 4720 50  0001 C CNN
F 3 "" H 9485 4720 50  0001 C CNN
	1    9485 4720
	1    0    0    -1  
$EndComp
Text Label 9485 1720 2    50   ~ 0
VBUS
$Comp
L power:GND #PWR055
U 1 1 5FCD0CD1
P 6720 1335
F 0 "#PWR055" H 6720 1085 50  0001 C CNN
F 1 "GND" H 6725 1162 50  0000 C CNN
F 2 "" H 6720 1335 50  0001 C CNN
F 3 "" H 6720 1335 50  0001 C CNN
	1    6720 1335
	1    0    0    -1  
$EndComp
Text Label 5820 1035 2    50   ~ 0
VBUS
Connection ~ 7270 1035
Connection ~ 7420 1035
Wire Wire Line
	7420 1035 7270 1035
Connection ~ 7770 1035
Wire Wire Line
	7770 1235 7620 1235
Connection ~ 7420 1235
Wire Wire Line
	7420 1235 7270 1235
$Comp
L power:GND #PWR054
U 1 1 5FCFBA10
P 7420 1235
F 0 "#PWR054" H 7420 985 50  0001 C CNN
F 1 "GND" H 7425 1062 50  0000 C CNN
F 2 "" H 7420 1235 50  0001 C CNN
F 3 "" H 7420 1235 50  0001 C CNN
	1    7420 1235
	1    0    0    -1  
$EndComp
Wire Wire Line
	9485 870  9485 1020
Wire Wire Line
	7420 1035 7620 1035
Wire Wire Line
	7620 1035 7770 1035
Wire Wire Line
	7620 1235 7420 1235
Wire Wire Line
	6420 1035 6020 1035
Connection ~ 5870 1035
Wire Wire Line
	5870 1035 5820 1035
Connection ~ 6020 1035
Wire Wire Line
	6020 1035 5870 1035
Wire Wire Line
	6020 1235 5870 1235
$Comp
L power:GND #PWR053
U 1 1 5FD24C1B
P 5870 1235
F 0 "#PWR053" H 5870 985 50  0001 C CNN
F 1 "GND" H 5875 1062 50  0000 C CNN
F 2 "" H 5870 1235 50  0001 C CNN
F 3 "" H 5870 1235 50  0001 C CNN
	1    5870 1235
	1    0    0    -1  
$EndComp
Connection ~ 5870 1235
Text Label 13420 5090 2    50   ~ 0
Display_RES
Text Label 13420 5190 2    50   ~ 0
Display_DC
Text Label 13420 5290 2    50   ~ 0
Display_BL
NoConn ~ 9485 4220
NoConn ~ 9485 1520
NoConn ~ 11285 2220
NoConn ~ 11285 2320
NoConn ~ 9485 2420
NoConn ~ 9485 2520
NoConn ~ 9485 2720
NoConn ~ 9485 2820
NoConn ~ 9485 3920
NoConn ~ 9485 4020
NoConn ~ 11285 3320
NoConn ~ 11285 3420
NoConn ~ 11285 3120
NoConn ~ 11285 3220
NoConn ~ 11285 2520
Text Label 11285 3620 0    50   ~ 0
SR1_SerialOut
Text Label 11285 3720 0    50   ~ 0
SR2_SerialOut
Text Label 11285 3820 0    50   ~ 0
SR3_SerialOut
Text Label 11285 3920 0    50   ~ 0
SR4_SerialIn
Text Label 11285 4020 0    50   ~ 0
SR5_SerialIn
Wire Wire Line
	8865 9210 8365 9210
Connection ~ 8365 9210
Wire Wire Line
	8365 9210 8365 9385
Wire Wire Line
	8365 9135 8365 9210
$Comp
L power:GND #PWR043
U 1 1 5FBD5FC5
P 10640 9435
F 0 "#PWR043" H 10640 9185 50  0001 C CNN
F 1 "GND" H 10790 9385 50  0000 C CNN
F 2 "" H 10640 9435 50  0001 C CNN
F 3 "" H 10640 9435 50  0001 C CNN
	1    10640 9435
	1    0    0    -1  
$EndComp
$Comp
L power:+3V3 #PWR039
U 1 1 5FBD704F
P 10140 9160
F 0 "#PWR039" H 10140 9010 50  0001 C CNN
F 1 "+3V3" H 10155 9333 50  0000 C CNN
F 2 "" H 10140 9160 50  0001 C CNN
F 3 "" H 10140 9160 50  0001 C CNN
	1    10140 9160
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR046
U 1 1 5FBD7059
P 10140 10710
F 0 "#PWR046" H 10140 10460 50  0001 C CNN
F 1 "GND" H 10145 10537 50  0000 C CNN
F 2 "" H 10140 10710 50  0001 C CNN
F 3 "" H 10140 10710 50  0001 C CNN
	1    10140 10710
	1    0    0    -1  
$EndComp
Wire Wire Line
	10640 9235 10140 9235
Connection ~ 10140 9235
Wire Wire Line
	10140 9235 10140 9410
Wire Wire Line
	10140 9160 10140 9235
NoConn ~ 10540 10510
NoConn ~ 8765 10485
Text Label 8765 9585 0    50   ~ 0
LED1R
Text Label 10540 9610 0    50   ~ 0
LED1G
Text Label 8765 9685 0    50   ~ 0
LED2R
Text Label 10540 9710 0    50   ~ 0
LED2G
Text Label 8765 9785 0    50   ~ 0
LED3R
Text Label 10540 9810 0    50   ~ 0
LED3G
Text Label 8765 9885 0    50   ~ 0
LED4R
Text Label 8765 9985 0    50   ~ 0
LED5R
Text Label 8765 10085 0    50   ~ 0
LED6R
Text Label 8765 10185 0    50   ~ 0
LED7R
Text Label 8765 10285 0    50   ~ 0
LED8R
Text Label 10540 9910 0    50   ~ 0
LED4G
Text Label 10540 10010 0    50   ~ 0
LED5G
Text Label 10540 10110 0    50   ~ 0
LED6G
Text Label 10540 10210 0    50   ~ 0
LED7G
Text Label 10540 10310 0    50   ~ 0
LED8G
Wire Wire Line
	9740 10210 9740 10685
Wire Wire Line
	9740 10685 10140 10685
Wire Wire Line
	10140 10685 10140 10710
Connection ~ 10140 10710
Wire Wire Line
	7965 10185 7965 10685
Wire Wire Line
	7965 10685 8365 10685
Connection ~ 8365 10685
Wire Wire Line
	7965 9885 7790 9885
Wire Wire Line
	7790 9885 7790 9210
Wire Wire Line
	7790 9210 8365 9210
Wire Wire Line
	10140 9235 9515 9235
Wire Wire Line
	9515 9235 9515 9910
Wire Wire Line
	9515 9910 9740 9910
Text Label 8345 8005 2    50   ~ 0
SR_CLK
Text Label 10550 8085 2    50   ~ 0
SR_CLK
Text Label 13085 7950 2    50   ~ 0
SR_CLK
Text Label 9345 6805 0    50   ~ 0
SR1_SerialOut
Text Label 7965 9585 2    50   ~ 0
SR4_SerialIn
Text Label 9740 9610 2    50   ~ 0
SR5_SerialIn
NoConn ~ 10550 6885
NoConn ~ 8345 6805
NoConn ~ 13085 6750
Text Label 11550 6885 0    50   ~ 0
SR2_SerialOut
Text Label 14085 6750 0    50   ~ 0
SR3_SerialOut
Text Label 10550 7885 2    50   ~ 0
SR_Shift
Text Label 13085 7750 2    50   ~ 0
SR_Shift
Text Label 8345 7805 2    50   ~ 0
SR_Shift
Text Label 7965 9785 2    50   ~ 0
SR_CLK2
Text Label 9740 9810 2    50   ~ 0
SR_CLK2
Wire Wire Line
	13085 7550 13085 7450
$Comp
L power:GND #PWR044
U 1 1 5FAF585B
P 13085 7550
F 0 "#PWR044" H 13085 7300 50  0001 C CNN
F 1 "GND" H 12935 7500 50  0000 C CNN
F 2 "" H 13085 7550 50  0001 C CNN
F 3 "" H 13085 7550 50  0001 C CNN
	1    13085 7550
	1    0    0    -1  
$EndComp
Connection ~ 13085 7550
Text Label 7965 10085 2    50   ~ 0
SR_Shift2
Text Label 9740 10110 2    50   ~ 0
SR_Shift2
Text Label 11285 4120 0    50   ~ 0
SR_Shift
Text Label 11285 4220 0    50   ~ 0
SR_CLK
NoConn ~ 11285 4720
NoConn ~ 11285 4620
NoConn ~ 11285 4520
Text Label 11285 2420 0    50   ~ 0
Display_BL
Text Label 11285 2620 0    50   ~ 0
Display_RES
NoConn ~ 9485 3820
NoConn ~ 11285 1920
NoConn ~ 11285 2020
Wire Wire Line
	2990 9885 3090 9885
NoConn ~ 2990 9585
Connection ~ 7620 1035
Connection ~ 7620 1235
Wire Wire Line
	7770 1035 8270 1035
Wire Wire Line
	2990 9485 2990 9260
Wire Wire Line
	3240 9685 3440 9685
Wire Wire Line
	3240 9685 2990 9685
Connection ~ 3240 9685
Wire Wire Line
	3090 9835 3090 9885
Connection ~ 3090 9885
Wire Wire Line
	3090 9885 3240 9885
Wire Wire Line
	3090 9635 3090 9485
Wire Wire Line
	3090 9485 2990 9485
Connection ~ 2990 9485
Text Notes 3390 9435 0    50   ~ 0
6N137 has \nopen collector/drain
Text Label 2990 9260 0    50   ~ 0
VBUS
Text Label 1130 6510 2    50   ~ 0
ButtonDown
Text Label 1130 6160 2    50   ~ 0
ButtonUp
Text Label 1130 5660 2    50   ~ 0
Button8
Text Label 1130 5310 2    50   ~ 0
Button7
Text Label 1130 4960 2    50   ~ 0
Button6
Text Label 1130 4610 2    50   ~ 0
Button5
Text Label 1130 4260 2    50   ~ 0
Button4
Text Label 1130 3910 2    50   ~ 0
Button3
Text Label 1130 3560 2    50   ~ 0
Button2
Text Label 1130 3160 2    50   ~ 0
Button1
Wire Wire Line
	1760 3155 1760 3555
Connection ~ 1760 3555
Wire Wire Line
	1760 3555 1760 3905
Connection ~ 1760 3905
Wire Wire Line
	1760 3905 1760 4255
Connection ~ 1760 4255
Wire Wire Line
	1760 4255 1760 4605
Connection ~ 1760 4605
Wire Wire Line
	1760 4605 1760 4955
Connection ~ 1760 4955
Wire Wire Line
	1760 4955 1760 5305
Connection ~ 1760 5305
Wire Wire Line
	1760 5305 1760 5655
Connection ~ 1760 5655
Wire Wire Line
	1760 5655 1760 6155
Connection ~ 1760 6155
Wire Wire Line
	1760 6155 1760 6505
Connection ~ 1760 6505
Wire Wire Line
	1760 6505 1760 6705
Wire Wire Line
	3340 2135 3240 2135
NoConn ~ 9485 1420
Wire Wire Line
	7020 1035 7270 1035
NoConn ~ 9485 3220
NoConn ~ 9485 3620
NoConn ~ 9485 3720
$Comp
L power:GND #PWR01
U 1 1 61083D3B
P 7060 2695
F 0 "#PWR01" H 7060 2445 50  0001 C CNN
F 1 "GND" H 7065 2522 50  0000 C CNN
F 2 "" H 7060 2695 50  0001 C CNN
F 3 "" H 7060 2695 50  0001 C CNN
	1    7060 2695
	1    0    0    -1  
$EndComp
Text Label 7060 1895 0    50   ~ 0
VBUS
Text Label 6660 2395 2    50   ~ 0
USB_D-
Text Label 7460 2395 0    50   ~ 0
USB_D+
Text Label 6660 2195 2    50   ~ 0
USB_OUT_D-
Text Label 7460 2195 0    50   ~ 0
USB_OUT_D+
$Sheet
S 12760 1100 1250 535 
U 6107E652
F0 "Audio" 50
F1 "Audio.sch" 50
F2 "VUSB" I L 12760 1295 50 
$EndSheet
Text Label 13100 2730 2    50   ~ 0
SDIO_CMD
Text Label 13100 2930 2    50   ~ 0
SDIO_CLK
Text Label 13100 3130 2    50   ~ 0
SDIO_DATA0
Text Label 13100 2630 2    50   ~ 0
SDIO_DATA3
$Comp
L power:GND #PWR05
U 1 1 6108F21B
P 14800 3430
F 0 "#PWR05" H 14800 3180 50  0001 C CNN
F 1 "GND" H 14805 3260 50  0000 C CNN
F 2 "" H 14800 3430 50  0001 C CNN
F 3 "" H 14800 3430 50  0001 C CNN
	1    14800 3430
	1    0    0    -1  
$EndComp
$Comp
L power:+3V3 #PWR02
U 1 1 6108F990
P 12585 2825
F 0 "#PWR02" H 12585 2675 50  0001 C CNN
F 1 "+3V3" H 12585 2975 50  0000 C CNN
F 2 "" H 12585 2825 50  0001 C CNN
F 3 "" H 12585 2825 50  0001 C CNN
	1    12585 2825
	1    0    0    -1  
$EndComp
Text Label 12610 1295 2    50   ~ 0
VBUS
Wire Wire Line
	12760 1295 12610 1295
Text GLabel 9485 3020 0    50   Input ~ 0
AUD_MCLK
Text GLabel 9485 3420 0    50   Input ~ 0
AUD_BCLK
Text GLabel 9485 3120 0    50   Input ~ 0
AUD_DIN
Text GLabel 9485 3320 0    50   Input ~ 0
AUD_LRCLK
Text Label 7835 4670 0    50   ~ 0
VBUS_HOST
Text Label 7535 4670 2    50   ~ 0
VBUS
Wire Wire Line
	5070 9640 4620 9640
Wire Wire Line
	4620 9640 4620 9630
Wire Wire Line
	7835 4670 7835 5070
Wire Wire Line
	7835 5070 7840 5070
Wire Wire Line
	7535 4670 7535 5070
Wire Wire Line
	7535 5070 7540 5070
Text Label 13100 2530 2    50   ~ 0
SDIO_DATA2
Text Label 13100 3230 2    50   ~ 0
SDIO_DATA1
NoConn ~ 13100 3330
Wire Wire Line
	12585 2825 13100 2825
Wire Wire Line
	13100 2825 13100 2830
$Comp
L power:GND #PWR0113
U 1 1 610F7DA5
P 12520 3030
F 0 "#PWR0113" H 12520 2780 50  0001 C CNN
F 1 "GND" H 12525 2860 50  0000 C CNN
F 2 "" H 12520 3030 50  0001 C CNN
F 3 "" H 12520 3030 50  0001 C CNN
	1    12520 3030
	1    0    0    -1  
$EndComp
Wire Wire Line
	13100 3030 12520 3030
Text Label 11285 1420 0    50   ~ 0
SDIO_DATA2
Text Label 11285 1320 0    50   ~ 0
SDIO_DATA1
Wire Wire Line
	9325 1120 9485 1120
$Comp
L power:+3V3 #PWR0114
U 1 1 61142637
P 8270 1035
F 0 "#PWR0114" H 8270 885 50  0001 C CNN
F 1 "+3V3" H 8270 1185 50  0000 C CNN
F 2 "" H 8270 1035 50  0001 C CNN
F 3 "" H 8270 1035 50  0001 C CNN
	1    8270 1035
	1    0    0    -1  
$EndComp
Wire Wire Line
	9325 1220 9485 1220
Wire Wire Line
	5380 8440 5365 8440
Wire Wire Line
	5120 8440 5120 8435
Wire Wire Line
	5380 8240 5215 8240
Wire Wire Line
	5120 8240 5120 8235
Wire Wire Line
	5340 6795 5330 6795
Wire Wire Line
	5080 6795 5080 6790
Wire Wire Line
	5340 6595 5180 6595
Wire Wire Line
	5080 6595 5080 6590
Wire Wire Line
	5275 4955 5250 4955
Wire Wire Line
	5015 4955 5015 4950
Wire Wire Line
	5015 4755 5015 4750
Wire Wire Line
	5020 3125 5080 3125
Wire Wire Line
	5275 3325 5230 3325
Wire Wire Line
	5875 3125 5910 3125
Wire Wire Line
	6175 4755 5930 4755
$Comp
L power:+3V3 #PWR06
U 1 1 61209995
P 5910 2595
F 0 "#PWR06" H 5910 2445 50  0001 C CNN
F 1 "+3V3" H 5910 2745 50  0000 C CNN
F 2 "" H 5910 2595 50  0001 C CNN
F 3 "" H 5910 2595 50  0001 C CNN
	1    5910 2595
	1    0    0    -1  
$EndComp
$Comp
L power:+3V3 #PWR07
U 1 1 6120A20F
P 5080 2605
F 0 "#PWR07" H 5080 2455 50  0001 C CNN
F 1 "+3V3" H 5080 2755 50  0000 C CNN
F 2 "" H 5080 2605 50  0001 C CNN
F 3 "" H 5080 2605 50  0001 C CNN
	1    5080 2605
	1    0    0    -1  
$EndComp
Wire Wire Line
	5080 2605 5080 2675
Wire Wire Line
	5080 2675 5230 2675
Connection ~ 5080 2675
Wire Wire Line
	5230 2875 5230 3325
Connection ~ 5230 3325
Wire Wire Line
	5230 3325 5020 3325
Wire Wire Line
	5080 2875 5080 3125
Connection ~ 5080 3125
Wire Wire Line
	5080 3125 5275 3125
Wire Wire Line
	5910 2595 5910 2655
Wire Wire Line
	5910 2855 5910 3125
Connection ~ 5910 3125
Wire Wire Line
	5910 3125 6175 3125
$Comp
L power:+3V3 #PWR08
U 1 1 6123FE6B
P 1250 2840
F 0 "#PWR08" H 1250 2690 50  0001 C CNN
F 1 "+3V3" H 1250 2990 50  0000 C CNN
F 2 "" H 1250 2840 50  0001 C CNN
F 3 "" H 1250 2840 50  0001 C CNN
	1    1250 2840
	1    0    0    -1  
$EndComp
$Comp
L power:+3V3 #PWR011
U 1 1 612643E7
P 5930 4225
F 0 "#PWR011" H 5930 4075 50  0001 C CNN
F 1 "+3V3" H 5930 4375 50  0000 C CNN
F 2 "" H 5930 4225 50  0001 C CNN
F 3 "" H 5930 4225 50  0001 C CNN
	1    5930 4225
	1    0    0    -1  
$EndComp
$Comp
L power:+3V3 #PWR012
U 1 1 612643ED
P 5100 4235
F 0 "#PWR012" H 5100 4085 50  0001 C CNN
F 1 "+3V3" H 5100 4385 50  0000 C CNN
F 2 "" H 5100 4235 50  0001 C CNN
F 3 "" H 5100 4235 50  0001 C CNN
	1    5100 4235
	1    0    0    -1  
$EndComp
Wire Wire Line
	5100 4235 5100 4305
Wire Wire Line
	5100 4305 5250 4305
Connection ~ 5100 4305
Wire Wire Line
	5930 4225 5930 4285
Wire Wire Line
	5930 4485 5930 4755
Connection ~ 5930 4755
Wire Wire Line
	5930 4755 5875 4755
Wire Wire Line
	5250 4505 5250 4955
Connection ~ 5250 4955
Wire Wire Line
	5250 4955 5015 4955
Wire Wire Line
	5100 4505 5100 4755
Wire Wire Line
	5015 4755 5100 4755
Connection ~ 5100 4755
Wire Wire Line
	5100 4755 5275 4755
$Comp
L power:+3V3 #PWR013
U 1 1 61289FE6
P 6010 6065
F 0 "#PWR013" H 6010 5915 50  0001 C CNN
F 1 "+3V3" H 6010 6215 50  0000 C CNN
F 2 "" H 6010 6065 50  0001 C CNN
F 3 "" H 6010 6065 50  0001 C CNN
	1    6010 6065
	1    0    0    -1  
$EndComp
$Comp
L power:+3V3 #PWR014
U 1 1 61289FF0
P 5180 6075
F 0 "#PWR014" H 5180 5925 50  0001 C CNN
F 1 "+3V3" H 5180 6225 50  0000 C CNN
F 2 "" H 5180 6075 50  0001 C CNN
F 3 "" H 5180 6075 50  0001 C CNN
	1    5180 6075
	1    0    0    -1  
$EndComp
Wire Wire Line
	5180 6075 5180 6145
Wire Wire Line
	5180 6145 5330 6145
Connection ~ 5180 6145
Wire Wire Line
	6010 6065 6010 6125
Wire Wire Line
	6010 6325 6010 6595
Wire Wire Line
	5330 6345 5330 6795
Wire Wire Line
	5180 6345 5180 6595
Connection ~ 5180 6595
Wire Wire Line
	5180 6595 5080 6595
Connection ~ 5330 6795
Wire Wire Line
	5330 6795 5080 6795
Wire Wire Line
	5940 6595 6010 6595
Wire Wire Line
	6010 6595 6240 6595
Connection ~ 6010 6595
Wire Wire Line
	6280 8240 6045 8240
$Comp
L power:+3V3 #PWR016
U 1 1 612AE1AC
P 6045 7710
F 0 "#PWR016" H 6045 7560 50  0001 C CNN
F 1 "+3V3" H 6045 7860 50  0000 C CNN
F 2 "" H 6045 7710 50  0001 C CNN
F 3 "" H 6045 7710 50  0001 C CNN
	1    6045 7710
	1    0    0    -1  
$EndComp
$Comp
L power:+3V3 #PWR019
U 1 1 612AE1B6
P 5215 7720
F 0 "#PWR019" H 5215 7570 50  0001 C CNN
F 1 "+3V3" H 5215 7870 50  0000 C CNN
F 2 "" H 5215 7720 50  0001 C CNN
F 3 "" H 5215 7720 50  0001 C CNN
	1    5215 7720
	1    0    0    -1  
$EndComp
Wire Wire Line
	5215 7720 5215 7790
Wire Wire Line
	5215 7790 5365 7790
Connection ~ 5215 7790
Wire Wire Line
	6045 7710 6045 7770
Wire Wire Line
	6045 7970 6045 8240
Wire Wire Line
	5365 7990 5365 8440
Wire Wire Line
	5215 7990 5215 8240
Connection ~ 5215 8240
Wire Wire Line
	5215 8240 5120 8240
Connection ~ 5365 8440
Wire Wire Line
	5365 8440 5120 8440
Connection ~ 6045 8240
Wire Wire Line
	6045 8240 5980 8240
Wire Wire Line
	1360 3155 1250 3155
Wire Wire Line
	1130 3155 1130 3160
Wire Wire Line
	1250 3095 1250 3155
Connection ~ 1250 3155
Wire Wire Line
	1250 3155 1130 3155
Wire Wire Line
	1250 2840 1250 2895
$Comp
L power:+3V3 #PWR025
U 1 1 612EBE26
P 1240 3295
F 0 "#PWR025" H 1240 3145 50  0001 C CNN
F 1 "+3V3" H 1100 3310 50  0000 C CNN
F 2 "" H 1240 3295 50  0001 C CNN
F 3 "" H 1240 3295 50  0001 C CNN
	1    1240 3295
	1    0    0    -1  
$EndComp
Wire Wire Line
	1240 3295 1240 3350
Wire Wire Line
	1130 3560 1240 3560
Wire Wire Line
	1360 3560 1360 3555
Wire Wire Line
	1240 3550 1240 3560
Connection ~ 1240 3560
Wire Wire Line
	1240 3560 1360 3560
$Comp
L power:+3V3 #PWR026
U 1 1 6130D6D2
P 1240 3640
F 0 "#PWR026" H 1240 3490 50  0001 C CNN
F 1 "+3V3" H 1100 3655 50  0000 C CNN
F 2 "" H 1240 3640 50  0001 C CNN
F 3 "" H 1240 3640 50  0001 C CNN
	1    1240 3640
	1    0    0    -1  
$EndComp
Wire Wire Line
	1240 3640 1240 3695
Wire Wire Line
	1130 3910 1240 3910
Wire Wire Line
	1360 3910 1360 3905
Wire Wire Line
	1240 3895 1240 3910
Connection ~ 1240 3910
Wire Wire Line
	1240 3910 1360 3910
$Comp
L power:+3V3 #PWR027
U 1 1 613301D6
P 1240 4000
F 0 "#PWR027" H 1240 3850 50  0001 C CNN
F 1 "+3V3" H 1100 4015 50  0000 C CNN
F 2 "" H 1240 4000 50  0001 C CNN
F 3 "" H 1240 4000 50  0001 C CNN
	1    1240 4000
	1    0    0    -1  
$EndComp
Wire Wire Line
	1240 4000 1240 4055
Wire Wire Line
	1360 4255 1240 4255
Wire Wire Line
	1240 4255 1130 4255
Wire Wire Line
	1130 4255 1130 4260
Connection ~ 1240 4255
$Comp
L power:+3V3 #PWR030
U 1 1 61353DD1
P 1245 4340
F 0 "#PWR030" H 1245 4190 50  0001 C CNN
F 1 "+3V3" H 1105 4355 50  0000 C CNN
F 2 "" H 1245 4340 50  0001 C CNN
F 3 "" H 1245 4340 50  0001 C CNN
	1    1245 4340
	1    0    0    -1  
$EndComp
Wire Wire Line
	1245 4340 1245 4395
$Comp
L power:+3V3 #PWR031
U 1 1 6136131C
P 1250 4695
F 0 "#PWR031" H 1250 4545 50  0001 C CNN
F 1 "+3V3" H 1110 4710 50  0000 C CNN
F 2 "" H 1250 4695 50  0001 C CNN
F 3 "" H 1250 4695 50  0001 C CNN
	1    1250 4695
	1    0    0    -1  
$EndComp
Wire Wire Line
	1250 4695 1250 4750
$Comp
L power:+3V3 #PWR034
U 1 1 6136ED64
P 1245 5045
F 0 "#PWR034" H 1245 4895 50  0001 C CNN
F 1 "+3V3" H 1105 5060 50  0000 C CNN
F 2 "" H 1245 5045 50  0001 C CNN
F 3 "" H 1245 5045 50  0001 C CNN
	1    1245 5045
	1    0    0    -1  
$EndComp
Wire Wire Line
	1245 5045 1245 5100
$Comp
L power:+3V3 #PWR035
U 1 1 6137EBCB
P 1245 5390
F 0 "#PWR035" H 1245 5240 50  0001 C CNN
F 1 "+3V3" H 1105 5405 50  0000 C CNN
F 2 "" H 1245 5390 50  0001 C CNN
F 3 "" H 1245 5390 50  0001 C CNN
	1    1245 5390
	1    0    0    -1  
$EndComp
Wire Wire Line
	1245 5390 1245 5445
$Comp
L power:+3V3 #PWR045
U 1 1 6138C66E
P 1250 5860
F 0 "#PWR045" H 1250 5710 50  0001 C CNN
F 1 "+3V3" H 1110 5875 50  0000 C CNN
F 2 "" H 1250 5860 50  0001 C CNN
F 3 "" H 1250 5860 50  0001 C CNN
	1    1250 5860
	1    0    0    -1  
$EndComp
Wire Wire Line
	1250 5860 1250 5915
$Comp
L power:+3V3 #PWR047
U 1 1 6139A7AA
P 1245 6230
F 0 "#PWR047" H 1245 6080 50  0001 C CNN
F 1 "+3V3" H 1105 6245 50  0000 C CNN
F 2 "" H 1245 6230 50  0001 C CNN
F 3 "" H 1245 6230 50  0001 C CNN
	1    1245 6230
	1    0    0    -1  
$EndComp
Wire Wire Line
	1245 6230 1245 6285
Wire Wire Line
	1130 6510 1245 6510
Wire Wire Line
	1360 6510 1360 6505
Wire Wire Line
	1245 6485 1245 6510
Connection ~ 1245 6510
Wire Wire Line
	1245 6510 1360 6510
Wire Wire Line
	1130 6160 1250 6160
Wire Wire Line
	1360 6160 1360 6155
Wire Wire Line
	1250 6115 1250 6160
Connection ~ 1250 6160
Wire Wire Line
	1250 6160 1360 6160
Wire Wire Line
	1130 5660 1245 5660
Wire Wire Line
	1360 5660 1360 5655
Wire Wire Line
	1245 5645 1245 5660
Connection ~ 1245 5660
Wire Wire Line
	1245 5660 1360 5660
Wire Wire Line
	1130 5310 1245 5310
Wire Wire Line
	1360 5310 1360 5305
Wire Wire Line
	1245 5300 1245 5310
Connection ~ 1245 5310
Wire Wire Line
	1245 5310 1360 5310
Wire Wire Line
	1130 4960 1250 4960
Wire Wire Line
	1360 4960 1360 4955
Wire Wire Line
	1250 4950 1250 4960
Connection ~ 1250 4960
Wire Wire Line
	1250 4960 1360 4960
Wire Wire Line
	1130 4610 1245 4610
Wire Wire Line
	1360 4610 1360 4605
Wire Wire Line
	1245 4595 1245 4610
Connection ~ 1245 4610
Wire Wire Line
	1245 4610 1360 4610
Connection ~ 3340 2135
Text Label 7280 3460 2    50   ~ 0
VBUS
Text Label 8080 3460 0    50   ~ 0
VBUS_HOST
$Comp
L power:GND #PWR0106
U 1 1 612678CF
P 7680 3860
F 0 "#PWR0106" H 7680 3610 50  0001 C CNN
F 1 "GND" H 7685 3687 50  0000 C CNN
F 2 "" H 7680 3860 50  0001 C CNN
F 3 "" H 7680 3860 50  0001 C CNN
	1    7680 3860
	1    0    0    -1  
$EndComp
Text Label 8080 3660 0    50   ~ 0
USB_MIDI_D+
Text Label 8080 3560 0    50   ~ 0
USB_MIDI_D-
$Comp
L power:PWR_FLAG #FLG0101
U 1 1 5FBF5788
P 990 1110
F 0 "#FLG0101" H 990 1185 50  0001 C CNN
F 1 "PWR_FLAG" H 990 1283 50  0000 C CNN
F 2 "" H 990 1110 50  0001 C CNN
F 3 "~" H 990 1110 50  0001 C CNN
	1    990  1110
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0105
U 1 1 5FBF61A7
P 990 1110
F 0 "#PWR0105" H 990 860 50  0001 C CNN
F 1 "GND" H 995 937 50  0000 C CNN
F 2 "" H 990 1110 50  0001 C CNN
F 3 "" H 990 1110 50  0001 C CNN
	1    990  1110
	1    0    0    -1  
$EndComp
$Comp
L power:+3V3 #PWR0107
U 1 1 6130A283
P 7200 3660
F 0 "#PWR0107" H 7200 3510 50  0001 C CNN
F 1 "+3V3" H 7200 3810 50  0000 C CNN
F 2 "" H 7200 3660 50  0001 C CNN
F 3 "" H 7200 3660 50  0001 C CNN
	1    7200 3660
	1    0    0    -1  
$EndComp
Wire Wire Line
	7280 3660 7200 3660
Text Label 11285 4320 0    50   ~ 0
SR_CLK2
NoConn ~ 1990 2135
Text Label 11285 4420 0    50   ~ 0
SR_Shift2
$Comp
L Momentum-rescue:USB_B_Micro-Connector J1
U 1 1 5F9D2684
P 2090 1735
F 0 "J1" H 2147 2202 50  0000 C CNN
F 1 "USB_B_Micro" H 2147 2111 50  0000 C CNN
F 2 "Connector_USB:USB_Micro-B_Molex_47346-0001" H 2240 1685 50  0001 C CNN
F 3 "~" H 2240 1685 50  0001 C CNN
F 4 "C132560" H 2090 1735 50  0001 C CNN "LCSC"
	1    2090 1735
	1    0    0    -1  
$EndComp
$Comp
L Momentum-rescue:TPD3S014-Power_Protection U2
U 1 1 612336B9
P 7680 3560
F 0 "U2" H 7680 3927 50  0000 C CNN
F 1 "TPD3S014" H 7680 3836 50  0000 C CNN
F 2 "Package_TO_SOT_SMD:SOT-23-6" H 7680 3910 50  0001 C CNN
F 3 "http://www.ti.com/lit/ds/symlink/tpd3s014.pdf" H 7480 3810 50  0001 C CNN
F 4 "C87384" H 7680 3560 50  0001 C CNN "LCSC"
	1    7680 3560
	1    0    0    -1  
$EndComp
$Comp
L Connector:USB_A J2
U 1 1 5FCAF2EF
P 3340 1735
F 0 "J2" H 3397 2202 50  0000 C CNN
F 1 "USB_A_For_Host" H 3397 2111 50  0000 C CNN
F 2 "Connector_USB:USB_A_Stewart_SS-52100-001_Horizontal" H 3490 1685 50  0001 C CNN
F 3 "~" H 3490 1685 50  0001 C CNN
F 4 "C456008" H 3340 1735 50  0001 C CNN "LCSC"
	1    3340 1735
	1    0    0    -1  
$EndComp
$Comp
L Device:R_Small R42
U 1 1 6139B64E
P 1245 6385
F 0 "R42" V 1049 6385 50  0000 C CNN
F 1 "5k6" V 1140 6385 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric" H 1245 6385 50  0001 C CNN
F 3 "~" H 1245 6385 50  0001 C CNN
F 4 "C4382" H 1245 6385 50  0001 C CNN "LCSC"
	1    1245 6385
	-1   0    0    1   
$EndComp
$Comp
L Device:R_Small R41
U 1 1 6138D4F0
P 1250 6015
F 0 "R41" V 1054 6015 50  0000 C CNN
F 1 "5k6" V 1145 6015 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric" H 1250 6015 50  0001 C CNN
F 3 "~" H 1250 6015 50  0001 C CNN
F 4 "C4382" H 1250 6015 50  0001 C CNN "LCSC"
	1    1250 6015
	-1   0    0    1   
$EndComp
$Comp
L Device:R_Small R40
U 1 1 6137FA2B
P 1245 5545
F 0 "R40" V 1049 5545 50  0000 C CNN
F 1 "5k6" V 1140 5545 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric" H 1245 5545 50  0001 C CNN
F 3 "~" H 1245 5545 50  0001 C CNN
F 4 "C4382" H 1245 5545 50  0001 C CNN "LCSC"
	1    1245 5545
	-1   0    0    1   
$EndComp
$Comp
L Device:R_Small R39
U 1 1 6136FBA2
P 1245 5200
F 0 "R39" V 1049 5200 50  0000 C CNN
F 1 "5k6" V 1140 5200 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric" H 1245 5200 50  0001 C CNN
F 3 "~" H 1245 5200 50  0001 C CNN
F 4 "C4382" H 1245 5200 50  0001 C CNN "LCSC"
	1    1245 5200
	-1   0    0    1   
$EndComp
$Comp
L Device:R_Small R38
U 1 1 61362138
P 1250 4850
F 0 "R38" V 1054 4850 50  0000 C CNN
F 1 "5k6" V 1145 4850 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric" H 1250 4850 50  0001 C CNN
F 3 "~" H 1250 4850 50  0001 C CNN
F 4 "C4382" H 1250 4850 50  0001 C CNN "LCSC"
	1    1250 4850
	-1   0    0    1   
$EndComp
$Comp
L Device:R_Small R37
U 1 1 61354BCB
P 1245 4495
F 0 "R37" V 1049 4495 50  0000 C CNN
F 1 "5k6" V 1140 4495 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric" H 1245 4495 50  0001 C CNN
F 3 "~" H 1245 4495 50  0001 C CNN
F 4 "C4382" H 1245 4495 50  0001 C CNN "LCSC"
	1    1245 4495
	-1   0    0    1   
$EndComp
$Comp
L Device:R_Small R36
U 1 1 61330FAE
P 1240 4155
F 0 "R36" V 1044 4155 50  0000 C CNN
F 1 "5k6" V 1135 4155 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric" H 1240 4155 50  0001 C CNN
F 3 "~" H 1240 4155 50  0001 C CNN
F 4 "C4382" H 1240 4155 50  0001 C CNN "LCSC"
	1    1240 4155
	-1   0    0    1   
$EndComp
$Comp
L Device:R_Small R35
U 1 1 6130E488
P 1240 3795
F 0 "R35" V 1044 3795 50  0000 C CNN
F 1 "5k6" V 1135 3795 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric" H 1240 3795 50  0001 C CNN
F 3 "~" H 1240 3795 50  0001 C CNN
F 4 "C4382" H 1240 3795 50  0001 C CNN "LCSC"
	1    1240 3795
	-1   0    0    1   
$EndComp
$Comp
L Device:R_Small R34
U 1 1 612ECBBA
P 1240 3450
F 0 "R34" V 1044 3450 50  0000 C CNN
F 1 "5k6" V 1135 3450 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric" H 1240 3450 50  0001 C CNN
F 3 "~" H 1240 3450 50  0001 C CNN
F 4 "C4382" H 1240 3450 50  0001 C CNN "LCSC"
	1    1240 3450
	-1   0    0    1   
$EndComp
$Comp
L Device:R_Small R33
U 1 1 612CDFE5
P 1250 2995
F 0 "R33" V 1054 2995 50  0000 C CNN
F 1 "5k6" V 1145 2995 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric" H 1250 2995 50  0001 C CNN
F 3 "~" H 1250 2995 50  0001 C CNN
F 4 "C4382" H 1250 2995 50  0001 C CNN "LCSC"
	1    1250 2995
	-1   0    0    1   
$EndComp
$Comp
L Device:R_Small R14
U 1 1 612AE1A2
P 6045 7870
F 0 "R14" V 5849 7870 50  0000 C CNN
F 1 "5k6" V 5940 7870 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric" H 6045 7870 50  0001 C CNN
F 3 "~" H 6045 7870 50  0001 C CNN
F 4 "C4382" H 6045 7870 50  0001 C CNN "LCSC"
	1    6045 7870
	-1   0    0    1   
$EndComp
$Comp
L Device:R_Small R32
U 1 1 612AE198
P 5365 7890
F 0 "R32" V 5169 7890 50  0000 C CNN
F 1 "5k6" V 5260 7890 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric" H 5365 7890 50  0001 C CNN
F 3 "~" H 5365 7890 50  0001 C CNN
F 4 "C4382" H 5365 7890 50  0001 C CNN "LCSC"
	1    5365 7890
	-1   0    0    1   
$EndComp
$Comp
L Device:R_Small R31
U 1 1 612AD46A
P 5215 7890
F 0 "R31" V 5019 7890 50  0000 C CNN
F 1 "5k6" V 5110 7890 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric" H 5215 7890 50  0001 C CNN
F 3 "~" H 5215 7890 50  0001 C CNN
F 4 "C4382" H 5215 7890 50  0001 C CNN "LCSC"
	1    5215 7890
	-1   0    0    1   
$EndComp
$Comp
L Device:R_Small R11
U 1 1 61289FDC
P 6010 6225
F 0 "R11" V 5814 6225 50  0000 C CNN
F 1 "5k6" V 5905 6225 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric" H 6010 6225 50  0001 C CNN
F 3 "~" H 6010 6225 50  0001 C CNN
F 4 "C4382" H 6010 6225 50  0001 C CNN "LCSC"
	1    6010 6225
	-1   0    0    1   
$EndComp
$Comp
L Device:R_Small R13
U 1 1 61289FD2
P 5330 6245
F 0 "R13" V 5134 6245 50  0000 C CNN
F 1 "5k6" V 5225 6245 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric" H 5330 6245 50  0001 C CNN
F 3 "~" H 5330 6245 50  0001 C CNN
F 4 "C4382" H 5330 6245 50  0001 C CNN "LCSC"
	1    5330 6245
	-1   0    0    1   
$EndComp
$Comp
L Device:R_Small R12
U 1 1 612892F8
P 5180 6245
F 0 "R12" V 4984 6245 50  0000 C CNN
F 1 "5k6" V 5075 6245 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric" H 5180 6245 50  0001 C CNN
F 3 "~" H 5180 6245 50  0001 C CNN
F 4 "C4382" H 5180 6245 50  0001 C CNN "LCSC"
	1    5180 6245
	-1   0    0    1   
$EndComp
$Comp
L Device:R_Small R8
U 1 1 612643E1
P 5930 4385
F 0 "R8" V 5734 4385 50  0000 C CNN
F 1 "5k6" V 5825 4385 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric" H 5930 4385 50  0001 C CNN
F 3 "~" H 5930 4385 50  0001 C CNN
F 4 "C4382" H 5930 4385 50  0001 C CNN "LCSC"
	1    5930 4385
	-1   0    0    1   
$EndComp
$Comp
L Device:R_Small R10
U 1 1 612643DB
P 5250 4405
F 0 "R10" V 5054 4405 50  0000 C CNN
F 1 "5k6" V 5145 4405 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric" H 5250 4405 50  0001 C CNN
F 3 "~" H 5250 4405 50  0001 C CNN
F 4 "C4382" H 5250 4405 50  0001 C CNN "LCSC"
	1    5250 4405
	-1   0    0    1   
$EndComp
$Comp
L Device:R_Small R9
U 1 1 612643D5
P 5100 4405
F 0 "R9" V 4904 4405 50  0000 C CNN
F 1 "5k6" V 4995 4405 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric" H 5100 4405 50  0001 C CNN
F 3 "~" H 5100 4405 50  0001 C CNN
F 4 "C4382" H 5100 4405 50  0001 C CNN "LCSC"
	1    5100 4405
	-1   0    0    1   
$EndComp
$Comp
L Device:R_Small R3
U 1 1 611F13D6
P 5910 2755
F 0 "R3" V 5714 2755 50  0000 C CNN
F 1 "5k6" V 5805 2755 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric" H 5910 2755 50  0001 C CNN
F 3 "~" H 5910 2755 50  0001 C CNN
F 4 "C4382" H 5910 2755 50  0001 C CNN "LCSC"
	1    5910 2755
	-1   0    0    1   
$EndComp
$Comp
L Device:R_Small R5
U 1 1 611C613C
P 5230 2775
F 0 "R5" V 5034 2775 50  0000 C CNN
F 1 "5k6" V 5125 2775 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric" H 5230 2775 50  0001 C CNN
F 3 "~" H 5230 2775 50  0001 C CNN
F 4 "C4382" H 5230 2775 50  0001 C CNN "LCSC"
	1    5230 2775
	-1   0    0    1   
$EndComp
$Comp
L Device:R_Small R4
U 1 1 6117FE0E
P 5080 2775
F 0 "R4" V 4884 2775 50  0000 C CNN
F 1 "5k6" V 4975 2775 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric" H 5080 2775 50  0001 C CNN
F 3 "~" H 5080 2775 50  0001 C CNN
F 4 "C4382" H 5080 2775 50  0001 C CNN "LCSC"
	1    5080 2775
	-1   0    0    1   
$EndComp
$Comp
L Connector:TestPoint TP2
U 1 1 61156A16
P 9325 1220
F 0 "TP2" V 9220 1350 50  0000 C CNN
F 1 "Boot" V 9325 1495 50  0000 C CNN
F 2 "TestPoint:TestPoint_Pad_D1.5mm" H 9525 1220 50  0001 C CNN
F 3 "~" H 9525 1220 50  0001 C CNN
F 4 " " H 9325 1220 50  0001 C CNN "LCSC"
	1    9325 1220
	0    -1   -1   0   
$EndComp
$Comp
L Connector:TestPoint TP1
U 1 1 611259C8
P 9325 1120
F 0 "TP1" V 9435 1245 50  0000 C CNN
F 1 "Reset" V 9325 1405 50  0000 C CNN
F 2 "TestPoint:TestPoint_Pad_D1.5mm" H 9525 1120 50  0001 C CNN
F 3 "~" H 9525 1120 50  0001 C CNN
F 4 " " H 9325 1120 50  0001 C CNN "LCSC"
	1    9325 1120
	0    -1   -1   0   
$EndComp
$Comp
L Jumper:SolderJumper_2_Open JP1
U 1 1 610D0BC7
P 7690 5070
F 0 "JP1" H 7690 5275 50  0000 C CNN
F 1 "SolderJumper_2_Open" H 7690 5184 50  0000 C CNN
F 2 "Jumper:SolderJumper-2_P1.3mm_Open_RoundedPad1.0x1.5mm" H 7690 5070 50  0001 C CNN
F 3 "~" H 7690 5070 50  0001 C CNN
F 4 " " H 7690 5070 50  0001 C CNN "LCSC"
	1    7690 5070
	1    0    0    -1  
$EndComp
$Comp
L Momentum-rescue:Micro_SD_Card_Hroparts_TF-01A-Connector J10
U 1 1 61084FF6
P 14000 2830
F 0 "J10" H 14080 2822 50  0000 L CNN
F 1 "SD Card" H 14080 2731 50  0000 L CNN
F 2 "Connector_Card:MicroSD_Hroparts_TF-01A" H 14000 2830 50  0001 C CNN
F 3 "~" H 14000 2830 50  0001 C CNN
F 4 "C91145" H 14000 2830 50  0001 C CNN "LCSC"
	1    14000 2830
	1    0    0    -1  
$EndComp
$Comp
L Power_Protection:USBLC6-2SC6 U4
U 1 1 6108329C
P 7060 2295
F 0 "U4" H 7060 2876 50  0000 C CNN
F 1 "USBLC6-2SC6" H 7060 2785 50  0000 C CNN
F 2 "Package_TO_SOT_SMD:SOT-23-6" H 7060 1795 50  0001 C CNN
F 3 "https://www.st.com/resource/en/datasheet/usblc6-2.pdf" H 7260 2645 50  0001 C CNN
F 4 "C7519" H 7060 2295 50  0001 C CNN "LCSC"
	1    7060 2295
	1    0    0    -1  
$EndComp
$Comp
L Regulator_Linear:AMS1117-3.3 U5
U 1 1 6071D167
P 6720 1035
F 0 "U5" H 6720 1277 50  0000 C CNN
F 1 "AMS1117-3.3" H 6720 1186 50  0000 C CNN
F 2 "Package_TO_SOT_SMD:SOT-223-3_TabPin2" H 6720 1235 50  0001 C CNN
F 3 "http://www.advanced-monolithic.com/pdf/ds1117.pdf" H 6820 785 50  0001 C CNN
F 4 "C6186" H 6720 1035 50  0001 C CNN "LCSC"
	1    6720 1035
	1    0    0    -1  
$EndComp
$Comp
L 74xx:74HC595 U10
U 1 1 5FBD7063
P 10140 10010
F 0 "U10" H 10115 9785 50  0000 C CNN
F 1 "74HC595" H 10090 9685 50  0000 C CNN
F 2 "Package_SO:SOIC-16_3.9x9.9mm_P1.27mm" H 10140 10010 50  0001 C CNN
F 3 "http://www.ti.com/lit/ds/symlink/sn74hc595.pdf" H 10140 10010 50  0001 C CNN
F 4 " C5947" H 10140 10010 50  0001 C CNN "LCSC"
	1    10140 10010
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C17
U 1 1 5FBD7045
P 10640 9335
F 0 "C17" H 10732 9381 50  0000 L CNN
F 1 "0.1uF" H 10732 9290 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric" H 10640 9335 50  0001 C CNN
F 3 "~" H 10640 9335 50  0001 C CNN
F 4 "C49678" H 10640 9335 50  0001 C CNN "LCSC"
	1    10640 9335
	1    0    0    -1  
$EndComp
$Comp
L 74xx:74HC595 U9
U 1 1 5FB5E026
P 8365 9985
F 0 "U9" H 8390 9835 50  0000 C CNN
F 1 "74HC595" H 8365 9660 50  0000 C CNN
F 2 "Package_SO:SOIC-16_3.9x9.9mm_P1.27mm" H 8365 9985 50  0001 C CNN
F 3 "http://www.ti.com/lit/ds/symlink/sn74hc595.pdf" H 8365 9985 50  0001 C CNN
F 4 " C5947" H 8365 9985 50  0001 C CNN "LCSC"
	1    8365 9985
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C19
U 1 1 5FD1AD88
P 6020 1135
F 0 "C19" H 6070 1435 50  0000 L CNN
F 1 "2.2uF" H 6020 1335 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric" H 6020 1135 50  0001 C CNN
F 3 "~" H 6020 1135 50  0001 C CNN
F 4 "C49217" H 6020 1135 50  0001 C CNN "LCSC"
	1    6020 1135
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C18
U 1 1 5FD1A5D1
P 5870 1135
F 0 "C18" H 5820 1435 50  0000 L CNN
F 1 "220nF" H 5770 1335 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric" H 5870 1135 50  0001 C CNN
F 3 "~" H 5870 1135 50  0001 C CNN
F 4 "C5378" H 5870 1135 50  0001 C CNN "LCSC"
	1    5870 1135
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C22
U 1 1 5FD0F4DF
P 7620 1135
F 0 "C22" H 7570 1485 50  0000 L CNN
F 1 "220nF" H 7520 1385 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric" H 7620 1135 50  0001 C CNN
F 3 "~" H 7620 1135 50  0001 C CNN
F 4 "C5378" H 7620 1135 50  0001 C CNN "LCSC"
	1    7620 1135
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C23
U 1 1 5FCE89BA
P 7770 1135
F 0 "C23" H 7720 1435 50  0000 L CNN
F 1 "10nF" H 7670 1335 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric" H 7770 1135 50  0001 C CNN
F 3 "~" H 7770 1135 50  0001 C CNN
F 4 "C1710" H 7770 1135 50  0001 C CNN "LCSC"
	1    7770 1135
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C21
U 1 1 5FCE651E
P 7420 1135
F 0 "C21" H 7370 1435 50  0000 L CNN
F 1 "4.7uF" H 7320 1335 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric" H 7420 1135 50  0001 C CNN
F 3 "~" H 7420 1135 50  0001 C CNN
F 4 "C1779" H 7420 1135 50  0001 C CNN "LCSC"
	1    7420 1135
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C20
U 1 1 5FCE5CDC
P 7270 1135
F 0 "C20" H 7170 1485 50  0000 L CNN
F 1 "2.2uF" H 7120 1385 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric" H 7270 1135 50  0001 C CNN
F 3 "~" H 7270 1135 50  0001 C CNN
F 4 "C49217" H 7270 1135 50  0001 C CNN "LCSC"
	1    7270 1135
	1    0    0    -1  
$EndComp
$Comp
L Device:R_Small R30
U 1 1 5FCAEDDA
P 2990 7835
F 0 "R30" V 2890 7835 50  0000 C CNN
F 1 "100R" V 3090 7835 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric" H 2990 7835 50  0001 C CNN
F 3 "~" H 2990 7835 50  0001 C CNN
F 4 "C17408" H 2990 7835 50  0001 C CNN "LCSC"
	1    2990 7835
	0    1    1    0   
$EndComp
$Comp
L Device:R_Small R29
U 1 1 5FCAEDD4
P 2990 7635
F 0 "R29" V 2794 7635 50  0000 C CNN
F 1 "100R" V 2885 7635 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric" H 2990 7635 50  0001 C CNN
F 3 "~" H 2990 7635 50  0001 C CNN
F 4 "C17408" H 2990 7635 50  0001 C CNN "LCSC"
	1    2990 7635
	0    1    1    0   
$EndComp
$Comp
L Device:R_Small R28
U 1 1 5FCA6CD3
P 2990 7135
F 0 "R28" V 2890 7135 50  0000 C CNN
F 1 "100R" V 3090 7135 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric" H 2990 7135 50  0001 C CNN
F 3 "~" H 2990 7135 50  0001 C CNN
F 4 "C17408" H 2990 7135 50  0001 C CNN "LCSC"
	1    2990 7135
	0    1    1    0   
$EndComp
$Comp
L Device:R_Small R27
U 1 1 5FCA6CCD
P 2990 6935
F 0 "R27" V 2794 6935 50  0000 C CNN
F 1 "100R" V 2885 6935 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric" H 2990 6935 50  0001 C CNN
F 3 "~" H 2990 6935 50  0001 C CNN
F 4 "C17408" H 2990 6935 50  0001 C CNN "LCSC"
	1    2990 6935
	0    1    1    0   
$EndComp
$Comp
L Device:R_Small R26
U 1 1 5FC9E506
P 2990 6485
F 0 "R26" V 2890 6485 50  0000 C CNN
F 1 "100R" V 3090 6485 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric" H 2990 6485 50  0001 C CNN
F 3 "~" H 2990 6485 50  0001 C CNN
F 4 "C17408" H 2990 6485 50  0001 C CNN "LCSC"
	1    2990 6485
	0    1    1    0   
$EndComp
$Comp
L Device:R_Small R25
U 1 1 5FC9E500
P 2990 6285
F 0 "R25" V 2794 6285 50  0000 C CNN
F 1 "100R" V 2885 6285 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric" H 2990 6285 50  0001 C CNN
F 3 "~" H 2990 6285 50  0001 C CNN
F 4 "C17408" H 2990 6285 50  0001 C CNN "LCSC"
	1    2990 6285
	0    1    1    0   
$EndComp
$Comp
L Device:R_Small R24
U 1 1 5FC9600F
P 2990 5785
F 0 "R24" V 2890 5785 50  0000 C CNN
F 1 "100R" V 3090 5785 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric" H 2990 5785 50  0001 C CNN
F 3 "~" H 2990 5785 50  0001 C CNN
F 4 "C17408" H 2990 5785 50  0001 C CNN "LCSC"
	1    2990 5785
	0    1    1    0   
$EndComp
$Comp
L Device:R_Small R23
U 1 1 5FC96009
P 2990 5585
F 0 "R23" V 2794 5585 50  0000 C CNN
F 1 "100R" V 2885 5585 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric" H 2990 5585 50  0001 C CNN
F 3 "~" H 2990 5585 50  0001 C CNN
F 4 "C17408" H 2990 5585 50  0001 C CNN "LCSC"
	1    2990 5585
	0    1    1    0   
$EndComp
$Comp
L Device:R_Small R22
U 1 1 5FC8DF4C
P 2990 5185
F 0 "R22" V 2890 5185 50  0000 C CNN
F 1 "100R" V 3090 5185 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric" H 2990 5185 50  0001 C CNN
F 3 "~" H 2990 5185 50  0001 C CNN
F 4 "C17408" H 2990 5185 50  0001 C CNN "LCSC"
	1    2990 5185
	0    1    1    0   
$EndComp
$Comp
L Device:R_Small R21
U 1 1 5FC8DF46
P 2990 4985
F 0 "R21" V 2794 4985 50  0000 C CNN
F 1 "100R" V 2885 4985 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric" H 2990 4985 50  0001 C CNN
F 3 "~" H 2990 4985 50  0001 C CNN
F 4 "C17408" H 2990 4985 50  0001 C CNN "LCSC"
	1    2990 4985
	0    1    1    0   
$EndComp
$Comp
L Device:R_Small R20
U 1 1 5FC85AD9
P 2990 4535
F 0 "R20" V 2890 4535 50  0000 C CNN
F 1 "100R" V 3090 4535 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric" H 2990 4535 50  0001 C CNN
F 3 "~" H 2990 4535 50  0001 C CNN
F 4 "C17408" H 2990 4535 50  0001 C CNN "LCSC"
	1    2990 4535
	0    1    1    0   
$EndComp
$Comp
L Device:R_Small R19
U 1 1 5FC85AD3
P 2990 4335
F 0 "R19" V 2794 4335 50  0000 C CNN
F 1 "100R" V 2885 4335 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric" H 2990 4335 50  0001 C CNN
F 3 "~" H 2990 4335 50  0001 C CNN
F 4 "C17408" H 2990 4335 50  0001 C CNN "LCSC"
	1    2990 4335
	0    1    1    0   
$EndComp
$Comp
L Device:R_Small R18
U 1 1 5FC7DAC2
P 2990 3885
F 0 "R18" V 2890 3885 50  0000 C CNN
F 1 "100R" V 3090 3885 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric" H 2990 3885 50  0001 C CNN
F 3 "~" H 2990 3885 50  0001 C CNN
F 4 "C17408" H 2990 3885 50  0001 C CNN "LCSC"
	1    2990 3885
	0    1    1    0   
$EndComp
$Comp
L Device:R_Small R17
U 1 1 5FC7DABC
P 2990 3685
F 0 "R17" V 2794 3685 50  0000 C CNN
F 1 "100R" V 2885 3685 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric" H 2990 3685 50  0001 C CNN
F 3 "~" H 2990 3685 50  0001 C CNN
F 4 "C17408" H 2990 3685 50  0001 C CNN "LCSC"
	1    2990 3685
	0    1    1    0   
$EndComp
$Comp
L Device:R_Small R16
U 1 1 5FC6E88F
P 2990 3235
F 0 "R16" V 2890 3235 50  0000 C CNN
F 1 "100R" V 3090 3235 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric" H 2990 3235 50  0001 C CNN
F 3 "~" H 2990 3235 50  0001 C CNN
F 4 "C17408" H 2990 3235 50  0001 C CNN "LCSC"
	1    2990 3235
	0    1    1    0   
$EndComp
$Comp
L Device:R_Small R15
U 1 1 5FC6C494
P 2990 3035
F 0 "R15" V 2794 3035 50  0000 C CNN
F 1 "100R" V 2885 3035 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric" H 2990 3035 50  0001 C CNN
F 3 "~" H 2990 3035 50  0001 C CNN
F 4 "C17408" H 2990 3035 50  0001 C CNN "LCSC"
	1    2990 3035
	0    1    1    0   
$EndComp
$Comp
L 74xx:74HC165 U8
U 1 1 5FBE8E89
P 13585 7350
F 0 "U8" H 13635 7350 50  0000 C CNN
F 1 "74HC165" H 13635 7200 50  0000 C CNN
F 2 "Package_SO:SOIC-16_3.9x9.9mm_P1.27mm" H 13585 7350 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/74HC_HCT165.pdf" H 13585 7350 50  0001 C CNN
F 4 "C5613" H 13585 7350 50  0001 C CNN "LCSC"
	1    13585 7350
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C16
U 1 1 5FBE8E6B
P 13835 6400
F 0 "C16" H 13927 6446 50  0000 L CNN
F 1 "0.1uF" H 13927 6355 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric" H 13835 6400 50  0001 C CNN
F 3 "~" H 13835 6400 50  0001 C CNN
F 4 "C49678" H 13835 6400 50  0001 C CNN "LCSC"
	1    13835 6400
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C15
U 1 1 5FBD728E
P 8865 9310
F 0 "C15" H 8957 9356 50  0000 L CNN
F 1 "0.1uF" H 8957 9265 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric" H 8865 9310 50  0001 C CNN
F 3 "~" H 8865 9310 50  0001 C CNN
F 4 "C49678" H 8865 9310 50  0001 C CNN "LCSC"
	1    8865 9310
	1    0    0    -1  
$EndComp
$Comp
L 74xx:74HC165 U7
U 1 1 5FBCBD68
P 11050 7485
F 0 "U7" H 11100 7485 50  0000 C CNN
F 1 "74HC165" H 11100 7335 50  0000 C CNN
F 2 "Package_SO:SOIC-16_3.9x9.9mm_P1.27mm" H 11050 7485 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/74HC_HCT165.pdf" H 11050 7485 50  0001 C CNN
F 4 "C5613" H 11050 7485 50  0001 C CNN "LCSC"
	1    11050 7485
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C14
U 1 1 5FBCBD4A
P 11300 6535
F 0 "C14" H 11392 6581 50  0000 L CNN
F 1 "0.1uF" H 11392 6490 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric" H 11300 6535 50  0001 C CNN
F 3 "~" H 11300 6535 50  0001 C CNN
F 4 "C49678" H 11300 6535 50  0001 C CNN "LCSC"
	1    11300 6535
	1    0    0    -1  
$EndComp
$Comp
L 74xx:74HC165 U6
U 1 1 5FB27FE0
P 8845 7405
F 0 "U6" H 8895 7405 50  0000 C CNN
F 1 "74HC165" H 8895 7255 50  0000 C CNN
F 2 "Package_SO:SOIC-16_3.9x9.9mm_P1.27mm" H 8845 7405 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/74HC_HCT165.pdf" H 8845 7405 50  0001 C CNN
F 4 "C5613" H 8845 7405 50  0001 C CNN "LCSC"
	1    8845 7405
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C11
U 1 1 5FB2AB59
P 9095 6455
F 0 "C11" H 9187 6501 50  0000 L CNN
F 1 "0.1uF" H 9187 6410 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric" H 9095 6455 50  0001 C CNN
F 3 "~" H 9095 6455 50  0001 C CNN
F 4 "C49678" H 9095 6455 50  0001 C CNN "LCSC"
	1    9095 6455
	1    0    0    -1  
$EndComp
$Comp
L Device:R_Small R7
U 1 1 5FAF5C98
P 5170 9540
F 0 "R7" V 5345 9545 50  0000 C CNN
F 1 "10R" V 5250 9545 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric" H 5170 9540 50  0001 C CNN
F 3 "~" H 5170 9540 50  0001 C CNN
F 4 "C17415" H 5170 9540 50  0001 C CNN "LCSC"
	1    5170 9540
	0    -1   -1   0   
$EndComp
$Comp
L Device:R_Small R6
U 1 1 5FAD7D4D
P 5170 9640
F 0 "R6" V 4970 9640 50  0000 C CNN
F 1 "33R" V 5070 9640 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric" H 5170 9640 50  0001 C CNN
F 3 "~" H 5170 9640 50  0001 C CNN
F 4 "C17634" H 5170 9640 50  0001 C CNN "LCSC"
	1    5170 9640
	0    -1   -1   0   
$EndComp
$Comp
L Device:R_Small R2
U 1 1 5FADC10F
P 3240 9585
F 0 "R2" V 3044 9585 50  0000 C CNN
F 1 "5k6" V 3135 9585 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric" H 3240 9585 50  0001 C CNN
F 3 "~" H 3240 9585 50  0001 C CNN
F 4 "C4382" H 3240 9585 50  0001 C CNN "LCSC"
	1    3240 9585
	-1   0    0    1   
$EndComp
$Comp
L Mechanical:MountingHole_Pad H7
U 1 1 5FB20E52
P 15040 9335
F 0 "H7" H 15140 9384 50  0000 L CNN
F 1 "MountingHole_Pad" H 15140 9293 50  0000 L CNN
F 2 "MountingHole:MountingHole_3.2mm_M3_DIN965_Pad" H 15040 9335 50  0001 C CNN
F 3 "~" H 15040 9335 50  0001 C CNN
F 4 " " H 15040 9335 50  0001 C CNN "LCSC"
	1    15040 9335
	1    0    0    -1  
$EndComp
$Comp
L Device:Rotary_Encoder_Switch SW12
U 1 1 5FAAFB45
P 5575 4855
F 0 "SW12" H 5575 5222 50  0000 C CNN
F 1 "Rotary_Encoder_Switch" H 5575 5131 50  0000 C CNN
F 2 "Rotary_Encoder:RotaryEncoder_Alps_EC11E-Switch_Vertical_H20mm" H 5425 5015 50  0001 C CNN
F 3 "~" H 5575 5115 50  0001 C CNN
F 4 " " H 5575 4855 50  0001 C CNN "LCSC"
	1    5575 4855
	1    0    0    -1  
$EndComp
$Comp
L Device:Rotary_Encoder_Switch SW14
U 1 1 5FAA887D
P 5680 8340
F 0 "SW14" H 5680 8707 50  0000 C CNN
F 1 "Rotary_Encoder_Switch" H 5680 8616 50  0000 C CNN
F 2 "Rotary_Encoder:RotaryEncoder_Alps_EC11E-Switch_Vertical_H20mm" H 5530 8500 50  0001 C CNN
F 3 "~" H 5680 8600 50  0001 C CNN
F 4 " " H 5680 8340 50  0001 C CNN "LCSC"
	1    5680 8340
	1    0    0    -1  
$EndComp
$Comp
L Device:Rotary_Encoder_Switch SW13
U 1 1 5FAA3CC3
P 5640 6695
F 0 "SW13" H 5640 7062 50  0000 C CNN
F 1 "Rotary_Encoder_Switch" H 5640 6971 50  0000 C CNN
F 2 "Rotary_Encoder:RotaryEncoder_Alps_EC11E-Switch_Vertical_H20mm" H 5490 6855 50  0001 C CNN
F 3 "~" H 5640 6955 50  0001 C CNN
F 4 " " H 5640 6695 50  0001 C CNN "LCSC"
	1    5640 6695
	1    0    0    -1  
$EndComp
$Comp
L Device:Rotary_Encoder_Switch SW11
U 1 1 5FA554C4
P 5575 3225
F 0 "SW11" H 5575 3592 50  0000 C CNN
F 1 "Rotary_Encoder_Switch" H 5575 3501 50  0000 C CNN
F 2 "Rotary_Encoder:RotaryEncoder_Alps_EC11E-Switch_Vertical_H20mm" H 5425 3385 50  0001 C CNN
F 3 "~" H 5575 3485 50  0001 C CNN
F 4 " " H 5575 3225 50  0001 C CNN "LCSC"
	1    5575 3225
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole_Pad H6
U 1 1 5FAA221F
P 14740 9335
F 0 "H6" H 14840 9384 50  0000 L CNN
F 1 "MountingHole_Pad" H 14840 9293 50  0000 L CNN
F 2 "MountingHole:MountingHole_3.2mm_M3_DIN965_Pad" H 14740 9335 50  0001 C CNN
F 3 "~" H 14740 9335 50  0001 C CNN
F 4 " " H 14740 9335 50  0001 C CNN "LCSC"
	1    14740 9335
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole_Pad H5
U 1 1 5FAA1FD7
P 14440 9335
F 0 "H5" H 14540 9384 50  0000 L CNN
F 1 "MountingHole_Pad" H 14540 9293 50  0000 L CNN
F 2 "MountingHole:MountingHole_3.2mm_M3_DIN965_Pad" H 14440 9335 50  0001 C CNN
F 3 "~" H 14440 9335 50  0001 C CNN
F 4 " " H 14440 9335 50  0001 C CNN "LCSC"
	1    14440 9335
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole_Pad H4
U 1 1 5FAA1DC5
P 14140 9335
F 0 "H4" H 14240 9384 50  0000 L CNN
F 1 "MountingHole_Pad" H 14240 9293 50  0000 L CNN
F 2 "MountingHole:MountingHole_3.2mm_M3_DIN965_Pad" H 14140 9335 50  0001 C CNN
F 3 "~" H 14140 9335 50  0001 C CNN
F 4 " " H 14140 9335 50  0001 C CNN "LCSC"
	1    14140 9335
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole_Pad H3
U 1 1 5FAA1A0D
P 13840 9335
F 0 "H3" H 13940 9384 50  0000 L CNN
F 1 "MountingHole_Pad" H 13940 9293 50  0000 L CNN
F 2 "MountingHole:MountingHole_3.2mm_M3_DIN965_Pad" H 13840 9335 50  0001 C CNN
F 3 "~" H 13840 9335 50  0001 C CNN
F 4 " " H 13840 9335 50  0001 C CNN "LCSC"
	1    13840 9335
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole_Pad H2
U 1 1 5FA91D80
P 13540 9335
F 0 "H2" H 13640 9384 50  0000 L CNN
F 1 "MountingHole_Pad" H 13640 9293 50  0000 L CNN
F 2 "MountingHole:MountingHole_3.2mm_M3_DIN965_Pad" H 13540 9335 50  0001 C CNN
F 3 "~" H 13540 9335 50  0001 C CNN
F 4 " " H 13540 9335 50  0001 C CNN "LCSC"
	1    13540 9335
	1    0    0    -1  
$EndComp
$Comp
L Device:LED_Dual_AAC D10
U 1 1 5FA8E002
P 2590 7735
F 0 "D10" H 2590 8160 50  0000 C CNN
F 1 "LED_Dual_AAC" H 2590 8069 50  0000 C CNN
F 2 "LED_THT:LED_D3.0mm-3" H 2590 7735 50  0001 C CNN
F 3 "~" H 2590 7735 50  0001 C CNN
F 4 " " H 2590 7735 50  0001 C CNN "LCSC"
	1    2590 7735
	1    0    0    -1  
$EndComp
$Comp
L Device:LED_Dual_AAC D9
U 1 1 5FA8D94D
P 2590 7035
F 0 "D9" H 2590 7460 50  0000 C CNN
F 1 "LED_Dual_AAC" H 2590 7369 50  0000 C CNN
F 2 "LED_THT:LED_D3.0mm-3" H 2590 7035 50  0001 C CNN
F 3 "~" H 2590 7035 50  0001 C CNN
F 4 " " H 2590 7035 50  0001 C CNN "LCSC"
	1    2590 7035
	1    0    0    -1  
$EndComp
$Comp
L Device:LED_Dual_AAC D8
U 1 1 5FA8D23E
P 2590 6385
F 0 "D8" H 2590 6810 50  0000 C CNN
F 1 "LED_Dual_AAC" H 2590 6719 50  0000 C CNN
F 2 "LED_THT:LED_D3.0mm-3" H 2590 6385 50  0001 C CNN
F 3 "~" H 2590 6385 50  0001 C CNN
F 4 " " H 2590 6385 50  0001 C CNN "LCSC"
	1    2590 6385
	1    0    0    -1  
$EndComp
$Comp
L Device:LED_Dual_AAC D7
U 1 1 5FA8CD50
P 2590 5685
F 0 "D7" H 2590 6110 50  0000 C CNN
F 1 "LED_Dual_AAC" H 2590 6019 50  0000 C CNN
F 2 "LED_THT:LED_D3.0mm-3" H 2590 5685 50  0001 C CNN
F 3 "~" H 2590 5685 50  0001 C CNN
F 4 " " H 2590 5685 50  0001 C CNN "LCSC"
	1    2590 5685
	1    0    0    -1  
$EndComp
$Comp
L Device:LED_Dual_AAC D6
U 1 1 5FA8C97A
P 2590 5085
F 0 "D6" H 2590 5510 50  0000 C CNN
F 1 "LED_Dual_AAC" H 2590 5419 50  0000 C CNN
F 2 "LED_THT:LED_D3.0mm-3" H 2590 5085 50  0001 C CNN
F 3 "~" H 2590 5085 50  0001 C CNN
F 4 " " H 2590 5085 50  0001 C CNN "LCSC"
	1    2590 5085
	1    0    0    -1  
$EndComp
$Comp
L Device:LED_Dual_AAC D5
U 1 1 5FA8C590
P 2590 4435
F 0 "D5" H 2590 4860 50  0000 C CNN
F 1 "LED_Dual_AAC" H 2590 4769 50  0000 C CNN
F 2 "LED_THT:LED_D3.0mm-3" H 2590 4435 50  0001 C CNN
F 3 "~" H 2590 4435 50  0001 C CNN
F 4 " " H 2590 4435 50  0001 C CNN "LCSC"
	1    2590 4435
	1    0    0    -1  
$EndComp
$Comp
L Device:LED_Dual_AAC D4
U 1 1 5FA8BD19
P 2590 3785
F 0 "D4" H 2590 4210 50  0000 C CNN
F 1 "LED_Dual_AAC" H 2590 4119 50  0000 C CNN
F 2 "LED_THT:LED_D3.0mm-3" H 2590 3785 50  0001 C CNN
F 3 "~" H 2590 3785 50  0001 C CNN
F 4 " " H 2590 3785 50  0001 C CNN "LCSC"
	1    2590 3785
	1    0    0    -1  
$EndComp
$Comp
L Device:LED_Dual_AAC D3
U 1 1 5FA8ADBF
P 2590 3135
F 0 "D3" H 2590 3560 50  0000 C CNN
F 1 "LED_Dual_AAC" H 2590 3469 50  0000 C CNN
F 2 "LED_THT:LED_D3.0mm-3" H 2590 3135 50  0001 C CNN
F 3 "~" H 2590 3135 50  0001 C CNN
F 4 " " H 2590 3135 50  0001 C CNN "LCSC"
	1    2590 3135
	1    0    0    -1  
$EndComp
$Comp
L Switch:SW_Push SW10
U 1 1 5FA507ED
P 1560 6505
F 0 "SW10" H 1560 6790 50  0000 C CNN
F 1 "SW_Push" H 1560 6699 50  0000 C CNN
F 2 "Button_Switch_THT:SW_PUSH-12mm" H 1560 6705 50  0001 C CNN
F 3 "~" H 1560 6705 50  0001 C CNN
F 4 " " H 1560 6505 50  0001 C CNN "LCSC"
	1    1560 6505
	1    0    0    -1  
$EndComp
$Comp
L Switch:SW_Push SW9
U 1 1 5FA5012B
P 1560 6155
F 0 "SW9" H 1560 6440 50  0000 C CNN
F 1 "SW_Push" H 1560 6349 50  0000 C CNN
F 2 "Button_Switch_THT:SW_PUSH-12mm" H 1560 6355 50  0001 C CNN
F 3 "~" H 1560 6355 50  0001 C CNN
F 4 " " H 1560 6155 50  0001 C CNN "LCSC"
	1    1560 6155
	1    0    0    -1  
$EndComp
$Comp
L Switch:SW_Push SW8
U 1 1 5FA4FCED
P 1560 5655
F 0 "SW8" H 1560 5940 50  0000 C CNN
F 1 "SW_Push" H 1560 5849 50  0000 C CNN
F 2 "Button_Switch_THT:SW_PUSH-12mm" H 1560 5855 50  0001 C CNN
F 3 "~" H 1560 5855 50  0001 C CNN
F 4 " " H 1560 5655 50  0001 C CNN "LCSC"
	1    1560 5655
	1    0    0    -1  
$EndComp
$Comp
L Switch:SW_Push SW7
U 1 1 5FA4F919
P 1560 5305
F 0 "SW7" H 1560 5590 50  0000 C CNN
F 1 "SW_Push" H 1560 5499 50  0000 C CNN
F 2 "Button_Switch_THT:SW_PUSH-12mm" H 1560 5505 50  0001 C CNN
F 3 "~" H 1560 5505 50  0001 C CNN
F 4 " " H 1560 5305 50  0001 C CNN "LCSC"
	1    1560 5305
	1    0    0    -1  
$EndComp
$Comp
L Switch:SW_Push SW6
U 1 1 5FA4F68C
P 1560 4955
F 0 "SW6" H 1560 5240 50  0000 C CNN
F 1 "SW_Push" H 1560 5149 50  0000 C CNN
F 2 "Button_Switch_THT:SW_PUSH-12mm" H 1560 5155 50  0001 C CNN
F 3 "~" H 1560 5155 50  0001 C CNN
F 4 " " H 1560 4955 50  0001 C CNN "LCSC"
	1    1560 4955
	1    0    0    -1  
$EndComp
$Comp
L Switch:SW_Push SW5
U 1 1 5FA4F472
P 1560 4605
F 0 "SW5" H 1560 4890 50  0000 C CNN
F 1 "SW_Push" H 1560 4799 50  0000 C CNN
F 2 "Button_Switch_THT:SW_PUSH-12mm" H 1560 4805 50  0001 C CNN
F 3 "~" H 1560 4805 50  0001 C CNN
F 4 " " H 1560 4605 50  0001 C CNN "LCSC"
	1    1560 4605
	1    0    0    -1  
$EndComp
$Comp
L Switch:SW_Push SW4
U 1 1 5FA4F10E
P 1560 4255
F 0 "SW4" H 1560 4540 50  0000 C CNN
F 1 "SW_Push" H 1560 4449 50  0000 C CNN
F 2 "Button_Switch_THT:SW_PUSH-12mm" H 1560 4455 50  0001 C CNN
F 3 "~" H 1560 4455 50  0001 C CNN
F 4 " " H 1560 4255 50  0001 C CNN "LCSC"
	1    1560 4255
	1    0    0    -1  
$EndComp
$Comp
L Switch:SW_Push SW3
U 1 1 5FA4EDE8
P 1560 3905
F 0 "SW3" H 1560 4190 50  0000 C CNN
F 1 "SW_Push" H 1560 4099 50  0000 C CNN
F 2 "Button_Switch_THT:SW_PUSH-12mm" H 1560 4105 50  0001 C CNN
F 3 "~" H 1560 4105 50  0001 C CNN
F 4 " " H 1560 3905 50  0001 C CNN "LCSC"
	1    1560 3905
	1    0    0    -1  
$EndComp
$Comp
L Switch:SW_Push SW2
U 1 1 5FA4E4C8
P 1560 3555
F 0 "SW2" H 1560 3840 50  0000 C CNN
F 1 "SW_Push" H 1560 3749 50  0000 C CNN
F 2 "Button_Switch_THT:SW_PUSH-12mm" H 1560 3755 50  0001 C CNN
F 3 "~" H 1560 3755 50  0001 C CNN
F 4 " " H 1560 3555 50  0001 C CNN "LCSC"
	1    1560 3555
	1    0    0    -1  
$EndComp
$Comp
L Switch:SW_Push SW1
U 1 1 5FA4A728
P 1560 3155
F 0 "SW1" H 1560 3440 50  0000 C CNN
F 1 "SW_Push" H 1560 3349 50  0000 C CNN
F 2 "Button_Switch_THT:SW_PUSH-12mm" H 1560 3355 50  0001 C CNN
F 3 "~" H 1560 3355 50  0001 C CNN
F 4 " " H 1560 3155 50  0001 C CNN "LCSC"
	1    1560 3155
	1    0    0    -1  
$EndComp
$Comp
L Momentum-rescue:STAND-OFF-REFLOW2.5MM_TALL-MicroMod H1
U 1 1 5FA2BC79
P 9535 5120
F 0 "H1" H 9615 5141 59  0000 L CNN
F 1 "STAND-OFF-REFLOW2.5MM_TALL" H 9615 5036 59  0000 L CNN
F 2 "MicroMod:STAND-OFF-REFLOW-M2.5" H 9535 5120 50  0001 C CNN
F 3 "~" H 9535 5120 50  0001 C CNN
F 4 " " H 9535 5120 50  0001 C CNN "LCSC"
	1    9535 5120
	1    0    0    -1  
$EndComp
$Comp
L Momentum-rescue:ILI9341_240x320_TFT-Display_Graphic U3
U 1 1 5FA2B2DD
P 13720 5490
F 0 "U3" H 13620 5440 50  0000 L CNN
F 1 "ILI9341_240x320_TFT" H 13320 5340 50  0000 L CNN
F 2 "Display:TFT-graphic_display_240x320_ILI9341" H 13720 5490 50  0001 C CNN
F 3 "~" H 13720 5490 50  0001 C CNN
F 4 " " H 13720 5490 50  0001 C CNN "LCSC"
	1    13720 5490
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C5
U 1 1 5FA48615
P 3090 9735
F 0 "C5" H 3182 9781 50  0000 L CNN
F 1 "0.1uF" H 3115 9835 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric" H 3090 9735 50  0001 C CNN
F 3 "~" H 3090 9735 50  0001 C CNN
F 4 "C49678" H 3090 9735 50  0001 C CNN "LCSC"
	1    3090 9735
	1    0    0    1   
$EndComp
$Comp
L Diode:BAT54S D2
U 1 1 5FA40F83
P 2040 9785
F 0 "D2" V 1940 9385 50  0000 L CNN
F 1 "BAT54S" V 1840 9385 50  0000 L CNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 2115 9910 50  0001 L CNN
F 3 "https://www.diodes.com/assets/Datasheets/ds11005.pdf" H 1920 9785 50  0001 C CNN
F 4 "C47546" H 2040 9785 50  0001 C CNN "LCSC"
	1    2040 9785
	0    1    -1   0   
$EndComp
$Comp
L Device:R_Small R1
U 1 1 5FA3EDC7
P 2290 9685
F 0 "R1" V 2094 9685 50  0000 C CNN
F 1 "220R" V 2185 9685 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric" H 2290 9685 50  0001 C CNN
F 3 "~" H 2290 9685 50  0001 C CNN
F 4 "C17557" H 2290 9685 50  0001 C CNN "LCSC"
	1    2290 9685
	0    1    1    0   
$EndComp
$Comp
L Diode:BAT54S D1
U 1 1 5FA39B6A
P 1790 9335
F 0 "D1" V 1836 9423 50  0000 L CNN
F 1 "BAT54S" V 1745 9423 50  0000 L CNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 1865 9460 50  0001 L CNN
F 3 "https://www.diodes.com/assets/Datasheets/ds11005.pdf" H 1670 9335 50  0001 C CNN
F 4 "C47546" H 1790 9335 50  0001 C CNN "LCSC"
	1    1790 9335
	0    1    -1   0   
$EndComp
$Comp
L Isolator:6N137 U1
U 1 1 5FA33652
P 2690 9685
F 0 "U1" H 2690 10152 50  0000 C CNN
F 1 "6N137" H 2690 10061 50  0000 C CNN
F 2 "Package_DIP:SMDIP-8_W9.53mm" H 2690 9185 50  0001 C CNN
F 3 "https://docs.broadcom.com/docs/AV02-0940EN" H 1840 10235 50  0001 C CNN
F 4 "C110020" H 2690 9685 50  0001 C CNN "LCSC"
	1    2690 9685
	1    0    0    -1  
$EndComp
$Comp
L Connector:AudioJack3 J9
U 1 1 5FA32BA9
P 5470 9640
F 0 "J9" H 5470 9890 50  0000 R CNN
F 1 "MIDI_TRS_Out" H 5670 10040 50  0000 R CNN
F 2 "Connector_Audio:Jack_3.5mm_Ledino_KB3SPRS_Horizontal" H 5470 9640 50  0001 C CNN
F 3 "~" H 5470 9640 50  0001 C CNN
F 4 " " H 5470 9640 50  0001 C CNN "LCSC"
	1    5470 9640
	-1   0    0    1   
$EndComp
$Comp
L Connector:AudioJack3 J8
U 1 1 5FA32089
P 1240 9685
F 0 "J8" H 1190 9385 50  0000 R CNN
F 1 "MIDI_TRS_In" H 1290 9285 50  0000 R CNN
F 2 "Connector_Audio:Jack_3.5mm_Ledino_KB3SPRS_Horizontal" H 1240 9685 50  0001 C CNN
F 3 "~" H 1240 9685 50  0001 C CNN
F 4 " " H 1240 9685 50  0001 C CNN "LCSC"
	1    1240 9685
	1    0    0    -1  
$EndComp
$Comp
L Momentum-rescue:MICROMOD-2222-C-MicroMod J6
U 1 1 5F9D7F58
P 10385 3020
F 0 "J6" H 10385 5306 59  0000 C CNN
F 1 "MICROMOD-2222-C" H 10385 5201 59  0000 C CNN
F 2 "MicroMod:M.2-CONNECTOR-E" H 10385 3020 50  0001 C CNN
F 3 "~" H 10385 3020 50  0001 C CNN
F 4 "C841659" H 10385 3020 50  0001 C CNN "LCSC"
	1    10385 3020
	1    0    0    -1  
$EndComp
$Comp
L Momentum-rescue:ElectroTechiquePCBLogo-Graphic ET1
U 1 1 61252C34
P 14620 8660
F 0 "ET1" H 14645 8706 50  0000 L CNN
F 1 "ElectroTechiquePCBLogo" H 14645 8615 50  0000 L CNN
F 2 "ElectroTechnique:ElectroTechniquePCBLogo" H 14620 8660 50  0001 C CNN
F 3 "~" H 14620 8660 50  0001 C CNN
F 4 " " H 14620 8660 50  0001 C CNN "LCSC"
	1    14620 8660
	1    0    0    -1  
$EndComp
$EndSCHEMATC
