/*
* @file TheTimeMan.h
* Copyright (C) 2014 Cory J. Potter - All Rights Reserved
* You may use, distribute and modify this code under the
* terms of the LICENSE.txt
* NOT INTENDED FOR COMMERCIAL USE!
* You should have received a copy of the LICENSE.txt with
* this file. If not, please write to: <bitsandbots@gmail.com>
*/

#ifndef __CLOCKSETTER_H__
#define __CLOCKSETTER_H__

#ifdef ARDUINO

//#define CLOCK_ADDRESS 0x68
//const unsigned long DEFAULT_TIME = 1420092000; // 06:00:00, 1/1/2015
//tmElements_t tm;

bool write(tmElements_t &tm)
{
	Wire.beginTransmission( CLOCK_ADDRESS );
	Wire.write((uint8_t)0x00); // reset register pointer
	Wire.write(dec2bcd(tm.Second)) ;
	Wire.write(dec2bcd(tm.Minute));
	Wire.write(dec2bcd(tm.Hour));      // sets 24 hour format
	Wire.write(dec2bcd(tm.Wday));
	Wire.write(dec2bcd(tm.Day));
	Wire.write(dec2bcd(tm.Month));
	Wire.write(dec2bcd(tmYearToY2k(tm.Year)));

	uint8_t result = Wire.endTransmission ();
	if (result == 0 ) { exists = true; } else { exists = false; errorHandler( result ); Serial.println("     on RTC read"); }
	return exists;
}

time_t get()   // Aquire data from buffer and convert to time_t
{
	tmElements_t tm;
	if (read(tm) == false) return 0;
	return(makeTime(tm));
}

bool set(time_t t)
{
	bool r = false;
	tmElements_t tm;
	breakTime(t, tm);
	tm.Second |= 0x80;  // stop the clock
	r = write(tm);
	tm.Second &= 0x7f;  // start the clock
	r = write(tm);
	return r;
}

bool getTime(const char *str) {
	int Hour, Min, Sec;

	if (sscanf(str, "%d:%d:%d", &Hour, &Min, &Sec) != 3) return false;
	tm.Hour = Hour;
	tm.Minute = Min;
	tm.Second = Sec;
	return true;
}

bool getDate(const char *str) {
	char Month[12];
	int Day, Year;
	uint8_t monthIndex;

	if (sscanf(str, "%s %d %d", Month, &Day, &Year) != 3) return false;
	for (monthIndex = 0; monthIndex < 12; monthIndex++) {
		if (strcmp(Month, monthName[monthIndex]) == 0) break;
	}
	if (monthIndex >= 12) return false;
	tm.Day = Day;
	tm.Month = monthIndex + 1;
	tm.Year = CalendarYrToTm(Year);
	return true;
}

bool checkConsoleTime() {
	bool r = false;
	
	// get the date and time the compiler was run
	// and configure the RTC with this info
	if ( ( getDate(__DATE__) && getTime(__TIME__) ) )
	{
		if ( write(tm) )
		{
			setTime(tm.Hour, tm.Minute, tm.Second, tm.Day, tm.Month, tmYearToCalendar(tm.Year));
			r = true;
			Serial.print(F("DS1307 configured Time=")); Serial.print(__TIME__); Serial.print(", Date = " ); Serial.println(__DATE__);
		}
	}

	return r;
}

#else
#error This code is only for use on Arduino.
#endif // ARDUINO

#endif // __CLOCKSETTER_H__
