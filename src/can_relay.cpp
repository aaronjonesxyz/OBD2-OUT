/**************************************************************************
  Electron Taming Service CAN-OUT Firmware

  MCP1515:
  Clock Crystal: 8MHz
  CS: PA11
  INT: PA2
  SI: PA7
  SO: PA6
  SCK: PA5

  SSD1306:
  SDA: PB15
  XCLK: PB13
  CS: PA0
  DC: PA1
  RES: PA8
 **************************************************************************/

#include "globals.h"
#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <CAN.h>
#include <OBD2.h>
#include <SD.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "rotEnc.h"
#include "outputController.h"
#include "logging.h"
#include "gauges.h"
#include "displayFuncs.h"
using namespace std;

uint8_t readSettings();
uint8_t saveSettings();
void pollOBD();

File file;
uint8_t settingsLoaded = 0;

void setup() {
  // Pin allocation for logic controlled outputs
  pinMode( OUTPUT_CONTROL_1, OUTPUT );
  pinMode( OUTPUT_CONTROL_2, OUTPUT );
  pinMode( OUTPUT_CONTROL_3, OUTPUT );
  pinMode( OUTPUT_CONTROL_4, OUTPUT );
  digitalWrite( OUTPUT_CONTROL_1, LOW );
  digitalWrite( OUTPUT_CONTROL_2, LOW );
  digitalWrite( OUTPUT_CONTROL_3, LOW );
  digitalWrite( OUTPUT_CONTROL_4, LOW );

  CAN.setPins( MCP2515_CS_PIN, MCP2515_INT_PIN );
  CAN.setClockFrequency( 8e6 ); // 8MHz Crystal

  // Init OLED display, SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC)) {
    Serial.println("SSD1306 allocation failed");
    for (;;); // loop forever if init fails
  }

  // Clear the display buffer for startup screen
  display.clearDisplay();

  display.setTextSize(0);
  display.setTextColor(WHITE);
  display.setCursor(35, 2);
  display.println("ETS CAN-OUT");
  display.display();

  display.setCursor(0, 15);
  display.print("OBD2 Init:");
  display.display();

  for ( int att = 0; att < 3; att++ ) { // 3 attempts to connect to OBD2 CANBUS
    if (!OBD2.begin()) {
      if ( att == 2 ) {
        display.print(" FAIL!");
        display.display();
        delay(200);
      }
    } else {
      OBDConnected = 1;
      display.print(" OK!");
      display.display();
      delay(200);
      break;
    }
  }

  display.setCursor(0, 30);
  display.print("SD Init:");
  display.display();
  if (!SD.begin(SDCARD_CS)) {
    display.print(" FAIL!");
    loadPIDs();
  } else {
    display.print(" OK!");
    display.display();
    display.setCursor(0, 45);
    display.print("Settings File:");
    display.display();
    switch ( readSettings() ){
    case 1:
      display.print(" OK!");
      settingsLoaded = 1;
      break;
    case 0:
      display.print(" FAIL!");
      break;
    case 3:
      display.print(" NEW");
      settingsLoaded = 1;
      break;
    }
  }
  display.display();

  delay(100);

  RotEnc.begin();

  logger.init();

  xyPlotter.begin();
}

/*======================================================== Main Loop ======================================================================*/

void loop() {
  for(;;) {
    if( RotEnc.getNextInput() == BUTTONPRESS ){
      menuSystem();
      saveSettings();
    }
    pollOBD();
    currentPIDValues[0] = random( -40, 100 );
    for( auto &c: settings.outControl ) {
      c.update();
    }
    if( ( millis() - updateTime ) > UPDATE_INTERVAL ) {
      updateTime = millis();
      ( settings.dataDisplayStyle == 0 ) ? xyPlotter.update() : textDisplay();
    }
    if( ( millis() - loggingTime ) > settings.loggingFreq ) {
      loggingTime = millis();
      logger.logEntry();
    }
  }
}

void pollOBD() {
  for ( int i = 0; i < masterPID_n; i++ ) {
    if( settings.supportedPIDs[i] ) {
      currentPIDValues[i] = OBD2.pidRead( masterPIDList[i] );
    }
  }
}

uint8_t readSettings() {
  if( !digitalRead( ROTENC_BUTTON ) && SD.exists( "settings.dat" ) ) {
    SD.remove( "settings.dat" );
  }
  if( SD.exists( "settings.dat" ) ){
    file = SD.open( "settings.dat" , O_RDWR);
    if(file){
      file.seek(0);
      char buf[18];
      file.readBytes( &settings.dataDisplayStyle, 1 );
      file.readBytes( settings.dataDisplayPIDs, 4 );
      file.readBytes( (byte*)&settings.graphRangeH, 4 );
      file.readBytes( (byte*)&settings.graphRangeL, 4 );
      file.readBytes( settings.supportedPIDs, masterPID_n );
      file.readBytes( (byte*)&settings.loggingFreq, 2 );
      for( auto& oC: settings.outControl ) {
        file.readBytes( buf, 18 );
        oC.name = buf;
        file.readBytes( &oC.outputPin, 1 );
        file.readBytes( &oC.PIDs[0], 1 );
        file.readBytes( &oC.PIDs[1], 1 );
        file.readBytes( &oC.PIDs[2], 1 );
        file.readBytes( &oC.minOnTime, 1 );
        file.readBytes( &oC.maxOnTime, 1 );
        for( auto& cC: oC.controlCase ) {
          file.readBytes( &cC.logic, 1 );
          for( auto& rO: cC.relOps ) {
            file.readBytes( &rO, 1 );
          }
          for( auto& cV: cC.compValues ) {
            file.readBytes( &cV, 1 );
          }
          file.readBytes( &cC.hysteresis, 1 );
        }
      }
      file.close();
      return 1;
    } else {
      return 0;
    }
  } else {
    loadPIDs();
    return saveSettings();
  }
}

uint8_t saveSettings() {
  uint8_t newFile = 0;
  if( !SD.exists( "settings.dat" ) ) {
    file = SD.open("settings.dat", FILE_WRITE);
    file.close();
    newFile = 1;
    loadPIDs();
  }
  file = SD.open("settings.dat", O_RDWR);
  if(file){
    file.seek(0);
    char buf[18];
    file.write( settings.dataDisplayStyle );
    file.write( settings.dataDisplayPIDs, 4 );
    file.write( (byte*)&settings.graphRangeH, 4 );
    file.write( (byte*)&settings.graphRangeL, 4 );
    file.write( settings.supportedPIDs, masterPID_n );
    file.write( (byte*)&settings.loggingFreq, 2 );
    //file.write( &logFreqH, 1 );
    for( auto& oC: settings.outControl ) {
      oC.name.toCharArray( buf, 18 );
      file.write( buf, 18 );
      file.write( &oC.outputPin, 1 );
      file.write( &oC.PIDs[0], 1 );
      file.write( &oC.PIDs[1], 1 );
      file.write( &oC.PIDs[2], 1 );
      file.write( &oC.minOnTime, 1 );
      file.write( &oC.maxOnTime, 1 );
      for( auto& cC: oC.controlCase ) {
        file.write( &cC.logic, 1 );
        for( auto& rO: cC.relOps ) {
          file.write( &rO, 1 );
        }
        for( auto& cV: cC.compValues ) {
          file.write( &cV, 1 );
        }
        file.write( &cC.hysteresis, 1 );
      }
    }
    file.close();
    return ( newFile ) ? 3 : 1;
  } else {
    return 0;
  }
}