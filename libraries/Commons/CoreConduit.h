/*
* @file CoreConduitRF.h
* Copyright (C) 2014 Cory J. Potter - All Rights Reserved
* You may use, distribute and modify this code under the
* terms of the LICENSE.txt
* NOT INTENDED FOR COMMERCIAL USE!
* You should have received a copy of the LICENSE.txt with
* this file. If not, please write to: <bitsandbots@gmail.com>
*/

#ifndef __CORECONDUIT_H__
#define __CORECONDUIT_H__

#ifdef ARDUINO

// Define STaTe MaCHiNe fLaGs
//unsigned long txTimer = 2500;
// uint8_t txCounter = 0;
uint8_t rxCounter = 0;
uint8_t expectedDataCounter = 0;
uint8_t haveData = 0;
uint8_t requestSync = 0;
// bool iAmOnMyOwn = false;
// unsigned long started_waiting_at = 0;

enum SENSOR_TYPE {
	SENSOR_UNKNOWN,
	SENSOR_TEMPF,
	SENSOR_TEMPC,
	SENSOR_HUMIDITY,
	SENSOR_MOISTURE,
	SENSOR_PHOTO,
	SENSOR_FLOAT,
	SENSOR_PRESSURE,
	SENSOR_CURRENT,
	SENSOR_FLOW,
SENSOR_DISTANCE};

enum APPLIANCE_TYPE {
	APPLIANCE_UNKNOWN,
	APPLIANCE_INTAKE_FAN,
	APPLIANCE_EXHAUST_FAN,
	APPLIANCE_CIRCULATION_FAN,
	APPLIANCE_LIGHT,
	APPLIANCE_PUMP,
	APPLIANCE_HUMIDIFIER,
APPLIANCE_HEATER};

typedef struct Sensor
{
	uint8_t pin;		// PIN_A2
	uint8_t powerPin;		// PIN_D7
	SENSOR_TYPE type;		// The type of the sensor (ex: SENSOR_TEMPF)
	uint8_t freq;		// How frequently should the sensor take a reading?
	uint8_t minVal;		// 65.0
	uint8_t maxVal;		// 75.5
	unsigned long timestamp;
	float value;
	struct Sensor *next;
} Sensor;

typedef struct Appliance
{
	uint8_t pin;			// i.e.,PIN_A2
	APPLIANCE_TYPE type;	// The type of the sensor (ex: SENSOR_TEMPF)
	bool ready;
	unsigned long timestamp;
	bool state;
	struct Appliance *next;
} Appliance;

/************************************************************************/
/*  Objects to store generic Sensors and Appliances                     */
/************************************************************************/
Sensor rxSensorObject = { /*empty*/ };
Appliance rxAppObject = { /*empty*/ };
//Alert rxAlertObject		  = { /*empty*/ };
DataObject rxDataObject = {};
DataObject txDataObject = {};

unsigned long xTime = 0;
// Todo:  It would be fancier to automatically total these, yet they don't change:
uint8_t totalSensors = 6;
uint8_t totalAppliances = 6;
//uint8_t totalAlerts = 8;

//Sensor Sensor_Current	= {PIN_A3, PIN_A3, SENSOR_CURRENT, 1, 0, 100, 0, 25, NULL};
Sensor Sensor_Light	    = {PIN_A1, PIN7, SENSOR_PHOTO, 1, 50, 100, 0, 25, NULL};
Sensor Sensor_Humidity	= {PIN6, PIN5, SENSOR_HUMIDITY, 1, 40, 70, 0, 50, &Sensor_Light};
Sensor Sensor_Temp		= {PIN6, PIN5, SENSOR_TEMPF, 1, 70, 75, 0, 75, &Sensor_Humidity};
Sensor Sensor_TankLevel	= {PIN_A0, PIN10_CS, SENSOR_PRESSURE, 1, 50, 50, 0, 75, &Sensor_Temp};
Sensor Sensor_Moisture	= {PIN_A2, PIN7, SENSOR_MOISTURE, 1, 10, 100, 0, 75, &Sensor_TankLevel};
Sensor Sensor_FeedFloat	= {PIN_A3, PIN7, SENSOR_FLOAT, 1, 0, 1, 0, 1, &Sensor_Moisture};
//Sensor Sensor_HumFloat	= {PIN12_MOSI, PIN10_CS, SENSOR_FLOAT,    1, 0, 1, 0, 1, &Sensor_FeedFloat};

