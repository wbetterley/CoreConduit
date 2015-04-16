/*
* @file Controller_nRF.ino
* Copyright (C) 2014 Cory J. Potter - All Rights Reserved
* You may use, distribute and modify this code under the
* terms of the LICENSE.txt
* NOT INTENDED FOR COMMERCIAL USE!
* You should have received a copy of the LICENSE.txt with
* this file. If not, please write to: <bitsandbots@gmail.com>
*/

/*
Arduino Networked Communications
Manage Sensors and Appliances
Share Data with UI
*/

/************************************************************************/
/*
Remove Dead Code
If your project is a mash-up of code from several sources, chances are there are parts
that are not getting used and can be eliminated to save space.

Unused Libraries - Are all the #include libraries actually used?
Unused Functions - Are all the functions acutally being called?
Unused Variables - Are all the variables actually being used?
Unreachable Code - Are there conditional expressions which will never be true?

Hint - If you are not sure about an #include, a function or a variable. Comment it out.
If the program still compiles, that code is not being used.

Consolidate Repeated Code
If you have the same sequence of code statements in two or more places, consider making a function out of them.

Eliminate the Bootloader
If space is really-really tight, you might consider eliminating the bootloader.
This can save as much as 2K or 4K of Flash - depending on which bootloader you are currently using.
The downside of this is that you will need to load your code using an ISP programmer
instead of via a standard USB cable.



*/
/************************************************************************/

//
// Hardware configuration
//

#include <Wire.h>
#include <I2C_Anything.h>
#include <Time.h>
#include <commons.h>
#include <Clock.h>
#include <TheTimeMan.h>
// replaced DS1307 library in TheTimeMan.h
#include <SPI.h>
//#include <Ethernet.h>
#include "nRF24L01.h"
#include "RF24.h"
#include <EEPROM.h>
#include <EEPROMAnything/EEPROMAnything.h>
#include <DHT.h>
#include <DataObject.h>
#include <CoreConduit.h>
#include <CoreConduitRF.h>
#include <TheDecider.h>
//#include <TheNotifier.h>
#include <MySettings.h>
//#include <TheRecorder.h>
#include <RCSwitchLite.h>

/*-----( Declare Constants )-----*/

//#define aref_voltage 3.3         // we tie 3.3V to ARef and measure it with a multimeter!
#define aref_voltage 5.0         // we tie 3.3V to ARef and measure it with a multimeter!

// Uncomment whatever type you're using!
#define DHTTYPE DHT22   // DHT 22  (AM2302)
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

// Water Tank Depth Pressure Sensor
#define TANK_SENSOR 0
#define TANK_EMPTY 0
#define TANK_FULL  1023

// Humidity Temperature Sensor
// Initialize DHT sensor for normal 16mhz Arduino
DHT dht(Sensor_Humidity.pin, DHTTYPE);
// Connect pin 1 (on the left) of the sensor to +5V
// NOTE: If using a board with 3.3V logic like an Arduino Due connect pin 1
// to 3.3V instead of 5V!
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND
// Connect a 4.7k - 10K resistor from pin 2 (data) to pin 1 (power) of the sensor

