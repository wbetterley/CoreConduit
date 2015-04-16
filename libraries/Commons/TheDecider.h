/*
* @file TheDecider.h
* Copyright (C) 2014 Cory J. Potter - All Rights Reserved
* You may use, distribute and modify this code under the
* terms of the LICENSE.txt
* NOT INTENDED FOR COMMERCIAL USE!
* You should have received a copy of the LICENSE.txt with
* this file. If not, please write to: <bitsandbots@gmail.com>
*/

/**

Indoor Gardening Checklist

Environment:
Keep Environment Clean: Easier to work in and helps to prevent contaminating plants.
Temperature:  Day 70°-75°F, Night 55°-60°F is optimum for most plants
Humidity:  40%-60% is ideal for most growing.  Check walls and ceilings for molds or condensation.
Air Circulation and Ventilation: Proper circulation will prevent dead zones of bad and/or cold air at lower levels and hot layers of air near the ceiling.
Check top and underside of leaves for likely signs of disease, insects or nutrient deficiency.
Promptly remove and dispose of any dead, dying or diseased foliage in the growing area.  Conduct any necessary shaping, training, or stressing of branches and ensure foliage is properly supported (via string, netting or stakes).

Lights:
Check Lights:  Maintain lamps 12-36 inches above plants!  Check timer is correctly programmed for the given phase of growth.  Check electrical connections/plugs.
Check for signs of leaf burn on foliage closest to the lamp.
Rotate or turn plants as needed to get uniform growth.  Ensure oscillating fans are covering all foliage, especially those closest to the lamp.
Check for signs of insufficient light e.g. sparse, spindly, foliage.  Ensure foliage is not being unduly shaded by other plants.

Nutrient Solution:
Check/adjust EC. For recirculating systems, add top-up water as needed.  Use an EC meter to measure the strength/concentration of the nutrient solution to keep adequate concentration level.
Check pH: Ideal pH for most plants is between 5.5 and 6.5
Check nutrient temperature is between 18-21°C (64-70°F).
Check for plumbing leaks; pooling in trays/channels/pots.
Check drainage and/or feed outlets (drippers) are not being blocked by roots.
Check roots/medium at various points for signs of disease, rotting or molding materials, insects or over/under watering.
Check pumps and timers are working properly.  Ensure nutrient schedule appropriate for the current phase of growth.
Recirculating systems: Discard and replace old nutrient every 7-14 days.
Conduct optional foliar spraying and apply optional additives e.g. compost tea.

Keep a Growing Journal:  Note finding for each time you observe something.
Need Assistance?  support@coreconduit.com

*/





#ifndef __THEDECIDER_H__
#define __THEDECIDER_H__

#ifdef ARDUINO

#include <RCSwitchLite.h>

// Declare functions we'll define after this function
void initializeAppliances( void );
void checkAppliances ( void );
void txSwitchOff(uint8_t switchNumber);
void txSwitchOn(uint8_t switchNumber);
bool updateAppliance(Appliance *app);
bool myAppliance(Appliance *app, bool ready);
Appliance *previousAppliance;

/************************************************************************/
/*  REPLACE THE FOLLOWING CODES 
/*  with the codes received through the SetupC.ino                                                                     */
/************************************************************************/

// ETEKCITY #1419
unsigned int pulseLength = 188;
// unsigned long mySwitchOn[] = {24,333107,333251,333571,335107,341251};
// unsigned long mySwitchOff[] = {24,333116,333260,333580,335116,341260};

// ETEKCITY #1415
unsigned long mySwitchOn[] = { 24,21811,21955,22275,23811,29955 };
unsigned long mySwitchOff[] = { 24,21820,21964,22284,23820,29964 };

