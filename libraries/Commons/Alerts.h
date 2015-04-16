/*
* @file Alerts.h
* Copyright (C) 2014 Cory J. Potter - All Rights Reserved
* You may use, distribute and modify this code under the
* terms of the LICENSE.txt
* NOT INTENDED FOR COMMERCIAL USE!
* You should have received a copy of the LICENSE.txt with
* this file. If not, please write to: <bitsandbots@gmail.com>
*/

#ifndef __ALERTS_H__
#define __ALERTS_H__

#ifdef ARDUINO

enum ALERT_TYPE {

	ALERT_TEMP, /* Temperature out-of-range */
	ALERT_FANS, /* Temperature doesn't drop after the fans are activated. */
	ALERT_HUMID, /* Humidity out-of-range */
	ALERT_LIGHT, /* Luminescence too low OR oN/oFF not as scheduled */
	ALERT_FLOAT, /* Water level too low */
	ALERT_MOISTURE, /* Soil Moisture remaining - too soggy, too dry */
	/* ALERT_CLEAN,   Time to clean */
	ALERT_CHANGE, /*  Time to add/remove nutrient solution */
	/* ALERT_TREAT,  Time for prevention treatment */
	/* ALERT_PESTS,   Pest Checker:  Consider temp, humidity, lighting schedule */
	ALERT_SYSTEM
};

typedef struct Alert
{
	ALERT_TYPE type;		// The type of the alert (ex: ALERT_TEMP)
	uint8_t freq;		// How frequently should the alert be sent?
	bool state;		// Alert is ON / OFF
	bool triggered;  // flag is true if Alert has been activated
	unsigned long timestamp;
	struct Alert *next;
} Alert;

// 		typedef struct DataObject
// 		{
// 			char object;
// 			unsigned long timestamp;
// 			byte type;
// 			uint8_t freq;
// 			float value;
// 			bool ready;
// 			bool triggered;
// 			bool state;
// 		} DataObject;


Alert rxAlertObject		  = { /*empty*/ };
uint8_t totalAlerts = 8;

const char* alert_message[] = { 
	"Too Cold! Require Heater",
	"Too Hot! Need Better Ventilation",
	"Humidity Out-of-Range",
	"Luminescence Fault",
	"Water Level Low",
	"Soil Moisture Low",
	"Time to Adjust Nutrient Solution",
	"Something is Wrong!" };

Alert Alert_Temp     = { ALERT_TEMP, 0, true, false, DEFAULT_TIME, NULL };
Alert Alert_Fans     = { ALERT_FANS, 0, true, false, DEFAULT_TIME, &Alert_Temp };
Alert Alert_Humid    = { ALERT_HUMID, 0, true, false, DEFAULT_TIME, &Alert_Fans };
Alert Alert_Light    = { ALERT_LIGHT, 0, true, false, DEFAULT_TIME, &Alert_Humid };
Alert Alert_Float    = { ALERT_FLOAT, 0, true, false,  DEFAULT_TIME, &Alert_Light };
Alert Alert_Moist    = { ALERT_MOISTURE, 1, true, false,  DEFAULT_TIME, &Alert_Float };
Alert Alert_Change   = { ALERT_CHANGE, 7, true, false, DEFAULT_TIME, &Alert_Moist };
Alert Alert_System  = { ALERT_SYSTEM, 0, false, false, DEFAULT_TIME, &Alert_Change };
/************************************************************************/
// 			(char) object; (unsigned long) timestamp; (byte) type; (uint8_t) freq; (float) value; (bool) ready; (bool) triggered; (bool) state;

// DataObject Alert_Temp     = { 'n', DEFAULT_TIME, ALERT_TEMP, 0, 0, true, false, false, NULL };
// DataObject Alert_Fans     = { 'n', DEFAULT_TIME, ALERT_FANS, 0, 0, true, false, false, &Alert_Temp };
// DataObject Alert_Humid    = { 'n', DEFAULT_TIME, ALERT_HUMID, 0, 0, true, false, false, &Alert_Fans };
// DataObject Alert_Light    = { 'n', DEFAULT_TIME, ALERT_LIGHT, 0, 0, true, false, false, &Alert_Humid };
// DataObject Alert_Float    = { 'n', DEFAULT_TIME, ALERT_FLOAT, 0, 0, true, false,  false, &Alert_Light };
// DataObject Alert_Moist    = { 'n', DEFAULT_TIME, ALERT_MOISTURE, 1, 0, true, false,  false, &Alert_Float };
// DataObject Alert_Change   = { 'n', DEFAULT_TIME, ALERT_CHANGE, 7, 0, true, false, false, &Alert_Moist };
// DataObject Alert_System  = { 'n', DEFAULT_TIME, ALERT_SYSTEM, 0, 0, false, false, false, &Alert_Change };
/************************************************************************/