// NULL for the first - We cannot point to an object hasn't been created yet.

// Remote Power Switch Transmitter on PIN11_MISO = Data PIN12_MOSI = Power, PIN13_SCK = Ground

// Initialize Appliances
Appliance Appliance_IntakeFan  = {101, APPLIANCE_INTAKE_FAN, true, DEFAULT_TIME, OFF, NULL };
Appliance Appliance_ExhaustFan  = {102, APPLIANCE_EXHAUST_FAN, true, DEFAULT_TIME, OFF, &Appliance_IntakeFan };
// Appliance Appliance_HumFan  = {6, APPLIANCE_FAN, true, 0, ON, &Appliance_FeedPump};
// Appliance Appliance_HumPump = {5, APPLIANCE_PUMP, true, 0, ON, &Appliance_HumFan};
// Appliance Appliance_Humidifier = {4, APPLIANCE_HUMIDIFIER, true, 0, ON, &Appliance_HumPump};
Appliance Appliance_Humidifier = {104, APPLIANCE_HUMIDIFIER, true, DEFAULT_TIME, OFF, &Appliance_ExhaustFan};
Appliance Appliance_Light = {103, APPLIANCE_LIGHT, true, DEFAULT_TIME, OFF, &Appliance_Humidifier};
Appliance Appliance_Heater = {106, APPLIANCE_HEATER, true, DEFAULT_TIME, OFF, &Appliance_Light};
Appliance Appliance_FeedPump = {105, APPLIANCE_PUMP, true, 0, OFF, &Appliance_Heater};

// 			(char) object; (unsigned long) timestamp; (byte) type; (uint8_t) freq; (float) value; (bool) ready; (bool) triggered; (bool) state;
// DataObject Sensor_Light	    = {'s', DEFAULT_TIME, SENSOR_PHOTO, 1, 25, false, false, false };
// DataObject Sensor_Humidity	= {'s', DEFAULT_TIME, SENSOR_HUMIDITY, 1, 50, false, false, false };
// DataObject Sensor_Temp		= {'s', DEFAULT_TIME, SENSOR_TEMPF, 1, 75, false, false, false };
// DataObject Sensor_TankLevel	= {'s', DEFAULT_TIME, SENSOR_PRESSURE, 1, 75, false, false, false };
// DataObject Sensor_Moisture	= {'s', DEFAULT_TIME, SENSOR_MOISTURE, 1, 75, false, false, false };
// DataObject Sensor_FeedFloat	= {'s', DEFAULT_TIME, SENSOR_FLOAT, 1, 1, false, false, false };
// Initialize Appliances
// DataObject Appliance_IntakeFan	= {'a', DEFAULT_TIME, APPLIANCE_INTAKE_FAN	, 0, 0, true, OFF, false };
// DataObject Appliance_ExhaustFan	= {'a', DEFAULT_TIME, APPLIANCE_EXHAUST_FAN	, 0, 0, true, OFF, false };
// DataObject Appliance_Humidifier	= {'a', DEFAULT_TIME, APPLIANCE_HUMIDIFIER	, 0, 0, true, OFF, false};
// DataObject Appliance_Light		= {'a', DEFAULT_TIME, APPLIANCE_LIGHT		, 0, 0, true, OFF, false };
// DataObject Appliance_Heater		= {'a', DEFAULT_TIME, APPLIANCE_HEATER		, 0, 0, true, OFF, false };
// DataObject Appliance_FeedPump	= {'a', DEFAULT_TIME, APPLIANCE_PUMP		, 0, 0, true, OFF,  false };

/************************************************************************/

bool SyncSensor(Sensor *sensor);
bool SyncAppliance(Appliance *app);
void setDataObject(DataObject *dataObject, Sensor *sensor);
void setDataObject(DataObject *dataObject, Appliance *app);
bool SyncSensor(DataObject *sensor);
bool SyncAppliance(DataObject *app);


/************************************************************************/
void setDataObject(DataObject *dataObject, Sensor *sensor)
{
	//	DataObject dataObject;
	dataObject->object = 's';
	dataObject->timestamp = (unsigned long) sensor->timestamp;
	dataObject->type = (byte) sensor->type;
	dataObject->freq = (uint8_t) sensor->freq;
	dataObject->value = (float) sensor->value;
	dataObject->ready = false;
	dataObject->triggered = false;
	dataObject->state = false;
}

