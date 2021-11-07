// outputController.h - Output Control Class for OBD2-OUT
// Copyright Aaron Jones

#ifndef OUTPUTCONTROLLER_H
#define OUTPUTCONTROLLER_H
#endif

struct controlCase_t {
  int logic; // AND/OR operator, DISABLED ignores this case

  int relOps[3]; // Relational operators MORE_THAN / LESS_THAN, DISABLED for 2-way or single case
  
  int compValues[3]; // Values to compare
};

class outputControllerClass {
  public:
    void caseComparator();
    void pinControl( int state );
    void activeCaseCheck();
  
  public:
    int outputPin;

    int PIDx;
    int PIDy;
    int PIDz;

    long int minOnTime;
    long int maxOnTime;

    int ctrlStatus;
    controlCase_t* activeCase;
    long int onMillis;

    int hysteresis[3];

    controlCase_t controlCase[4];
};