#ifndef DISPLAYFUNCS_H
#define DISPLAYFUNCS_H

#include <Arduino.h>
#include <Adafruit_SSD1306.h>

#define ALERT_TIME 1500
#define ALERT_MAX 10

#define TEXT_MAXLEN 18

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
  OUTPUTCONTROL,
  PIDRELOAD,

  DATADISPLAY_STYLE,
  DATADISPLAY_DATA,
  DATADISPLAY_GRAPHRANGEH,
  DATADISPLAY_GRAPHRANGEL,

  LOGGING_FREQUENCY,
  LOGGING_DATA,

  OUTPUTCONTROL_MENU,

  OUTPUTCONTROL_PIDS = 15,
  OUTPUTCONTROL_CTRLLOGIC,

  OUTPUTCONTROL_CTRLLOGIC_LOGIC,
  OUTPUTCONTROL_CTRLLOGIC_RELOPS,
  OUTPUTCONTROL_CTRLLOGIC_COMPVALS,
  OUTPUTCONTROL_CTRLLOGIC_HYSTERESIS
};

void loadPIDs();

int menuSelector( std::vector< String > items, String title, int cursor);

int numberSelector( int min, int max, String title, int number = 0 );

int PIDSelector( int cursor = 0 );

String textEdit( String original, String title );

void menuSystem();

void checkAlerts();

void outputStateAlert( int output, int state );

#endif