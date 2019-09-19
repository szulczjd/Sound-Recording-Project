/*
  SD card datalogger
 
 This example shows how to log data from sound mic and record on SD card 
 If using Arduino as programmer 
   RX -> PIN0 to MINI RXI
   TX <- PIN1 to MINI TXO
   Reset to MINI DTR
 
 The circuit:
 * Mic output = analog pins 0
 * 
 * SD card attached - SPI bus as follows:
 ** MOSI - pin 11 MOSI
 ** MISO  - pin 12 MISO
 ** SCK - pin 13 SCK on mini
 ** CS - pin 4 uno nano or 10 pro mini
 * 
 * RTC brakout pins to I2C
 * SDA to A4 or SDA on mini
 * SCL to A5 or SCL on mini
 

 	 
 */

#include <SPI.h> //SPI library
#include <SD.h>  //Memory Card Library
#include "U8glib.h" //OLED Library
#include <Wire.h> //Wire Library used by I2C devices

#include "DS3231.h"

RTClib RTC;

U8GLIB_SH1106_128X64 u8g(U8G_I2C_OPT_NONE);  // I2C / TWI //OLED constructor
// used as the CS pin, the hardware CS pin (4 on most Nano boards SS pin on mini),

const int CardchipSelect = 10;

int analogPin = A0;  //for microphone output
String dataString = ""; //used to pass total constructed string of Microphone and date data
String sensorString = "";//to convert Microphone output to string 
String datestring =""; //to convert rtc number to date string
unsigned long delayinterval = 10000; //sampling delay

                                                                                                                                                                                                  

  
void setup()
{
Serial.begin(9600);
Wire.begin();
CheckCard();   //checks for card present

}

void loop()
{ 
unsigned int sensor; //Microphone variable
unsigned int sensordif = 0; //microphone output high low signals

    datedata(); //date and time function
    
   //determines low and high mic (sensor) readings
    
  unsigned int highsensor = 0; //initializes high raeadiing from microphoone
  unsigned int lowsensor = 100;//initializes low raeadiing from microphoone  
 //   Serial.println("start loop"); // for debugging
    for(int i = 0; i<=100; i++) //loop to read microphone output and determine high low readings
    {
    delay(100);
//    Serial.print("i = ");// for debugging
//    Serial.print(i);// for debugging

  
    sensor = analogRead(analogPin);
    sensor = sensor * 100/1024;

   //Serial.print("  sensor = ");// for debugging

//   Serial.println(sensor); // for debugging
    if (sensor  > highsensor)
    {
      highsensor = sensor;
//      Serial.print("highsensor = ");
//      Serial.println(highsensor);   
    }
    if (sensor < lowsensor)
    { 
      lowsensor = sensor;
//      Serial.print("lowsensor = ");// for debugging
//      Serial.println(lowsensor);// for debugging
    }

    
    }
    sensordif=highsensor - lowsensor;
//  Serial.println(sensordif);// for debugging
    sensorString = String(sensordif);
    
    dataString = String("sensor difference= " + sensorString + " datetime= " + datestring);
   //card write function
    writedata();
    // print to the serial port
    Serial.println(dataString);
     // picture draw function
    draw(sensordif); //passing high-low sensor difference to draw function
 
    delay(delayinterval);
    
 
  } 

 

  void CheckCard() //Checks SD card
{
 Serial.print("Initializing SD card...");
  // make sure that the default chip select pin is set to
  // output, even if you don't use it:
  pinMode(CardchipSelect, OUTPUT);
  
  // see if the card is present and can be initialized begins by checking notSD.begin = true if false 
  // card is OK use use
  if (!SD.begin(CardchipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
}
  Serial.println("card initialized.");
}

//creates datetime data string 
int datedata(){
    
// c[0] = year, c[1] = month, c[2] = day, c[3] = hour c[4] = min
    String s[] = {" "," "," "," "," "," "};
    
    DateTime now = RTC.now();
    int c[]= {0,0,0,0,0};
    c[0] = now.year();
    c[1] = now.month();
    c[2] = now.day();
    c[3] = now.hour();
    c[4] = now.minute();
 
    for (int i=0; i<=4; i++)
    { 
//    Serial.print(i);
//    Serial.print(" "); 
//    Serial.print(c[i], DEC);
    s[i]=String (c[i], DEC);
//    Serial.print(" ");
//    Serial.print(s[i]);
//    Serial.print(" ");
    
    }
    datestring = s[0] + s[1] + s[2] + s[3] + s[4];
//    Serial.println();
//    Serial.println(datestring);
//    Serial.println();
  
  
}
//SD card data write
  void writedata()
{

 File dataFile = SD.open("datalog.txt", FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
  
    
  } 
 
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening datalog.txt");
  }
   digitalWrite(CardchipSelect, HIGH);
}

void draw(int sensordif1) //now calling microphone high-low difference "sensordif1"
{
   // Serial.println(sensordif1);//for debugging
    DateTime now = RTC.now();
    int d[]= {0,0,0,0,0};
    d[0] = now.year();
    d[1] = now.month();
    d[2] = now.day();
    d[3] = now.hour();
    d[4] = now.minute();
    u8g.firstPage();
    
  do {
  // graphic commands to redraw the complete screen should be placed here 
  u8g.setFont(u8g_font_unifont);
  //u8g.setFont(u8g_font_osb21);

  enum{BufSize=3};
  char buf[BufSize];
  u8g.drawStr(0,10,"sen");
  snprintf (buf, BufSize, "%d" ,sensordif1);
  u8g.drawStr(30,10,buf);
  char buf1[1];
   u8g.drawStr(0,20,"mon");
   sprintf (buf1,"%d" ,d[1]);
   u8g.drawStr(30,20,buf1);
   u8g.drawStr(0,30,"day");
   char buf2[1];
   sprintf (buf2,"%d" ,d[2]);
   u8g.drawStr(30,30,buf2);
   char buf3[1];
   u8g.drawStr(0,40,"hr");
   sprintf (buf3,"%d" ,d[3]);
   u8g.drawStr(30,40,buf3);  
   char buf4[1];
   u8g.drawStr(0,50,"min");
   sprintf (buf4,"%d" ,d[4]);
   u8g.drawStr(30,50,buf4);  
   } while( u8g.nextPage() );
  
  // rebuild the picture after some delay
  delay(100);
}
