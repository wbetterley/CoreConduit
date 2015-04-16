# CoreConduit

The CoreConduit - Modular Arduino Garden Controller

  ____                ____                _       _ _   
 / ___|___  _ __ ___ / ___|___  _ __   __| |_   _(_) |_ 
| |   / _ \| '__/ _ \ |   / _ \| '_ \ / _` | | | | | __|
| |__| (_) | | |  __/ |__| (_) | | | | (_| | |_| | | |_ 
 \____\___/|_|  \___|\____\___/|_| |_|\__,_|\__,_|_|\__|
                                                        

The CoreConduit - Modular Arduino Garden Controller

In order to compile the sketches in this project you must have the needed libraries installed.  Inside the library directory included with the source-code are several library dependencies.  The directory "commons" is unique to this project and required.

The RF wireless outlets' remote control that you are using for this project is needed to receive the codes using the StartCore.ino sketch.

You will need to copy the codes received from the serial output to this file at the designated location:
/libraries/commons/TheDecider.h

Required Libraries:

Time @ http://www.arduino.cc
Tone @ http://www.arduino.cc
VirtualWire
RF24 @ https://github.com/maniacbug/RF24/archives/master
DHT22 sensor library developed by Ben Adams
DallasTemperature library at http://milesburton.com/index.php?title=Dallas_Temperature_Control_Library
OneWire code has been derived from http://www.arduino.cc/playground/Learning/OneWire
LiquidCrystal library from Adafruit Industries
SD library
SDfat library