void setDataObject(DataObject *dataObject, Appliance *app)
{
	//	DataObject dataObject;
	dataObject->object = 'a';
	dataObject->timestamp = (unsigned long) app->timestamp;
	dataObject->type = (byte) app->type;
	dataObject->freq = 0;
	dataObject->value = 0;
	dataObject->ready = (bool) app->ready;
	dataObject->triggered = false;
	dataObject->state = (bool) app->state;
}

/************************************************************************/

bool SyncSensor(DataObject *sensor)
{
	bool r = false;
	
	// 	if((Sensor_Current.type == sensor->type))
	// 	{
	// 		Sensor_Current.freq = sensor->freq;
	// 		Sensor_Current.timestamp = sensor->timestamp;
	// 		Sensor_Current.value = sensor->value;
	// 		r = true;
	// 	}
	if((Sensor_Light.type == sensor->type))
	{
		Sensor_Light.freq = sensor->freq;
		Sensor_Light.timestamp = sensor->timestamp;
		Sensor_Light.value = sensor->value;
		r = true;
	}
	if((Sensor_Humidity.type == sensor->type))
	{
		Sensor_Humidity.freq = sensor->freq;
		Sensor_Humidity.timestamp = sensor->timestamp;
		if ( sensor->value > 10) { Sensor_Humidity.value = sensor->value; }
		r = true;
	}
	if((Sensor_Temp.type == sensor->type))
	{
		Sensor_Temp.freq = sensor->freq;
		Sensor_Temp.timestamp = sensor->timestamp;
		if ( sensor->value > 10) { Sensor_Temp.value = sensor->value; }
		r = true;
	}
	if((Sensor_Moisture.type == sensor->type))
	{
		Sensor_Moisture.freq = sensor->freq;
		Sensor_Moisture.timestamp = sensor->timestamp;
		Sensor_Moisture.value = sensor->value;
		r = true;
	}
	if((Sensor_FeedFloat.type == sensor->type))
	{
		Sensor_FeedFloat.freq = sensor->freq;
		Sensor_FeedFloat.timestamp = sensor->timestamp;
		Sensor_FeedFloat.value = sensor->value;
		r = true;
	}
	if((Sensor_TankLevel.type == sensor->type))
	{
		Sensor_TankLevel.freq = sensor->freq;
		Sensor_TankLevel.timestamp = sensor->timestamp;
		Sensor_TankLevel.value = sensor->value;
		r = true;
	}
	return r;
}

bool SyncAppliance(DataObject *app)
{
	bool r = false;

	// catch some sync errors
	if ( app->timestamp < DEFAULT_TIME ) {
		if (DEBUG) { Serial.print(app->type); Serial.print(F(" not app time sync: ")); Serial.println(app->timestamp); }
	return r; }

	if((Appliance_IntakeFan.type == app->type))
	{
		Appliance_IntakeFan.ready = app->ready;
		Appliance_IntakeFan.state = app->state;
		Appliance_IntakeFan.timestamp = app->timestamp;
		r = true;
	}
	if((Appliance_ExhaustFan.type == app->type))
	{
		Appliance_ExhaustFan.ready = app->ready;
		Appliance_ExhaustFan.state = app->state;
		Appliance_ExhaustFan.timestamp = app->timestamp;
		r = true;
	}
	if((Appliance_Humidifier.type == app->type))
	{
		Appliance_Humidifier.ready = app->ready;
		Appliance_Humidifier.state = app->state;
		Appliance_Humidifier.timestamp = app->timestamp;
		r = true;
	}

	if((Appliance_Light.type == app->type))
	{
		Appliance_Light.ready = app->ready;
		Appliance_Light.state = app->state;
		Appliance_Light.timestamp = app->timestamp;
		r = true;
	}

	if((Appliance_Heater.type == app->type))
	{
		Appliance_Heater.ready = app->ready;
		Appliance_Heater.state = app->state;
		Appliance_Heater.timestamp = app->timestamp;
		r = true;
	}

	if((Appliance_FeedPump.type == app->type))
	{
		Appliance_FeedPump.ready = app->ready;
		Appliance_FeedPump.state = app->state;
		Appliance_FeedPump.timestamp = app->timestamp;
		r = true;
	}

	// else return false
	return r;
}


