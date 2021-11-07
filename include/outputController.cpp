// outputController.h - Output Control Class for OBD2-OUT
// Copyright Aaron Jones

#include "globals.h"
#include "outputController.h"

void outputControllerClass::caseComparator() {

  // Check current PID values against cases
  int currValues[3] = {
    currentPIDValues[ PIDx ],
    currentPIDValues[ PIDy ],
    currentPIDValues[ PIDz ]
  };
  if( ctrlStatus == OFF ) {
    for ( auto& cCase: controlCase ) {  // Iterate through the output control cases
        switch ( cCase.logic ) {  // Make sure case is enabled, check logical operator
          case AND:
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

          case OR:
            int hitCount = 0;
            for ( int i = 0; i < 3; i++ ) { // Truth table index
              switch ( cCase.relOps[i] ) {
                case MORE_THAN: if ( currValues[i] >= cCase.compValues[i] ) { onCount++; } break;

                case LESS_THAN: if ( currValues[i] <= cCase.compValues[i] ) { onCount++; } break;

                case DISABLED: onCount++; break;
              }
              if ( onCount >= 1 ) { pinControl( ON ); activeCase = &cCase; break; }
            }
            break;

          case DISABLED:
            break;
        }
      }
    }
  }

void outputControllerClass::pinControl ( int state ) {
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

void outputControllerClass::activeCaseCheck() {
  int currValues[3] = {
    currentPIDValues[ PIDx ],
    currentPIDValues[ PIDy ],
    currentPIDValues[ PIDz ]
  };
  for ( int i = 0; i < 3; i++ ) {
    switch ( activeCase->relOps[i] ) {
      case MORE_THAN: if ( ( ( currValues[i] < ( activeCase->compValues[i] - hysteresis[i] ) ) && onMillis > minOnTime ) ||
      ( onMillis > maxOnTime ) ) { pinControl( OFF ); ctrlStatus = OFF; } break;                                            // Check if value is outside hysteresis or over max activation time

      case LESS_THAN: if ( ( ( currValues[i] > ( activeCase->compValues[i] + hysteresis[i] ) ) && onMillis > minOnTime ) ||
      ( onMillis > maxOnTime ) ) { pinControl( OFF ); ctrlStatus = OFF; } break;

      case DISABLED: break;

    }
  }
}