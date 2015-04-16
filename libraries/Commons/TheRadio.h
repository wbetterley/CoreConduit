/*
Copyright (C) 2014 Cory J. Potter <cor75@hotmail.com>

*/

/**
* @file TheRadio.h
*	good communication is important.
*/

#ifndef __THERADIO_H__
#define __THERADIO_H__

#ifdef ARDUINO
EasyTransferVirtualWire ET;

bool txAllDataObjects ( void )
{
	bool r = false;
	Sensor *sensor = &Sensor_FeedFloat;
	Appliance *app = &Appliance_FeedPump;
	Alert *alert = &Alert_System;

	if ( DEBUG ) { Serial.print(F(" Sending Data ")); }

	for (; sensor != NULL; sensor = sensor->next )
	{
		radioDataObject = setDataObject( sensor );
		//send the data
		ET.sendData();
		delay(1000);
		printDataObject( radioDataObject );
	} // end for

	for (; app != NULL; app = app->next )
	{
		radioDataObject = setDataObject( app );
		//send the data
		ET.sendData();
		delay(1000);
		printDataObject( radioDataObject );
	} // end for

	for (; alert != NULL; alert = alert->next )
	{
		radioDataObject = setDataObject( alert );
		//send the data
		ET.sendData();
		delay(1000);
		printDataObject( radioDataObject );
	} // end for
	return r;
}


// Read an ID from DataObject and save it to the struct
bool rxAllDataObjects( void ) // Number = position in DataObject to get the 8 Bytes from
{
	bool r = false;
	Sensor *sensor = &Sensor_FeedFloat;
	Appliance *app = &Appliance_FeedPump;
	Alert *alert = &Alert_System;

	for (; sensor != NULL; sensor = sensor->next )
	{
		if( ET.receiveData() )
		{
			r = true;
			SyncSensor( &radioDataObject );
			printDataObject( radioDataObject );
		}
			delay(1000);
	} // end for

	for (; app != NULL; app = app->next )
	{
		if( ET.receiveData() )
		{
			r = true;
			SyncAppliance( &radioDataObject );
			printDataObject( radioDataObject );
		}
			delay(1000);
	} // end for


	for (; alert != NULL; alert = alert->next )
	{
		if( ET.receiveData() )
		{
			r = true;
			SyncAlert( &radioDataObject );
			printDataObject( radioDataObject );
		}
			delay(1000);
	} // end for
	return r;
}


#else
#error This code is only for use on Arduino.
#endif // ARDUINO

#endif // __THERADIO_H__
