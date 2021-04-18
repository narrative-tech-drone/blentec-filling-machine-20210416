/*
  SD card file dump

  This example shows how to read a file from the SD card using the
  SD library and send it over the serial port.

  This example code is in the public domain.

*/

#include <SPI.h>
#include <SD.h>
// #include "Arduino_JSON.h"

// set up variables using the SD utility library functions:
Sd2Card card;
SdVolume volume;
SdFile root;

#define CSV_FILENAME "LOGDATA.CSV"

const int chipSelect = A7;
volatile bool error_sd = false;

void open_csv(char *filename_csv){
  File dataFile = SD.open(filename_csv);
  // if the file is available, write to it:
  long i = 0;
  if (dataFile) {
    while (dataFile.available()) {
      dataFile.read();
      i++;
    }
    Serial.print("loaded file: ");
    Serial.println(filename_csv);
    Serial.println(i);
    dataFile.close();
  } else {// if the file isn't open, pop up an error:
    Serial.println("can't open the file...");
  }
}

void SD_initialize() {
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  Serial.print("Initializing SD card...");

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    error_sd = true;
    return;
  }

  // print the type of card
  Serial.println();
  Serial.print("Card type:         ");
  switch (card.type()) {
    case SD_CARD_TYPE_SD1:
      Serial.println("SD1");
      break;
    case SD_CARD_TYPE_SD2:
      Serial.println("SD2");
      break;
    case SD_CARD_TYPE_SDHC:
      Serial.println("SDHC");
      break;
    default:
      Serial.println("Unknown");
  }

  Serial.println("card initialized.");

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
//  open_csv(CSV_FILENAME);

}

void SD_logging_data(int data){
  File dataFile = SD.open(CSV_FILENAME, FILE_WRITE);
  // if the file is available, write to it:
  if (dataFile) {
    DateTime now = rtc.now();

    dataFile.print(now.year(), DEC);
    dataFile.print('/');
    dataFile.print(now.month(), DEC);
    dataFile.print('/');
    dataFile.print(now.day(), DEC);
    dataFile.print(' ');
    dataFile.print(now.hour(), DEC);
    dataFile.print(':');
    dataFile.print(now.minute(), DEC);
    dataFile.print(':');
    dataFile.print(now.second(), DEC);
    dataFile.print(',');
    dataFile.print("Volume :");
    dataFile.print(',');
    dataFile.print(data, DEC);
    dataFile.println(',');

    dataFile.close();
    // print to the serial port too:
    Serial.print(data);
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening logfile");
    error_sd = true;
  }
}

void SD_logging_settingdata(int data){
  File dataFile = SD.open(CSV_FILENAME, FILE_WRITE);
  // if the file is available, write to it:
  if (dataFile) {
    DateTime now = rtc.now();

    dataFile.print(now.year(), DEC);
    dataFile.print('/');
    dataFile.print(now.month(), DEC);
    dataFile.print('/');
    dataFile.print(now.day(), DEC);
    dataFile.print(' ');
    dataFile.print(now.hour(), DEC);
    dataFile.print(':');
    dataFile.print(now.minute(), DEC);
    dataFile.print(':');
    dataFile.print(now.second(), DEC);
    dataFile.print(',');
    dataFile.print("Setting :");
    dataFile.print(',');
    dataFile.print(data, DEC);
    dataFile.println(',');

    dataFile.close();
    // print to the serial port too:
    Serial.print(data);
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening logfile");
    error_sd = true;
  }
}