/************************************************************************/

// void printSensor(Sensor *sensor) {
// 	if ( DEBUG ) {
// 		Serial.print("Sensor.Pin = ");
// 		Serial.print(sensor->pin);
// 		// 	Serial.print(", type = ");
// 		// 	Serial.print(sensor->type);
// 		// 	Serial.print(", freq = ");
// 		// 	Serial.print(sensor->freq);
// 		// 	Serial.print(", minVal = ");
// 		// 	Serial.print(sensor->minVal);
// 		// 	Serial.print(", maxVal = ");
// 		// 	Serial.print(sensor->maxVal);
// 		Serial.print(", timestamp = ");
// 		Serial.print(sensor->timestamp);
// 		Serial.print(", value = ");
// 		Serial.println(sensor->value);
// 	}
// }
//
//
void printAppliance(Appliance *app ) {
	if ( DEBUG ) {
		Serial.print(F("Appliance.Pin = "));
		Serial.print((uint8_t) app->pin);
		Serial.print(F(", type = "));
		Serial.print((uint8_t) app->type);
		Serial.print(F(", ready = "));
		Serial.print((bool) app->ready);
		Serial.print(F(", timestamp = "));
		Serial.print((unsigned long) app->timestamp);
		Serial.print(F(", state = "));
		Serial.println((bool) app->state);
	}
}

/************************************************************************/
/*  I2C Network Functions                                               */
/************************************************************************/
bool txSensorData( Sensor *sensor, const byte SEND_TO_ADDRESS )
{
	bool r = false;

	for (; sensor != NULL; sensor = sensor->next )
	{
		Wire.beginTransmission (SEND_TO_ADDRESS);
		Wire.write ((byte *) sensor, sizeof(*sensor));
		uint8_t result = Wire.endTransmission ();
		if (result == 0 ) { r = true; } else { errorHandler( result ); break; if ( DEBUG ) { Serial.println(F(" on txSensor")); } }
		delay(100);
	}
	return r;
}

bool txApplianceData( Appliance *app, const byte SEND_TO_ADDRESS )
{
	bool r = false;

	for (; app != NULL; app = app->next )
	{
		Wire.beginTransmission (SEND_TO_ADDRESS);
		Wire.write ((byte *) app, sizeof(*app));
		uint8_t result = Wire.endTransmission ();
		if (result == 0 ) { r = true; } else { errorHandler( result ); break; if ( DEBUG ) { Serial.println(F(" on txAppliance")); } }
		delay(100);
	}
	return r;
}

// bool txAlertData( Alert *alert, const byte SEND_TO_ADDRESS )
// {
// 	bool r = false;
// 
// 	for (; alert != NULL; alert = alert->next )
// 	{
// 		Wire.beginTransmission (SEND_TO_ADDRESS);
// 		Wire.write ((byte *) alert, sizeof(*alert));
// 		uint8_t result = Wire.endTransmission ();
// 		if (result == 0 ) { r = true; } else { errorHandler( result ); break; if ( DEBUG ) { Serial.println(F(" on txAlert")); } }
// 		delay(100);
// 	}
// 	return r;
// }

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent (int howMany) { if ( howMany > 0 ) { haveData = howMany; } }  // end of receiveEvent

