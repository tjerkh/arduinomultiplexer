#include <SoftwareSerial.h>
#include <AltSoftSerial.h>

#include <SPI.h>
#include <SD.h>

#define dbg Serial    // use Serial for debug
#define esp Serial1   // use Serial1 for ESP8266
#define gps Serial2   // use Serial2 for GPS
#define wnd Serial3   // use Serial3 for AIS

#define DEBUG 1

SoftwareSerial ais(10, 11); // RX, TX

// AltSoftSerial always uses these pins:
//
// Board          Transmit  Receive   PWM Unusable
// -----          --------  -------   ------------
// Arduino Mega      46        48       44, 45
AltSoftSerial mar; 

const int chipSelect = 4;
char filename[16];

char aisBuffer[100];
int aisBufferLength = 0;

char gpsBuffer[100];
int gpsBufferLength = 0;

char wndBuffer[100];
int wndBufferLength = 0;

void setup() {
  dbg.begin(38400);
  esp.begin(115200);
  gps.begin(9600);
  ais.begin(38400);
  mar.begin(4800);

  writeToLog("Start Multiplexer");

  initSdCard();
}

void loop() {
 char c;
 
  
  if (gps.available())
  {
    processGpsRead(gps.read());
  }

  if (ais.available())
  {
    processAisRead(ais.read());
  }

  if (wnd.available())
  {
    processWndRead(wnd.read());
  }
}

void processWndRead(char c)
{ 
  wndBuffer[gpsBufferLength] = c;
  wndBufferLength = wndBufferLength + 1;
  //if (c == 0x0D || c == 0x0A)
  if (c == 0x0A)
  { 
    esp.write(wndBuffer,wndBufferLength);
    writeToLog("WND", wndBuffer, wndBufferLength);
    wndBufferLength = 0;
  }
}

void processGpsRead(char c)
{ 
  gpsBuffer[gpsBufferLength] = c;
  gpsBufferLength = gpsBufferLength + 1;
  //if (c == 0x0D || c == 0x0A)
  if (c == 0x0A)
  { 
    esp.write(gpsBuffer,gpsBufferLength);
    mar.write(gpsBuffer,gpsBufferLength);
    writeToLog("GPS", gpsBuffer, gpsBufferLength);
    gpsBufferLength = 0;
  }
}

void processAisRead(char c)
{ 
  aisBuffer[aisBufferLength] = c;
  aisBufferLength = aisBufferLength + 1;
  //if (c == 0x0D || c == 0x0A)
  if (c == 0x0A)
  { 
    esp.write(aisBuffer,aisBufferLength);
    writeToLog("AIS", aisBuffer, aisBufferLength);
    aisBufferLength = 0;
  }
}

void writeToLog(String logMessage)
{
  #if DEBUG == 1
    dbg.print(millis());
    dbg.print("\t");
    dbg.println(logMessage);
  #endif

  File logFile = SD.open(filename, FILE_WRITE);
  logFile.print(millis());
  logFile.print("\t");
  logFile.println(logMessage);
  logFile.close();
}

void writeToLog(char source[3], char logBuffer[100], int logBufferLength)
{
  #if DEBUG == 1
    dbg.print(millis());
    dbg.print("\t");
    dbg.write(source);
    dbg.print("\t");
    dbg.write(logBuffer,logBufferLength);
  #endif

  File logFile = SD.open(filename, FILE_WRITE);
  logFile.print(millis());
  logFile.print("\t");
  logFile.write(source);
  logFile.print("\t");
  logFile.write(logBuffer,logBufferLength);
  logFile.close();
}

void initSdCard() {
  if (SD.begin(chipSelect)) {
    int n = 0;
    snprintf(filename, sizeof(filename), "log%03d.log", n); // includes a three-digit sequence number in the file name
    while(SD.exists(filename)) {
      n++;
      snprintf(filename, sizeof(filename), "log%03d.log", n);
    }
  Serial.println(filename);
  writeToLog(filename);
  }
}

