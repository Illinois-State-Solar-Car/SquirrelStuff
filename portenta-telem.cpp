#include "Arduino.h"

//imports for the SD card writing

#include "SDMMCBlockDevice.h"
#include "FATFileSystem.h"

SDMMCBlockDevice block_device;
mbed::FATFileSystem fs("fs");
FILE *fp = fopen("/fs/test.txt", "a");


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
  float onePow;
  float twoPow;
  float threePow;
  float totPow;
  float oneTemp;
  float twoTemp;

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

  int err =  fs.mount(&block_device);
  if (err) {
    // Reformat if we can't mount the filesystem
    // this should only happen on the first boot
    Serial.println("No filesystem found, formatting... ");
    fflush(stdout);
    err = fs.reformat(&block_device);
  }

  



  Serial.print("ready to go!");


  

}

bool bmsReceived = true; //will use these to check if we have received a new packet
bool motorReceived = true;
bool arrReceived = true;

CarData data;


  


void loop() {

  
  if(myUART0.available())
  {
    myUART0.readBytes(readData,24);

    BMSData packet1 = *(BMSData*) &readData;

    data.voltage = packet1.voltage;
    data.amperage = packet1.amperage;
    data.highTemp = packet1.highTemp;
    data.lowTemp = packet1.lowTemp;
    data.highVoltage = packet1.highVoltage;
    data.lowVoltage = packet1.lowVoltage;
    //Serial.print(packet1.highVoltage);

    bmsReceived = true; //reset value to be true after receiving a packet


  }

  if(myUART1.available())
  {
    myUART1.readBytes(readData,24);

    MotorData packet1 = *(MotorData*) &readData;

    data.odometer = packet1.odometer;
    data.motorTemp = packet1.motorTemp;
    data.sinkTemp = packet1.sinkTemp;
    data.mph = packet1.mph;
    data.motorCurrent = packet1.motorCurrent;

    //Serial.print(packet1.motorTemp);
    //Serial.print(packet1.odometer); 

    motorReceived = true;

  } 

  if(myUART2.available())
  {
    myUART2.readBytes(readData,24);

    ArrayData packet1 = *(ArrayData*) &readData;
    
    data.onePow = packet1.onePow;
    data.twoPow = packet1.twoPow;
    data.threePow = packet1.threePow;
    data.totPow = packet1.totPow;
    data.oneTemp = packet1.oneTemp;
    data.twoTemp = packet1.twoTemp;

    arrReceived = true;
  }
  //SD Card Stuff
  if(arrReceived && motorReceived && bmsReceived){ //if all 3 are updated 
    unsigned long timestamp = millis(); //grab the time

    arrReceived = false; //will be false in production version
    motorReceived = false; //reset the values
    bmsReceived = false;

    //myUART3.write();

    
    

   
    fprintf(fp,String(data.voltage).c_str());
    fprintf(fp,String(data.amperage).c_str());
    fprintf(fp,String(data.highTemp).c_str());
    fprintf(fp,String(data.lowTemp).c_str());
    fprintf(fp,String(data.odometer).c_str());
    fprintf(fp,String(data.motorTemp).c_str());
    fprintf(fp,String(data.sinkTemp).c_str());
    fprintf(fp,String(data.mph).c_str());
    fprintf(fp,String(data.motorCurrent).c_str());
    fprintf(fp,String(data.onePow).c_str());
    fprintf(fp,String(data.twoPow).c_str());
    fprintf(fp,String(data.threePow).c_str());
    fprintf(fp,String(data.oneTemp).c_str());
    fprintf(fp,String(data.twoTemp).c_str());
    fclose(fp);
    Serial.println("Data writen to data.csv");
  }
}