/************************************************************************/
/*	I2C Communications                                                  */
/************************************************************************/
const int MY_ADDRESS = 42;
const int SEND_TO_ADDRESS = 22;
/************************************************************************/
/*  Timers			                                            		*/
/************************************************************************/
Timer Timer_Log 			= { TIMER_LOG, 300000UL, true, false, 0, NULL };
// Timer Timer_Lcd 			= { TIMER, 12000UL, true, false, 0, &Timer_Log };
// Timer Timer_Lcd_Cycle 		= {	TIMER_LCD_CYCLE, 4000UL, true, false, 0, &Timer_Lcd };
// Timer Timer_Lcd_Scroller 	= { TIMER_LCD_SCROLLER, 1000UL, true, false, 0, &Timer_Lcd_Cycle };
Timer Timer_rxData			= { TIMER_RX_DATA, 2500L, true, false, 0, &Timer_Log };
Timer Timer_Save_Settings 	= {	TIMER_SAVE_SETTINGS, 3600000UL, true, false,  0, &Timer_Log };
Timer Timer_Sensor_Read		= { TIMER_SENSOR_READINGS, 5000UL, true, false,  0, &Timer_Save_Settings };
Timer Timer_Alerts			= { TIMER_ALERTS, 60000UL, true, false,  0, &Timer_Sensor_Read };
/************************************
*               Setup              *
************************************/
void setup()
{
	
	pinMode( PIN5, OUTPUT );
	digitalWrite( PIN5, LOW );
	
	// 	RF24 radio(PIN9_RF_CE, PIN10_CS);   // make sure this corresponds to the pins you are using
	// 	const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };
	// *SPI:							(Arduino Pin) - Module Pin
	// 10 (SS) "Slave Select"			  10 (SS) to CS
	// 11 (MOSI) "Master Out Slave In"   11 (MOSI) to DI
	// 12 (MISO) "Master In Slave Out"   12 (MISO) to DO
	// 13 (SCK) "System Clock"           13 (SCK) to CLK
	// and G to GND and + to 5V
	//

	// Ground
	pinMode( PIN2_INT0, OUTPUT );
	digitalWrite( PIN2_INT0, LOW );
	// RF Module for Remote Switches
	pinMode( PIN4, OUTPUT );
	// Power
	pinMode( PIN3_INT1, OUTPUT );
	digitalWrite( PIN3_INT1, HIGH );
	
	/************************************************************************/
	/*  Interrupts		                                                    */
	/************************************************************************/
	// attachInterrupt(0, interrupted, CHANGE);
	// two interrupts are available: #0 on pin 2,
	// and interrupt				 #1 on pin 3.

	/************************************************************************/
	// Start the serial port to communicate to the PC
	isDebugMode(0);
	/************************************************************************/
	/* I2C Communications                                                   */
	/************************************************************************/
	Wire.onReceive ( receiveEvent );
	// 	Wire.onRequest ( requestEvent );
	Wire.begin (MY_ADDRESS);
	/************************************************************************/
	//checkConsoleTime();
	clockFlag = setClock();
	/************************************************************************/
	//readHiddenDataObject();
	if ( readHiddenDataObject() != true )
	{
		txDataObject.freq		= 42;
		txDataObject.type		= 22;
		txDataObject.timestamp	= now();
		txDataObject.value		= 99999.99;
		txDataObject.ready		= true;
		txDataObject.state		= true;
		txDataObject.triggered	= false;
		
		saveOneDataObject( &txDataObject );
		if ( readHiddenDataObject() != true ) { wipeMemory(0); }
	}
	/************************************************************************/

	radio.begin();
	radio.openWritingPipe(pipes[0]);
	radio.openReadingPipe(1,pipes[1]);
	radio.startListening();

	randomSeed( analogRead( PIN_A0 ) );
	dht.begin();
	initializeAppliances();
}

