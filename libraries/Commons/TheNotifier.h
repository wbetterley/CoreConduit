/*
* @file TheNotifier.h
* Copyright (C) 2014 Cory J. Potter - All Rights Reserved
* You may use, distribute and modify this code under the
* terms of the LICENSE.txt
* NOT INTENDED FOR COMMERCIAL USE!
* You should have received a copy of the LICENSE.txt with
* this file. If not, please write to: <bitsandbots@gmail.com>
*/

/**
*
*	Activate Lifecycle
*	ASSUMPTIONS:  growing "tomato-type" plants indoors in NW America
*
* Alert:  Temperature out-of-range
* Alert:  Temperature doesn't drop after the fans are activated.
* Alert:  Humidity out-of-range
* Alert:  Luminescence too low OR oN/oFF not as scheduled
* Alert:  Water level too low
* Alert:  Soil Moisture remaining - too soggy, too dry
* Reminder:  Time to clean
* Reminder:  Time to add/remove nutrient solution
* Reminder:  Time for prevention treatment
* Reminder:  Pest Checker:  Consider temp, humidity, lighting schedule
*
*/

#ifndef __THENOTIFIER_H__
#define __THENOTIFIER_H__

#ifdef ARDUINO

// unsigned long time_diff = 0;
// Declare here so we can use it before we define it.
bool timeDiff ( Alert *alert );

bool TheNotifier( Alert *alert ) {
	bool r = false;

	if ( Sensor_Temp.value == 0 ) { Alert_Temp.state = false; } else { Alert_Temp.state = true; }
	if ( Sensor_Humidity.value == 0 ) { Alert_Humid.state = false; } else { Alert_Humid.state = true; }

	// Is this Alert even turned ON?
	if ( (clockFlag) && ( alert->state == true ) )
	{
		switch (alert->type)
		{
			// Alert Alert_Temp = {1, ALERT_TEMP, "Too Cold! Require Heater", true. false, 0, &Alert_Unknown };
			// if temp < min && temp.timestamp has been cold for X amount of time.
			case ALERT_TEMP:
			// if temp < min && temp.timestamp has been cold for X amount of time.
			if ( ( Sensor_Temp.value < (Sensor_Temp.minVal - 3) ) )
			{	//&& ( Appliance_IntakeFan.state == false ) ) {
				//		if ( timeDiff( &Appliance_IntakeFan ) ) {
				if ( timeDiff( &Alert_Fans ) )
				{
					Alert_Temp.triggered = true; r = true;
					Alert_Temp.timestamp = now();
				}
			}
			break;
			// Alert Alert_Fans = {1, ALERT_FANS, "Too Hot! Need More Ventilation", true, false, 0, &Alert_Temp };
			// if temp > max && fan = ON && fan.timestamp has been on for X amount of time.
			case ALERT_FANS:
			// if temp > max && fan = ON && fan.timestamp has been on for X amount of time.
			if ( ( Sensor_Temp.value > (Sensor_Temp.maxVal + 3) ) )
			{	// && ( Appliance_IntakeFan.state == true ) ) {
				if ( timeDiff( &Alert_Fans ) )
				{
					Alert_Fans.triggered = true; r = true;
					Alert_Fans.timestamp = now();
				}
			}
			break;
			// Alert Alert_Humid = {1, ALERT_HUMID, "Humidity Out-of-Range", true, false, 0, &Alert_Fans };
			// if humidity > max && fan = ON && fan.timestamp has been on for X amount of time.
			// OR
			// if humidity < min && Humidifier = ON && humidity.timestamp has been on for X amount of time.
			case ALERT_HUMID:
			// if humidity > max && fan = ON && fan.timestamp has been on for X amount of time.
			// OR
			// if humidity < min && Humidifier = ON && humidity.timestamp has been on for X amount of time.
			if ( ( Sensor_Humidity.value < (Sensor_Humidity.minVal -3) ) ) { // && ( Appliance_Humidifier.state == true ) ) {
				//		if ( timeDiff( &Appliance_Humidifier ) ) {
				if ( timeDiff( &Alert_Humid ) ) {
					Alert_Humid.triggered = true; r = true;
					Alert_Humid.timestamp = now();
				}
			}
			break;
			// Alert Alert_Light = {1, ALERT_LIGHT, "Luminescence Fault", true, false, 0, &Alert_Humid };
			case ALERT_LIGHT:
			// 			if ( Sensor_Light.value < Sensor_Light.minVal ) {
			// 				//		if ( checkSensitivity( &Appliance_Light ) ) {
			// 				Alert_Light.triggered = true; r = true;
			// 				Alert_Light.timestamp = now();
			// 				//		}
			// 			}
			// 			if ( Sensor_Light.value > Sensor_Light.maxVal ) {
			// 				//		if ( checkSensitivity( &Appliance_Light ) ) {
			// 				Alert_Light.triggered = true; r = true;
			// 				Alert_Light.timestamp = now();
			// 				//		}
			// 			}
			break;
			// Alert Alert_Float = {1, ALERT_FLOAT, "Water Level", true, false,  0, &Alert_Light };
			// float sensor activated
			case ALERT_FLOAT:
			if( Sensor_FeedFloat.value == 0 ) {
				//		if ( timeDiff( &Appliance_FeedPump ) ) {
				if ( timeDiff( &Alert_Float ) ) {
					// Disable the Pump
					Appliance_FeedPump.ready = false;
					Alert_Float.triggered = true;
					Alert_Float.timestamp = now();
					r = true;
				}
			}
			break;
			// if moisture sensor > max && FeedPump = OFF for X amount of time
			// if moisture sensor < min && FeedPump = ON for X amount of time
			case ALERT_MOISTURE:
			if ( ( Sensor_Moisture.value < Sensor_Moisture.minVal ) && ( Appliance_FeedPump.state == false ) ) {
				if ( timeDiff( &Alert_Moist ) ) {
					Alert_Moist.triggered = true;
					Alert_Moist.timestamp = now();
					r = true;
				}
			}
			if ( ( Sensor_Moisture.value > Sensor_Moisture.maxVal ) && ( Appliance_FeedPump.state == true ) )
			{
				if ( timeDiff( &Alert_Moist ) ) {
					Alert_Moist.triggered = true;
					Alert_Moist.timestamp = now();
					r = true;
				}
			}
			break;
			case ALERT_CHANGE:
			if ( Alert_Change.state == true ) {
				if ( timeDiff( &Alert_Change ) ) {
					Alert_Change.triggered = true;
					Alert_Change.timestamp = now();
					r = true;
				}
			}
			break;
			case ALERT_SYSTEM:
			if ( Alert_System.timestamp == Sensor_Temp.timestamp ) { Alert_System.state = true; }
			if ( Alert_System.state == true )
			{
				Alert_System.triggered = false;
				Alert_System.timestamp = now();
				if ( timeDiff( &Alert_System ) )
				{
					Alert_System.triggered = true;
					r = true;
				}
			}
			break;
		}
	}
//else  return false
return r;
}

