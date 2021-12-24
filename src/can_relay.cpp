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

void readSettings();
void pollOBD();

void in1_ISR() {
  if( millis() - in1Debounce > 5 ) { RotEnc.input1Handler(); }
  in1Debounce = millis();
}

void in2_ISR() {
  if( millis() - in2Debounce > 5 ) { RotEnc.input2Handler(); }
  in2Debounce = millis();
}

void button_ISR() {
  if( millis() - buttonDebounce > 100 ) { RotEnc.buttonHandler(); }
  buttonDebounce = millis();
}

void setup() {
  Serial.begin(9600);

  for( auto &n : settings.outputNames ) n.reserve(18);

  pinMode( OUTPUT_CONTROL_1, OUTPUT );
  pinMode( OUTPUT_CONTROL_2, OUTPUT );
  pinMode( OUTPUT_CONTROL_3, OUTPUT );
  pinMode( OUTPUT_CONTROL_4, OUTPUT );
  digitalWrite( OUTPUT_CONTROL_1, LOW );
  digitalWrite( OUTPUT_CONTROL_2, LOW );
  digitalWrite( OUTPUT_CONTROL_3, LOW );
  digitalWrite( OUTPUT_CONTROL_4, LOW );

  pinMode( ROTENC_IN1, INPUT_PULLUP );
  pinMode( ROTENC_IN2, INPUT_PULLUP );
  pinMode( ROTENC_BUTTON, INPUT_PULLUP );

  CAN.setPins( MCP2515_CS_PIN, MCP2515_INT_PIN );
  CAN.setClockFrequency( 8e6 ); // 8MHz Crystal

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC)) {
    Serial.println("SSD1306 allocation failed");
    for (;;); // loop forever
  }

  // Clear the buffer
  display.clearDisplay();

  display.setTextSize(0);
  display.setTextColor(WHITE);
  display.setCursor(0, 29);
  display.print("     ETS CAN-OUT");
  display.display();

  delay(1000);

  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("OBD2.");
  display.display();

  

  for ( int att = 0; att < 3; att++ ) { // 3 attempts to connect to the OBD2 system
    if (!OBD2.begin()) {
      display.print(".");
      display.display();
      if ( att == 2 ) {
        display.print(" FAIL!");
        display.display();
      }
    } else {
      display.print(" OK!");
      display.display();
      delay(50);
      display.clearDisplay();
      display.setCursor(0, 0);
      display.print("Checking PIDs... /r/n");
      int i = 0;
      for (int pid = 0; pid < 96; pid++) {
        if( OBD2.pidSupported(pid) ){ supportedPIDs[i] = pid; i++; }
      }
      supportedPIDs_n = i;
      display.print( i );
      display.print( " PIDs found." );
      display.display();
      delay(100);
      break;
    }
  }

  display.clearDisplay();
  display.setCursor(0, 0);

  attachInterrupt( digitalPinToInterrupt( ROTENC_IN1 ), in1_ISR, CHANGE);
  attachInterrupt( digitalPinToInterrupt( ROTENC_IN2 ), in2_ISR, CHANGE);
  attachInterrupt( digitalPinToInterrupt( ROTENC_BUTTON ), button_ISR, FALLING);

  LoggerClass log();

  outputStateAlert( 2, 0 );
  outputStateAlert( 0, 1 );
  outputStateAlert( 1, 0 );

  activePIDs[2] = 15;

}

void loop() {
  settings.outputNames[0] = textEdit( settings.outputNames[0], "Output 1 Name" );
  Histogram hist( 2, -20, 80 );
  for(;;) {
    if( RotEnc.getNextInput() == BUTTONPRESS ){
      menuSystem();
      display.clearDisplay();
      display.display();
    }
    pollOBD();
    hist.update();
  }
  for( auto& c: outControl ) {
    c.update();
  }
}

void pollOBD() {
  for ( int i = 0; i < activePIDs_n; i++ ) {
    currentPIDValues[i] = OBD2.pidRead( activePIDs[i] );
  }
}