/*********************************
*        STaTe MaCHiNe          *
*********************************/
void loop()
{

// 	if ( radio.available() )
// 	{
// 		// reset the timer
// 		checkTimer( &Timer_rxData);
// 		DataObject * _rxDataObject = &rxDataObject;
// 		radio.read( _rxDataObject, sizeof(rxDataObject) );
// 		if ( rxDataObject.type < 1) { if ( DEBUG ) { Serial.print(F("err on ")); Serial.println(expectedDataCounter); } }
// 
// 		if (DEBUG) { Serial.print(F("expected:  ")); Serial.print(expectedDataCounter); Serial.print("  "); }
// 		if ( DEBUG ) { printDataObject( &rxDataObject ); }
// 
// 		switch ( rxDataObject.object )
// 		{
// 			case 'a':
// 			if ( SyncAppliance( &rxDataObject ) ) { expectedDataCounter++; } else { if (DEBUG) { Serial.print(F("error syncing app ")); Serial.println(expectedDataCounter); } }
// 			break;
// 			
// 			case 's':
// 			if ( SyncSensor( &rxDataObject ) ) { expectedDataCounter++; } else { if(DEBUG) { Serial.print(F("error syncing sensor ")); Serial.println(expectedDataCounter); } }
// 			break;
// 			
// 			case 'n':
// 			break;
// 			
// 			default:
// 			break;
// 		}
// 		if ( expectedDataCounter >= (totalSensors + totalAppliances) )
// 		{
// 			if (DEBUG) { Serial.print(F("resetting with ")); Serial.println(expectedDataCounter); }
// 			expectedDataCounter = 0;
// 		}
// 	} // end if
// 
// 	if ( ( expectedDataCounter > 0 ) && ( checkTimer( &Timer_rxData ) ) ) { if( DEBUG ) { Serial.print("timed out: "); Serial.println(expectedDataCounter); } expectedDataCounter = 0;  }




	// YIELD to other processes and delay for the amount of time we want between readings
	// rxSwitchCodes();

	// If we receive anything, it will be appliance updates/overrides


	if ( radio.available() )
	{
		// reset the timer
		checkTimer( &Timer_rxData);
		
		radio.read( &rxDataObject, sizeof(rxDataObject) );
		if ( rxDataObject.type < 1)
		{ if (DEBUG ) { Serial.print(F("err on ")); Serial.println(expectedDataCounter); }

			} else {
			
			if (expectedDataCounter < totalAppliances )
			{ if ( SyncAppliance( &rxDataObject ) ) { expectedDataCounter++; } else { if (DEBUG) { Serial.print(F("err sync app ")); Serial.println(expectedDataCounter); } } }
			
			if ( expectedDataCounter >= totalAppliances )
			{
				if (DEBUG) { Serial.print(F("resetting with ")); Serial.println(expectedDataCounter); }
				expectedDataCounter = 0;
			}
		}
	} // end if

	if ( ( expectedDataCounter > 0 ) && ( checkTimer( &Timer_rxData ) ) )
	{ if( DEBUG ) { Serial.print("timed out: "); Serial.println(expectedDataCounter); } expectedDataCounter = 0;  }

	// I2C Communications
	if ( haveData > 0 ) { receiveData( haveData ); checkTimer( &Timer_Sensor_Read ); haveData = 0; }

	/************************************************************************/
	/*  Timers			                                            		*/
	/************************************************************************/
if ( clockFlag == true ) {
	if ( checkTimer( &Timer_Log ) ) {  checkAppliances(); }
	if ( checkTimer( &Timer_Sensor_Read ) ) { keepAlive(); readSensors(); TheDecider(); }
}

	if ( checkTimer( &Timer_Alerts ) )
	{
		tx_nRF_SensorData( &Sensor_FeedFloat );
		delay(100);
		tx_nRF_ApplianceData( &Appliance_FeedPump );
	}

	// 	if ( ( checkTimer( &Timer_Save_Settings ) ) || ( Timer_Save_Settings.triggered == true ) )
	// 	{ saveAllDataObjects(); Timer_Save_Settings.triggered = false; }

} // end loop

/************************************************************************/
/* SENSOR READINGS                                                      */
/************************************************************************/
void readSensors()
{

	conservePower();
	
	readPhotocell();
	readDHT();
	readSoilMoisture();
	readFloat();
	readTankPressure();
	
	if ( DEBUG ) {
		Serial.print(F("  Photocell: "));
		Serial.print( Sensor_Light.value );
		Serial.print(F("  SoilMoisture: "));
		Serial.print( Sensor_Moisture.value );
		Serial.print(F("  TankLevel: "));
		Serial.print( Sensor_TankLevel.value );
		Serial.println("");
	}
	
	conservePower();
}


/************************************************************************/
/* SENSOR SUBROUTINES                                                   */
/************************************************************************/

/**
* readTankFloat()
*
* Reads the Water Depth Sensor MPX20xx
* returns int 0-100 representation of tank level.
*/
bool readTankPressure( void )
{
	bool r = false;
	
	pinMode(Sensor_TankLevel.pin, INPUT_PULLUP);
	int sensorValue = analogRead( Sensor_TankLevel.pin );

	// map(value, fromLow, fromHigh, toLow, toHigh)
	// value: the number to map
	// fromLow: the lower bound of the value's current range
	// fromHigh: the upper bound of the value's current range
	// toLow: the lower bound of the value's target range
	// toHigh: the upper bound of the value's target range
int constrainedValue = 0;
int tankLevel = 0;

	if ( sensorValue )
	{
		constrainedValue = constrain( sensorValue, TANK_EMPTY, TANK_FULL );
		tankLevel = map( constrainedValue, TANK_EMPTY, TANK_FULL, 0, 100 );
		Sensor_TankLevel.timestamp = millis();
		Sensor_TankLevel.value = tankLevel;
		r = true;
	}

	if ( DEBUG ) {
		Serial.print(F("  Raw: "));
		Serial.print( sensorValue );
		Serial.print(F("  constrained: "));
		Serial.print( constrainedValue );
		Serial.print(F("  TankLevel: "));
		Serial.print( tankLevel );
		Serial.println("");
	}


	return r;
}


