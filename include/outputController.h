// OutputController.h - Output Control Class for OBD2-OUT
// Copyright Aaron Jones
#ifndef OUTPUTCONTROLLER_H
#define OUTPUTCONTROLLER_H

#include <vector>

enum {
  MORE_THAN,
  LESS_THAN,
  ROC_UP,
  ROC_DOWN,
  AND,
  OR,
  DISABLED,
  ON,
  OFF
};

extern std::vector<String> logicStrings;

struct controlCase_t {
  uint8_t index;

  uint8_t* pids[2];

  float pidPrev[2];

  uint8_t logic; // AND/OR operator, DISABLED ignores this case

  uint8_t relOps[2]; // Relational operators MORE_THAN / LESS_THAN, DISABLED for 2-way or single case
  
  float compValues[2]; // Values to compare

  int16_t hysteresis[2];
};

class OutputControllerClass { 
  public:
    OutputControllerClass( String nam, uint8_t out, uint8_t p1, uint8_t p2, uint8_t p3, uint8_t minOn, uint8_t maxOn ) : 
    name(nam), outputPin(out), PIDs{ p1, p2, p3 }, minOnTime(minOn), maxOnTime(maxOn) {}
    void update();
    void pinControl( int state );
  
  public:
    String name;
    uint8_t outputPin;

    uint8_t PIDs[3] = {0};

    uint16_t minOnTime; // x 100 = milliseconds, 0 = no min
    uint16_t maxOnTime; // 0 = no max

    uint32_t lastMillis = 0;

    int ctrlStatus = OFF;
    controlCase_t* activeCase = NULL;
    unsigned long int onMillis = 0;

    controlCase_t controlCase[4] = 
    {{ 1, { &PIDs[0], &PIDs[1] }, {0}, OR, { DISABLED, DISABLED }, { 0, 0 }, 200 },
    { 2, { &PIDs[0], &PIDs[1] }, {0}, OR, { DISABLED, DISABLED }, { 0, 0 }, 200 },
    { 3, { &PIDs[0], &PIDs[1] }, {0}, OR, { DISABLED, DISABLED }, { 0, 0 }, 200 },
    { 4, { &PIDs[0], &PIDs[1] }, {0}, OR, { DISABLED, DISABLED }, { 0, 0 }, 200 }};
};

#endif