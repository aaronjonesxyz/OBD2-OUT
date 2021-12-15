#include <Arduino.h>
#include <Adafruit_SSD1306.h>

class Histogram {
    int pid;
    String name;
    String unit;
    int rangeH;
    int rangeL;
    float stepsPerUnit;
    int history[20] = {0};

    public:
    Histogram( int p, int rL, int rH );
    void update( Adafruit_SSD1306 *display );
};