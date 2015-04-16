/*
Copyright (C) 2014 Cory J. Potter <cor75@hotmail.com>

*/

/**
* @file TheDataLogger.h
*	to know where you are, you have to know where you've been.
*/

#ifndef __THEDATALOGGER_H__
#define __THEDATALOGGER_H__

#ifdef ARDUINO

//#include <EEPROM.h>
//#include <EEPROMAnything.h>

// Tell it where to store your config data in EEPROM
#define memoryBase 32
unsigned int slotSize = 8;
unsigned int settingsAddress = 0;
unsigned int logdataAddress = 0;
unsigned int lastAddress = 0;

bool wipeMemory ( void );
/************************************************************************/
/* SETTINGS

What settings should be kept?

* Last Save Timestamp
* Current Phase of Plant Life Cycle

PrimaryKey = timestamp (5 bytes until year 2300 )
timestamp | Temperature(C) | Humidity(RH) |

EEPROM_readAnything(0, configuration);
EEPROM_writeAnything(0, configuration);


*/
/************************************************************************/

typedef struct DataLog
{
	unsigned long timestamp;
	int tempC;
	int humidity;
} DataLog;

DataLog tempDataLog = {};

void successWrite( void )
{
	if ( DEBUG ) { Serial.println("EEPROM:  Write Successful! "); }
}
void successDelete( void )
{ 
	if ( DEBUG ) { Serial.println("EEPROM:  Delete Successful! "); }
}
void failedWrite(void )
{ 
	if ( DEBUG ) { Serial.println("EEPROM:  Failed to Write! "); }
}

bool saveDataLog ( void ) {
	bool r = false;

	slotSize = sizeof(tempDataLog);

	uint8_t num = EEPROM.read(0);
	if ( DEBUG ) {
		Serial.print(F(" Number / slotSize: "));
		Serial.print( num );
		Serial.print(" / ");
		Serial.println(slotSize);

		Serial.print(F(" Saving Data "));
	}
	tempDataLog.timestamp = now();
	tempDataLog.tempC = Sensor_Temp.value;
	tempDataLog.humidity = Sensor_Humidity.value;

	if ( DEBUG ) {
		Serial.print( tempDataLog.timestamp );
		Serial.print( " | " );
		Serial.print(Sensor_Temp.value);
		Serial.print( " | " );
		Serial.println(Sensor_Humidity.value);
	}
	unsigned int start = ( num * slotSize ) + 1; // Figure out where the next slot starts

	if( EEPROM_writeAnything( start, tempDataLog ) > 0 )
	{
		r = true;
		num++; // Increment the counter by one
		if ( DEBUG ) {
			Serial.print(F(" Number: "));
			Serial.println( num );
		}
		EEPROM.write( 0, num ); // Write the new count to the counter
		successWrite();
	}
	else
	{
		failedWrite();
	}
	return r;
}

// Read an ID from EEPROM and save it to the struct
bool readDataLog( uint8_t number ) // Number = position in EEPROM to get the 5 Bytes from
{
	bool r = false;
	slotSize = sizeof(tempDataLog);
	unsigned int start = (number * slotSize ) - (slotSize - 1); // Figure out starting position
	EEPROM_readAnything(start, tempDataLog);
	return r;
}

// Delete an array stored in EEPROM from the designated slot
void deleteDataLog( uint8_t slotNumber )
{
	//	bool r = false;
	
	unsigned int start;// = ( num * 5 ) + 1; // Figure out where the next slot starts
	uint8_t looping; // The number of times the loop repeats
	uint8_t j;

	slotSize = sizeof(tempDataLog);
	uint8_t count = EEPROM.read(0); // Read the first Byte of EEPROM that
	// stores the number of ID's in EEPROM
	
	start = (slotNumber * slotSize) - ( slotSize - 1 );
	if ( DEBUG ) {
		Serial.print("Start: ");
		Serial.print(start);
		Serial.print(" | ");
	}
	looping = ( ( count - slotNumber ) * slotSize );
	if ( DEBUG ) {
		Serial.print("Looping: ");
		Serial.print(looping);
		Serial.print(" | ");
	}
	count--; // Decrement the counter by one

	EEPROM.write( 0, count ); // Write the new count to the counter

	for ( j = 0; j < looping; j++ ) // Loop the card shift times
	{
		EEPROM.write( start + j, EEPROM.read( start + slotSize + j ) ); // Shift the array values to slotSize places earlier in the EEPROM
	}
	successDelete();
}

bool wipeMemory ( void ) {
	bool r = false;

	if ( DEBUG ) { Serial.println(F("Wiping EEPROM memories!")); }
	
	for (int i = 0; i < 512; i++) // Loop repeats equal to the number of array in EEPROM
	{
		EEPROM.write(i, 0);
	}
	
	return r;
}

void printAllDataLogs( void )
{
	if ( DEBUG ) {
		uint8_t num = EEPROM.read(0);
		if ( num != 0 ) {
			for (uint8_t i = 1; i < num ;i++)
			{
				readDataLog( num );
				Serial.print( "tempDataLog:  " );
				Serial.print( tempDataLog.timestamp );
				Serial.print( " | " );
				Serial.print( tempDataLog.tempC );
				Serial.print( " | " );
				Serial.print( tempDataLog.humidity );
				Serial.println("");
			}
		}
	}
}





	#else
	#error This code is only for use on Arduino.
	#endif // ARDUINO

	#endif // __THEDATALOGGER_H__

