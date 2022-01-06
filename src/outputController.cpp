// OutputController.h - Output Control Class for OBD2-OUT
// Copyright Aaron Jones

#include <Arduino.h>
#include "outputController.h"
#include <OBD2.h>
#include "globals.h"

void OutputControllerClass::update() {

  // Check current PID values against cases
  float currValues[3] = {
    OBD2.pidRead( masterPIDList[PIDs[0]] ),
    OBD2.pidRead( masterPIDList[PIDs[1]] ),
    OBD2.pidRead( masterPIDList[PIDs[2]] )
  };
  if( ctrlStatus == OFF ) {
    for ( auto& cCase: controlCase ) {  // Iterate through the output control cases
      switch ( cCase.logic ) {  // Make sure case is enabled, check logical operator
        case AND: {
          int onCount = 0;
          for ( int i = 0; i < 3; i++ ) { // 
            switch ( cCase.relOps[i] ) {
              case MORE_THAN: if ( currValues[i] >= cCase.compValues[i] ) { onCount++; } break;

              case LESS_THAN: if ( currValues[i] <= cCase.compValues[i] ) { onCount++; } break;

              case DISABLED: onCount++; break;
            }
          }
          if ( onCount == 3 ) { pinControl( ON ); activeCase = &cCase; }
          break;
        }

        case OR: {
          int onCount = 0;
          for ( int i = 0; i < 3; i++ ) { // Truth table index
            switch ( cCase.relOps[i] ) {
              case MORE_THAN: if ( currValues[i] >= cCase.compValues[i] ) { onCount++; } break;

              case LESS_THAN: if ( currValues[i] <= cCase.compValues[i] ) { onCount++; } break;

              case DISABLED: onCount++; break;
            }
            if ( onCount >= 1 ) { pinControl( ON ); activeCase = &cCase; break; }
          }
          break;
        }

        case DISABLED:
          break;
      }
    }
  }

  // Check if any outputs need to be turned off
  for ( int i = 0; i < 3; i++ ) {
    switch ( activeCase->relOps[i] ) {
      case MORE_THAN: if ( ( ( currValues[i] < ( activeCase->compValues[i] - activeCase->hysteresis ) ) && ( millis() - onMillis ) > (minOnTime * 100) ) ||
      ( ( millis() - onMillis ) > (maxOnTime * 100) ) ) { pinControl( OFF ); ctrlStatus = OFF; } break;                                            // Check if value is outside hysteresis or over max activation time

      case LESS_THAN: if ( ( ( currValues[i] > ( activeCase->compValues[i] + activeCase->hysteresis ) ) && ( millis() - onMillis ) > (minOnTime * 100) ) ||
      ( ( millis() - onMillis ) > (maxOnTime * 100) ) ) { pinControl( OFF ); ctrlStatus = OFF; } break;

      case DISABLED: break;

    }
  }
}

void OutputControllerClass::pinControl ( int state ) {
  if( state == ON ){
    digitalWrite( outputPin, HIGH );
    onMillis = millis();
    ctrlStatus = ON;
  } else {
    digitalWrite( outputPin, LOW );
    onMillis = 0;
    ctrlStatus = OFF;
  }
}