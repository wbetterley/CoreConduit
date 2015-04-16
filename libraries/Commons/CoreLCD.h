/*
* @file CoreLCD.h
* Copyright (C) 2014 Cory J. Potter - All Rights Reserved
* You may use, distribute and modify this code under the
* terms of the LICENSE.txt
* NOT INTENDED FOR COMMERCIAL USE!
* You should have received a copy of the LICENSE.txt with
* this file. If not, please write to: <bitsandbots@gmail.com>
*/

#ifndef __CORELCD_H__
#define __CORELCD_H__

#ifdef ARDUINO

#define btnRIGHT	50
#define btnUP		195
#define btnDOWN		380
#define btnLEFT		555
#define btnSELECT	790
#define btnNONE		0

unsigned long LcdButtonsDelay = 0;
uint8_t lcdScrollerMessage;
Appliance *previousAppliance;
Alert *previousAlert;
uint8_t currentLetter = 0;
uint8_t alertCounter = 0;
uint8_t modeSelect = 0;
uint8_t selectMode = 0;
uint8_t selectOption = 0;
bool lcdOffered = false;
// defines for some useful symbols
const byte degreeSymbol = B11011111;
// const byte piSymbol     = B11110111;
// const byte centsSymbol  = B11101100;
// const byte sqrtSymbol   = B11101000;
// const byte omegaSymbol  = B11110100;  // the symbol used for ohms
// byte plantSymbol[8] = { B00000, B10001, B11011, B01110, B00100, B10101, B01110, B00100 };


//byte charCode = 32; // the first printable ascii character

const byte numRows = 2;
const byte numCols = 16;

char statusON[] = "ON";
char statusOFF[] = "OFF";
char statusYES[] = "YES";
char statusNO[] = "NO";
char statusXX[] = "XX";

/************************************************************************/
/*  LCD w/ BUTTON FOR BACKLIGHT											*/
/*  RTC + ISR + BASIC MULTI-THREADING SUPPORT							*/
/************************************************************************
LCD MODULE

* LCD RS pin
* LCD Enable
* LCD D4 pin
* LCD D5 pin
* LCD D6 pin
* LCD D7 pin
* LCD R/W pin to ground
* 10K resistor:
* ends to +5V and ground
* wiper to LCD VO pin (pin 3)

OR

LCD DFRobot Buttons Shield

Pin			Function
Analog 0	Button (select, up, right, down and left)
Digital 4	DB4
Digital 5	DB5
Digital 6	DB6
Digital 7	DB7
Digital 8	RS (Data or Signal Display Selection)
Digital 9	Enable
Digital 10	Backlit Control

OR

i2c/SPI LCD backpack

The circuit:
* 5V			to Arduino 5V pin
* GND			to Arduino GND pin
* SDA/CLK		to Analog #5 / Digital 13
* SCL/DAT/SI	to Analog #4 / Digital 12
* NC/SO			Not Connected / Digital 11

;The following pins connected from MCP23008 to LCD
;
;P0 - D4
;P1 - D5
;P2 - D6
;P3 - D7
;P4 - RS
;P5 - RW (not used, set to 0 to ground for write)
;P6 - Bl (backlight switch)
;P7 - E
*/
/************************************************************************/

// Start the LCD instance
/*** PINS 10=13 RESERVED FOR USING SD CARD  **/
//LiquidCrystal lcd(PIN9_RF_CE, PIN8, PIN4,PIN5,PIN6,PIN7);
//LiquidCrystal lcd(PIN8,PIN9_RF_CE,PIN4,PIN5,PIN6,PIN7);
// Connect via I2C, default address #0 (A0-A2 not jumpered)
// LiquidCrystal lcd(0);
// Connect via SPI.
// LiquidCrystal lcd(PIN12_MOSI, PIN13_SCK, PIN11_MISO);


/************************************************************************/
/* LCD Functions		                                                */
/************************************************************************/

// Start the LCD instance
/*** PINS 10=13 RESERVED FOR USING SD CARD  **/
//LiquidCrystal lcd(PIN9_RF_CE, PIN8, PIN4,PIN5,PIN6,PIN7);
LiquidCrystal lcd(PIN8,PIN9_RF_CE,PIN4,PIN5,PIN6,PIN7);
// Connect via I2C, default address #0 (A0-A2 not jumpered)
// LiquidCrystal lcd(0);
// Connect via SPI.
// LiquidCrystal lcd(PIN12_MOSI, PIN13_SCK, PIN11_MISO);


char* displayStatus(Appliance *app)
{
	if ( (app->state == true) )
	{
		return statusON;
		} else {
		return statusOFF;
	}
}

