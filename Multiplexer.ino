#include <SoftwareSerial.h>
#include <AltSoftSerial.h>

#define dbg Serial    // use Serial for debug
#define esp Serial1   // use Serial1 for ESP8266
#define gps Serial2   // use Serial2 for GPS
#define wnd Serial3   // use Serial3 for AIS

SoftwareSerial ais(10, 11); // RX, TX

// AltSoftSerial always uses these pins:
//
// Board          Transmit  Receive   PWM Unusable
// -----          --------  -------   ------------
// Arduino Mega      46        48       44, 45
AltSoftSerial mar; 

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

  dbg.println("Start Multiplexer");
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
    dbg.write(" WND ");
    dbg.write(wndBuffer,wndBufferLength);
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
    dbg.write(" GPS ");
    dbg.write(gpsBuffer,gpsBufferLength);
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
    dbg.write(" AIS ");
    dbg.write(aisBuffer,aisBufferLength);
    aisBufferLength = 0;
  }
}

