#ifndef GAUGES_H
#define GAUGES_H

#include <Arduino.h>
#include <Adafruit_SSD1306.h>

enum {
PID,
RANGE
};

class XYPlotter {
    int pid;
    uint8_t newPid = 0;
    uint8_t newRange = 0;
    String name;
    String unit;
    float stepsPerUnit;
    int history[50] = {0};
    uint32_t updateTime = 0;

    public:
    void begin();
    void newData( uint8_t );
    void update();
};

void textDisplay();

#endif