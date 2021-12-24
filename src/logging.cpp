#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include <CAN.h>
#include <OBD2.h>
#include "logging.h"
#include "globals.h"


LoggerClass::LoggerClass( ){ // Check if a counter file for this log exists, read and write new count

  if (!SD.begin(SDCARD_CS)) {
  Serial.println("Card failed, or not present");
  }
  Serial.println("Card initialized.");
  if( SD.exists( "f.num" ) ){
    Serial.println("Count file found.");
    file = SD.open( "f.num" , O_RDWR);
    if(file){
      fnum = file.read();
      file.seek(0);
      fnum++;
      file.write(fnum);
      file.close();
    } else{
      Serial.println("File I/O error!");
    }
  } else {
    file = SD.open("f.num", FILE_WRITE);
    if(file){
      file.write("0");
      file.close();
    } else {
      Serial.println("File I/O error!");
    }
  }

  sprintf( filename, "SDLog%d.csv", fnum );
  file = SD.open(filename, FILE_WRITE);
  Serial.println(file);
  if(file){
    file.print("Time");
    for( int i = 0; i < activePIDs_n; i++ ){
      file.print( "," );
      file.print( OBD2.pidName( activePIDs[i] ) );
    }
    file.print("/r/nSecs");
    for( int i = 0; i < activePIDs_n; i++ ){
      file.print( "," );
      file.print( OBD2.pidUnits( activePIDs[i] ) );
    }
    file.close();
  } else {
    Serial.println("File I/O Error!");
  }
}

int LoggerClass::logEntry( int curPIDVals[20] ){
  file = SD.open(filename, FILE_WRITE);
  if(file){
    float time = millis() / 1000;
    file.printf( "%.3f", time );
    for( int i = 0; i < activePIDs_n; i++ ){
      file.print( "," );
      file.print( curPIDVals[i] );
    }
    file.close();
    return 1;
  } else {
    return 0;
  }
}