void receiveData ( int haveData )
{
	switch ( haveData )
	{
		default:
		if ( DEBUG ) {
			Serial.print(F("rx: ")); Serial.print( haveData ); Serial.println(F(" bytes."));
		}
		break;
		
		case 4:
		xTime = 0;
		I2C_readAnything (xTime);
		if ( DEBUG ) {
			Serial.print(F("xTime:  ")); Serial.println( xTime );
		}
		break;

		case 11:
		xTime = now();
		// assume we rxAppObject
		I2C_readAnything( rxAppObject );

		// 		if ( DEBUG ) { Serial.print(F("SynceApp: ")); Serial.print( rxAppObject.type );
		// 		Serial.print("  A:  "); Serial.println(rxAppObject.state); }

		if( ( SyncAppliance( &rxAppObject ) ) == false ) { if ( DEBUG ) { Serial.print(F("fail SyncApp: ")); Serial.println( rxAppObject.type ); 
			Serial.print(F("A:  ")); Serial.println(rxAppObject.type); } }
		break;

		case 13:
		xTime = now();
		// assume rxAlertObject
// 		I2C_readAnything ( rxAlertObject );
		// 		if ( DEBUG ) { Serial.print(F("SyncAlert: ")); Serial.println( rxAlertObject.type );
		// 		Serial.print("triggered? "); Serial.println(rxAlertObject.triggered); }

// 		if( ( SyncAlert( &rxDataObject ) ) == false ) { if ( DEBUG ) { Serial.print(F("fail SyncAlert: ")); Serial.println( rxAlertObject.type );
// 			Serial.print(F("type ")); Serial.println(rxAlertObject.type); } }
		break;

		case 17:
		xTime = now();
		// assume rxSensorObject
		I2C_readAnything ( rxSensorObject );
		// 		if ( DEBUG ) { Serial.print(F("SyncSensor: ")); Serial.println( rxSensorObject.type );
		// 		Serial.print("S: "); Serial.println(rxSensorObject.value); }

		if( ( SyncSensor( &rxSensorObject ) ) == false ) { if ( DEBUG ) { Serial.print(F("fail SyncSensor: ")); Serial.println( rxSensorObject.type );
		Serial.print("S: "); Serial.println(rxSensorObject.type); } }
		break;
	}
}

bool txRequest(uint8_t selectOption, const byte SEND_TO_ADDRESS )
{
	bool r = false;
	xTime = now();
	Wire.beginTransmission (SEND_TO_ADDRESS);
	I2C_writeAnything( xTime );
	uint8_t result = Wire.endTransmission ();
	if (result == 0 ) { r = true; } else { errorHandler( result ); if ( DEBUG ) { Serial.println(F(" xTime")); } }
	delay(100);
	switch ( selectOption )
	{
		case 1: r = txApplianceData( &Appliance_FeedPump, SEND_TO_ADDRESS );
		break;
		//case 2: r = txAlertData( &Alert_System, SEND_TO_ADDRESS );
		//break;
	}
	return r;
}

void print2digits(int number) {
	if (DEBUG)
	{
		if (number >= 0 && number < 10) {
			Serial.print('0');
		}
		Serial.print(number);
	}
}

void printTime ( void )
{
	if (DEBUG)
	{
		print2digits( hourFormat12() );
		Serial.print(':');
		print2digits( minute() );
		Serial.print(':');
		print2digits( second() );
		Serial.print(", ");
		Serial.print( day() );
		Serial.print('/');
		Serial.print( month() );
		Serial.print('/');
		Serial.print( year() );
		Serial.print("  ");
	}
}

/************************************************************************/
/* keepAlive - Debugging tool                                           */
/************************************************************************/
void keepAlive(void)
{
//	if ( clockFlag == false ) { clockFlag = setClock(); }
	if ( DEBUG ) {
		printTime();
		Serial.print(F("  free RAM:  ")); Serial.println( freeRam() );
	}
}



/************************************************************************/


