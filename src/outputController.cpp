// OutputController.h - Output Control Class for OBD2-OUT
// Copyright Aaron Jones

#include <Arduino.h>
#include "outputController.h"
#include "displayFuncs.h"
#include <OBD2.h>
#include "globals.h"

std::vector<String> logicStrings { "OVER", "UNDER", "+ R.O.C.", "- R.O.C.", "AND", "OR", "DISABLED", "ON", "OFF" };

void OutputControllerClass::update() {

  // Check current PID values against cases
  float currValues[3] = {
    currentPIDValues[PIDs[0]],
    currentPIDValues[PIDs[1]],
    currentPIDValues[PIDs[2]]
  };

  float casePIDValues[2];
  if( ctrlStatus == OFF ) {
    for ( auto& cCase: controlCase ) {  // Iterate through the output control cases

      casePIDValues[0] = currentPIDValues[*cCase.pids[0]];
      casePIDValues[1] = currentPIDValues[*cCase.pids[1]];

      int onCount = 0;
      for ( int i = 0; i < 2; i++ ) {
        int more, up = 0;
        switch ( cCase.relOps[i] ) {
          case MORE_THAN:
            more = 1;
          case LESS_THAN:
            if ( ( more ) ? casePIDValues[i] >= cCase.compValues[i] : casePIDValues[i] <= cCase.compValues[i] ) { onCount++; } break;

          case ROC_UP:
            up = 1;
          case ROC_DOWN:
            if( lastMillis == 0 ) {
              lastMillis = millis();
            } else if( ( millis() - lastMillis ) >= 500 ) {
              float saveValue = casePIDValues[i];
              casePIDValues[i]  = ( up ) ? ( casePIDValues[i] - cCase.pidPrev[i] ) * 2 : ( cCase.pidPrev[i] - casePIDValues[i] ) * 2;
              if( casePIDValues[i] >= cCase.compValues[i] ) onCount++;
              cCase.pidPrev[i] = saveValue;
            }
            break;

          case DISABLED: break;
        }

        if( cCase.logic == AND && onCount >= 3 ) {
          pinControl( ON );
          activeCase = &cCase;
        } else if( onCount >= 1 ) {
          pinControl( ON );
          activeCase = &cCase;
        }
      }
    }
  }

  // Check if any outputs need to be turned off
  if( ctrlStatus == ON ) {
    for ( int i = 0; i < 2; i++ ) {
      uint8_t more = 0;
      switch ( activeCase->relOps[i] ) {
        
        case MORE_THAN:
        more = 1;
        case LESS_THAN:
          if( ( ( ( more ) ? casePIDValues[i] : activeCase->compValues[i] + activeCase->hysteresis[i] < ( more ) ? activeCase->compValues[i] - activeCase->hysteresis[i] : casePIDValues[i] ) && ( millis() - onMillis ) > minOnTime )
          || ( ( millis() - onMillis ) > (maxOnTime * 100) ) ) {
            pinControl( OFF );
          } break;

        case DISABLED: break;

      }
    }
  }
}

void OutputControllerClass::pinControl ( int state ) {
  if( state == ON ){
    digitalWrite( outputPin, HIGH );
    onMillis = millis();
    ctrlStatus = ON;
    outputStateAlert( this, 1 );
  } else {
    digitalWrite( outputPin, LOW );
    onMillis = 0;
    ctrlStatus = OFF;
    outputStateAlert( this, 0 );
  }
}