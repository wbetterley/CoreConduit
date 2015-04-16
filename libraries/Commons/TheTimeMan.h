/*
* @file TheTimeMan.h
* Copyright (C) 2014 Cory J. Potter - All Rights Reserved
* You may use, distribute and modify this code under the
* terms of the LICENSE.txt
* NOT INTENDED FOR COMMERCIAL USE!
* You should have received a copy of the LICENSE.txt with
* this file. If not, please write to: <bitsandbots@gmail.com>
*/

#ifndef __THETIMEMAN_H__
#define __THETIMEMAN_H__

#ifdef ARDUINO


// Define STaTe MaCHiNe fLaGs
// unsigned long txTimer = 2500;
uint8_t napTimeCounter = 0;

enum TIMER_TYPE {
	TIMER_LOG,
	TIMER_ALERTS,
	TIMER_LCD,
	TIMER_LCD_CYCLE,
	TIMER_LCD_SCROLLER,
	TIMER_RX_DATA,
	TIMER_SAVE_SETTINGS,
	TIMER_SENSOR_READINGS,
	TIMER_LCD_BUTTON
};

typedef struct Timer
{
	TIMER_TYPE type;
	unsigned long interval;
	bool state;		// Timer is ON / OFF
	bool triggered;  // flag
	unsigned long timestamp; // time of last execution
	struct Timer *next;
} Timer;

// Timer Timer_Log 			= { LOG_TIMER, 60L, true, false, 0, NULL };
// Timer Timer_Lcd 			= { LCD_TIMER, 12L, true, false, 0, &Timer_Log };
// Timer Timer_Lcd_Cycle 		= {	LCD_CYCLE_TIMER, 4L, true, false, 0, &Timer_Lcd };
// Timer Timer_Lcd_Scroller 	= { LCD_SCROLLER_TIMER, 1L, true, false, 0, &Timer_Lcd_Cycle };
// Timer Timer_Save_Settings 	= {	SAVE_SETTINGS_TIMER, EEPROM_SAVE_INTERVAL, true, false,  0, &Timer_Lcd_Scroller };
// Timer Timer_Sensor_Read		= { SENSOR_READINGS_TIMER, 15L, true, false,  0, &Timer_Save_Settings };

bool napCheck ( void )
{
	bool r = false;
	if ( napTimeCounter > 4 )
	{
		r = true;
		napTimeCounter = 0;
	}
	napTimeCounter++;
	return r;
}

bool checkTimer ( Timer *timer )
{
	bool r = false;
	time_millis = millis();

if ( timer->state == true )
{
	if ( time_millis > timer->timestamp )
	{
		if ( timer->timestamp != 0 ) { r = true; }
 		//if (DEBUG) { time_diff = ( time_millis - timer->timestamp ); }
		timer->timestamp = ( time_millis + timer->interval ) - ( time_millis % timer->interval );
		//if (DEBUG) { Serial.print("time_diff: "); Serial.println(time_diff); }
	}
}
	return r;
}

void resetAllTimers( Timer *timer )
{
	time_millis = millis();
	for (; timer != NULL; timer = timer->next ) {
		{
		if ( timer->state == true ) { timer->timestamp = ( time_millis + timer->interval ) - ( time_millis % timer->interval ); }
		}
	} // end for
}

/**
bool SyncTimer(volatile Timer *timer) {
bool r = false;
for (; timer != NULL; timer = timer->next ) {

switch (timer->type)
{
case LOG_TIMER:
if ( timer->interval ) { Timer_Log.interval = timer->interval; }
Timer_Log.state = timer->state;
Timer_Log.triggered = timer->triggered;
if ( ( timer->timestamp > 0 ) && ( Timer_Log.timestamp < timer->timestamp ) )
{ Timer_Log.timestamp = timer->timestamp; }
r = true;
break;
case LCD_TIMER:
if ( timer->interval ) { Timer_Lcd.interval = timer->interval; }
Timer_Lcd.state = timer->state;
Timer_Lcd.triggered = timer->triggered;
if ( ( timer->timestamp > 0 ) && ( Timer_Lcd.timestamp < timer->timestamp ) )
{ Timer_Lcd.timestamp = timer->timestamp; }
r = true;
break;
case LCD_CYCLE_TIMER:
if ( timer->interval ) { Timer_Lcd_Cycle.interval = timer->interval; }
Timer_Lcd_Cycle.state = timer->state;
Timer_Lcd_Cycle.triggered = timer->triggered;
if ( ( timer->timestamp > 0 ) && ( Timer_Lcd_Cycle.timestamp < timer->timestamp ) )
{ Timer_Lcd_Cycle.timestamp = timer->timestamp; }
r = true;
break;
case LCD_SCROLLER_TIMER:
if ( timer->interval ) { Timer_Lcd_Scroller.interval = timer->interval; }
Timer_Lcd_Scroller.state = timer->state;
Timer_Lcd_Scroller.triggered = timer->triggered;
if ( ( timer->timestamp > 0 ) && ( Timer_Lcd_Scroller.timestamp < timer->timestamp ) )
{ Timer_Lcd_Scroller.timestamp = timer->timestamp; }
r = true;
break;
case SAVE_SETTINGS_TIMER:
if ( timer->interval ) { Timer_Save_Settings.interval = timer->interval; }
Timer_Save_Settings.state = timer->state;
Timer_Save_Settings.triggered = timer->triggered;
if ( ( timer->timestamp > 0 ) && ( Timer_Save_Settings.timestamp < timer->timestamp ) )
{ Timer_Save_Settings.timestamp = timer->timestamp; }
r = true;
break;
case SENSOR_READINGS_TIMER:
if ( timer->interval ) { Timer_Sensor_Read.interval = timer->interval; }
Timer_Sensor_Read.state = timer->state;
Timer_Sensor_Read.triggered = timer->triggered;
if ( ( timer->timestamp > 0 ) && ( Timer_Sensor_Read.timestamp < timer->timestamp ) )
{ Timer_Sensor_Read.timestamp = timer->timestamp; }
r = true;
break;
}

}
return r;
}

**/

#else
#error This code is only for use on Arduino.
#endif // ARDUINO

#endif // __THETIMEMAN_H__
