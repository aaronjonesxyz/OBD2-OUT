/**************************************************************************
  Electron Taming Service CAN Display Firmware

  MCP1515:
  CS PA11
  INT PA2
  SI PA7
  SO PA6
  SCK PA5

  SSD1306:
  SDA PB15
  XCLK PB13
  CS PA0
  DC PA1
  RES PA8
 **************************************************************************/

#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <CAN.h>
#include <OBD2.h>
#include <SD.h>
#include <EEPROM.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ClickEncoder.h>

#define MCP2515_CS_PIN          15
#define MCP2515_INT_PIN         2

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

SPIClass SPI_2(PB15, PB14, PB13);

#define OLED_DC     1
#define OLED_CS     0
#define OLED_RESET  8
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT,
                         &SPI_2, OLED_DC, OLED_RESET, OLED_CS);

#define OUTPUT_CONTROL_1  PB3
#define OUTPUT_CONTROL_2  PB4
#define OUTPUT_CONTROL_3  PB5
#define OUTPUT_CONTROL_4  PB8

#define HIGHER_THAN 0
#define LOWER_THAN  1

#define AND 0
#define OR 1

#define DISABLED 2

const int PROGMEM PIDs[] = {
  CALCULATED_ENGINE_LOAD,
  ENGINE_COOLANT_TEMPERATURE,
  INTAKE_MANIFOLD_ABSOLUTE_PRESSURE,
  ENGINE_RPM,
  VEHICLE_SPEED,
  TIMING_ADVANCE,
  AIR_INTAKE_TEMPERATURE,
  THROTTLE_POSITION,
  AMBIENT_AIR_TEMPERATURE
};

struct ctrlStruct{
  int outputPin = 0;

  int PIDx = 0;
  int PIDy = 0;
  int PIDz = 0;

  int minOnTime = 0;
  int maxOnTime = 0;

  int activateFlag = 0;

  int ctrlStatus = 0;

  int activationTime = 0;

  
  int truthTable[4][2][4] = {
    {{ 0,0,0,0 },
    { 0,0,0,0 }},
    {{ 0,0,0,0 },
    { 0,0,0,0 }},
    {{ 0,0,0,0 },
    { 0,0,0,0 }},
    {{ 0,0,0,0 },
    { 0,0,0,0 }}
  };

  /*
   * OPERATOR   X   Y   Z
   * 
   *            >/< >/< >/<
   * &/|/OFF    val val val
   * 
   */

};

struct {
  int LeftGaugePID;
  int RightGaugePID;

  int activePIDs[10];
  int numOutputs;

  ctrlStruct outputCtrl[4];
} settings;

int currentPIDValues[10] = {0};
int numPIDs;

void setup() {
  Serial.begin(9600);

  CAN.setPins( MCP2515_CS_PIN, MCP2515_INT_PIN );

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC)) {
    Serial.println("SSD1306 allocation failed");
    for (;;); // Don't proceed, loop forever
  }

  // Clear the buffer
  display.clearDisplay();

  display.setTextSize(0);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.print("ETS CAN Relay");
  display.display();

  delay(1000);

  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("OBD2.");
  display.display();

  

  for ( int att = 0; att < 3; att++ ) {
    if (!OBD2.begin()) {
      display.print(".");
      display.display();
      if ( att == 2 ) {
        display.print(" FAIL!");
        display.display();
        while (1) {
          ;;
        }
      }
    } else {
      display.print(" OK!");
      display.display();
      delay(500);
      break;
    }
  }

  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("Checking PIDs... ");
  for (int pid = 0; pid < 96; pid++) {
    if (OBD2.pidSupported(pid)) {
      Serial.println(OBD2.pidName(pid));
      display.print("/");
      display.print(pid);
      display.display();
    }
  }
}

void loop() {
  while (1) {
    ;;
  }
}

void drawGauges() {

}

void pollOBD() {
  for ( int i = 0; i < numPIDs; i++ ) {
    currentPIDValues[i] = OBD2.pidRead( settings.activePIDs[i] );
  }
}

void outputControl() {
  for ( int i = 0; i < 4; i++ ) { // outputCtrl structs
    for ( int ii = 0; ii < 4; ii++ ) { // truthTable
      if ( settings.outputCtrl[i].truthTable[ii][1][0] != 2 || ii == 0 ) { // If this row of the truth table is not OFF -- TODO SWITCH FOR AND OR
        int tableOut[3] = { 0,0,0 };
        for ( int iii = 1; iii < 4; iii++ ) {
          switch ( settings.outputCtrl[i].truthTable[ii][0][iii] ){ // Check if we are looking for HIGHER or LOWER than truthTable value
            case HIGHER_THAN:
              if ( currentPIDValues[ settings.outputCtrl[i].PIDx ] >= settings.outputCtrl[i].truthTable[ii][1][iii] ) {
                tableOut[iii-1] = 1;
              }
              break;
              
            case LOWER_THAN:
              if ( currentPIDValues[ settings.outputCtrl[i].PIDx ] <= settings.outputCtrl[i].truthTable[ii][1][iii] ) {
                tableOut[iii-1] = 1;
              }
              break;
            
            case DISABLED:
              tableOut[iii-1] = 1;
              break;
          }
         
        }
        
      }
      
    }
  }
}

void appendLog() {
  
}
