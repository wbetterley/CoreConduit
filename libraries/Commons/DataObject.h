/*
* @file DataObject.h
* Copyright (C) 2014 Cory J. Potter - All Rights Reserved
* You may use, distribute and modify this code under the
* terms of the LICENSE.txt
* NOT INTENDED FOR COMMERCIAL USE!
* You should have received a copy of the LICENSE.txt with
* this file. If not, please write to: <bitsandbots@gmail.com>
*/

/**
* @file DataObject.h
*	generic container
*/

#ifndef __DATAOBJECT_H__
#define __DATAOBJECT_H__

#ifdef ARDUINO

typedef struct DataObject
{
	char object;
	unsigned long timestamp;
	byte type;
	uint8_t freq;
	float value;
	bool ready;
	bool triggered;
	bool state;
} DataObject;

// DataObject rxDataObject;
// DataObject txDataObject;

/************************************************************************/
void printDataObject( DataObject *dataObject )
{
	if ( DEBUG )
	{
		Serial.print(F(" type: "));
		Serial.print( (byte) dataObject->type );
		Serial.print(F(" timestamp: "));
		Serial.print( (unsigned long) dataObject->timestamp );
		Serial.print(F(" value: "));
		Serial.print( (float) dataObject->value );
		Serial.print(F(" triggered: "));
		Serial.print( (bool) dataObject->triggered );
		Serial.print(F(" state: "));
		Serial.print( (bool) dataObject->state );
		Serial.print(F(" ready: "));
		Serial.print( (bool) dataObject->ready );
		Serial.println();
	}
}




#else
#error This code is only for use on Arduino.
#endif // ARDUINO

#endif // __DATAOBJECT_H__