#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_SSD1306.h>
#include <vector>

#ifndef GLOBALS_H
#define GLOBALS_H

#define MCP2515_CS_PIN  PA11
#define MCP2515_INT_PIN PA2
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_DC       PA1
#define OLED_CS       PA0
#define OLED_RESET    PA8

#define OUTPUT_CONTROL_1  PB6
#define OUTPUT_CONTROL_2  PB7
#define OUTPUT_CONTROL_3  PB8
#define OUTPUT_CONTROL_4  PB9

enum {
  MORE_THAN,
  LESS_THAN,
  AND,
  OR,
  DISABLED,
  ON,
  OFF
};

enum {
  HISTOGRAM,
  TEXT1,
  TEXT2,
  TEXT3,
  TEXT4
};

extern SPIClass SPI_2;

extern Adafruit_SSD1306 display;

extern int supportedPIDs[96];
extern int supportedPIDs_n;
extern int activePIDs[20];
extern int activePIDs_n;
extern int currentPIDValues[20];
extern unsigned long int in1Debounce;
extern unsigned long int in2Debounce;
extern unsigned long int buttonDebounce;

extern std::vector<String> mainMenuLabels;
extern std::vector<String> gaugeMenuLabels;
extern std::vector<String> loggingMenuLabels;
extern std::vector<String> displayStyleLabels;
extern std::vector<String> outputControlLabels;

struct settings_t {
  int dataDisplayStyle = HISTOGRAM;

  // Put supported PIDs in here, skip PID check if supported PID list already exists

  std::vector<String> outputNames { "Intercooler Spray", "Output 2", "Output 3", "Output 4" };
};

extern settings_t settings;

#endif