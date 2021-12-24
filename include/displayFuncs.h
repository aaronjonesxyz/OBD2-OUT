#include <Arduino.h>
#include <Adafruit_SSD1306.h>

#ifndef DISPLAYFUNCS_H
#define DISPLAYFUNCS_H

#define ALERT_TIME 1500
#define ALERT_MAX 10

#define TEXT_MAXLEN 18

int menuCursorPos[4] = { 0, 0, 0, 0 };

struct alert_t {
  int output = -1;
  int state = -1;
};

const String stateName[2] = {{"ON"}, {"OFF"}};

extern alert_t alertQueue[ALERT_MAX];

enum {
  NONE,
  OUT1_ON,
  OUT1_OFF,
  OUT2_ON,
  OUT2_OFF,
  OUT3_ON,
  OUT3_OFF,
  OUT4_ON,
  OUT4_OFF,
};

enum {
  MAINMENU,
  DATADISPLAY,
  LOGGING,
  OUTPUTCONTROL
};

int menuSelector( std::vector< String > items, String title, int cursor);

int numberSelector( int min, int max, String title, int number );

int PIDSelector( int current );

String textEdit( String original, String title );

void menuSystem();

void checkAlerts();

void outputStateAlert( int output, int state );

#endif