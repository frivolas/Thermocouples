/***************************************************
  This is an example for the Adafruit Thermocouple Sensor w/MAX31855K

  Designed specifically to work with the Adafruit Thermocouple Sensor
  ----> https://www.adafruit.com/products/269

  These displays use SPI to communicate, 3 pins are required to
  interface
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
 ****************************************************/

/*
  SD card datalogger

  This example shows how to log data from three analog sensors
  to an SD card using the SD library.

  The circuit:
   analog sensors on analog ins 0, 1, and 2
   SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
 ** CS - pin 4

  created  24 Nov 2010
  modified 9 Apr 2012
  by Tom Igoe

  This example code is in the public domain.

*/


#include <SPI.h>
#include <SD.h>
#include "Adafruit_MAX31855.h"

const int chipSelect = 4;
unsigned long startMillis = 0;
unsigned long LONG_DELAY_MS = 60000; //1 min in ms
double timeStampSec = 0;
double timeStampMin = 0;

// Define pins to create three thermocouple instances with software SPI
// All thermocouples share the same DO and CLK pins and each one has a
// unique CS pin
#define ThermoDO   9
#define ThermoCLK  8

#define ThermoCS1   5 //Thermo 1 - Drive
#define ThermoCS2   6 //Thermo 2 - Duck
#define ThermoCS3   7 //Thermo 3 - Bend

// Initialize the Thermocouples
Adafruit_MAX31855 ThermoDrive(ThermoCLK, ThermoCS1, ThermoDO);
Adafruit_MAX31855 ThermoDuck(ThermoCLK, ThermoCS2, ThermoDO);
Adafruit_MAX31855 ThermoBend(ThermoCLK, ThermoCS3, ThermoDO);

// boolean to check the state of the thermos
boolean thermoFlag = false;

// check the health of the thermocouples
void checkThermos() {
  if (isnan(ThermoDrive.readCelsius())) {
    Serial.println("Something wrong with Drive thermocouple!");
    thermoFlag = true;
  }
  else if (isnan(ThermoDrive.readCelsius())) {
    Serial.println("Something wrong with Duck thermocouple!");
    thermoFlag = true;
  }
  else if (isnan(ThermoDrive.readCelsius())) {
    Serial.println("Something wrong with Bend thermocouple!");
    thermoFlag = true;
  } else {
    Serial.println("All thermocouples OK");
    thermoFlag = false;
  }
}

void setup() {
#ifndef ESP8266
  while (!Serial);     // will pause Zero, Leonardo, etc until serial console opens
#endif

  Serial.begin(9600);

  Serial.println("Start MAX31855");
  // wait for MAX chip to stabilize
  delay(500);
  Serial.print("Initializing SD card...");
  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }
  Serial.println("card initialized.");

  //check if the thermos are running
  checkThermos();
  if (thermoFlag) {
    Serial.println("Thermocouples fail!");
    //stop the show
    return;
  }

  //SAVE THE HEADER TO FILE
  //Initialize file
  File dataFile = SD.open("DIWLog.txt", FILE_WRITE);
  // if the file is available, write to it:
  if (dataFile) {
    String headerString = "Drive,Duck,Bend,Minutes,";
    dataFile.println(headerString);
    dataFile.close();
    // print to the serial port too:
    Serial.println(headerString);
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening log file");
    // and stop the show
    return;
  }
}


void loop() {
  //set current time
  startMillis = millis();

  // Define/reset the string for assembling the data to log:
  String dataString = "";

  // Thermocouple readouts in Celsius
  double tempDrive = ThermoDrive.readCelsius();
  double tempDuck = ThermoDuck.readCelsius();
  double tempBend = ThermoBend.readCelsius();

  timeStampSec = millis() / 1000; //sec
  timeStampMin = timeStampSec / 60.0; //min

  // Append data points to string to be saved
  // Add a "," between readings because it will  be a CSV
  // Drive motor temp
  dataString += String(tempDrive);
  dataString += ",";
  // Duck motor temp
  dataString += String(tempDuck);
  dataString += ",";  //will  be a CSV
  // Bend motor temp
  dataString += String(tempBend);
  dataString += ",";  //will  be a CSV

  dataString += timeStampMin; //Timestamp in minutes
  dataString += ",";  //will  be a CSV

  //SAVE THE DATA POINT TO FILE
  //Initialize file
  File dataFile = SD.open("DIWLog.txt", FILE_WRITE);
  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
    // print to the serial port too:
    Serial.println(dataString);
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening log file");
  }

  //Now wait for a minute (or as long as LONG_DELAY tells you)
  while (millis() - startMillis < LONG_DELAY_MS);

}



