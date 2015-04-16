/*
* @file CoreConduitRF.h
* Copyright (C) 2014 Cory J. Potter - All Rights Reserved
* You may use, distribute and modify this code under the
* terms of the LICENSE.txt
* NOT INTENDED FOR COMMERCIAL USE!
* You should have received a copy of the LICENSE.txt with
* this file. If not, please write to: <bitsandbots@gmail.com>
*/

#ifndef __CORECONDUITRF_H__
#define __CORECONDUITRF_H__

#ifdef ARDUINO

#include <CoreConduit.h>

RF24 radio(PIN9_RF_CE, PIN10_CS);   // make sure this corresponds to the pins you are using
// NOTE: the "LL" at the end of the constant is "LongLong" type
const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };
//const uint64_t pipes[4] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL, 0xE8E8F0F0E1LL, 0xE8E8F0F0D2LL };

// Defined in CoreConduit
// DataObject rxDataObject = {};
// DataObject txDataObject = {};

bool receiveRFdataObject( DataObject *rxDataObject );
void transmitRFdataObject( DataObject *txDataObject );

/************************************************************************/
/*  nRF Network Functions                                               */
/************************************************************************/
bool tx_nRF_SensorData( Sensor *sensor )
{
	bool r = false;

	radio.stopListening();
	for (; sensor != NULL; sensor = sensor->next )
	{
		// we need data to sent...
		setDataObject( &txDataObject, sensor );

		// radio stuff
		radio.write( &txDataObject, sizeof(txDataObject) );
		// serial print received data
		printDataObject( &txDataObject );
		// end of serial printing
		delay(50);
	}
	// end of radio stuff
	radio.startListening();
	return r;
}

bool tx_nRF_ApplianceData( Appliance *app )
{
	bool r = false;

	radio.stopListening();
	for (; app != NULL; app = app->next )
	{
		// we need data to sent...
		setDataObject( &txDataObject, app );

		// radio stuff
		radio.write( &txDataObject, sizeof(txDataObject) );
		// end of radio stuff
		
		// serial print received data
		if ( DEBUG ) { printDataObject( &txDataObject ); }
		// end of serial printing
		delay(50);
	}
	radio.startListening();
	return r;
}

bool tx_nRF_Request( void )
{
	bool r = false;
	xTime = now();

	radio.stopListening();
	radio.write( &xTime, sizeof(xTime) );
	radio.startListening();
	delay(50);
	r = tx_nRF_ApplianceData( &Appliance_FeedPump );
	// end of radio stuff
	return r;
}


bool receiveRFdataObject( DataObject *rxDataObject )
{
	bool r = false;
	// radio stuff
	if ( radio.available() )
	{
		radio.read( rxDataObject, sizeof(rxDataObject) );
		r = true;
		// serial print received data
		if (DEBUG) { 
			Serial.print(F("rx:  "));
			printDataObject( rxDataObject );
		}
	}
	
	return r;
}

void transmitRFdataObject( DataObject *txDataObject )
{
	// radio stuff
	radio.stopListening(); radio.write( txDataObject, sizeof(txDataObject) ); radio.startListening();
	// serial print transmitted data
	if (DEBUG) { 
		Serial.print(F("tx:  "));
		printDataObject( txDataObject );
		// end of serial printing
	}
}

/************************************************************************/


#else
#error This code is only for use on Arduino.
#endif // ARDUINO
#endif // __CORECONDUITRF_H__
