// OutputController.h - Output Control Class for OBD2-OUT
// Copyright Aaron Jones
#include "globals.h"

#ifndef OUTPUTCONTROLLER_H
#define OUTPUTCONTROLLER_H

struct controlCase_t {
  int logic; // AND/OR operator, DISABLED ignores this case

  int relOps[3]; // Relational operators MORE_THAN / LESS_THAN, DISABLED for 2-way or single case
  
  int compValues[3]; // Values to compare

  int hysteresis;
};

class OutputControllerClass { // Set default values!!
  public:
    OutputControllerClass( int out );
    void update();
    void pinControl( int state );
  
  public:
    int outputPin;

    int PIDx;
    int PIDy;
    int PIDz;

    long int minOnTime; // milliseconds, 0 = no min
    long int maxOnTime; // 0 = no max

    int ctrlStatus;
    controlCase_t* activeCase;
    unsigned long int onMillis;

    controlCase_t controlCase[4] = {
      { DISABLED, { MORE_THAN, MORE_THAN, MORE_THAN }, { 0, 0, 0 }, 200 }
    };
};

extern OutputControllerClass outControl[4];

#endif