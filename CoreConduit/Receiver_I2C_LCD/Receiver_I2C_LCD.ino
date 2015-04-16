/*
* @file Receiver_I2C_LCD.ino
* Copyright (C) 2014 Cory J. Potter - All Rights Reserved
* You may use, distribute and modify this code under the
* terms of the LICENSE.txt
* NOT INTENDED FOR COMMERCIAL USE!
* You should have received a copy of the LICENSE.txt with
* this file. If not, please write to: <bitsandbots@gmail.com>
*/

/*
Arduino Networked Communications
Requires LCD Shield with buttons on Arduino Uno Pin A0
*/


/************************************************************************/
/**

Arduino Uno

*I2C:
A4 (SDA)
A5 (SCL)

*SPI:							(Arduino Pin) - Module Pin
10 (SS) "Slave Select"			  10 (SS) to CS
11 (MOSI) "Master Out Slave In"   11 (MOSI) to DI
12 (MISO) "Master In Slave Out"   12 (MISO) to DO
13 (SCK) "System Clock"           13 (SCK) to CLK
and G to GND and + to 5V

**/
/************************************************************************/

#include <Wire.h>
#include <I2C_Anything.h>
#include <Time.h>
#include <commons.h>
#include <Clock.h>
#include <TheTimeMan.h>
// replaced DS1307 library in TheTimeMan.h
#include <SPI.h>
// #include "nRF24L01.h"
// #include "RF24.h"
#include <EEPROM.h>
#include <EEPROMAnything/EEPROMAnything.h>
#include <LiquidCrystal.h>
#include <DataObject.h>
#include <CoreConduit.h>
//#include <CoreConduitRF.h>
#include <Alerts.h>
#include <TheNotifier.h>
#include <MySettings.h>
#include <TheRecorder.h>
#include <CoreLCD.h>

/*-----( Declare Constants )-----*/
// MOVED TO CoreLCD.h

// Start the LCD instance
// LiquidCrystal lcd(PIN8,PIN9_RF_CE,PIN4,PIN5,PIN6,PIN7);

/************************************************************************/
/*  Network Configuration                                               */
/************************************************************************/

/************************************************************************/
/*	I2C Communications                                                  */
/************************************************************************/
const int MY_ADDRESS = 22;
const int SEND_TO_ADDRESS = 42;
/************************************************************************/
/*  Timers			                                            		*/
/************************************************************************/

Timer Timer_Log 			= { TIMER_LOG, 60000UL, true, false, 0, NULL };
Timer Timer_Lcd 			= { TIMER_LCD, 12000UL, true, false, 0, &Timer_Log };
Timer Timer_Lcd_Cycle 		= {	TIMER_LCD_CYCLE, 6000UL, true, false, 0, &Timer_Lcd };
Timer Timer_Lcd_Scroller 	= { TIMER_LCD_SCROLLER, 500UL, true, false, 0, &Timer_Lcd_Cycle };
Timer Timer_Save_Settings 	= {	TIMER_SAVE_SETTINGS, 3600000UL, true, false,  0, &Timer_Lcd_Scroller };
//Timer Timer_Sensor_Read		= { SENSOR_READINGS_TIMER, 15000L, true, false,  0, &Timer_Save_Settings };

void setup()
{
	// If you want to set the aref to something other than 5v
	// analogReference(EXTERNAL);

	pinMode(PIN_A0, INPUT);

	/************************************************************************/
	/*  Interrupts		                                                    */
	/************************************************************************/

	// attachInterrupt(0, interrupted, CHANGE);
	// two interrupts are available: #0 on pin 2,
	// and interrupt				 #1 on pin 3.
	/************************************************************************/

	// Start the serial port to communicate to the PC
	isDebugMode(0);
	/************************************************************************/
	/* I2C Communications                                                   */
	/************************************************************************/
	Wire.onReceive ( receiveEvent );
	Wire.begin (MY_ADDRESS);
	/************************************************************************/
	//if( ( DEBUG ) ) { checkConsoleTime(); }
	clockFlag = setClock();
	/************************************************************************/

	/************************************************************************/
	lcd.begin(numCols,numRows);   // initialize the lcd for 16 chars 2 lines

	lcd.setBacklight(HIGH);
	lcd.noCursor();
	//-------- Write characters on the display ------------------
	// NOTE: Cursor Position: (CHAR, LINE) start at 0
	lcd.print("One moment...");
	/************************************************************************/
}


