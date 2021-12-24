#include "globals.h"
#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_SSD1306.h>
#include <vector>

SPIClass SPI_2(PB15, PB14, PB13);

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT,
                         &SPI_2, OLED_DC, OLED_RESET, OLED_CS);

int supportedPIDs[96];
int supportedPIDs_n = 0;
int activePIDs[20];
int activePIDs_n = 0;
int currentPIDValues[20];
unsigned long int in1Debounce = 0;
unsigned long int in2Debounce = 0;
unsigned long int buttonDebounce = 0;

std::vector<String> mainMenuLabels { "Data Display", "Logging", "Output Control" };
std::vector<String> gaugeMenuLabels { "Style", "Data" };
std::vector<String> loggingMenuLabels { "Frequency", "Data" };
std::vector<String> displayStyleLabels { "Histogram", "Text x1", "Text x2", "Text x3", "Text x4" };
std::vector<String> outputControlLabels { "Name", "PIDs", "Control Logic", "Min On Time", "Max Off Time" };

settings_t settings;