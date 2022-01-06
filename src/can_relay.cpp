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

  delay(200);

  RotEnc.begin();

  logger.init();

  xyPlotter.begin();
}

void loop() {
  for(;;) {
    if( RotEnc.getNextInput() == BUTTONPRESS ){
      menuSystem();
      saveSettings();
    }
    pollOBD();
    currentPIDValues[0] = random( -40, 100 );
    currentPIDValues[1] = random( -40, 100 );
    if( ( millis() - updateTime ) > UPDATE_INTERVAL ) {
      updateTime = millis();
      ( settings.dataDisplayStyle == 0 ) ? xyPlotter.update() : textDisplay();
    }
    for( auto &c: settings.outControl ) {
      c.update();
    }
    logger.logEntry();
  }
}

void pollOBD() {
  for ( int i = 0; i < settings.activePIDs_n; i++ ) {
    currentPIDValues[i] = OBD2.pidRead( masterPIDList[settings.activePIDs[i]] );
  }
}

uint8_t readSettings() {
  Serial.println("Card initialized.");
  if( SD.exists( "settings.dat" ) ){
    file = SD.open( "settings.dat" , O_RDWR);
    if(file){
      file.seek(0);
      char buf[18];
      file.readBytes( &settings.dataDisplayStyle, 1 );
      file.readBytes( settings.supportedPIDs, masterPID_n );
      file.readBytes( &settings.supportedPIDs_n, 1 );
      file.readBytes( settings.activePIDs, 20 );
      file.readBytes( &settings.activePIDs_n, 1 );
      uint8_t logFreqH, logFreqL;
      file.readBytes( &logFreqL, 1 );
      file.readBytes( &logFreqH, 1 );
      settings.loggingFreq = logFreqL | ( logFreqH << 8 );
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
    file.write( settings.supportedPIDs, masterPID_n );
    file.write( &settings.supportedPIDs_n, 1 );
    file.write( settings.activePIDs, 20 );
    file.write( &settings.activePIDs_n, 1 );
    uint8_t logFreqH, logFreqL;
    logFreqL = settings.loggingFreq & 0xFF;
    logFreqH = ( settings.loggingFreq >> 8 ) & 0xFF;
    file.write( &logFreqL, 1 );
    file.write( &logFreqH, 1 );
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