/*
* @file TheRecorder.h
* Copyright (C) 2014 Cory J. Potter - All Rights Reserved
* You may use, distribute and modify this code under the
* terms of the LICENSE.txt
* NOT INTENDED FOR COMMERCIAL USE!
* You should have received a copy of the LICENSE.txt with
* this file. If not, please write to: <bitsandbots@gmail.com>
*/

/**
* @file TheRecorder.h
*	to know where you are, you have to know where you've been.
*/

#ifndef __THERECORDER_H__
#define __THERECORDER_H__

#ifdef ARDUINO

/************************************************************************/

bool saveAllDataObjects ( void )
{
	bool r = false;

	Sensor *sensor = &Sensor_FeedFloat;
	Appliance *app = &Appliance_FeedPump;
	Alert *alert = &Alert_System;

	slotSize = sizeof( txDataObject );
	num = EEPROM.read(0);
	// catch numbers out-of-bounds
	if( ( num < 1 ) || ( num > 30 ) ){ 	if ( DEBUG ) { Serial.print(F("bad num: ")); Serial.println( num ); } return r; }
	start = 0;
	// 	if ( DEBUG )
	// 	{
	// 		Serial.print(F(" Number / slotSize: "));
	// 		Serial.print( num );
	// 		Serial.print(" / ");
	// 		Serial.println(slotSize);
	// 	}
	num = 1;

	if ( DEBUG ) { Serial.print(F(" Saving Data ")); }

	for (; sensor != NULL; sensor = sensor->next )
	{
		start = ( num * slotSize ) + 1; // Figure out where the next slot starts
		setDataObject( &txDataObject, sensor );
		if( EEPROM_writeAnything( start, txDataObject ) > 0 )
		{
			r = true; num++; // Increment the counter by one
			printDataObject( &txDataObject );
			successWrite(); } else { failedWrite();
		}
	} // end for

	for (; app != NULL; app = app->next )
	{
		start = ( num * slotSize ) + 1; // Figure out where the next slot starts
		setDataObject( &txDataObject, app );

		if( EEPROM_writeAnything( start, txDataObject ) > 0 )
		{
			r = true; num++; // Increment the counter by one
			printDataObject( &txDataObject );
			successWrite(); } else { failedWrite();
		}
	} // end for


	for (; alert != NULL; alert = alert->next )
	{
		start = ( num * slotSize ) + 1; // Figure out where the next slot starts
		setDataObject( &txDataObject, (Alert*) alert );
		if( EEPROM_writeAnything( start, txDataObject ) > 0 )
		{	r = true; num++; // Increment the counter by one
			printDataObject( &txDataObject );
			successWrite(); } else { failedWrite();
		}
	} // end for

	if ( r == true )
	{
		if ( DEBUG ) { Serial.print(F(" Number: ")); Serial.println( num ); }
		if ( EEPROM.read(0) != num ) { EEPROM.write( 0, num ); } // Write the new count to the counter
		successWrite();
		} else { failedWrite();
	}
	return r;
}


// Read an ID from EEPROM and save it to the struct
bool readAllDataObjects( DataObject *rxDataObject ) // Number = position in EEPROM to get the 8 Bytes from
{
	bool r = false;

	slotSize = sizeof( &rxDataObject );
	num = EEPROM.read(0);
	// catch numbers out-of-bounds
	if( ( num < 1 ) || ( num > 30 ) ){ 	if ( DEBUG ) { Serial.print(F("bad num: ")); Serial.println( num ); } return r; }
	start = 0;
	// 		if ( DEBUG ) {
	// 			Serial.print(F(" Number / slotSize: "));
	// 			Serial.print( num );
	// 			Serial.print(" / ");
	// 			Serial.println(slotSize);
	// 		}

	if (num != ( totalSensors + totalAppliances + totalAlerts ) ) {
		// we shall have no good data to read
		// abort
		return false;
	}

	// skip one hidden dataObject
	num = 2;
	
	Sensor *sensor = &Sensor_FeedFloat;
	Appliance *app = &Appliance_FeedPump;
	Alert *alert = &Alert_System;

	for (; sensor != NULL; sensor = sensor->next )
	{
		start = ( num * slotSize ) - ( slotSize - 1 ); // Figure out starting position
		if (EEPROM_readAnything( start, rxDataObject ) )
		{
			r = true; num++; // Increment the counter by one
			SyncSensor( rxDataObject );
			printDataObject( rxDataObject );
		}
	} // end for

	for (; app != NULL; app = app->next )
	{
		start = ( num * slotSize ) - ( slotSize - 1 ); // Figure out starting position
		if ( EEPROM_readAnything( start, rxDataObject ) )
		{
			r = true; num++; // Increment the counter by one
			SyncAppliance( rxDataObject );
			printDataObject( rxDataObject );
		}
	} // end for


	for (; alert != NULL; alert = alert->next )
	{
		start = ( num * slotSize ) - ( slotSize - 1 ); // Figure out starting position
		if ( EEPROM_readAnything( start, rxDataObject ) )
		{
			r = true; num++; // Increment the counter by one
			SyncAlert( rxDataObject );
			printDataObject( rxDataObject );
		}
	}
	return r;
}

/*
// Delete an array stored in EEPROM from the designated slot
void deleteDataObject( uint8_t slotNumber )
{
//	bool r = false;

unsigned int start;// = ( num * 5 ) + 1; // Figure out where the next slot starts
uint8_t looping; // The number of times the loop repeats
uint8_t j;

slotSize = sizeof( eepromDataObject );
uint8_t count = EEPROM.read( 0 ); // Read the first Byte of EEPROM that
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

*/

#else
#error This code is only for use on Arduino.
#endif // ARDUINO

#endif // __THERECORDER_H__
