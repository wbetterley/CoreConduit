/*
* @file Receiver_nRF_I2C.ino
* Copyright (C) 2014 Cory J. Potter - All Rights Reserved
* You may use, distribute and modify this code under the
* terms of the LICENSE.txt
* NOT INTENDED FOR COMMERCIAL USE!
* You should have received a copy of the LICENSE.txt with
* this file. If not, please write to: <bitsandbots@gmail.com>
*/

/*
Arduino Networked Communications
Requires nRF module and connection with LCD Shield+buttons on Arduino Uno Pin A0
*/


/************************************************************************/
/**

Arduino Uno

*I2C:
A4 (SDA)
A5 (SCL)

*SPI:							(Arduino Pin) - Module Pin
10 (SS) "Slave Select"			  10 (SS) to CS

11 (MOSI) "Master Out Slave In"   11 (MOSI) to DI
12 (MISO) "Master In Slave Out"   12 (MISO) to DO
13 (SCK) "System Clock"           13 (SCK) to CLK
and G to GND and + to 5V

**/
/************************************************************************/

#include <Wire.h>
#include <I2C_Anything.h>
#include <Time.h>
#include <commons.h>
#include <Clock.h>
#include <TheTimeMan.h>
// replaced DS1307 library in TheTimeMan.h
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
//#include <LiquidCrystal.h>
#include <EEPROM.h>
#include <EEPROMAnything.h>
#include <DataObject.h>
#include <CoreConduit.h>
#include <CoreConduitRF.h>
#include <Alerts.h>
#include <TheNotifier.h>
#include <MySettings.h>
#include <TheRecorder.h>
//#include <Commons/CoreLCD.h>

// RF24 radio(9,10);   // make sure this corresponds to the pins you are using
// const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };

/************************************************************************/
/*  Network Configuration                                               */
/************************************************************************/
/************************************************************************/
/*	I2C Communications                                                  */
/************************************************************************/
const int MY_ADDRESS = 42;
//const int SEND_TO_ADDRESS = 22;
const int SEND_TO_LCDISPLAY = 22;
const int SEND_TO_SDLOGGER = 32;

/************************************************************************/
/*  Timers			                                            		*/
/************************************************************************/

Timer Timer_Log 			= { TIMER_LOG, 60000UL, true, false, 0, NULL };
// Timer Timer_Lcd 			= { TIMER_LCD, 12000UL, true, false, 0, &Timer_Log };
// Timer Timer_Lcd_Cycle 		= {	TIMER_LCD_CYCLE, 6000UL, true, false, 0, &Timer_Lcd };
// Timer Timer_Lcd_Scroller 	= { TIMER_LCD_SCROLLER, 500UL, true, false, 0, &Timer_Lcd_Cycle };
Timer Timer_rxData			= { TIMER_RX_DATA, 6000UL, true, false, 0, &Timer_Log };
Timer Timer_Save_Settings 	= {	TIMER_SAVE_SETTINGS, 3600000UL, true, false,  0, &Timer_rxData };
//Timer Timer_Sensor_Read		= { SENSOR_READINGS_TIMER, 15000UL, true, false,  0, &Timer_Save_Settings };

