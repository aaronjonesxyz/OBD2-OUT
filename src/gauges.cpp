#include "gauges.h"
#include "globals.h"
#include "displayFuncs.h"
#include <OBD2.h>
#include <Adafruit_SSD1306.h>

Histogram::Histogram( int p, int rL, int rH ) {
  pid = p;
  rangeH = rH;
  rangeL = rL;
  stepsPerUnit = 39.00 / ( rH - rL );
  name = OBD2.pidName( activePIDs[pid] );
  unit = OBD2.pidUnits( activePIDs[pid] );
  updateTime = millis();
}

void Histogram::update() {
  int curVal = currentPIDValues[pid];
  int lineX, lineY, _lineX, _lineY;
  display.clearDisplay();
  display.setCursor( 0, 56 );
  display.print( name );
  display.setCursor( 0, 0 );
  display.setTextSize(2);
  display.print( curVal );
  display.setTextSize(1);
  display.setCursor( 127-( unit.length() * 6), 0 );
  display.print( unit );
  display.drawFastHLine( 0, 54, 128, WHITE );
  display.drawFastHLine( 0, 15, 128, WHITE );
  display.setCursor( 0, 17 );
  display.print( rangeH );
  display.setCursor( 0, 46 );
  display.print( rangeL );

  lineX = 127;
  lineY = 54 - ( ( curVal - rangeL ) * stepsPerUnit );
  if( lineY < 16 ){ lineY = 16; } else if( lineY > 55 ){ lineY = 55; }
  for( int i = 1; i < 20; i++ ) {
      _lineX = lineX - 5;
      _lineY = 54 - ( ( history[i] - rangeL ) * stepsPerUnit );
      if( _lineY < 16 ){ _lineY = 16; } else if( _lineY > 55 ){ _lineY = 55; }
      display.drawLine( lineX, lineY, _lineX, _lineY, WHITE );
      lineX = _lineX;
      lineY = _lineY;
  }

  checkAlerts();

  display.display();

  if( ( millis() - updateTime ) > UPDATE_INTERVAL ) {
    for( int i = 19; i > 0; i-- ) {
        history[i] = history[i-1];
    }
    history[0] = curVal;
    updateTime = millis();
  }
}

Histogram hist( 0, 10, 80 );