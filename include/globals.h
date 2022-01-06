#ifndef GLOBALS_H
#define GLOBALS_H

#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_SSD1306.h>
#include <vector>
#include "OBD2.h"
#include "gauges.h"
#include "logging.h"

#define UPDATE_INTERVAL 100

#define MCP2515_CS_PIN  PA11
#define MCP2515_INT_PIN PA2
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_DC       PA1
#define OLED_CS       PA0
#define OLED_RESET    PA8

#define SDCARD_CS PB12

#define OUTPUT_CONTROL_1  PB6
#define OUTPUT_CONTROL_2  PB7
#define OUTPUT_CONTROL_3  PB8
#define OUTPUT_CONTROL_4  PB9

#define SETTINGS_ITEMS 28

const uint8_t masterPIDList[] = {
  CALCULATED_ENGINE_LOAD,
  ENGINE_COOLANT_TEMPERATURE,
  SHORT_TERM_FUEL_TRIM_BANK_1,
  LONG_TERM_FUEL_TRIM_BANK_1,
  SHORT_TERM_FUEL_TRIM_BANK_2,
  LONG_TERM_FUEL_TRIM_BANK_2,
  FUEL_PRESSURE,
  INTAKE_MANIFOLD_ABSOLUTE_PRESSURE,
  ENGINE_RPM,
  VEHICLE_SPEED,
  TIMING_ADVANCE,
  AIR_INTAKE_TEMPERATURE,
  MAF_AIR_FLOW_RATE,
  THROTTLE_POSITION,
  OXYGEN_SENSOR_1_SHORT_TERM_FUEL_TRIM,
  OXYGEN_SENSOR_2_SHORT_TERM_FUEL_TRIM,
  RUN_TIME_SINCE_ENGINE_START,
  FUEL_RAIL_PRESSURE,
  FUEL_RAIL_GAUGE_PRESSURE,
  OXYGEN_SENSOR_1_FUEL_AIR_EQUIVALENCE_RATIO,
  OXYGEN_SENSOR_2_FUEL_AIR_EQUIVALENCE_RATIO,
  FUEL_TANK_LEVEL_INPUT,
  ABSOLULTE_BAROMETRIC_PRESSURE,
  ABSOLUTE_LOAD_VALUE,
  FUEL_AIR_COMMANDED_EQUIVALENCE_RATE,
  AMBIENT_AIR_TEMPERATURE,
  COMMANDED_THROTTLE_ACTUATOR,
  ETHANOL_FUEL_PERCENTAGE,
  FUEL_RAIL_ABSOLUTE_PRESSURE,
  ENGINE_OIL_TEMPERATURE,
  FUEL_INJECTION_TIMING,
  ENGINE_FUEL_RATE
};
const uint8_t masterPID_n = 29;

enum {
  XYGRAPH,
  TEXT1,
  TEXT2,
  TEXT3,
  TEXT4
};

extern uint32_t updateTime;

extern SPIClass SPI_2;

extern Adafruit_SSD1306 display;

extern float currentPIDValues[20];
extern uint8_t OBDConnected;

extern std::vector<String> mainMenuLabels;
extern std::vector<String> gaugeMenuLabels;
extern std::vector<String> loggingMenuLabels;
extern std::vector<String> displayStyleLabels;
extern std::vector<String> outputControlLabels;

#include "outputController.h"

struct settings_t {
  uint8_t dataDisplayStyle = XYGRAPH;
  int graphRangeH = 100;
  int graphRangeL = -40;
  uint8_t supportedPIDs[masterPID_n];
  uint8_t supportedPIDs_n;
  uint8_t activePIDs[20];
  uint8_t activePIDs_n;

  uint16_t loggingFreq = 1000;

  OutputControllerClass outControl[4] = {
    OutputControllerClass("Intercooler Spray", OUTPUT_CONTROL_1, 0, 0, 0, 10, 50),
    OutputControllerClass("Oil Pressure", OUTPUT_CONTROL_1, 0, 0, 0, 10, 50),
    OutputControllerClass("Shift Light", OUTPUT_CONTROL_1, 0, 0, 0, 10, 50),
    OutputControllerClass("Thermofans", OUTPUT_CONTROL_1, 0, 0, 0, 10, 50)
  };
};

extern settings_t settings;

extern XYPlotter xyPlotter;
extern LoggerClass logger;

void QD( float );

#endif