void setup()
{
	// If you want to set the aref to something other than 5v
	// analogReference(EXTERNAL);

/************************************************************************/
/* 
 - CONNECTIONS: nRF24L01 Modules See:
 http://arduino-info.wikispaces.com/Nrf24L01-2.4GHz-HowTo
 1 - GND
 2 - VCC 3.3V !!! NOT 5V
 3 - CE to Arduino pin 9
 4 - CSN to Arduino pin 10
 5 - SCK to Arduino pin 13
 6 - MOSI to Arduino pin 11
 7 - MISO to Arduino pin 12
 8 - UNUSED

 NOTE! Power Problems:
 Many users have had trouble getting the nRF24L01 modules to work. 
 Many times the problem is that the 3.3V Power to the module does not have enough current capability,
 or current surges cause problems. Here are suggestions:
	- Connect a .3.3 uF to 10 uF (MicroFarad) capacitor directly on the module from +3.3V to Gnd (Watch + and - !)
		[Some users say 10 uF or more..]
	- A separate 3.3V power supply (Maybe this one?)
	- An Arduino compatible, which has an added 3.3V regulator (But maybe add a .1 uF capacitor on the radio module).
** This is especially important if you are connecting the module with jumper wires.

*/
/************************************************************************/

//  pinMode(PIN_A0, INPUT);

	// Ground
	pinMode( PIN2_INT0, OUTPUT );
	digitalWrite( PIN2_INT0, LOW );
	// Buzzer
	pinMode( PIN3_INT1, OUTPUT );
	// Power
	pinMode( PIN4, OUTPUT );
	digitalWrite( PIN4, HIGH );


	/************************************************************************/
	/*  Interrupts		                                                    */
	/************************************************************************/
	// attachInterrupt(0, interrupted, CHANGE);
	// two interrupts are available: #0 on pin 2,
	// and interrupt				 #1 on pin 3.
	/************************************************************************/

	// Start the serial port to communicate to the PC
	isDebugMode(1);
	/************************************************************************/
	/* I2C Communications                                                   */
	/************************************************************************/
	Wire.onReceive ( receiveEvent );
	Wire.begin (MY_ADDRESS);
	/************************************************************************/
	clockFlag = setClock();
	/************************************************************************/
	//readHiddenDataObject();
	rxDataObject.freq		= 250;
	rxDataObject.type		= 250;
	rxDataObject.timestamp	= now();
	rxDataObject.value		= 99999.99;
	rxDataObject.ready		= true;
	rxDataObject.state		= true;
	rxDataObject.triggered	= false;
	if ( readHiddenDataObject() != true )
	{
		saveOneDataObject( &rxDataObject );
		if ( readAllDataObjects( &rxDataObject ) != true ) { wipeMemory(0); }
	}
	/************************************************************************/
	radio.begin();
	radio.openWritingPipe(pipes[1]);
	radio.openReadingPipe(1,pipes[0]);
	radio.startListening();
	
	randomSeed(analogRead(0));
	sendAudibleAlert();
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

	if ( radio.available() )
	{
		// reset the timer
		checkTimer( &Timer_rxData);
		radio.read( &rxDataObject, sizeof(rxDataObject) );
		if ( rxDataObject.type < 1) { if ( DEBUG ) { Serial.print(F("err on ")); Serial.println(expectedDataCounter); } }
		
		if (DEBUG) { Serial.print(F("expected:  ")); Serial.print(expectedDataCounter); Serial.print("  "); }
		if ( DEBUG ) { printDataObject( &rxDataObject ); }

		if (expectedDataCounter < totalSensors)
		{
			if ( SyncSensor( &rxDataObject ) ) { expectedDataCounter++; } else { if(DEBUG) { Serial.print(F("err sync sensor ")); Serial.println(expectedDataCounter); } }
			} else { // if ( expectedDataCounter > sensors )
			
				if (expectedDataCounter < (totalSensors + totalAppliances) )
				{
					if ( SyncAppliance( &rxDataObject ) )
					{ expectedDataCounter++; } else { if (DEBUG) { Serial.print(F("err sync app ")); Serial.println(expectedDataCounter); } }
				}
			} // end else expectedDataCounter
			if ( expectedDataCounter >= (totalSensors + totalAppliances) )
			{
				if (DEBUG) { Serial.print(F("resetting with ")); Serial.println(expectedDataCounter); }
				expectedDataCounter = 0;
			}
		// 			} // end else
		// 		} // end while

	} // end if

	if ( ( expectedDataCounter > 0 ) && ( checkTimer( &Timer_rxData ) ) ) { if( DEBUG ) { Serial.print(F("timed out: ")); Serial.println(expectedDataCounter); } expectedDataCounter = 0;  }

	// I2C Communications
	if ( haveData > 0 ) {
		receiveData( haveData );
		checkTimer( &Timer_Log );
		haveData = 0;
	}

	if ( requestSync ) { tx_nRF_Request(); requestSync = 0; }

	/************************************************************************/
	/*  Timers			                                            		*/
	/************************************************************************/
	if ( checkTimer( &Timer_Log ) )
	{
		// I2C with LCDISPLAY Unit
		txSensorData( &Sensor_FeedFloat, SEND_TO_LCDISPLAY );
		txApplianceData( &Appliance_FeedPump, SEND_TO_LCDISPLAY );
		txAlertData( &Alert_System, SEND_TO_LCDISPLAY );
		// I2C with SDLOGGER Unit
// 		txSensorData( &Sensor_FeedFloat, SEND_TO_SDLOGGER );
// 		txApplianceData( &Appliance_FeedPump, SEND_TO_SDLOGGER );
		// txAlertData( &Alert_System, SEND_TO_ADDRESS );
		// Check for alerts here and use the buzzer
		if ( checkAlerts() ) { sendAudibleAlert(); }
		keepAlive();
		rxCounter = 0;
	}


	/************************************************************************/
	/*   EEPROM Save Timer                                                   */
	/************************************************************************/
	if ( checkTimer( &Timer_Save_Settings ) )
	{
		if ( clockFlag ) { saveAllDataObjects(); }
		// Check for alerts here and use the buzzer
		if ( countAlerts() > 0 ) { sendAudibleAlert(); }
	}
}

