#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <CAN.h>
#include <OBD2.h>
#include <SD.h>
#include <EEPROM.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <bitset>
#include "outputController.h"

#ifndef GLOBALS_H
#define GLOBALS_H
#endif

#define MCP2515_CS_PIN  PA11
#define MCP2515_INT_PIN PA2
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define ROTENC_IN1    PC13
#define ROTENC_IN2    PB8
#define ROTENC_BUTTON PB6

#define OLED_DC       PA1
#define OLED_CS       PA0
#define OLED_RESET    PA8

#define OUTPUT_CONTROL_1  PB6
#define OUTPUT_CONTROL_2  PB7
#define OUTPUT_CONTROL_3  PB9
#define OUTPUT_CONTROL_4  PC14

enum {
MORE_THAN,
LESS_THAN,
AND,
OR,
DISABLED,
ON,
OFF
};

struct {
  int LeftGaugePID; // Make this a struct; PID, last 10 values, histogram scale settings, default values
  int RightGaugePID;

  int activePIDs[10];

  outputControllerClass outputCtrl[4];
} settings = {
  AIR_INTAKE_TEMPERATURE,
  ENGINE_COOLANT_TEMPERATURE,

  { AIR_INTAKE_TEMPERATURE, ENGINE_COOLANT_TEMPERATURE, ENGINE_RPM, VEHICLE_SPEED, TIMING_ADVANCE },


};

std::bitset<96> enabledPids;
int currentPIDValues[10];