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

const int debugButton = 22; 
const int logLedWrite = 23;
const int aisLedRead  = 24;
const int gpsLedRead  = 26;
const int wndLedRead  = 28;
const int marLedWrite = 31;
const int espLedWrite = 33;

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

  pinMode(debugButton, INPUT);
  pinMode(logLedWrite, OUTPUT);
  pinMode(aisLedRead, OUTPUT);
  pinMode(gpsLedRead, OUTPUT);
  pinMode(wndLedRead, OUTPUT);
  pinMode(marLedWrite, OUTPUT);
  pinMode(espLedWrite, OUTPUT);

  toggleAllLeds(HIGH);
  
  writeToLog("Start Multiplexer");

  initSdCard();

  toggleAllLeds(LOW);
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
    processWndRead();
  }
}

void processWndRead()
{ 
  digitalWrite(wndLedRead, HIGH);
  char c = wnd.read();
  wndBuffer[gpsBufferLength] = c;
  wndBufferLength = wndBufferLength + 1;
  digitalWrite(wndLedRead, LOW);
  //if (c == 0x0D || c == 0x0A)
  if (c == 0x0A)
  { 
    espWrite(wndBuffer, wndBufferLength);
    writeToLog("WND", wndBuffer, wndBufferLength);
    wndBufferLength = 0;
  }
}

void espWrite(char espBuffer[100], int espBufferLength)
{
    digitalWrite(espLedWrite, HIGH);
    esp.write(espBuffer,espBufferLength);
    digitalWrite(espLedWrite, LOW);
}

void marWrite(char marBuffer[100], int marBufferLength)
{
    digitalWrite(marLedWrite, HIGH);
    esp.write(marBuffer,marBufferLength);
    digitalWrite(marLedWrite, LOW);
}

void processGpsRead(char c)
{ 
  digitalWrite(gpsLedRead, HIGH);
  gpsBuffer[gpsBufferLength] = c;
  gpsBufferLength = gpsBufferLength + 1;
  digitalWrite(gpsLedRead, LOW);
  //if (c == 0x0D || c == 0x0A)
  if (c == 0x0A)
  { 
    espWrite(gpsBuffer, gpsBufferLength);
    marWrite(gpsBuffer, gpsBufferLength);
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
    espWrite(aisBuffer, aisBufferLength);
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

  if(digitalRead(debugButton) == HIGH)
  {
    digitalWrite(logLedWrite, HIGH);
    File logFile = SD.open(filename, FILE_WRITE);
    logFile.print(millis());
    logFile.print("\t");
    logFile.println(logMessage);
    logFile.close();
    digitalWrite(logLedWrite, LOW);
  }
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

  if(digitalRead(debugButton) == HIGH)
  {
    digitalWrite(logLedWrite, HIGH);
    File logFile = SD.open(filename, FILE_WRITE);
    logFile.print(millis());
    logFile.print("\t");
    logFile.write(source);
    logFile.print("\t");
    logFile.write(logBuffer,logBufferLength);
    logFile.close();
    digitalWrite(logLedWrite, LOW);
  }
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

void toggleAllLeds(boolean state){
  digitalWrite(logLedWrite, state);
  digitalWrite(aisLedRead, state);
  digitalWrite(gpsLedRead, state);
  digitalWrite(wndLedRead, state);
  digitalWrite(marLedWrite, state);
  digitalWrite(espLedWrite, state);
}