uint8_t getAlertType( void )
{
		uint8_t r = 0;
		// Start at the beginning struct
		Alert *alert = &Alert_System;

		for (; alert != NULL; alert = alert->next ) {
			// TheNotifier returns true if an alert has been triggered
			// but only the initial trigger
			if ( alert->triggered )
			{
				r = alert->type;
				break;
			}
		}
		
		// add 1 to account for base 0
		return r++;
}
void sendAudibleAlert( void )
{

	uint8_t howManyTimes = getAlertType();


	pinMode( PIN2_INT0, OUTPUT );
	pinMode( PIN3_INT1, OUTPUT );
	pinMode( PIN4, OUTPUT );

	// Power ON
	digitalWrite( PIN4, HIGH );
	// Ground
	digitalWrite( PIN2_INT0, LOW );

for ( ; howManyTimes > 0; howManyTimes-- )
{
// send audible
	waka( PIN3_INT1 );
}

	// Power OFF
	digitalWrite( PIN4, LOW );
	// Ground
	digitalWrite( PIN2_INT0, LOW );

}

bool checkAlerts( void )
{
	bool r = false;
	
	// Start at the beginning struct
	Alert *alert = &Alert_System;

	for (; alert != NULL; alert = alert->next ) {
		// TheNotifier returns true if an alert has been triggered
		// but only the initial trigger
		r = TheNotifier( alert );
	}
	return r;
}

void beep (int speakerPin, float noteFrequency, long noteDuration)
{
	int x;
	// Convert the frequency to microseconds
	float microsecondsPerWave = 1000000/noteFrequency;
	// Calculate how many HIGH/LOW cycles there are per millisecond
	float millisecondsPerCycle = 1000/(microsecondsPerWave * 2);
	// Multiply noteDuration * number or cycles per millisecond
	float loopTime = noteDuration * millisecondsPerCycle;
	// Play the note for the calculated loopTime.
	for (x=0;x<loopTime;x++)
	{
		digitalWrite(speakerPin,HIGH);
		delayMicroseconds(microsecondsPerWave);
		digitalWrite(speakerPin,LOW);
		delayMicroseconds(microsecondsPerWave);
	}
}


void waka( uint8_t speakerPin ) {
	for (int i=1000; i<3000; i=i*1.05) {
		beep(speakerPin,i,10);
	}
	delay(100);
	for (int i=2000; i>1000; i=i*.95) {
		beep(speakerPin,i,10);
	}
}

