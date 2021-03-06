#include "globals.h"
#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_SSD1306.h>
#include <vector>

uint32_t updateTime = 0;
uint32_t loggingTime = 0;

SPIClass SPI_2(PB15, PB14, PB13);

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT,
                         &SPI_2, OLED_DC, OLED_RESET, OLED_CS);

float currentPIDValues[masterPID_n];
uint8_t OBDConnected = 0;

std::vector<String> mainMenuLabels { "Data Display", "Logging Frequency", "Output Control" };
std::vector<String> gaugeMenuLabels { "Style", "Data", "Graph Upper Limit", "Graph Lower Limit" };
std::vector<String> displayStyleLabels { "Plotter", "Text x1", "Text x2" };
std::vector<String> outputControlLabels { "Name", "PIDs", "Control Logic", "Min On Time", "Max On Time" };

settings_t settings;

XYPlotter xyPlotter;
LoggerClass logger;

// Macro for writing a variable to OLED for debugging
void QD( float d ) {
  display.println(d);
  display.display();
}

void QDHex( int d ) {
  display.println(d, HEX);
  display.display();
}