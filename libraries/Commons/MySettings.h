/*
* @file MySettings.h
* Copyright (C) 2014 Cory J. Potter - All Rights Reserved
* You may use, distribute and modify this code under the
* terms of the LICENSE.txt
* NOT INTENDED FOR COMMERCIAL USE!
* You should have received a copy of the LICENSE.txt with
* this file. If not, please write to: <bitsandbots@gmail.com>
*/

/**
* @file MySettings.h
*	to know where you are, you have to know where you've been.
*/

#ifndef __MYSETTINGS_H__
#define __MYSETTINGS_H__

#ifdef ARDUINO

/************************************************************************/


// Tell it where to store your config data in EEPROM
unsigned int slotSize = 13;
uint8_t num = 0;
uint8_t start = 0;
// unsigned int settingsAddress = 0;
// unsigned int logdataAddress = 0;
// unsigned int lastAddress = 0;

bool wipeMemory ( void );
/************************************************************************/
void successWrite( void )
{
	// No news is good news ;-)
	// if ( DEBUG ) {  Serial.println("EEPROM:  Write Successful! ");  }
}
void successDelete( void )
{
	if ( DEBUG ) { Serial.println("EEPROM:  Delete Successful! "); }
}
void failedWrite(void )
{
	if ( DEBUG ) { Serial.println("EEPROM:  Failed to Write! "); }
}

// Read an ID from EEPROM and save it to the struct
bool readHiddenDataObject( void ) // Number = position in EEPROM to get the 8 Bytes from
{
	bool r = false;
	slotSize = sizeof( rxDataObject );
	num = EEPROM.read(0);
	// catch numbers out-of-bounds
	if( ( num < 1 ) || ( num > 30 ) ){ 	if ( DEBUG ) { Serial.print(F("bad num: ")); Serial.println( num ); } return r; }
	start = 0;
	//	if ( DEBUG ) { Serial.print(F(" Number / slotSize: ")); Serial.print( num ); Serial.print(" / "); Serial.println(slotSize); }
	num = 1;
	start = ( num * slotSize ) - ( slotSize - 1 ); // Figure out starting position
	if (EEPROM_readAnything( start, rxDataObject ) )
	{
		r = true;

		printDataObject( &rxDataObject );
	}
	return r;
}

bool saveOneDataObject ( DataObject *txDataObject )
{
	bool r = false;
	slotSize = sizeof( txDataObject );
	num = EEPROM.read(0);
	// catch numbers out-of-bounds
	if( ( num < 0 ) || ( num > 30 ) ){ 	if ( DEBUG ) { Serial.print(F("bad num: ")); Serial.println( num ); } return r; }
	start = 0;
	num = 0;
	if ( DEBUG ) { Serial.print(F(" Saving Hidden Data ")); }
	start = ( num * slotSize ) + 1; // Figure out where the next slot starts
	if( EEPROM_writeAnything( start, txDataObject ) > 0 )
	{
		r = true; num++; // Increment the counter by one
		printDataObject( txDataObject );
		successWrite(); } else { failedWrite();
	}

	if ( r == true )
	{
		if ( DEBUG ) { Serial.print(F(" Number: ")); Serial.println( num ); }
		if ( EEPROM.read(0) != num ) { EEPROM.write( 0, num ); } // Write the new count to the counter
		successWrite();
	}
	else
	{
		failedWrite();
	} // end if
	return r;
}

bool wipeMemory ( bool forceWipe ) {
	bool r = false;
	if ( ( forceWipe ) || ( ( EEPROM.read(0) ) != 0 ) )
	{
		if ( DEBUG ) { Serial.println(F("Wiping EEPROM memories!")); }
		
		for (int i = 0; i < 512; i++) // Loop repeats equal to the number of array in EEPROM
		{
			EEPROM.write(i, 0);
		}
		
		} else {
		if ( DEBUG ) { Serial.println(F("No EEPROM memories to wipe!")); }
	}
	
	return r;
}



#else
#error This code is only for use on Arduino.
#endif // ARDUINO

#endif // __MYSETTINGS_H__