bool TheDecider( void )
{
	bool r = false;
	time_diff = 0;
	time_now = now();

	// update app.ready if it's been 5 minutes since last change.
	Appliance * app = &Appliance_FeedPump;
	for (; app != NULL; app = app->next )
	{
		time_diff = (time_now - app->timestamp);
		if (time_diff > 300) { app->ready = true; }
	}

	if ( Sensor_Humidity.value == 0 ) { Appliance_Humidifier.ready = false; }
	if ( Sensor_Temp.value == 0 )
	{ Appliance_ExhaustFan.state = true; Appliance_ExhaustFan.ready = false; Appliance_IntakeFan.state = true; Appliance_IntakeFan.ready = false; }

	// RH Range
	if (Appliance_Humidifier.ready == true) {
		// IF the humidity is less than the minVal and the Humidifier is OFF
		// THEN turn ON the Humidifier
		if ((Sensor_Humidity.value < Sensor_Humidity.minVal) && (Appliance_Humidifier.state != true))
		{ r = true; Appliance_Humidifier.state = true; myAppliance(&Appliance_Humidifier, false); }
		
		// IF the humidity is less than the minVal, turn ON the PUMP
		if ( (Sensor_Humidity.value < Sensor_Humidity.minVal) )
		{ r = true; Appliance_FeedPump.state = true; myAppliance(&Appliance_FeedPump, false); }

		// IF the humidity is greater than the maxVal and the Humidifier is ON
		// THEN turn OFF the Humidifier
		if ((Sensor_Humidity.value > Sensor_Humidity.maxVal) && (Appliance_Humidifier.state != false))
		{ r = true; Appliance_Humidifier.state = false; myAppliance(&Appliance_Humidifier, false); }
	}

	// IF the temperature is less than the minVal and the IntakeFan is ON
	// THEN turn OFF the IntakeFan
	if ( Appliance_IntakeFan.ready == true)
	{
		if ( ( Sensor_Temp.value < (Sensor_Temp.minVal + 3 ) ) && (Appliance_IntakeFan.state == true) )
		{ r = true; Appliance_IntakeFan.state = false; myAppliance(&Appliance_IntakeFan, false); }

		// IF the temperature is greater than the maxVal and the IntakeFan is OFF
		// THEN turn ON the IntakeFan
		if ( ( Sensor_Temp.value > ( Sensor_Temp.maxVal - 3 ) ) && (Appliance_IntakeFan.state == false) )
		{ r = true; Appliance_IntakeFan.state = true; myAppliance(&Appliance_IntakeFan, false); }
	} // if ((Appliance_IntakeFan.ready == true)
	
	// IF the temperature is less than the minVal and the ExhaustFan is ON
	// THEN turn OFF the ExhaustFan
	if ( Appliance_ExhaustFan.ready == true)
	{

		if ( ( Sensor_Temp.value < Sensor_Temp.minVal) && (Appliance_ExhaustFan.state == true))
		{ r = true; Appliance_ExhaustFan.state = false; myAppliance(&Appliance_ExhaustFan, false); }
		// IF the temperature is greater than the maxVal and the ExhaustFan is OFF
		// THEN turn ON the ExhaustFan
		if ( ( Sensor_Temp.value > Sensor_Temp.maxVal) && (Appliance_ExhaustFan.state == false) )
		{ r = true; Appliance_ExhaustFan.state = true; myAppliance(&Appliance_ExhaustFan, false); }

	} // end if ((Appliance_ExhaustFan.ready == true)
	
	time_diff = (time_now - Appliance_IntakeFan.timestamp);
	// IF the IntakeFan is OFF
	// AND it's been 15 minutes since the IntakeFan was turned ON
	// THEN turn ON the IntakeFan
	if ( ( Appliance_IntakeFan.state == false ) && ( time_diff > 900 ) )
	{ r = true; Appliance_IntakeFan.state = true; myAppliance( &Appliance_IntakeFan, false );
		if (DEBUG) { Serial.print( F("IntakeFan: ") ); Serial.println( time_diff ); }
	}

	// IF the light reading is greater than the minVal
	// AND the ExhaustFan is ON - THEN turn OFF Light(s)
	if ( Appliance_Light.ready )
	{
		if ( (Sensor_Light.value > Sensor_Light.minVal) && (Appliance_ExhaustFan.state == true) )
		{ r = true; Appliance_Light.state = false; myAppliance( &Appliance_Light, false ); }
		// IF the light reading is less than the minVal
		// THEN turn OFF lights
		if (Sensor_Light.value < Sensor_Light.minVal)
		{ r = true; Appliance_Light.state = false; myAppliance(&Appliance_Light, false); }
	}
	
	if (Appliance_FeedPump.ready == true)
	{
		// IF the soil moisture reading is less than the minVal
		// AND the FeedPump is OFF AND the FeedPump is ready
		// THEN turn ON the FeedPump
		if ( ( Sensor_Moisture.value < Sensor_Moisture.minVal) && (Appliance_FeedPump.state == false) )
		{ r = true;
			Appliance_FeedPump.state = true; myAppliance(&Appliance_FeedPump, false);
		}

		// IF the soil moisture reading is greater than the maxVal
		// AND the FeedPump is ON
		// THEN turn OFF the FeedPump
		if ( ( Sensor_Moisture.value > Sensor_Moisture.maxVal) && (Appliance_FeedPump.state == true) )
		{ r = true;
			Appliance_FeedPump.state = false; myAppliance(&Appliance_FeedPump, false);
		}
		// IF the FeedFloat is less than or equal to the minVal
		// AND the FeedPump is ON
		// THEN turn OFF the FeedPump AND not ready
		if (( Sensor_FeedFloat.value <= Sensor_FeedFloat.minVal) && (Appliance_FeedPump.state == true) )
		{ r = true;
			Appliance_FeedPump.state = false; myAppliance(&Appliance_FeedPump, false);
		}
	} // end Appliance_FeedPump.ready
	
	// THEN calculate time since the last time FeedPump was on
	time_diff = (time_now - Appliance_FeedPump.timestamp);
	// IF the FeedPump is ON
	// AND it's been 5 minutes since the pump was turned ON
	// THEN turn OFF the FeedPump
	if ( (Appliance_FeedPump.state == true) && ( time_diff > 300 ) )
	{
		r = true; Appliance_FeedPump.state = false; myAppliance( &Appliance_FeedPump, false );
	}
	// IF the FeedPump is OFF
	// AND it's been oneHour since the pump was turned ON
	// THEN turn ON the FeedPump				// ridiculous way of getting half hour
	if ( (Appliance_FeedPump.state == false) && ( time_diff > (oneHour/2) ) )
	{
		r = true; Appliance_FeedPump.state = true; myAppliance( &Appliance_FeedPump, false );
	}

	/*
	ToDo:  
	*/

	return r;
}

