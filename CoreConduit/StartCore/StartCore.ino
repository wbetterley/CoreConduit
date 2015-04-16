
#include <Wire.h>
#include <I2C_Anything.h>
#include <Time.h>
#include <commons.h>
#include <TheTimeMan.h>
// replaced DS1307 library in TheTimeMan.h
#include <SPI.h>
#include <CoreConduit.h>
#include <RCSwitch.h>

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

/************************************************************************/
/*	I2C Communications                                                  */
/************************************************************************/

const int MY_ADDRESS = 42;
const int SEND_TO_ADDRESS = 22;


/************************************
*               Setup              *
************************************/
RCSwitch mySwitch;
	uint8_t numCodes = 0;

void setup()
{
	//	pinMode(PIN1_TX, OUTPUT);
	pinMode(PIN2_INT0, OUTPUT);
	// 	pinMode(PIN2_INT0, OUTPUT);
	// 	pinMode(PIN3_INT1, OUTPUT);
	pinMode(PIN5, OUTPUT);
	
	// due to the configuration of the Prototyping Shield using PIN13_SCK for Rx ground
	// Remote Power Switch Transmitter on PIN11_MISO = Data PIN12_MOSI = Power, PIN13_SCK = Ground
	digitalWrite(PIN2_INT0, HIGH); //Pull High
	// 	digitalWrite(PIN2_INT0, HIGH); //Pull High
	// 	digitalWrite(PIN3_INT1, HIGH); //Pull High
	digitalWrite(PIN5, LOW); //Pull to ground


	isDebugMode(1);

	/************************************************************************/
	/* I2C Communications                                                   */
	/************************************************************************/
	Wire.begin (MY_ADDRESS);


	mySwitch = RCSwitch();

	mySwitch.enableReceive(1);  // Receiver on interrupt 0 => that is pin #2
	//	mySwitch.setReceiveTolerance(10);

	checkConsoleTime();

}

void loop()
{
	Serial.flush();
	rxSwitchCodes();
}

unsigned long previousValue = NULL;

void rxSwitchCodes( void )
{
	if ( mySwitch.available() )
	{
		unsigned long value = mySwitch.getReceivedValue();
		if ( value != previousValue )
		{
			if( numCodes == 0 )
			{

				/** ETEKCITY #1419
				unsigned long mySwitchOn[] = {24,333107,333251,333571,335107,341251};
				unsigned long mySwitchOff[] = {24,333116,333260,333580,335116,341260};
				**/
				if( ! previousValue )
				{
				Serial.print("mySwitch.setPulseLength(");
				Serial.print( mySwitch.getReceivedDelay() );
				Serial.println(");");
				Serial.print("unsigned long mySwitchOn[] = { "); 
				} else { Serial.print("unsigned long mySwitchOff[] = { "); }

				Serial.print( mySwitch.getReceivedBitlength() );
				Serial.print(",");
				numCodes++;
			} // end if numCodes == 0

			if (value == 0)
			{ Serial.print("Unknown encoding"); }
			else
			{
				Serial.print( mySwitch.getReceivedValue() );
				previousValue = value;
				if ( numCodes < 5 )
				{ Serial.print(","); numCodes++; }
				else { Serial.println(" };"); numCodes = 0; }
			}
		} // end ( value != previousValue )
		mySwitch.resetAvailable();
	} // end if( mySwitch.available() )
}
