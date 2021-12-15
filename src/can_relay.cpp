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
#include "outputController.h"
#include "RotEnc.h"
#include "logging.h"
#include "gauges.h"
using namespace std;

void readSettings();
void pollOBD();

SPIClass SPI_2(PB15, PB14, PB13);

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT,
                         &SPI_2, OLED_DC, OLED_RESET, OLED_CS);

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

const char * mainMenuLabels[] = { "Data Display", "Logging", "Output Control" };
const char * gaugeMenuLabels[] = { "Style", "Data" };
const char * loggingMenuLabels[] = { "Frequency", "Data" };

int PIDSelector( int pids[96], int numActivePids ) { // Displays the supplied list of PIDs in a list with a selection cursor
  int cursor = 0;

  while(1) {
    display.clearDisplay();
    display.setCursor(0,24);
    display.print("-");
    switch( cursor ) {
      case 0:
        if( numActivePids ) {
          for( int i = 0; i < 5; i++ ){
            if( numActivePids >= i ){
              int textCursor = 24 + ( i * 8 );
              display.setCursor( 7, textCursor );
              display.print( OBD2.pidName(pids[i]) );
            }
          }
        }else{
          int textCursor = 24;
          display.setCursor( 7, textCursor );
          display.print( "No PIDS" );
        }
        display.display();
        break;
      case 1:
        for( int i = 0; i < 6; i++ ){
          if( numActivePids >= i ){ 
            int textCursor = 16 + ( i * 8 );
            display.setCursor( 7, textCursor );
            display.print( OBD2.pidName(pids[(cursor-1)+i]) );
          }
        }
        display.display();
        break;
      case 2:
        for( int i = 0; i < 7; i++ ){
          if( numActivePids >= i ){ 
            int textCursor = 8 + ( i * 8 );
            display.setCursor( 7, textCursor );
            display.print( OBD2.pidName(pids[(cursor-2)+i]) );
          }
        }
        display.display();
        break;
      default:
        for( int i = 0; i < 8; i++ ){
          if( numActivePids >= ((cursor-3)+i) ){
            int textCursor = ( i * 8 );
            display.setCursor( 7, textCursor );
            display.print( OBD2.pidName(pids[(cursor-3)+i]) );
          }
        }
        display.display();
        break;
    }
    int input = -1;
    while( input == -1 ){
      input = RotEnc.getNextInput();
    }

    switch( input ){
      case CCW:
        if( cursor > 0 ) cursor--;
        break;
      case CW:
        if( cursor < numActivePids ) cursor++;
        break;
      case BUTTONPRESS:
        return cursor;
    }
  }
}

int numberSelector( int number, int min, int max ) {
  if( number < min ) number = min;
  int cursor = 0;
  int cursorX = 20;
  int digitEdit = 0;
  int input = 0;

  while( 1 ){
    ( cursor < 4 ) ? cursorX = 45 + ( cursor * 6 ) : cursorX = 51 + ( cursor * 6 );
    display.clearDisplay();
    display.setCursor( 35, 0 );
    display.printf( "Set Value" );
    display.setCursor( 18 , 16 );
    display.printf( "Range: %d to %d", min, max );
    if( number >= 0 ) {
      display.setCursor( 45, 50 );
      display.printf( "%04d OK", number );
    } else {
      display.setCursor( 39, 50 );
      display.printf( "%05d OK", number );
    }

    display.setCursor( cursorX, 58 );
    ( digitEdit ) ? display.printf( "T" ) : display.printf( "^" );
    display.display();

    input = 0;
    while( !input ) {
      input = RotEnc.getNextInput();
    }

    if( digitEdit ){
      switch( input ) {
        case CW: 
          switch( cursor ) { 
            case 0: if( ( ( number / 1000 % 10 ) < 9 ) && number + 1000 <= max ) number += 1000; break;
            case 1: if( ( ( number / 100 % 10 ) < 9 ) && number + 100 <= max ) number += 100; break;
            case 2: if( ( ( number / 10 % 10 ) < 9 ) && number + 10 <= max ) number += 10; break;
            case 3: if( ( number % 10 < 9 ) && number + 1 <= max ) number++; break;
          } 
        break;
        case CCW: 
          switch( cursor ) { 
            case 0: if( ( ( number / 1000 % 10 ) > -9 ) && number - 1000 >= min ) number -= 1000; break;
            case 1: if( ( ( number / 100 % 10 ) > -9 ) && number - 100 >= min ) number -= 100; break;
            case 2: if( ( ( number / 10 % 10 ) > -9 ) && number - 10 >= min ) number -= 10; break;
            case 3: if( number > -9 ) number--; break;
          } 
          break;
        case BUTTONPRESS: digitEdit--; break;
      }
    } else {
      switch ( input ) {
      case CW: if( cursor < 4 ) cursor++; break;
      case CCW: if( cursor > 0 ) cursor--; break;
      case BUTTONPRESS: if( cursor == 4 ) return number; else digitEdit++; break;
      }
    }
  }
}

void setup() {
  Serial.begin(9600);

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
      display.print("Checking PIDs... /n");
      for (int pid = 0; pid < 96; pid++) {
        if (OBD2.pidSupported(pid)) {
          enabledPids.set( pid, true );
          numPids++;
        }
      }

      display.print( numPids );
      display.print( " PIDs found." );
      delay(100);
          break;
    }
  }

  int ii = 0;
  for( int i = 0; i <= 96; i++) {
    if( enabledPids[i] ) { activePIDs[ii] = i; ii++; }
  }

  display.clearDisplay();
  display.setCursor(0, 0);

  attachInterrupt( digitalPinToInterrupt( ROTENC_IN1 ), in1_ISR, CHANGE);
  attachInterrupt( digitalPinToInterrupt( ROTENC_IN2 ), in2_ISR, CHANGE);
  attachInterrupt( digitalPinToInterrupt( ROTENC_BUTTON ), button_ISR, FALLING);

  LoggerClass log( *activePIDs, 2 );
}

void loop() {
  int pid = PIDSelector( activePIDs, numPids );
  Histogram airin( pid, 0, 50 );
  for(;;) {
  pollOBD();
  airin.update( &display );
  delay(1000);
  }
}

void pollOBD() {
  for ( int i = 0; activePIDs[i] != 0; i++ ) {
    currentPIDValues[i] = OBD2.pidRead( activePIDs[i] );
  }
}