bool timeDiff ( Alert *alert ) {
	bool r = false;
	if ( clockFlag ) {
		time_diff = 0;
		time_now = now();
		// check for first time through and apply appropriate timestamp.
		if ( alert->timestamp == DEFAULT_TIME ) { alert->timestamp = time_now; }
		time_diff = ( time_now - alert->timestamp );
		// Has the alert waited long enough before sending out a notice?
		if ( time_diff > ( ( alert->freq * oneDay ) + 300 ) ) /* wait at least 5 minutes */
		{
			r = true;
			if ( DEBUG ) { Serial.print(F("alert time_diff:  "));  Serial.println( time_diff ); }
		}
	}
	return r;
}

uint8_t countAlerts( void )
{
	uint8_t alertCounter = 0;
	// Start at the beginning struct
	Alert *alert = &Alert_System;

	for (; alert != NULL; alert = alert->next ) {
		if ( alert->triggered ) { alertCounter++; }
	}
	return alertCounter;
}


bool clearOneAlert( void ) {
	bool r = false;
	
	Alert *alert = &Alert_System;

	for (; alert != NULL; alert = alert->next ) {
		// alert.message = alert->message;
		// alert.freq = alert->freq;
		if ( ( alert->triggered == true ) )
		{
			r = true;
			// update timestamp to now
			alert->timestamp = now();
			// turn the alert back on
			alert->state = true;
			// reset triggered flag
			alert->triggered = false;
			SyncAlert( alert );
			//stop looking and exit we only want one at a time
			break;
		}
	}
	return r;
}

#else
#error This code is only for use on Arduino.
#endif // ARDUINO
#endif // __THENOTIFIER_H__
