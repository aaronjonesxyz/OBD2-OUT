// OutputController.h - Output Control Class for OBD2-OUT
// Copyright Aaron Jones
#ifndef OUTPUTCONTROLLER_H
#define OUTPUTCONTROLLER_H

enum {
  MORE_THAN,
  LESS_THAN,
  AND,
  OR,
  DISABLED,
  ON,
  OFF
};

struct controlCase_t {
  uint8_t logic; // AND/OR operator, DISABLED ignores this case

  uint8_t relOps[3]; // Relational operators MORE_THAN / LESS_THAN, DISABLED for 2-way or single case
  
  uint8_t compValues[3]; // Values to compare

  uint8_t hysteresis;
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

    uint8_t minOnTime; // x 100 = milliseconds, 0 = no min
    uint8_t maxOnTime; // 0 = no max

    int ctrlStatus = OFF;
    controlCase_t* activeCase = NULL;
    unsigned long int onMillis = 0;

    controlCase_t controlCase[4] = 
    {{ DISABLED, { MORE_THAN, MORE_THAN, MORE_THAN }, { 0, 0, 0 }, 200 }};
};

#endif