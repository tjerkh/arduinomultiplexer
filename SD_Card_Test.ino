
#include <SPI.h>
#include <SD.h>

const int chipSelect = 10;
char filename[16];

void setup() {

  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  initSdCard();

}

void loop() {
  File logFile = SD.open(filename, FILE_WRITE);
  logFile.println(millis());
  Serial.println(millis());
  logFile.close();
  delay(2000); 
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
  }
}

