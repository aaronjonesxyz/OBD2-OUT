// OutputController.h - Output Control Class for OBD2-OUT
// Copyright Aaron Jones

#ifndef OUTPUTCONTROLLER_H
#define OUTPUTCONTROLLER_H

struct controlCase_t {
  int logic; // AND/OR operator, DISABLED ignores this case

  int relOps[3]; // Relational operators MORE_THAN / LESS_THAN, DISABLED for 2-way or single case
  
  int compValues[3]; // Values to compare
};

class OutputControllerClass { // Set default values!!
  public:
    OutputControllerClass( int outPin, int Px, int Py, int Pz );
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

    int hysteresis[3];

    controlCase_t controlCase[4];
};

#endif