bool SyncSensor(Sensor *sensor)
{
	bool r = false;

	// catch some sync errors
	if( ( sensor->timestamp > DEFAULT_TIME ) || ( sensor->timestamp < 1000L ) ){ return r; }
	if( ( sensor->type < 0 ) || ( sensor->type > 20 ) ){ return r; }
	if( ( sensor->freq < 0 ) || ( sensor->freq > 30 ) ){ return r; }
	if( ( sensor->minVal < 0 ) || ( sensor->minVal > 100 ) ){ return r; }
	if( ( sensor->maxVal < 0 ) || ( sensor->maxVal > 100 ) ){ return r; }
	if( ( sensor->value  < 0 ) || ( sensor->value  > 100 ) ){ return r; }
	

	// 	if((Sensor_Current.pin == sensor->pin) && (Sensor_Current.type == sensor->type))
	// 	{
	// 		Sensor_Current.freq = sensor->freq;
	// 		Sensor_Current.timestamp = sensor->timestamp;
	// 		Sensor_Current.minVal = sensor->minVal;
	// 		Sensor_Current.maxVal = sensor->maxVal;
	// 		Sensor_Current.value = sensor->value;
	// 		r = true;
	// 	}
	if((Sensor_Light.type == sensor->type))
	{
		Sensor_Light.freq = sensor->freq;
		Sensor_Light.timestamp = sensor->timestamp;
		Sensor_Light.minVal = sensor->minVal;
		Sensor_Light.maxVal = sensor->maxVal;
		Sensor_Light.value = sensor->value;
		r = true;
	}
	if((Sensor_Humidity.type == sensor->type))
	{
		Sensor_Humidity.freq = sensor->freq;
		Sensor_Humidity.timestamp = sensor->timestamp;
		Sensor_Humidity.minVal = sensor->minVal;
		Sensor_Humidity.maxVal = sensor->maxVal;
		if ( sensor->value > 10) { Sensor_Humidity.value = sensor->value; }
		r = true;
	}
	if((Sensor_Temp.type == sensor->type))
	{
		Sensor_Temp.freq = sensor->freq;
		Sensor_Temp.timestamp = sensor->timestamp;
		Sensor_Temp.minVal = sensor->minVal;
		Sensor_Temp.maxVal = sensor->maxVal;
		if ( sensor->value > 10) { Sensor_Temp.value = sensor->value; }
		r = true;
	}
	if((Sensor_Moisture.type == sensor->type))
	{
		Sensor_Moisture.freq = sensor->freq;
		Sensor_Moisture.timestamp = sensor->timestamp;
		Sensor_Moisture.minVal = sensor->minVal;
		Sensor_Moisture.maxVal = sensor->maxVal;
		Sensor_Moisture.value = sensor->value;
		r = true;
	}
	if((Sensor_FeedFloat.type == sensor->type))
	{
		Sensor_FeedFloat.freq = sensor->freq;
		Sensor_FeedFloat.timestamp = sensor->timestamp;
		Sensor_FeedFloat.minVal = sensor->minVal;
		Sensor_FeedFloat.maxVal = sensor->maxVal;
		Sensor_FeedFloat.value = sensor->value;
		r = true;
	}
	if((Sensor_TankLevel.type == sensor->type))
	{
		Sensor_TankLevel.freq = sensor->freq;
		Sensor_TankLevel.timestamp = sensor->timestamp;
		Sensor_TankLevel.minVal = sensor->minVal;
		Sensor_TankLevel.maxVal = sensor->maxVal;
		Sensor_TankLevel.value = sensor->value;
		r = true;
	}
	return r;
}

bool SyncAppliance(Appliance *app)
{
	bool r = false;
	//	Appliance *a = app;
	// Cannot update appliances that have been flagged?

	// catch some sync errors
	if ( app->timestamp > DEFAULT_TIME ) {
	if (DEBUG) { Serial.print(app->type); Serial.print(F(" app time sync: ")); Serial.println(app->timestamp); }
	return r; }
	if( ( app->type < 0 ) || ( app->type < 20 ) ) { return r; }
	if( ( app->ready < 0 ) || ( app->ready > 1 ) ) { return r; }
	if( ( app->state < 0 ) || ( app->state > 1 ) ) { return r; }

	if((Appliance_IntakeFan.type == app->type))
	{
		Appliance_IntakeFan.ready = app->ready;
		Appliance_IntakeFan.state = app->state;
		Appliance_IntakeFan.timestamp = app->timestamp;
		r = true;
	}
	if((Appliance_ExhaustFan.type == app->type))
	{
		Appliance_ExhaustFan.ready = app->ready;
		Appliance_ExhaustFan.state = app->state;
		Appliance_ExhaustFan.timestamp = app->timestamp;
		r = true;
	}
	if((Appliance_Humidifier.type == app->type))
	{
		Appliance_Humidifier.ready = app->ready;
		Appliance_Humidifier.state = app->state;
		Appliance_Humidifier.timestamp = app->timestamp;
		r = true;
	}

	if((Appliance_Light.type == app->type))
	{
		Appliance_Light.ready = app->ready;
		Appliance_Light.state = app->state;
		Appliance_Light.timestamp = app->timestamp;
		r = true;
	}

	if((Appliance_Heater.type == app->type))
	{
		Appliance_Heater.ready = app->ready;
		Appliance_Heater.state = app->state;
		Appliance_Heater.timestamp = app->timestamp;
		r = true;
	}

	if((Appliance_FeedPump.type == app->type))
	{
		Appliance_FeedPump.ready = app->ready;
		Appliance_FeedPump.state = app->state;
		Appliance_FeedPump.timestamp = app->timestamp;
		r = true;
	}

	// else return false
	return r;
}