void initializeAppliances( void )
{
	Appliance * app = &Appliance_FeedPump;
	for (; app != NULL; app = app->next )
	{
		myAppliance( app, true );
	}
}

void checkAppliances ( void )
{
	time_now = now();
	//Appliance * a = previousAppliance;
	Appliance * app = &Appliance_FeedPump;
	for (; app != NULL; app = app->next )
	{
		if ( time_now > app->timestamp )
		{
			time_diff =	( time_now - (app->timestamp) );
			// On/Off Buffer 5 minutes
			if ( time_diff > 300 )
			{
				myAppliance( app, true );
			}
		}
	}
	//previousAppliance = a->next;
}



// Compare current Appliance with requested Appliance
// Update to reflect change
bool updateAppliance(Appliance *app)
{
	bool r = false;
	// compare current state to target state
	bool current_state = false;
	// Relay or Remote Outlet
	if (app->pin < 100) {
		current_state = digitalRead(app->pin);
		if (app->state != current_state)
		{
			if(current_state == true)
			{
				digitalWrite(app->pin, LOW);
			}
			else
			{
				digitalWrite(app->pin, HIGH);
			}
			// if the update takes:
			r = true;
		}
	}
	else
	{
		// We're working with RCswitch controls
		if(app->state == false)
		{
			txSwitchOff( (app->pin - 100) );
		}
		else // app->state == true
		{
			txSwitchOn( (app->pin - 100) );
		}
		r = true;
	}
	// if the update fails:
	return r;
}

