#include <SPI.h>
#include <SD.h>

const int chipSelect = 10;


void setup() {
  
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  if (SD.begin(chipSelect)) {

    printRootDirectory();

    
    File logFile = SD.open("sample.log");

    long timeStamp1 = 0L;

    while (logFile.available()) {
      String timeStampString2 = logFile.readStringUntil('$');
      long timeStamp2 = timeStampString2.toInt();

      if(timeStamp2 > 0 ){
        delay(timeStamp2-timeStamp1);
        String nmeaSrting = logFile.readStringUntil('\n');
        Serial.println('$' + nmeaSrting);
        timeStamp1=timeStamp2;
      }
    }
    
    logFile.close();
  }
}



void printRootDirectory() {
  File dir = SD.open("/");
  
  while (true) {

    File entry =  dir.openNextFile();
    if (! entry) {
      // no more files
      break;
    }
    
    if (!entry.isDirectory()) {
      String fileName = entry.name();

      if(fileName.startsWith("LOG") & fileName.endsWith(".LOG"))
      {
        Serial.print(entry.name());
        Serial.print("\t\t");
        Serial.println(entry.size(), DEC);
      }
    }
    entry.close();
  }
}

void loop() {
  // put your main code here, to run repeatedly:

}