/************************************************************************/
/* 

bool SyncSensor( Sensor *sensor )
{
	bool r = false;
	// We start at the very beginning...
	Sensor *mySensor = &Sensor_FeedFloat;
	
	for ( ; mySensor != NULL; mySensor = mySensor->next )
	{
		if ( mySensor.type == sensor->type ) {
			mySensor.freq = sensor->freq;
			mySensor.timestamp = sensor->timestamp;
			mySensor.minVal = sensor->minVal;
			mySensor.maxVal = sensor->maxVal;
			mySensor.value = sensor->value;
			r = true;
			// We found the one we're looking for
			break;
		}
	}
	return r;
}

bool SyncAppliance( Appliance *app )
{
	bool r = false;
	// We start at the very beginning...
	Appliance *myApp = &Appliance_FeedPump;
	
	for ( ; myApp != NULL; myApp = myApp->next )
	{
		if ( myApp.type == app->type ) {
			myApp.timestamp = app->timestamp;
			myApp.ready = app->ready;
			myApp.state = app->state;
			r = true;
			// We found the one we're looking for
			break;
		}
	}
	return r;
}

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

                                                                     */
/************************************************************************/

// bool SyncAlert(Alert *alert) {
// 	bool r = false;
// 	//	for (; alert != NULL; alert = alert->next ) {
// 	
// 	switch (alert->type)
// 	{
// 		case ALERT_TEMP: /* Temperature out-of-range */
// 		//  Alert_Temp.message = alert->message;
// 		if ( alert->freq ) { Alert_Temp.freq = alert->freq; }
// 		Alert_Temp.state = alert->state;
// 		if ( ( alert->timestamp > DEFAULT_TIME ) && ( Alert_Temp.timestamp < alert->timestamp ) )
// 		{ Alert_Temp.triggered = alert->triggered; }
// 		Alert_Temp.timestamp = alert->timestamp;
// 		r = true;
// 		break;
// 		case ALERT_FANS: /* Temperature doesn't drop after the fans are activated. */
// 		//  Alert_Fans.message = alert->message;
// 		if ( alert->freq ) { Alert_Fans.freq = alert->freq; }
// 		Alert_Fans.state = alert->state;
// 		Alert_Fans.triggered = alert->triggered;
// 		if ( ( alert->timestamp > DEFAULT_TIME ) && ( Alert_Fans.timestamp < alert->timestamp ) )
// 		{ Alert_Fans.timestamp = alert->timestamp; }
// 		r = true;
// 		break;
// 		case ALERT_HUMID: /* Humidity out-of-range */
// 		//  Alert_Humid.message = alert->message;
// 		if ( alert->freq ) { Alert_Humid.freq = alert->freq; }
// 		Alert_Humid.state = alert->state;
// 		Alert_Humid.triggered = alert->triggered;
// 		if ( ( alert->timestamp > DEFAULT_TIME ) && ( Alert_Humid.timestamp < alert->timestamp ) )
// 		{ Alert_Humid.timestamp = alert->timestamp; }
// 		r = true;
// 		break;
// 		case ALERT_LIGHT: /* Luminescence too low OR oN/oFF not as scheduled */
// 		//  Alert_Light.message = alert->message;
// 		if ( alert->freq ) { Alert_Light.freq = alert->freq; }
// 		Alert_Light.state = alert->state;
// 		Alert_Light.triggered = alert->triggered;
// 		if ( ( alert->timestamp > DEFAULT_TIME ) && ( Alert_Light.timestamp < alert->timestamp ) )
// 		{ Alert_Light.timestamp = alert->timestamp; }
// 		r = true;
// 		break;
// 		case ALERT_FLOAT: /* Water level too low */
// 		//  Alert_Float.message = alert->message;
// 		if ( alert->freq ) { Alert_Float.freq = alert->freq; }
// 		Alert_Float.state = alert->state;
// 		Alert_Float.triggered = alert->triggered;
// 		if ( ( alert->timestamp > DEFAULT_TIME ) && ( Alert_Float.timestamp < alert->timestamp ) )
// 		{ Alert_Float.timestamp = alert->timestamp; }
// 		r = true;
// 		break;
// 		case ALERT_MOISTURE: /* Soil Moisture remaining - too soggy, too dry */
// 		//  Alert_Moist.message = alert->message;
// 		if ( alert->freq ) { Alert_Moist.freq = alert->freq; }
// 		Alert_Moist.state = alert->state;
// 		Alert_Moist.triggered = alert->triggered;
// 		if ( ( alert->timestamp > DEFAULT_TIME ) && ( Alert_Moist.timestamp < alert->timestamp ) )
// 		{ Alert_Moist.timestamp = alert->timestamp; }
// 		r = true;
// 		break;
// 		// 		case ALERT_CLEAN: /*  Time to clean */
// 		// 		//  Alert_Clean.message = alert->message;
// 		// 		if ( alert->freq ) { Alert_Clean.freq = alert->freq; }
// 		// 		Alert_Clean.state = alert->state;
// 		// 		Alert_Clean.triggered = alert->triggered;
// 		// 		if ( ( alert->timestamp > DEFAULT_TIME ) && ( Alert_Clean.timestamp < alert->timestamp ) )
// 		// 		{ Alert_Clean.timestamp = alert->timestamp; }
// 		// 		r = true;
// 		// 		break;
// 		case ALERT_CHANGE: /*  Time to add/remove nutrient solution */
// 		//  Alert_Change.message = alert->message;
// 		if ( alert->freq ) { Alert_Change.freq = alert->freq; }
// 		Alert_Change.state = alert->state;
// 		Alert_Change.triggered = alert->triggered;
// 		if ( ( alert->timestamp > DEFAULT_TIME ) && ( Alert_Change.timestamp < alert->timestamp ) )
// 		{ Alert_Change.timestamp = alert->timestamp; }
// 		r = true;
// 		break;
// 		// 		case ALERT_PESTS: /*  Pest Checker:  Consider temp, humidity, lighting schedule */
// 		// 		//  Alert_Pests.message = alert->message;
// 		// 		if ( alert->freq ) { Alert_Pests.freq = alert->freq; }
// 		// 		Alert_Pests.state = alert->state;
// 		// 		Alert_Pests.triggered = alert->triggered;
// 		// 		if ( ( alert->timestamp > DEFAULT_TIME ) && ( Alert_Pests.timestamp < alert->timestamp ) )
// 		// 		{ Alert_Pests.timestamp = alert->timestamp; }
// 		// 		r = true;
// 		// 		break;
// 		case ALERT_SYSTEM:
// 		//  Alert_Unknown.message = alert->message;
// 		if ( alert->freq ) { Alert_System.freq = alert->freq; }
// 		Alert_System.state = alert->state;
// 		Alert_System.triggered = alert->triggered;
// 		if ( ( alert->timestamp > DEFAULT_TIME ) && ( Alert_System.timestamp < alert->timestamp ) )
// 		{ Alert_System.timestamp = alert->timestamp; }
// 		r = true;
// 		break;
// 	}
// 	//	} // end for
// 	return r;
// }
// 
// void printAlerts( Alert *alert ) {
// 	if ( DEBUG ) {
// 		for (; alert != NULL; alert = alert->next ) {
// 			// Don't bother printing unless alert triggered
// 			//			if ( alert->triggered ) {
// 			Serial.print(F("Alert:  "));
// 			Serial.print(alert->type);
// 			Serial.print(F(" triggered:  "));
// 			Serial.print(alert->triggered);
// 			Serial.print(F(" : "));
// 			Serial.print(alert->timestamp);
// 			Serial.print(F(" message:  "));
// 			Serial.print(alert->message);
// 			Serial.println("");
// 			//			}
// 		}
// 	}
// }


bool applianceOverride( void ) {
	bool r = true;
	
	Appliance *app = &Appliance_FeedPump;

	for (; app != NULL; app = app->next ) {
		// update timestamp to now
		app->timestamp = now();
		// turn off the appliance
		app->state = false;
		// disable appliance flag
		app->ready = false;
		r = SyncAppliance( app );
	}
	return r;
}

void conservePower( void )
{
	for ( uint8_t _p = 0; _p < 8; _p++ )
	{
		digitalWrite( _p, LOW );
	}
}

#else
#error This code is only for use on Arduino.
#endif // ARDUINO
#endif // __CORECONDUIT_H__