bool myAppliance(Appliance *app, bool ready)
{
	bool r = false;
	//	Appliance *a = app;
	time_now = now();

	// Cannot update appliances that have been flagged
	if(app->ready)
	{
		if((Appliance_IntakeFan.pin == app->pin) && (Appliance_IntakeFan.type == app->type))
		{
			Appliance_IntakeFan.ready = ready;
			r = updateAppliance(app);
			Appliance_IntakeFan.state = app->state;
			Appliance_IntakeFan.timestamp = time_now;
		}
		if((Appliance_ExhaustFan.pin == app->pin) && (Appliance_ExhaustFan.type == app->type))
		{
			Appliance_ExhaustFan.ready = ready;
			r = updateAppliance(app);
			Appliance_ExhaustFan.state = app->state;
			Appliance_ExhaustFan.timestamp = time_now;
		}

		// 			if((Appliance_HumFan.pin == app->pin) && (Appliance_HumFan.type == app->type))
		// 			{
		// 				Appliance_HumFan.ready = app->ready;
		// 				Appliance_HumFan.state = app->state;
		// 					r = updateAppliance(app);
		// 					Appliance_HumFan.timestamp = now();
		// 			}
		// 			if((Appliance_HumPump.pin == app->pin) && (Appliance_HumPump.type == app->type))
		// 			{
		// 				Appliance_HumPump.ready = app->ready;
		// 				Appliance_HumPump.state = app->state;
		// 					r = updateAppliance(app);
		// 					Appliance_HumPump.timestamp = now();
		// 			}
		if((Appliance_Humidifier.pin == app->pin) && (Appliance_Humidifier.type == app->type))
		{
			Appliance_Humidifier.ready = ready;
			r = updateAppliance(app);
			Appliance_Humidifier.state = app->state;
			Appliance_Humidifier.timestamp = time_now;
		}

		if((Appliance_Light.pin == app->pin) && (Appliance_Light.type == app->type))
		{
			Appliance_Light.ready = ready;
			r = updateAppliance(app);
			Appliance_Light.state = app->state;
			Appliance_Light.timestamp = time_now;
		}

		if((Appliance_Heater.pin == app->pin) && (Appliance_Heater.type == app->type))
		{
			Appliance_Heater.ready = ready;
			r = updateAppliance(app);
			Appliance_Heater.state = app->state;
			Appliance_Heater.timestamp = time_now;
		}

		if((Appliance_FeedPump.pin == app->pin) && (Appliance_FeedPump.type == app->type))
		{
			Appliance_FeedPump.ready = ready;
			r = updateAppliance(app);
			Appliance_FeedPump.state = app->state;
			Appliance_FeedPump.timestamp = time_now;
		}

		//} //end of for loop

		} else {
		// return false
	}
	return r;
}

void txSwitchOn(uint8_t switchNumber)
{
	digitalWrite(PIN3_INT1, HIGH);
	RCSwitchLite mySwitch = RCSwitchLite();
	// try pulselength +-1)
	for ( uint8_t p = (pulseLength - 2); p < (pulseLength + 1) ; p++ )
	{
		mySwitch.setPulseLength(pulseLength);
		mySwitch.enableTransmit(PIN4);
		mySwitch.send(mySwitchOn[ switchNumber ], mySwitchOn[ 0 ]);
		// 			if ( DEBUG ) { Serial.print("#");
		// 				Serial.print( switchNumber );
		// 				Serial.print(", ON  "); Serial.print("sent[");
		//				Serial.print( mySwitchOff[ switchNumber ] );
		//		Serial.println("]");}
		delay(100);
	}
}

void txSwitchOff(uint8_t switchNumber)
{
	digitalWrite(PIN3_INT1, HIGH);
	RCSwitchLite mySwitch = RCSwitchLite();
	// try pulselength +-1)
	for ( uint8_t p = (pulseLength - 2); p < (pulseLength + 1) ; p++ )
	{
		// set pulse length.
		mySwitch.setPulseLength(pulseLength);

		mySwitch.enableTransmit(PIN4);
		mySwitch.send(mySwitchOff[ switchNumber ], mySwitchOff[ 0 ]);
		// 			if ( DEBUG ) { Serial.print("#");
		// 				Serial.print( switchNumber );
		// 				Serial.print(", OFF  "); Serial.print("sent[");
		//				Serial.print( mySwitchOff[ switchNumber ] );
		//		Serial.println("]");}
		delay(100);
	}
}



#else
#error This code is only for use on Arduino.
#endif // ARDUINO

#endif // __THEDECIDER_H__