/*********************************
*        STaTe MaCHiNe          *
*********************************/
void loop()
{
	
	// YIELD to other processes and delay for the amount of time we want between readings
	if ( ( millis() - LcdButtonsDelay ) > 750 ) { read_LCD_buttons(); } else { /* delay */ }

	if ( haveData > 0 ) { receiveData( haveData ); resetAllTimers( &Timer_Lcd_Scroller); haveData = 0; }
	if ( requestSync ) { txRequest( requestSync, SEND_TO_ADDRESS ); requestSync = 0; }

	/************************************************************************/
	/*  Timers			                                            		*/
	/************************************************************************/
	if ( checkTimer( &Timer_Log ) )
	{
		checkAlerts();
		keepAlive();
		if ( napCheck() == true )
		{
			// turn off backlight
			pinMode( PIN10_CS, OUTPUT );
			digitalWrite( PIN10_CS, LOW);
			// turn off display
			lcd.noDisplay();
		}
	}


	/************************************************************************/
	/*  LCD Timer				                                            */
	/************************************************************************/
	if ( checkTimer( &Timer_Lcd ) )
	{
		LcdDisplay( modeSelect );
	}


	if ( ( checkTimer( &Timer_Lcd_Cycle ) ) && ( Timer_Lcd_Cycle.triggered == true ) )
	{ if (modeSelect == 0) { Timer_Lcd_Cycle.triggered = LcdPrintApps( 0 );} }
	// else? other modes

	if ( ( checkTimer( &Timer_Lcd_Scroller ) ) && ( Timer_Lcd_Scroller.triggered == true ) )
	{
		currentLetter = LcdScroller( currentLetter, lcdScrollerMessage );
	}


	/************************************************************************/
	/*   EEPROM Save Timer                                                   */
	/************************************************************************/
	if ( checkTimer( &Timer_Save_Settings ) )
	{
		if ( clockFlag ) { saveAllDataObjects(); }
	}
}

void LcdDisplay ( uint8_t modeSelected )
{
	// if you called this function then
	// assume we want it's timer running
	Timer_Lcd.state = true;

	modeSelect = modeSelected;
	// Allow Notifier to send notifications
	alertCounter = countAlerts();
	if( alertCounter > 0 ) {
		modeSelect = 1;
		if ( DEBUG ) { Serial.print(F("alertCounter:  ")); Serial.println( alertCounter );
		Serial.print(F("modeSelect:  ")); Serial.println(modeSelect); }
	}

	Timer_Lcd_Cycle.triggered = false;
	Timer_Lcd_Scroller.triggered = false;
		
	switch ( modeSelect )
	{
		case 0:
		LcdPrintApps( 0 );
		LcdPrintSensors( 1 );
		// update timer
		checkTimer( &Timer_Lcd_Cycle );
		Timer_Lcd_Cycle.triggered = true;
		break;
		case 1:
		// Alert Mode
		Timer_Lcd_Scroller.triggered = true;
		LcdPrintAlertState( alertCounter );
		break;
		case 2:
		// Calibration Mode
		LcdClearRow( 0 );
		LcdClearRow( 1 );
		selectOption = LcdPrintCalibOptions( selectOption );
		break;
		case 3:
		// Program Mode
		LcdClearRow( 0 );
		LcdClearRow( 1 );
		selectOption = LcdPrintProgramOptions( selectOption );
		break;
	}
}

void LcdReset ( void )
{
	// reset the nap counter so we'll stay awake
	napTimeCounter = 0;
	pinMode( PIN10_CS, OUTPUT );
	digitalWrite( PIN10_CS, HIGH);
	LcdClearRow( 1 );
	LcdClearRow( 0 );
	lcd.display();
}