char* displayAlertStatus(Alert *a)
{
	if ( (a->state == true ) )
	{
		if ( (a->triggered == true ) )
		{
			return statusYES;
			} else {
			return statusNO;
		}
		} else {
		return statusXX;
	}
}

void LcdClearRow(uint8_t row)
{
	for ( uint8_t col = 0; col < numCols ; col++)
	{
		lcd.setCursor(col, row);
		lcd.print(" ");
	}

	lcd.setCursor(0, row);
}


/************************************************************************/
/* LCD Time Functions                                                   */
/************************************************************************/

void LcdPrintInt( float num )
{
	// Convert a float between 0-99 to an int and display
	// it to a designated LCD segment
	char buf[3];
	lcd.print(itoa( ( (int)num/10),buf,10) );
	lcd.print(itoa( ( (int)num%10),buf,10) );
}

void LcdPrintAMPM( boolean PM )
{
	// If PM is true, display "P" else display "A"
	if( PM == true )
	{
		lcd.print( "P"  );
	}

	else
	{
		lcd.print("A" );
	}

	lcd.print( "M" );
}

void LcdTimeRefresh() {
	// Get the current hour and minute
	uint8_t displayHour = hour();
	uint8_t displayMinute = minute();
	// PM indicator, 0-AM ; 1-PM
	uint8_t PM = 0;

	// If it is past noon, it is PM and subtract 12 from the display hour
	if ( displayHour > 12 )
	{
		PM = 1;
		displayHour = displayHour - 12;
	}

	// if it is midnight display 12 as the display hour and it is AM
	if ( displayHour == 0 )
	{
		displayHour = 12;
		PM = 0;
	}

	// If it is noon it is PM
	if( hour() == 12 ) PM = 1;

	// If displayHour is more or equal to 10, format the time on
	// the screen to fit the extra integer, Example: "HH:MMXM" where XM= AM/PM
	if(displayHour <= 9)
	{
		LcdPrintInt( displayHour );
		lcd.print( ":" );
		LcdPrintInt( displayMinute );
		LcdPrintAMPM( PM );
	}

	// If displayHour is not more or equal to 10, display the time
	// accordingly, Example: "H:MMXM" where XM = AM/PM
	else
	{
		LcdPrintInt( displayHour );
		lcd.print( ":" );
		LcdPrintInt( displayMinute );
		LcdPrintAMPM( PM );
	}

}

/************************************************************************/
/* Sensors		                                                        */
/************************************************************************/

void LcdPrintSensors( uint8_t row ) {
	uint8_t col = 2;
	LcdClearRow( row );
	lcd.setCursor(col, row);
	LcdPrintInt( Sensor_Temp.value );
	lcd.write(degreeSymbol);
	col = col + 4;
	lcd.setCursor(col, row);
	LcdPrintInt( Sensor_Humidity.value );
	lcd.print("%");
	col = col + 4;
	lcd.setCursor(col, row);
	LcdPrintInt( Sensor_TankLevel.value );
}

uint8_t showSelectMode ( uint8_t selectMode )
{
	LcdClearRow(0);
	lcd.print("Select Mode:");
	LcdClearRow(1);
	lcd.print("  ");

	if ( selectMode > 3 ) { selectMode = 0; }
	
	switch ( selectMode ) {
		case 0:
		lcd.print("Observation");
		break;
		case 1:
		lcd.print("Alert");
		break;
		case 2:
		lcd.print("Calibrate");
		break;
		case 3:
		lcd.print("Program");
		break;
	}
	return selectMode;
}

void LcdPrintAlertName ( Alert * a )
{
	switch ( a->type ) {
		case ALERT_TEMP: /* Temperature out-of-range */
		lcd.print("Temperature");
		break;
		case ALERT_FANS: /* Temperature doesn't drop after the fans are activated. */
		lcd.print("Ventilation");
		break;
		case ALERT_HUMID: /* Humidity out-of-range */
		lcd.print("Humidity");
		break;
		case ALERT_LIGHT: /* Luminescence too low OR oN/oFF not as scheduled */
		lcd.print("Luminescence");
		break;
		case ALERT_FLOAT: /* Water level too low */
		lcd.print("Float");
		break;
		case ALERT_MOISTURE: /* Soil Moisture remaining - too soggy, too dry */
		lcd.print("Moisture");
		break;
		case ALERT_CHANGE: /*  Time to add/remove nutrient solution */
		lcd.print("Change");
		break;
		case ALERT_SYSTEM:
		lcd.print("System");
		break;
	}
}