bool SyncAlert(Alert *alert);
bool SyncAlert(DataObject *alert);

void setDataObject(DataObject *dataObject, Alert *alert)
{
	// 	DataObject dataObject;
	dataObject->object = 'n';
	dataObject->timestamp = (unsigned long) alert->timestamp;
	dataObject->type = (byte) alert->type;
	dataObject->freq = (uint8_t) alert->freq;
	dataObject->value = 0;
	dataObject->ready = false;
	dataObject->triggered = (bool) alert->triggered;
	dataObject->state = (bool) alert->state;
}

bool txAlertData( Alert *alert, const byte SEND_TO_ADDRESS )
{
	bool r = false;

	for (; alert != NULL; alert = alert->next )
	{
		setDataObject( &txDataObject, alert );
		Wire.beginTransmission (SEND_TO_ADDRESS);
		//Wire.write ((byte *) &txDataObject, sizeof(&txDataObject));
		I2C_writeAnything( txDataObject );
		uint8_t result = Wire.endTransmission ();
		if (result == 0 ) { r = true; } else { errorHandler( result ); break; if ( DEBUG ) { Serial.println(F(" on txAlert")); } }
		delay(100);
	}
	return r;
}

bool SyncAlert(Alert *alert) {
	bool r = false;
	//	for (; alert != NULL; alert = alert->next ) {
	
	switch (alert->type)
	{
		case ALERT_TEMP: /* Temperature out-of-range */
		//  Alert_Temp.message = alert->message;
		if ( alert->freq ) { Alert_Temp.freq = alert->freq; }
		Alert_Temp.state = alert->state;
		if ( ( alert->timestamp > DEFAULT_TIME ) && ( Alert_Temp.timestamp < alert->timestamp ) )
		{ Alert_Temp.triggered = alert->triggered; }
		Alert_Temp.timestamp = alert->timestamp;
		r = true;
		break;
		case ALERT_FANS: /* Temperature doesn't drop after the fans are activated. */
		//  Alert_Fans.message = alert->message;
		if ( alert->freq ) { Alert_Fans.freq = alert->freq; }
		Alert_Fans.state = alert->state;
		Alert_Fans.triggered = alert->triggered;
		if ( ( alert->timestamp > DEFAULT_TIME ) && ( Alert_Fans.timestamp < alert->timestamp ) )
		{ Alert_Fans.timestamp = alert->timestamp; }
		r = true;
		break;
		case ALERT_HUMID: /* Humidity out-of-range */
		//  Alert_Humid.message = alert->message;
		if ( alert->freq ) { Alert_Humid.freq = alert->freq; }
		Alert_Humid.state = alert->state;
		Alert_Humid.triggered = alert->triggered;
		if ( ( alert->timestamp > DEFAULT_TIME ) && ( Alert_Humid.timestamp < alert->timestamp ) )
		{ Alert_Humid.timestamp = alert->timestamp; }
		r = true;
		break;
		case ALERT_LIGHT: /* Luminescence too low OR oN/oFF not as scheduled */
		//  Alert_Light.message = alert->message;
		if ( alert->freq ) { Alert_Light.freq = alert->freq; }
		Alert_Light.state = alert->state;
		Alert_Light.triggered = alert->triggered;
		if ( ( alert->timestamp > DEFAULT_TIME ) && ( Alert_Light.timestamp < alert->timestamp ) )
		{ Alert_Light.timestamp = alert->timestamp; }
		r = true;
		break;
		case ALERT_FLOAT: /* Water level too low */
		//  Alert_Float.message = alert->message;
		if ( alert->freq ) { Alert_Float.freq = alert->freq; }
		Alert_Float.state = alert->state;
		Alert_Float.triggered = alert->triggered;
		if ( ( alert->timestamp > DEFAULT_TIME ) && ( Alert_Float.timestamp < alert->timestamp ) )
		{ Alert_Float.timestamp = alert->timestamp; }
		r = true;
		break;
		case ALERT_MOISTURE: /* Soil Moisture remaining - too soggy, too dry */
		//  Alert_Moist.message = alert->message;
		if ( alert->freq ) { Alert_Moist.freq = alert->freq; }
		Alert_Moist.state = alert->state;
		Alert_Moist.triggered = alert->triggered;
		if ( ( alert->timestamp > DEFAULT_TIME ) && ( Alert_Moist.timestamp < alert->timestamp ) )
		{ Alert_Moist.timestamp = alert->timestamp; }
		r = true;
		break;
		// 		case ALERT_CLEAN: /*  Time to clean */
		// 		//  Alert_Clean.message = alert->message;
		// 		if ( alert->freq ) { Alert_Clean.freq = alert->freq; }
		// 		Alert_Clean.state = alert->state;
		// 		Alert_Clean.triggered = alert->triggered;
		// 		if ( ( alert->timestamp > DEFAULT_TIME ) && ( Alert_Clean.timestamp < alert->timestamp ) )
		// 		{ Alert_Clean.timestamp = alert->timestamp; }
		// 		r = true;
		// 		break;
		case ALERT_CHANGE: /*  Time to add/remove nutrient solution */
		//  Alert_Change.message = alert->message;
		if ( alert->freq ) { Alert_Change.freq = alert->freq; }
		Alert_Change.state = alert->state;
		Alert_Change.triggered = alert->triggered;
		if ( ( alert->timestamp > DEFAULT_TIME ) && ( Alert_Change.timestamp < alert->timestamp ) )
		{ Alert_Change.timestamp = alert->timestamp; }
		r = true;
		break;
		// 		case ALERT_PESTS: /*  Pest Checker:  Consider temp, humidity, lighting schedule */
		// 		//  Alert_Pests.message = alert->message;
		// 		if ( alert->freq ) { Alert_Pests.freq = alert->freq; }
		// 		Alert_Pests.state = alert->state;
		// 		Alert_Pests.triggered = alert->triggered;
		// 		if ( ( alert->timestamp > DEFAULT_TIME ) && ( Alert_Pests.timestamp < alert->timestamp ) )
		// 		{ Alert_Pests.timestamp = alert->timestamp; }
		// 		r = true;
		// 		break;
		case ALERT_SYSTEM:
		//  Alert_Unknown.message = alert->message;
		if ( alert->freq ) { Alert_System.freq = alert->freq; }
		Alert_System.state = alert->state;
		Alert_System.triggered = alert->triggered;
		if ( ( alert->timestamp > DEFAULT_TIME ) && ( Alert_System.timestamp < alert->timestamp ) )
		{ Alert_System.timestamp = alert->timestamp; }
		r = true;
		break;
	}
	//	} // end for
	return r;
}

