/*
* @file Receiver_I2C_SD.ino
* Copyright (C) 2014 Cory J. Potter - All Rights Reserved
* You may use, distribute and modify this code under the
* terms of the LICENSE.txt
* NOT INTENDED FOR COMMERCIAL USE!
* You should have received a copy of the LICENSE.txt with
* this file. If not, please write to: <bitsandbots@gmail.com>
*/

/*
Arduino Networked Communications
Requires I2C connection with Arduino + nRF
*/

#include <Wire/Wire.h>
#include <I2C_Anything/I2C_Anything.h>
#include <Time/Time.h>
#include <commons.h>
#include <Clock.h>
#include <ClockSetter.h>
#include <SdFat.h>
//#include <TheTimeMan.h>
#include <DataObject.h>
#include <CoreConduit.h>
/*
* Simple data logger.
*/

// Interval between data records in milliseconds.
// The interval must be greater than the maximum SD write latency plus the
// time to acquire and write data to the SD to avoid overrun errors.
// Run the bench example to check the quality of your SD card.
// const uint32_t SAMPLE_INTERVAL_MS = 200;

char filename[13];
char dataEntry[50];

// Define STaTe MaCHiNe fLaGs
unsigned long simpleTimer = 30000UL;
// SD chip select pin.  Be sure to disable any other SPI devices such as Enet.
const uint8_t chipSelect = SS;
// File system object.
SdFat sd;
// Log file.
SdFile file;

//==============================================================================
// Error messages stored in flash.
#define error(msg) error_P(PSTR(msg))
//------------------------------------------------------------------------------
void error_P(const char* msg) {
	sd.errorHalt_P(msg);
}

//------------------------------------------------------------------------------


//==============================================================================
bool txSensorData( Sensor *sensor, const byte SEND_TO_ADDRESS );
bool txApplianceData( Appliance *app, const byte SEND_TO_ADDRESS );
void printAppliance(Appliance *app );
bool SyncSensor(Sensor *sensor);
bool SyncAppliance(Appliance *app);
// bool SyncAlert(Alert *alert);

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

const int MY_ADDRESS = 32;
const int SEND_TO_ADDRESS = 42;

void setup() {

	Serial.begin(19200);
	DEBUG = true;
	/************************************************************************/
	/* I2C Communications                                                   */
	/************************************************************************/
	Wire.onReceive ( receiveEvent );
	Wire.begin (MY_ADDRESS);
	/************************************************************************/

	setClock();
	
	
	// Initialize the SD card at SPI_HALF_SPEED to avoid bus errors with
	// breadboards.  use SPI_FULL_SPEED for better performance.
	if (!sd.begin(chipSelect, SPI_HALF_SPEED)) sd.initErrorHalt();
	
setFileName();
}

//------------------------------------------------------------------------------
void loop() {
time_millis = millis();

	if ( haveData > 0 ) { receiveData( haveData ); haveData = 0; simpleTimer = time_millis + 30000UL; }

// Is this delay necessary?
delay(100);

	if ( time_millis > simpleTimer )
	{
		simpleTimer = time_millis + 30000UL;
		// simplest interval check
		setFileName();
		setDataEntry();
		logData();
		keepAlive();
	}
	
}
//------------------------------------------------------------------------------
void setFileName( void )
{
	// Assemble the file string
	strncpy(filename, "LOG_0000.CSV", 13);
	filename[4] = ( month() / 10 ) + '0';
	filename[5] = ( month() % 10 ) + '0';
	filename[6] = ( day() / 10 ) + '0';
	filename[7] = ( day() % 10 ) + '0';

	Serial.print(F("Logging to: "));
	Serial.println(filename);
}
// Log a data record.
void logData() {
	
	String DataEntry = dataEntry;
	if (DEBUG)
	{
	Serial.print(dataEntry); Serial.print(" | "); Serial.println(DataEntry.length()); Serial.flush();
	}
	//  if (!file.open(fileName, O_CREAT | O_WRITE | O_EXCL)) error("file.open");
	if (!file.open(filename, O_CREAT | O_WRITE | O_APPEND)) error("file.open");
	// Write data to file.  Start with log time in micros.
	file.print(DataEntry);
	file.println();
	delay(500);


	// Force data to SD and update the directory entry to avoid data loss.
	if (!file.sync() || file.getWriteError()) error("write error");
	delay(500);
	// Close file and stop.
	file.close();

}
//------------------------------------------------------------------------------

void setDataEntry( void )
{

	// Print the current time of the day as follows to the current File:
	// "HH:MM:SS, "
	char buf[6];
	const char divider[2] = ":";
	const char boxSeparator[3] = ", ";

	// Assemble the directory string
	if ( ( hour() < 10 ) && ( hour() > 0 ) )
	{ strncpy(dataEntry, "0", 50); strcat(dataEntry, itoa(hour(),buf,10)); }
	else
	{ strncpy(dataEntry, itoa(hour(),buf,10), 50); }

	strcat(dataEntry, divider );
	if ( minute() < 10 ) { strcat(dataEntry, "0" ); }
	strcat(dataEntry, itoa(minute(),buf,10));
	strcat(dataEntry, divider );
	if ( second() < 10 ) { strcat(dataEntry, "0" ); }
	strcat(dataEntry, itoa(second(),buf,10));
	strcat(dataEntry, boxSeparator );
	//strcat(dataEntry, itoa(Sensor_Temp.value,buf,10));
	//4 is mininum width, 2 is precision; float value is copied onto buff
	strcat(dataEntry, dtostrf(Sensor_Temp.value, 4, 2, buf));
	strcat(dataEntry, boxSeparator);
	//strcat(dataEntry, itoa(Sensor_Humidity.value,buf,10));
	//4 is mininum width, 2 is precision; float value is copied onto buff
	strcat(dataEntry, dtostrf(Sensor_Humidity.value, 4, 2, buf));
	strcat(dataEntry, boxSeparator);
	strcat(dataEntry, itoa(Sensor_TankLevel.value,buf,10));
	strcat(dataEntry, boxSeparator );
	strcat(dataEntry, itoa(Sensor_Light.value,buf,10));
	strcat(dataEntry, boxSeparator );
	strcat(dataEntry, itoa(Appliance_ExhaustFan.type,buf,10));
	strcat(dataEntry, divider);
	strcat(dataEntry, itoa(Appliance_ExhaustFan.state,buf,10));
	strcat(dataEntry, boxSeparator);
	strcat(dataEntry, itoa(Appliance_IntakeFan.type,buf,10));
	strcat(dataEntry, divider);
	strcat(dataEntry, itoa(Appliance_IntakeFan.state,buf,10));
	strcat(dataEntry, boxSeparator );
	strcat(dataEntry, itoa(Appliance_FeedPump.type,buf,10));
	strcat(dataEntry, divider);
	strcat(dataEntry, itoa(Appliance_FeedPump.state,buf,10));

	//	return dataEntry;
}

