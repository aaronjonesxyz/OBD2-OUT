#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include <CAN.h>
#include <OBD2.h>
#include "logging.h"
#include "globals.h"


void LoggerClass::init(){ // Check if a counter file for this log exists, read and write new count
  if( SD.exists( "f.num" ) ){
    file = SD.open( "f.num" , O_RDWR);
    if(file){
      file.readBytes( &fnum, 1 );
      file.seek(0);
      fnum++;
      file.write(fnum);
      file.close();
    } else{
    }
  } else {
    file = SD.open("f.num", FILE_WRITE);
    if(file){
      file.write(fnum);
      file.close();
    } else {
    }
  }

  sprintf( filename, "SDLog%d.csv", fnum );
  file = SD.open(filename, FILE_WRITE);
  file.close();
  file = SD.open(filename, O_WRITE | O_CREAT);
  if(file){
    file.print("Time");
    for( int i = 0; i < masterPID_n; i++ ){
      if( settings.supportedPIDs[i] ) {
        file.print( "," );
        file.print( OBD2.pidName( masterPIDList[i] ) );
      }
    }
    file.print("\r\nSecs");
    for( int i = 0; i < masterPID_n; i++ ){
      if( settings.supportedPIDs[i] ) {
        file.print( "," );
        file.print( OBD2.pidUnits( masterPIDList[i] ) );
      }
    }
    file.print("\r\n");
    file.flush();
  } else {
  }
}

void LoggerClass::logEntry(){
  if(file){
    file.print( String( float( millis()/1000.000 ), 3 ) );
    for( int i = 0; i < masterPID_n; i++ ){
      if( settings.supportedPIDs[i] ) {
        file.print( "," );
        file.print( currentPIDValues[i] );
      }
    }
    file.print("\r\n");
    lines++;
    if( lines == 4 ) file.flush(), lines = 0;
  }
}