void printAlerts( Alert *alert ) {
	if ( DEBUG ) {
		for (; alert != NULL; alert = alert->next ) {
			// Don't bother printing unless alert triggered
			//			if ( alert->triggered ) {
			Serial.print(F("Alert:  "));
			Serial.print(alert->type);
			Serial.print(F(" triggered:  "));
			Serial.print(alert->triggered);
			Serial.print(F(" : "));
			Serial.print(alert->timestamp);
			Serial.print(F(" message:  "));
			Serial.print(alert_message[alert->type]);
			Serial.println("");
			//			}
		}
	}
}

void receiveAlertData ( int haveData )
{
		I2C_readAnything( rxDataObject ); //, sizeof(rxDataObject) );
}


/**
bool SyncAlert(Alert *alert) {
	bool r = false;
	// We start at the very beginning...
	Alert *myAlert = &Alert_System;

	for (; myAlert != NULL; myAlert = myAlert->next ) {

		if ( myAlert.type == alert->type ) {
			if ( alert->freq ) { myAlert.freq = alert->freq; }
			myAlert.state = alert->state;
			if ( ( alert->timestamp > DEFAULT_TIME ) && ( myAlert.timestamp < alert->timestamp ) )
			{ myAlert.triggered = alert->triggered; }
			myAlert.timestamp = alert->timestamp;
			r = true;
			// We found the one we're looking for
			break;
		}
	}
	return r;
}
**/

/************************************************************************/

