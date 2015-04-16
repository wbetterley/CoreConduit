/*
* @file ClockSetter.h
* Copyright (C) 2014 Cory J. Potter - All Rights Reserved
* You may use, distribute and modify this code under the
* terms of the LICENSE.txt
* NOT INTENDED FOR COMMERCIAL USE!
* You should have received a copy of the LICENSE.txt with
* this file. If not, please write to: <bitsandbots@gmail.com>
*/

#ifndef __CLOCK_H__
#define __CLOCK_H__

#ifdef ARDUINO

#define CLOCK_ADDRESS 0x68
//const unsigned long DEFAULT_TIME = 1420092000; // 06:00:00, 1/1/2015
tmElements_t tm;

// Weekly Time Schedule?
const unsigned long oneDay =  86400UL;
const unsigned long oneHour = 3600UL;
// oneWeek = 604800 seconds * 1000 = milliseconds
// 4 weeks = 2419200 seconds
// 30 days = 2592000 seconds
// 365 days = 31536000 seconds
unsigned long time_now = 0;
unsigned long time_millis = 0;
unsigned long time_diff = 0;
bool clockFlag = false;

/************************************************************************/
/*		
Real-time Clock Module based on the DS1307
EEPROM memory AT24C32 with 32Kb
*/
/************************************************************************/

/*
DS1307 I2C real-time clock chip (RTC)
AT24C32 32K I2C EEPROM memory Includes a
LIR2032 rechargeable lithium battery
Breaks out DS1307 clock pin to offer clock
signals for microcontrollers.
Can be cascaded to other I2C devices.
Breakout pins for adding a DS18B20 temperature sensor DS1370
Real Time Clock: The DS1307 works best with 5V-based chips such as the Arduino.
The chip has 56 bytes of non-volatile RAM, full BCD code clock and calendar.

AT24C32 32Kb EEPROM Memory: The AT24C32 EEPROM has 32K-bit serial CMOS E2PROM
with I2C bus data transfer protocol. The AT24C32 can store 4096 words of 8 bits
(32Kb). It supports 1.8V to 5V power supply, and can be erased for one million
times, with a data up to 100 years.

I2C BUS: The DS1307 and the AT24C32 share the I2C Bus, you just need to connect
the SDA (Data) and SCL (Clock) lines to your Arduino for communication. No
external pull-ups are needed, these are included in the board. On your Arduino
(everything but the mega) SDA is on analog pin 4, and SCL is on analog pin 5. On
an arduino mega, SDA is digital 20, and SCL is digital 21.

DS18B20 Support: The breakout has 3 pins to add a DS18B20 digital temperature
sensor (not supplied on the board). The DS18B20 digital temperature sensor
adopts single-bus technology, which can effectively reduce external interference
and improve the measurement accuracy. Simply connect the DS pin to any digital
input of your Arduino
*/

const char *monthName[12] = {
	"Jan", "Feb", "Mar", "Apr", "May", "Jun",
	"Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};


/************************************************************************/
/*  Real-Time Clock                                                     */
/************************************************************************/
bool exists = false;
// UTILITY FUNCTIONS
// Convert Decimal to Binary Coded Decimal (BCD)
uint8_t dec2bcd(uint8_t num)
{
	return ((num/10 * 16) + (num % 10));
}

// Convert Binary Coded Decimal (BCD) to Decimal
uint8_t bcd2dec(uint8_t num)
{
	return ((num/16 * 10) + (num % 16));
}

// Aquire data from the RTC chip in BCD format
bool read(tmElements_t &tm)
{
	bool r = true;
	uint8_t sec;
	Wire.beginTransmission( CLOCK_ADDRESS );
	Wire.write((uint8_t)0x00); // reset register pointer
	uint8_t result = Wire.endTransmission ();
	if (result == 0 ) { exists = true; } else { exists = false; r = false;
	if (DEBUG) { errorHandler( result ); Serial.print(F(" on RTC write 0x00 ")); }
}
// request the 7 data fields   (secs, min, hr, dow, date, mth, yr)
Wire.requestFrom( CLOCK_ADDRESS, tmNbrFields);
uint8_t available = Wire.available();
if ( available < tmNbrFields ) { r = false;
//	if (DEBUG) { Serial.print(F("What's available? ")); Serial.println ( available ); }
	} else {
	sec = Wire.read();
	tm.Second = bcd2dec(sec & 0x7f);
	tm.Minute = bcd2dec(Wire.read() );
	tm.Hour =   bcd2dec(Wire.read() & 0x3f);  // mask assumes 24hr clock
	tm.Wday = bcd2dec(Wire.read() );
	tm.Day = bcd2dec(Wire.read() );
	tm.Month = bcd2dec(Wire.read() );
	tm.Year = y2kYearToTm((bcd2dec(Wire.read())));
	if (sec & 0x80) { r = false;
//		if (DEBUG) { Serial.println (F("clock is halted!" )); }
	}
}
return r;
}

bool setClock( void ) {
	bool r = false;
	time_now = now();
	
	if( read(tm) )
	{
		setTime(tm.Hour, tm.Minute, tm.Second, tm.Day, tm.Month, tmYearToCalendar(tm.Year));
		r = true; if ( DEBUG ) { Serial.println(F("RTC has been read.")); }
		} else {
		// if nothing else, set the time to default and we'll work from there.
		if ( time_now < DEFAULT_TIME ) { setTime( DEFAULT_TIME ); }
		if ( DEBUG ) { Serial.println(F("Waiting for DS1307...")); }
	}
	// end of if( RTC.read(tm) )
	return r;
}



#else
#error This code is only for use on Arduino.
#endif // ARDUINO

#endif // __CLOCK_H__
