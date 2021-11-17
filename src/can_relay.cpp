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
using namespace std;


SPIClass SPI_2(PB15, PB14, PB13);

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT,
                         &SPI_2, OLED_DC, OLED_RESET, OLED_CS);

File file;

void setup() {
  Serial.begin(9600);

  pinMode( OUTPUT_CONTROL_1, OUTPUT );
  pinMode( OUTPUT_CONTROL_2, OUTPUT );
  pinMode( OUTPUT_CONTROL_3, OUTPUT );
  pinMode( OUTPUT_CONTROL_4, OUTPUT );

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
        for (;;);
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
  display.print("Checking PIDs... /n");
  char numPids = 0;
  for (int pid = 0; pid < 96; pid++) {
    if (OBD2.pidSupported(pid)) {
      enabledPids.set( pid, true );
      numPids++;
    }
  }

  display.print( numPids );
  display.print( " PIDs found." )

  display.clearDisplay();
  display.setCursor(0, 0);

  // SD Card Init

  while(1) {
    if (!SD.begin(PB12)) {
      Serial.println("Card failed, or not present");
      display.print("No SD Card.");
      display.display();
      delay(250);
      return;
    }
    Serial.println("Card initialized.");
    display.print("SD Init.");
    display.display();
      delay(50);

    if( SD.exists("settings.dat") ){
      Serial.println("Settings file found.");
      file = SD.open("settings.dat", O_RDWR);
      if(file){

      } else{
        Serial.println("File I/O error!");
      }
    } else {
      file = SD.open("settings.dat", FILE_WRITE);
      if(file){
        file.close();
      } else {
        Serial.println("File I/O error!");
      }
    }
    /*sprintf(filename, "SDLog%d.csv", fnum );
    Serial.println(filename);
    file = SD.open(filename, FILE_WRITE);
    Serial.println(file);
    if(file){
      file.println(header1);
      file.println(header2);
      file.close();
    } else {
      Serial.println("File I/O Error!");
    }*/
  }
}

void loop() {
  while (1) {
    int iat = OBD2.pidRead( AIR_INTAKE_TEMPERATURE );
    display.clearDisplay();
    display.setCursor(0,0);
    display.print("IAT: ");
    display.print(iat);
    display.display();
    
  }
}

void pollOBD() {
  for ( int i = 0; i < numPIDs; i++ ) {
    currentPIDValues[i] = OBD2.pidRead( settings.activePIDs[i] );
  }
}

