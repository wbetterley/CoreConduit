/*
* @file commons.h
* Copyright (C) 2014 Cory J. Potter - All Rights Reserved
* You may use, distribute and modify this code under the
* terms of the LICENSE.txt
* NOT INTENDED FOR COMMERCIAL USE!
* You should have received a copy of the LICENSE.txt with
* this file. If not, please write to: <bitsandbots@gmail.com>
*/

#ifndef __COMMONS_H__
#define __COMMONS_H__

#ifdef ARDUINO

const unsigned long DEFAULT_TIME = 1420092000; // 06:00:00, 1/1/2015

/************************************************************************/
/*  ANALOG PIN DEFINITIONS                                              */
/************************************************************************/
#define PIN_A0 A0
#define PIN_A1 A1
#define PIN_A2 A2
#define PIN_A3 A3
#define PIN_A4_I2C_SDA A4
#define PIN_A5_I2C_SCL A5

/************************************************************************/
/*  DIGITAL PIN DEFINITIONS                                             */
/************************************************************************/

#define PIN0_RX 0
#define PIN1_TX 1
#define PIN2_INT0 2  //interrupt 0
#define PIN3_INT1 3  //interrupt 1
#define PIN4 4
#define PIN5 5
#define PIN6 6
#define PIN7 7
#define PIN8 8
#define PIN9_RF_CE 9
/** SPI CONNECTIONS **/
#define PIN10_CS 10  // Reserved for ChipSelect
/*
* Note that even if you don't use the hardware CS/SS pin,
* it must be left as an output or the SD library won't work.
*/
#define PIN11_MISO 11
#define PIN12_MOSI 12
#define PIN13_SCK 13
//LED_BUILTIN = 13

#define OFF false
#define ON  true

/************************************************************************/
/* Debugging Helpers                                                    */
/************************************************************************/

bool DEBUG = false;

void isDebugMode ( bool forceDebug )
{
	Serial.begin(19200);
	Serial.println(F("DEBUG MODE?"));
	//Wait for 5 seconds or until data is available on serial,
	//whichever occurs first.
	int test = 0;
	while( millis() < 5000 ) {
		test = Serial.read(); //We then clear the input buffer
		if (forceDebug) { test = forceDebug; }
		if ( test > 0 )
		{
			//If data is available, we enter here.
			Serial.print(F("received:  ") );
			Serial.println( test );
			//On timeout or availability of data, we come here.
			DEBUG = true; //Enable debug
			break;
		}
	} // end while
	
//	if ( DEBUG == false ) { Serial.println("Goodbye!"); Serial.end(); }
}


/*
What free_ram() is actually reporting
is the space between the heap and the stack.
it does not report any de-allocated memory
that is buried in the heap.
Buried heap space is not usable by the stack,
and may be fragmented enough that it is not
usable for many heap allocations either.
The space between the heap and the stack
is what you really need to monitor if you are
trying to avoid stack crashes.
*/

int freeRam () {
	extern int __heap_start, *__brkval;
	int v;
	return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}



/***********************************************************************
*
* Interrupt Service Routine (ISR)
*
* When writing an Interrupt Service Routine (ISR):
*** Keep it short
*** Don't use delay ()
*** Don't do serial prints
*** Make variables shared with the main code: volatile
*** Variables shared with main code may need to be protected by "critical sections"
*** Don't try to turn interrupts off or on
************************************************************************/

//void interrupted() {
// digitalWrite(PIN8, HIGH);
//}

void errorHandler ( uint8_t result )
{
	if ( DEBUG ) {
		switch ( result )
		{
			case 0: //success
			break;
			case 1: Serial.println(F("1 - Waiting for Start bit")); break;
			case 2: Serial.println(F("2 - slave in transmit mode")); break;
			case 3: Serial.println(F("3 - Sending to slave")); break;
			case 4: Serial.println(F("4 - Repeated Start")); break;
			case 5: Serial.println(F("5 - Slave in receiver mode")); break;
			case 6: Serial.println(F("6 - Receiving from slave")); break;
			case 7: Serial.println(F("7 - Stop bit")); break;
		}
	}
}


#else
#error This example is only for use on Arduino.
#endif // ARDUINO

#endif // __COMMONS_H__
