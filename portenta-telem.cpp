#include "Arduino.h"

//imports for the SD card writing

#include <SPI.h>
#include <SD.h>


const int cardPin = 10;


const int numChars = 24;
byte sendData[numChars];
byte readData[numChars];

struct BMSData {

  float voltage;
  float amperage;
  float highTemp;
  float lowTemp;
  float highVoltage;
  float lowVoltage;

};

struct MotorData {

  float odometer;
  float motorTemp;
  float sinkTemp;
  float mph;
  float motorCurrent;
};

struct ArrayData {

  float onePow;
  float twoPow;
  float threePow;
  float totPow;
  float oneTemp;
  float twoTemp;
  
};

struct CarData {
  float voltage;
  float amperage;
  float highTemp;
  float lowTemp;
  float highVoltage;
  float lowVoltage;
  float odometer;
  float motorTemp;
  float sinkTemp;
  float mph;
  float motorCurrent;

};


UART myUART0(PA_0,  PI_9,  NC, NC);  // TX, RX, RTS, CTC  NOTE: NC means not connected
UART myUART1(PA_9,  PA_10, NC, NC);   // pin D14 TX, pin D13 RX same as pre-defined Serial1
UART myUART2(PG_14, PG_9,  NC, NC);
UART myUART3(PJ_8,  PJ_9,  NC, NC);


void setup() {
  myUART0.begin(9600);
  myUART1.begin(9600);
  myUART2.begin(9600);
  myUART3.begin(9600);
  Serial.begin(9600);
  Serial.print("ready to go!");

  //SD card stuff
  if(!SD.begin(cardPin)){
    Serial.println("Card failed or not present :(");
    return;
  }
  Serial.println("Card initialized");

  File dataFile = SD.open("data.csv", FILE_WRITE);

  if(dataFile){
    dataFile.println("Time, voltage, amperage, highTemp, lowTemp, highVoltage, lowVoltage, mph, odomoter, motorTemp, heatsinkTemp, motorCurrent");
    dataFile.close();
    Serial.println("header written to csv");
  }
  else{
    Serial.println("error opening data.csv");
  }

}



bool bmsReceived = true; //will use these to check if we have received a new packet
bool motorReceived = true;
bool arrReceived = true;

String motor = "";
String bms = "";

void loop() {

  
  if(myUART0.available())
  {
    myUART0.readBytes(readData,24);

    BMSData packet1 = *(BMSData*) &readData;

    Serial.print(packet1.highVoltage);

    bmsReceived = true; //reset value to be true after receiving a packet

    bms = "," + String(packet1.voltage) + "," + String(packet1.amperage) + "," + String(packet1.highTemp) + "," + String(packet1.lowTemp) + "," + String(packet1.highVoltage) + "," + String(packet1.lowVoltage) + ",";

  }

  if(myUART1.available())
  {
    myUART1.readBytes(readData,24);

    MotorData packet1 = *(MotorData*) &readData;

    Serial.print(packet1.motorTemp);
    Serial.print(packet1.odometer); 

    motorReceived = true;

    motor = String(packet1.mph) + "," + String(packet1.odometer) + "," + String(packet1.motorTemp) + "," + String(packet1.sinkTemp) + "," + String(packet1.motorCurrent);
  }

  if(myUART2.available())
  {
    myUART2.readBytes(readData,24);

    ArrayData packet1 = *(ArrayData*) &readData;
    Serial.print(packet1.totPow);
    Serial.print("hi");

    arrReceived = true;
  }
  //SD Card Stuff
  if(arrReceived && motorReceived && bmsReceived){ //if all 3 are updated 
    unsigned long timestamp = millis(); //grab the time

    arrReceived = true; //will be false in production version
    motorReceived = false; //reset the values
    bmsReceived = false;
    String dataString = bms + motor;
    File dataFile = SD.open("data.csv", FILE_WRITE); //open the sd card

    if(dataFile){ //try to write to sd card
      dataFile.println(dataString);
      dataFile.close();
      Serial.println("Data writen to data.csv");
    }
    else{
      Serial.println("Error opening data.csv");
    }
  }
}
