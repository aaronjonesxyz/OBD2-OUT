#include "gauges.h"
#include "globals.h"
#include "displayFuncs.h"
#include <OBD2.h>
#include <Adafruit_SSD1306.h>

void XYPlotter::begin() {
  pid = settings.activePIDs[0];
  int rangeLAbs = ( settings.graphRangeL > -1 ) ? settings.graphRangeH - settings.graphRangeL : settings.graphRangeH + -settings.graphRangeL;
  stepsPerUnit = 37.00 / rangeLAbs;
  name = OBD2.pidName( masterPIDList[pid] );
  unit = OBD2.pidUnits( masterPIDList[pid] );
  updateTime = millis();
  for( auto &h : history ) h = 0;
}

void XYPlotter::newData( uint8_t type ) {
  switch( type ) {
    case PID: newPid = 1; break;
    case RANGE: newRange = 1; break;
  }
}

void XYPlotter::update() {
  if( newPid ) {
    pid = settings.activePIDs[0];
    int rangeLAbs = ( settings.graphRangeL > -1 ) ? settings.graphRangeH - settings.graphRangeL : settings.graphRangeH + -settings.graphRangeL;
    stepsPerUnit = 37.00 / rangeLAbs;
    name = OBD2.pidName( masterPIDList[pid] );
    unit = OBD2.pidUnits( masterPIDList[pid] );
    for( auto &h : history ) h = 0;
    newPid = 0;
  }
  if( newRange ) {
    int rangeLAbs = ( settings.graphRangeL > -1 ) ? settings.graphRangeH - settings.graphRangeL : settings.graphRangeH + -settings.graphRangeL;
    stepsPerUnit = 37.00 / rangeLAbs;
    newRange = 0;
  }
  int curVal = currentPIDValues[0];
  int lineX, lineY, _lineX, _lineY;
  display.setTextSize(1);
  display.clearDisplay();
  display.setCursor( 0, 56 );
  display.print( name );
  display.setCursor( 0, 0 );
  display.setTextSize(2);
  display.print( curVal );
  display.setCursor( 116 - ( ( unit.length() - 1 ) * 12 ), 0 );
  display.print( unit );
  display.drawFastHLine( 0, 54, 128, WHITE );
  display.drawFastHLine( 0, 15, 128, WHITE );
  display.setTextSize(1);
  display.setCursor( 0, 17 );
  display.print( settings.graphRangeH );
  display.setCursor( 0, 46 );
  display.print( settings.graphRangeL );

  // Draw the line graph
  lineX = 127;                                                                  // Start at the right edge
  int yZero = ( settings.graphRangeL > -1 ) ?                                   // Calculate Y coordinate for 0
    53 - ( settings.graphRangeL * stepsPerUnit ) :
    53 - ( -settings.graphRangeL * stepsPerUnit ); 
  lineY = yZero - ( history[0] * stepsPerUnit );                                // Calculate the position of the first point in the line
  if( lineY < 16 ){ lineY = 16; } else if( lineY > 53 ){ lineY = 53; }          // Bound the Y coordinate within the chart area
  for( int i = 1; i < 50; i++ ) {                                               // Repeat for each of the 49 previous saved values
      _lineX = lineX - 2;                                                       // Move the X coordinate 2 pixels to the left each time
      _lineY = yZero - ( history[i] * stepsPerUnit );
      if( _lineY < 16 ){ _lineY = 16; } else if( _lineY > 53 ){ _lineY = 53; }
      display.drawLine( lineX, lineY, _lineX, _lineY, WHITE );
      lineX = _lineX;                                                           // Store the value of the current coordinates to use as the next line's start point
      lineY = _lineY;
  }

  checkAlerts();

  display.display();

  for( int i = 49; i > 0; i-- ) {                                               // Shift the history array right to make room for the new value
      history[i] = history[i-1];
  }
  history[0] = curVal;
}

void textDisplay() {
  switch( settings.dataDisplayStyle ) {
    case 1: {                                                                   // TODO: function for drawing name, unit, val that takes position data
      String name = OBD2.pidName( masterPIDList[settings.activePIDs[0]] );
      String unit = OBD2.pidUnits( masterPIDList[settings.activePIDs[0]] );
      String val = String(currentPIDValues[0]);
      display.clearDisplay();
      display.setCursor( 64 - ( name.length() * 3), 0 );
      display.setTextSize(1);
      display.print( name );
      display.setCursor( 127 - ( unit.length() * 11), 49 );
      display.setTextSize(2);
      display.print( unit );
      display.setCursor( 64 - ( val.length() * 12), 16 );
      display.setTextSize(4);
      display.print(val);
      display.display();
      break; }
    
    case 2:
      String name = OBD2.pidName( masterPIDList[settings.activePIDs[0]] );
      String unit = OBD2.pidUnits( masterPIDList[settings.activePIDs[0]] );
      display.clearDisplay();
      display.setCursor( 64 - ( name.length() * 3), 0 );
      display.setTextSize(1);
      display.print( name );
      display.setCursor( 128 - ( unit.length() * 12), 17 );
      display.setTextSize(2);
      display.print( unit );
      display.setCursor( 0, 10 );
      display.setTextSize(3);
      display.print(currentPIDValues[0]);
      display.drawFastHLine( 0, 32, 127, WHITE );
      // Lower half
      name = OBD2.pidName( masterPIDList[settings.activePIDs[1]] );
      unit = OBD2.pidUnits( masterPIDList[settings.activePIDs[1]] );
      display.setCursor( 64 - ( name.length() * 3), 34 );
      display.setTextSize(1);
      display.print( name );
      display.setCursor( 128 - ( unit.length() * 12), 50 );
      display.setTextSize(2);
      display.print( unit );
      display.setCursor( 0, 43 );
      display.setTextSize(3);
      display.print(currentPIDValues[1]);
      display.display();
    break;

  }
}