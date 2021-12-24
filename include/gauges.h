#include <Arduino.h>
#include <Adafruit_SSD1306.h>

#define UPDATE_INTERVAL 1000

class Histogram {
    public:
    int pid;
    private:
    String name;
    String unit;
    int rangeH;
    int rangeL;
    float stepsPerUnit;
    int history[20] = {0};
    unsigned long int updateTime = 0;

    public:
    Histogram( int p, int rL, int rH );
    void update();
};

extern Histogram hist;