/************************************************************************/
/* DHT Humidity Temperature Sensor                                      */
/************************************************************************/
bool readDHT()
{
	bool r = true;

	// Reading temperature or humidity takes about 250 milliseconds!
	// Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
	float h = dht.readHumidity();
	// Read temperature as Celsius
	float t = dht.readTemperature();
	// Read temperature as Fahrenheit
	float f = dht.readTemperature(true);

	// Check if any reads failed and exit early (to try again).
	if (isnan(h) || isnan(t) || isnan(f) ||	( h == 0 ) || ( t == 0 ) || ( f == 0 ) )
	{
		Sensor_Humidity.value = 0;
		Sensor_Humidity.timestamp = millis();

		Sensor_Temp.value = 0;
		Sensor_Temp.timestamp = millis();
		
		if ( DEBUG ) { Serial.println(F("Failed to read from DHT sensor!")); }
		r = false;
	}
	else
	{
		
		Sensor_Humidity.value = h;
		Sensor_Humidity.timestamp = millis();

		Sensor_Temp.value = f;
		Sensor_Temp.timestamp = millis();

	}
	/*
	if ( DEBUG ) {
	Serial.print("RH: ");
	Serial.print(h);
	Serial.print(" %\t");
	Serial.print("Temp: ");
	Serial.print(t);
	Serial.print(" *C ");
	Serial.print(f);
	Serial.print(" *F\t");
	Serial.println("");
	}
	*/

	return r;
}

bool readFloat()
{
	bool r = false;
	//bool sensorValue = false;

	pinMode(Sensor_FeedFloat.pin, INPUT_PULLUP);
	int sensorValue = digitalRead( Sensor_FeedFloat.pin );

	Sensor_FeedFloat.timestamp = millis();
	Sensor_FeedFloat.value = sensorValue;

	return r;
}

bool readPhotocell()
{
	bool r = false;

	pinMode(Sensor_Light.pin, INPUT);
	int sensorValue = analogRead( Sensor_Light.pin );
	// 	if ( DEBUG ) {
	// 		Serial.print(F("before Photocell: "));
	// 		Serial.print( sensorValue );
	// 		Serial.println("");
	// 	}

	sensorValue = map( sensorValue, 1, 1023, 100, 1 );
	// map(value, fromLow, fromHigh, toLow, toHigh)
	// value: the number to map
	// fromLow: the lower bound of the value's current range
	// fromHigh: the upper bound of the value's current range
	// toLow: the lower bound of the value's target range
	// toHigh: the upper bound of the value's target range


	if(sensorValue)
	{
		Sensor_Light.timestamp = millis();
		Sensor_Light.value = sensorValue;
		r = true;
	}

	return r;
}
bool readSoilMoisture()
{
	bool r = false;

	pinMode(Sensor_Moisture.pin, INPUT);
	int sensorValue = analogRead( Sensor_Moisture.pin );
	// 	if ( DEBUG ) {
	// 		Serial.print(F("before Moisture: "));
	// 		Serial.print( sensorValue );
	// 		Serial.println("");
	// 	}

	sensorValue = map( sensorValue, 1, 1023, 100, 1 );
	// map(value, fromLow, fromHigh, toLow, toHigh)
	// value: the number to map
	// fromLow: the lower bound of the value's current range
	// fromHigh: the upper bound of the value's current range
	// toLow: the lower bound of the value's target range
	// toHigh: the upper bound of the value's target range

	if(sensorValue)
	{
		Sensor_Moisture.timestamp = millis();
		Sensor_Moisture.value = sensorValue;
		r = true;
	}

	return r;
}

// bool checkAlerts( void )
// {
// 	bool r = false;
//
// 	// Start at the beginning struct
// 	Alert *alert = &Alert_System;
//
// 	for (; alert != NULL; alert = alert->next ) {
// 		// TheNotifier returns true if an alert has been triggered
// 		// but only the initial trigger
// 		r = TheNotifier( alert );
// 	}
// 	return r;
// }