/************************************************************************/
/*   LCD Shield Buttons use PIN_A0                                      */
/************************************************************************/
void read_LCD_buttons( void )
{               // read the buttons

	int adc_key_in  = 0;
	adc_key_in = analogRead(PIN_A0);       // read the value from the sensor

	// buttons read are centered at these values: 0, 144, 329, 504, 741
	// we add approx 50 to those values and check to see if we are close
	// We make this the 1st option for speed reasons since it will be the most likely result
	
	if (adc_key_in < 1000)
	{
		// stop the timers from continuing on
		Timer_Lcd.state = false;
		Timer_Lcd_Cycle.triggered = false;
		Timer_Lcd_Scroller.triggered = false;
		LcdReset();
		
		LcdButtonsDelay = millis();

		if ( DEBUG ) { Serial.println(adc_key_in); }
		
		// For V1.1 us this threshold
		/*
		if (adc_key_in < 50)   return btnRIGHT;
		if (adc_key_in < 250)  return btnUP;
		if (adc_key_in < 450)  return btnDOWN;
		if (adc_key_in < 650)  return btnLEFT;
		if (adc_key_in < 850)  return btnSELECT;
		*/
		// For V1.0 comment the other threshold and use the one below:
		if (adc_key_in < btnRIGHT)
		{
			switch (modeSelect)
			{
				case 0:
				selectMode++;
				selectMode = showSelectMode( selectMode );
				break;
				case 1:
				LcdPrintAlertState(0);
				break;
				case 2:
				selectOption++;
				//selectOption = LcdPrintCalibOptions( selectOption );
				LcdDisplay( modeSelect );
				break;
				case 3:
				selectOption++;
				//selectOption = LcdPrintProgramOptions( selectOption );
				LcdDisplay( modeSelect );
				break;
			}
		}
		else if (adc_key_in < btnUP)
		{
			LcdTimeRefresh();
			delay(1000);
			LcdReset();
			selectMode = 0;
			selectOption = 0;
			modeSelect = 0;
			LcdDisplay(0);
		}
		else if (adc_key_in < btnDOWN)
		{
			LcdReset();
			selectMode = 0;
			selectOption = 0;
			modeSelect = 0;
			lcd.noDisplay();
			pinMode( PIN10_CS, OUTPUT );
			digitalWrite( PIN10_CS, LOW);
		}
		else if (adc_key_in < btnLEFT)
		{
			selectMode = 0;
			selectOption = 0;
			modeSelect = 0;
			LcdDisplay( modeSelect );
		}
		else if (adc_key_in < btnSELECT)
		{
					if (DEBUG) { Serial.print("modeSelected: "); Serial.println(modeSelect); }
			switch (modeSelect)
			{
				case 0:
					modeSelect = selectMode;
					LcdDisplay( modeSelect );
					break;
				case 1:
					if ( DEBUG ) { printAlerts( &Alert_System ); }
					if ( clearOneAlert() == true ) { requestSync = 2; LcdClearRow( 1 ); lcd.print("  Alert Cleared!  "); }
					delay(1000);
					LcdDisplay( 0 );
					break;
				case 2:
					if ( DEBUG ) { printAppliance( &Appliance_FeedPump ); }
					break;
				case 3:
					switch (selectOption)
					{
						case 0:
						if (DEBUG) { Serial.print("selectedOption: "); Serial.println( selectOption ); }
						//selectOption++;
						if ( ( lcdOffered ) ) { saveAllDataObjects(); selectOption = 0; }
						lcdOffered = LcdOfferConfirm( lcdOffered );
						break;
						case 1:
						if (DEBUG) { Serial.print("selectedOption: "); Serial.println( selectOption ); }
						//selectOption++;
						if ( ( lcdOffered ) )
						{
							if ( applianceOverride() == true )
							{ 
								requestSync = 1; LcdClearRow( 0 ); lcd.print("Appliance"); LcdClearRow( 1 ); lcd.print(" Override!"); selectOption = 0;
							}
						}
						delay(1000);
						lcdOffered = LcdOfferConfirm( lcdOffered );
						break;
						case 2:
						if ( lcdOffered ) { wipeMemory(1); selectOption = 0; }
						lcdOffered = LcdOfferConfirm( lcdOffered );
						break;
						case 3:
						if (DEBUG) { Serial.print("selectedOption: "); Serial.println( selectOption ); }
						break;
					}
					break;
			} // end switch (modeSelect)
		} // end else if (SELECT)
	} // end if (adc_key < 1000)
}

bool checkAlerts( void )
{
	bool r = false;
	
	// Start at the beginning struct
	Alert *alert = &Alert_System;

	for (; alert != NULL; alert = alert->next ) {
		// TheNotifier returns true if an alert has been triggered
		// but only the initial trigger
		r = TheNotifier( alert );
	}
	return r;
}