bool SyncAlert(DataObject *alert) {
	bool r = false;
	//	for (; alert != NULL; alert = alert->next ) {
	
	switch (alert->type)
	{
		case ALERT_TEMP: /* Temperature out-of-range */
		//  Alert_Temp.message = alert->message;
		//  Alert_Temp.freq = alert->freq;
		Alert_Temp.state = alert->state;
		if ( ( alert->timestamp > DEFAULT_TIME ) && ( Alert_Temp.timestamp < alert->timestamp ) )
		{ Alert_Temp.triggered = alert->triggered; }
		Alert_Temp.timestamp = alert->timestamp;
		r = true;
		break;
		case ALERT_FANS: /* Temperature doesn't drop after the fans are activated. */
		//  Alert_Fans.message = alert->message;
		//  Alert_Fans.freq = alert->freq;
		Alert_Fans.state = alert->state;
		Alert_Fans.triggered = alert->triggered;
		if ( ( alert->timestamp > DEFAULT_TIME ) && ( Alert_Fans.timestamp < alert->timestamp ) )
		{ Alert_Fans.timestamp = alert->timestamp; }
		r = true;
		break;
		case ALERT_HUMID: /* Humidity out-of-range */
		//  Alert_Humid.message = alert->message;
		//  Alert_Humid.freq = alert->freq;
		Alert_Humid.state = alert->state;
		Alert_Humid.triggered = alert->triggered;
		if ( ( alert->timestamp > DEFAULT_TIME ) && ( Alert_Humid.timestamp < alert->timestamp ) )
		{ Alert_Humid.timestamp = alert->timestamp; }
		r = true;
		break;
		case ALERT_LIGHT: /* Luminescence too low OR oN/oFF not as scheduled */
		//  Alert_Light.message = alert->message;
		//  Alert_Light.freq = alert->freq;
		Alert_Light.state = alert->state;
		Alert_Light.triggered = alert->triggered;
		if ( ( alert->timestamp > DEFAULT_TIME ) && ( Alert_Light.timestamp < alert->timestamp ) )
		{ Alert_Light.timestamp = alert->timestamp; }
		r = true;
		break;
		case ALERT_FLOAT: /* Water level too low */
		//  Alert_Float.message = alert->message;
		//  Alert_Float.freq = alert->freq
		Alert_Float.state = alert->state;
		Alert_Float.triggered = alert->triggered;
		if ( ( alert->timestamp > DEFAULT_TIME ) && ( Alert_Float.timestamp < alert->timestamp ) )
		{ Alert_Float.timestamp = alert->timestamp; }
		r = true;
		break;
		case ALERT_MOISTURE: /* Soil Moisture remaining - too soggy, too dry */
		//  Alert_Moist.message = alert->message;
		//  Alert_Moist.freq = alert->freq;
		Alert_Moist.state = alert->state;
		Alert_Moist.triggered = alert->triggered;
		if ( ( alert->timestamp > DEFAULT_TIME ) && ( Alert_Moist.timestamp < alert->timestamp ) )
		{ Alert_Moist.timestamp = alert->timestamp; }
		r = true;
		break;
		// 		case ALERT_CLEAN: /*  Time to clean */
		// 		//  Alert_Clean.message = alert->message;
		// 		if ( alert->freq ) { Alert_Clean.freq = alert->freq; }
		// 		Alert_Clean.state = alert->state;
		// 		Alert_Clean.triggered = alert->triggered;
		// 		if ( ( alert->timestamp > DEFAULT_TIME ) && ( Alert_Clean.timestamp < alert->timestamp ) )
		// 		{ Alert_Clean.timestamp = alert->timestamp; }
		// 		r = true;
		// 		break;
		case ALERT_CHANGE: /*  Time to add/remove nutrient solution */
		//  Alert_Change.message = alert->message;
		//  Alert_Change.freq = alert->freq;
		Alert_Change.state = alert->state;
		Alert_Change.triggered = alert->triggered;
		if ( ( alert->timestamp > DEFAULT_TIME ) && ( Alert_Change.timestamp < alert->timestamp ) )
		{ Alert_Change.timestamp = alert->timestamp; }
		r = true;
		break;
		// 		case ALERT_PESTS: /*  Pest Checker:  Consider temp, humidity, lighting schedule */
		// 		//  Alert_Pests.message = alert->message;
		// 		if ( alert->freq ) { Alert_Pests.freq = alert->freq; }
		// 		Alert_Pests.state = alert->state;
		// 		Alert_Pests.triggered = alert->triggered;
		// 		if ( ( alert->timestamp > DEFAULT_TIME ) && ( Alert_Pests.timestamp < alert->timestamp ) )
		// 		{ Alert_Pests.timestamp = alert->timestamp; }
		// 		r = true;
		// 		break;
		case ALERT_SYSTEM:
		//  Alert_Unknown.message = alert->message;
		//  Alert_System.freq = alert->freq;
		Alert_System.state = alert->state;
		Alert_System.triggered = alert->triggered;
		if ( ( alert->timestamp > DEFAULT_TIME ) && ( Alert_System.timestamp < alert->timestamp ) )
		{ Alert_System.timestamp = alert->timestamp; }
		r = true;
		break;
	}
	//	} // end for
	return r;
}


#else
#error This code is only for use on Arduino.
#endif // ARDUINO
#endif // __ALERTS_H__