uint8_t LcdPrintCalibOptions( uint8_t selectOption )
{
	LcdClearRow( 1 );
	LcdClearRow( 0 );

	if ( selectOption > 4 ) { selectOption = 0; }
	
	switch ( selectOption ) {
		case 0:
		lcd.print("Light:  ");
		lcd.print( Sensor_Light.value );
		break;
		case 1:
		lcd.print("Moisture:  ");
		lcd.print( Sensor_Moisture.value );
		break;
		case 2:
		lcd.print("Float: ");
		lcd.print( Sensor_FeedFloat.value );
		break;
		case 3:
		lcd.print("TankLevel: ");
		lcd.print( Sensor_TankLevel.value );
		break;
		case 4:
		lcd.print("Humidity:  ");
		lcd.print( Sensor_Humidity.value );
		break;
		case 5:
		lcd.print("Temp F:  ");
		lcd.print( Sensor_Temp.value );
		break;
	}
	return selectOption;
}

uint8_t LcdPrintProgramOptions( uint8_t selectOption )
{
	LcdClearRow( 1 );
	LcdClearRow( 0 );

	if ( selectOption > 3 ) { selectOption = 0; }
	
	switch ( selectOption ) {
		case 0:
		lcd.print("Make Memories");
		break;
		case 1:
		lcd.print("Remote Outlets");
		break;
		case 2:
		lcd.print("System Reset");
		break;
		case 3:
		lcd.print(" ");
		break;
	}
	return selectOption;
}



/************************************************************************/
/* Applications                                                         */
/************************************************************************/
void LcdPrintAppName ( Appliance * a )
{
	
	switch (a->type) {
		case APPLIANCE_UNKNOWN:
		lcd.print("Unknown");
		break;
		case APPLIANCE_LIGHT:
		lcd.print("Light");
		break;
		case APPLIANCE_CIRCULATION_FAN:
		lcd.print("Fan");
		break;
		case APPLIANCE_INTAKE_FAN:
		lcd.print("Intake");
		break;
		case APPLIANCE_EXHAUST_FAN:
		lcd.print("Exhaust");
		break;
		case APPLIANCE_PUMP:
		lcd.print("Pump");
		break;
		case APPLIANCE_HUMIDIFIER:
		lcd.print("Humid");
		break;
		case APPLIANCE_HEATER:
		lcd.print("Heater");
		break;
	}
}

bool LcdPrintApps( uint8_t row ) {
	bool r = true;
	Appliance * a = previousAppliance;
	if (a == NULL) { a = &Appliance_FeedPump; }
	//for (; a != NULL; a = a->next ) {
	LcdClearRow( row );
	LcdPrintAppName( a );
	lcd.print(" is ");
	lcd.print(displayStatus(a));

	previousAppliance = a->next;
	if( previousAppliance == NULL ) { r = false; }

	return r;
}

int LcdScroller ( unsigned int currentLetter, uint8_t type )
{
	uint8_t col = 0;
	uint8_t row = 1;
	lcd.setCursor( col , row );

	if (  (strlen(alert_message[type]) - currentLetter) < 15  ) { currentLetter = 0; }
	if ( currentLetter < strlen(alert_message[type]) ) {
		//		   	LcdClearRow( 1 );
		lcd.print( &alert_message[type][currentLetter] );
		lcd.print(" ");
		//	if ( DEBUG ) {
		// 			Serial.print(" message:  ");
		// 			Serial.print( &message[currentLetter] );
		// 			Serial.println(""); }
		currentLetter++;
	}
	
	return currentLetter;
}

bool LcdOfferConfirm ( bool offered )
{
	//	if ( !offered ) {
	if ( offered == false ) {
		LcdClearRow( 0 );
		lcd.print("Are you sure?");
		LcdClearRow( 1 );
		offered = true;
	} else {
		offered = false;
	}
	return offered;
}

bool LcdPrintAlertState( uint8_t numAlerts )
{
	bool r = true;
	Alert * a = previousAlert;
	if ( a == NULL ) { a = &Alert_System; }

	if ( numAlerts > 0 ) {
		for (a = &Alert_System; a != NULL; a = a->next ) {
			if ( a->triggered ) { break; }
		}
	}

	LcdClearRow( 0 );
	if ( DEBUG ) { Serial.print(F("alert.type: ")); Serial.println( (uint8_t) a->type); }
	LcdPrintAlertName( a );

	LcdClearRow( 1 );
	
	if (a->triggered)
	{ 
		lcdScrollerMessage = a->type;
		} else {
		lcd.print("triggered? ");
		lcd.print( displayAlertStatus( a ) );
	}
	previousAlert = a->next;
	if( previousAlert == NULL ) { r = false; }

	return r;
}



#else
#error This code is only for use on Arduino.
#endif // ARDUINO

#endif // __CORELCD_H__
