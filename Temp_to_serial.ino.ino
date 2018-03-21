// Thermocouples to Serial
// By Oscar Frias (@_frix_) 2016
// www.OscarFrias.com
//
// Thermos-to-Serial reads an array of 6 thermocouples distributed along the DIWire Pro machine
// and sends the values via Serial comm to a Processing sketch that will plot them and save
// them as a CSV or TXT file. 
//
// This script replaces the old version that would save the data in an SD Card, since
// those SD Cards kept getting lost or damaged, or the files being overwritten.
//
// The Arduino first stablishes a handshake with Processing to make sure they're synchronized
// once the handshake has been established, then the data starts to flow to processing.
//
// This sketch is loaded on an Arduino Mega
//
// Thanks to Processing.org for the examples.


#include <SPI.h>
#include "Adafruit_MAX31855.h"

unsigned long startMillis = 0;
unsigned long LONG_DELAY_MS = 1000; //1 seg in ms

// Define pins to create three thermocouple instances with software SPI
// All thermocouples share the same DO and CLK pins and each one has a
// unique CS pin. Pin layout for Arduino Mega.
#define ThermoDO   40
#define ThermoCLK  41
// Machine Motors
#define ThermoCS1   30 //Thermo 1 - A (Bend)
#define ThermoCS2   31 //Thermo 2 - X (Feed)
#define ThermoCS3   32 //Thermo 3 - Z (Duck)
// Motors Drivers
#define ThermoCS4   33 //Thermo 4 - AD (Bend Driver)
#define ThermoCS5   34 //Thermo 5 - XD (Feed Driver)
#define ThermoCS6   35 //Thermo 6 - T (Ambient temp)

// Initialize the Thermocouples
// Machine Motors
Adafruit_MAX31855 ThermoA(ThermoCLK, ThermoCS1, ThermoDO);
Adafruit_MAX31855 ThermoX(ThermoCLK, ThermoCS2, ThermoDO);
Adafruit_MAX31855 ThermoZ(ThermoCLK, ThermoCS3, ThermoDO);
// Motor Drivers
Adafruit_MAX31855 ThermoAD(ThermoCLK, ThermoCS4, ThermoDO);
Adafruit_MAX31855 ThermoXD(ThermoCLK, ThermoCS5, ThermoDO);
Adafruit_MAX31855 ThermoT(ThermoCLK, ThermoCS6, ThermoDO);

// boolean to check the state of the thermos
boolean thermoFlag = false;

// Serial byte in
int inByte = 0;


void setup() {
  // Start the Serial
  Serial.begin(9600);
  // wait for MAX chips to stabilize
  delay(500);
  
  establishContact(); // Send handshake to Processing
  
  //check if the thermos are running
  checkThermos();
  if (thermoFlag) {
    Serial.write("F");
    //stop the show
    return;
  }

}


void loop() {
  if (Serial.available()> 0) {
    // get the new byte:
    inByte = Serial.read();
    if (inByte == 'R') {
      readThermos();
    }
}
}



// check the health of the thermocouples
void checkThermos() {
  if (isnan(ThermoA.readCelsius())) {
    Serial.println("Something wrong with Bend motor thermocouple!");
    thermoFlag = true;
  }
  else if (isnan(ThermoX.readCelsius())) {
    Serial.println("Something wrong with Feed motor thermocouple!");
    thermoFlag = true;
  }
  else if (isnan(ThermoZ.readCelsius())) {
    Serial.println("Something wrong with Duck motor thermocouple!");
    thermoFlag = true;
  }
  else if (isnan(ThermoAD.readCelsius())) {
    Serial.println("Something wrong with X motor driver thermocouple!");
    thermoFlag = true;
  }
  else if (isnan(ThermoXD.readCelsius())) {
    Serial.println("Something wrong with Y motor driver thermocouple!");
    thermoFlag = true;
  }
  else if (isnan(ThermoT.readCelsius())) {
    Serial.println("Something wrong with the Ambient temp thermocouple!");
    thermoFlag = true;
  }
  else {
    Serial.println("All thermocouples OK");
    thermoFlag = false;
  }
}



void readThermos(){
  // Thermocouple readouts in Celsius
  // Machine Motors
  double tempA = ThermoA.readCelsius();
  delay(10);
  double tempX = ThermoX.readCelsius();
  delay(10);
  double tempZ = ThermoZ.readCelsius();
  delay(10);
  // Motor Drivers
  double tempAD = ThermoAD.readCelsius();
  delay(10);
  double tempXD = ThermoXD.readCelsius();
  delay(10);
  // Ambient Temperature
  double tempT = ThermoT.readCelsius();
  delay(10);

/*
Serial.print("A ");
Serial.print(tempA);
Serial.print("| X");
Serial.print(tempX);
Serial.print("| Z");
Serial.print(tempZ);
Serial.print("| AD");
Serial.print(tempAD);
Serial.print("| XD");
Serial.print(tempXD);
Serial.print("| Box");
Serial.println(tempT);
*/

  // Send the readings to the Serial port
  // so that the processing sketch can do its thing
  Serial.write((byte)tempA);
  delay(10);
  Serial.write((byte)tempX);
  delay(10);
  Serial.write((byte)tempZ);
  delay(10);
  Serial.write((byte)tempAD);
  delay(10);
  Serial.write((byte)tempXD);
  delay(10);
  Serial.write((byte)tempT);

}


// Let's handshake with processing.
// Send a char until they reply
void establishContact(){
  while(Serial.available() <= 0){
    Serial.write("A");
    delay(250);
  }
}

