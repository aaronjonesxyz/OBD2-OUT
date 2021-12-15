#include <bitset>
#include "outputController.h"

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

extern int activePIDs[96];
extern std::bitset<96> enabledPids;
extern int numPids;
extern int currentPIDValues[96];

extern unsigned long int in1Debounce;
extern unsigned long int in2Debounce;
extern unsigned long int buttonDebounce;

#endif