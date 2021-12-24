#include <vector>
#include <Arduino.h>
#include <Adafruit_SSD1306.h>
#include <OBD2.h>
#include "rotEnc.h"
#include "globals.h"
#include "gauges.h"
#include "displayFuncs.h"

alert_t alertQueue[ALERT_MAX] = { { -1, -1 } };
int alertLast = 0;
unsigned long int alertTime = 0;
int alertActive = 0;

int menuSelector( std::vector< String > items, String title, int cursor = 0 ) { 
  int titleCursor = 64 - ( ( title.length() / 2 ) * 6 );
  int itemsLen = items.size();
  items.push_back( "Back" );

  while(1) {
    display.clearDisplay();
    display.setCursor(titleCursor, 0);
    display.print( title );
    display.setCursor(0,32);
    display.print("-");
    int i = 0;
    switch( cursor ) {
      case 0:
        while( i <= itemsLen && i < 5 ) { 
          int textCursor = 32 + ( i * 8 );
          display.setCursor( 7, textCursor );
          display.print( items[i] );
          i++;
        }
        if( !itemsLen ) {
          int textCursor = 32;
          display.setCursor( 7, textCursor );
          display.print( "No Items" );
        }
        break;
      default:
        while( ((cursor-1)+i) <= itemsLen && i < 6 ) { 
          int textCursor = 24 + ( i * 8 );
          display.setCursor( 7, textCursor );
          display.print( items[(cursor-1)+i] );
          i++;
        }
        break;
    }
    display.display();

    int input = -1;
    while( input == -1 ){
      input = RotEnc.getNextInput();
    }

    switch( input ){
      case CCW:
        if( cursor > 0 ) cursor--;
        break;
      case CW:
        if( cursor < itemsLen ) cursor++;
        break;
      case BUTTONPRESS:
        return cursor;
    }
  }
}

int numberSelector( int min, int max, String title, int number = 0 ) {
  int titleCursor = 64 - ( ( title.length() * 6 ) / 2 );
  if( number < min ) number = min;
  int cursor = 0;
  int cursorX = 20;
  int digitEdit = 0;
  int input = 0;

  while( 1 ){
    ( cursor < 4 ) ? cursorX = 45 + ( cursor * 6 ) : cursorX = 51 + ( cursor * 6 );
    display.clearDisplay();
    display.setCursor(titleCursor, 0);
    display.print( title );
    display.setCursor( 18 , 16 );
    display.printf( "Range: %d to %d", min, max );
    if( number >= 0 ) {
      display.setCursor( 45, 50 );
      display.printf( "%04d OK", number );
    } else {
      display.setCursor( 39, 50 );
      display.printf( "%05d OK", number );
    }

    display.setCursor( cursorX, 58 );
    ( digitEdit ) ? display.printf( "T" ) : display.printf( "^" );
    display.display();

    input = 0;
    while( !input ) {
      input = RotEnc.getNextInput();
    }

    if( digitEdit ){
      switch( input ) {
        case CW: 
          switch( cursor ) { 
            case 0: if( ( ( number / 1000 % 10 ) < 9 ) && number + 1000 <= max ) number += 1000; break;
            case 1: if( ( ( number / 100 % 10 ) < 9 ) && number + 100 <= max ) number += 100; break;
            case 2: if( ( ( number / 10 % 10 ) < 9 ) && number + 10 <= max ) number += 10; break;
            case 3: if( ( number % 10 < 9 ) && number + 1 <= max ) number++; break;
          } 
        break;
        case CCW: 
          switch( cursor ) { 
            case 0: if( ( ( number / 1000 % 10 ) > -9 ) && number - 1000 >= min ) number -= 1000; break;
            case 1: if( ( ( number / 100 % 10 ) > -9 ) && number - 100 >= min ) number -= 100; break;
            case 2: if( ( ( number / 10 % 10 ) > -9 ) && number - 10 >= min ) number -= 10; break;
            case 3: if( number > -9 ) number--; break;
          } 
          break;
        case BUTTONPRESS: digitEdit--; break;
      }
    } else {
      switch ( input ) {
      case CW: if( cursor < 4 ) cursor++; break;
      case CCW: if( cursor > 0 ) cursor--; break;
      case BUTTONPRESS: if( cursor == 4 ) return number; else digitEdit++; break;
      }
    }
  }
}

int PIDSelector( int cursor = 0 ) { // Displays the supplied list of PIDs in a list with a selection cursor
  while(1) {
    display.clearDisplay();
    display.setCursor(0,24);
    display.print("-");
    switch( cursor ) {
      case 0:
        if( activePIDs_n ) {
          for( int i = 0; i < 5; i++ ){
            if( activePIDs_n >= i ){
              int textCursor = 24 + ( i * 8 );
              display.setCursor( 7, textCursor );
              display.print( OBD2.pidName(supportedPIDs[i]) );
            }
          }
        }else{
          int textCursor = 24;
          display.setCursor( 7, textCursor );
          display.print( "No PIDS" );
        }
        display.display();
        break;
      case 1:
        for( int i = 0; i < 6; i++ ){
          if( activePIDs_n >= i ){ 
            int textCursor = 16 + ( i * 8 );
            display.setCursor( 7, textCursor );
            display.print( OBD2.pidName(supportedPIDs[(cursor-1)+i]) );
          }
        }
        display.display();
        break;
      case 2:
        for( int i = 0; i < 7; i++ ){
          if( activePIDs_n >= i ){ 
            int textCursor = 8 + ( i * 8 );
            display.setCursor( 7, textCursor );
            display.print( OBD2.pidName(supportedPIDs[(cursor-2)+i]) );
          }
        }
        display.display();
        break;
      default:
        for( int i = 0; i < 8; i++ ){
          if( activePIDs_n >= ((cursor-3)+i) ){
            int textCursor = ( i * 8 );
            display.setCursor( 7, textCursor );
            display.print( OBD2.pidName(supportedPIDs[(cursor-3)+i]) );
          }
        }
        display.display();
        break;
    }
    int input = -1;
    while( input == -1 ){
      input = RotEnc.getNextInput();
    }

    switch( input ){
      case CCW:
        if( cursor > 0 ) cursor--;
        break;
      case CW:
        if( cursor < activePIDs_n ) cursor++;
        break;
      case BUTTONPRESS:
        return cursor;
    }
  }
}

String textEdit( String str, String title ){
  str.reserve(TEXT_MAXLEN);
  int titleCursor = 64 - ( ( title.length() * 6 ) / 2 );
  int cursor = 0;
  int charEdit = 0;
  int input = 0;

  while(1) {
    display.clearDisplay();
    display.setCursor( titleCursor, 0 );
    display.print( title );
    display.setCursor( 3, 30 );
    display.print( str );
    display.setCursor( 3 + ( cursor * 6 ), 38 );
    if( cursor == TEXT_MAXLEN ) {
      display.setCursor( 50, 50 );
      display.print( "OK?" );
    } else {
      ( charEdit ) ? display.printf( "T" ) : display.printf( "^" );
    }
    display.display();

    input = 0;
    while( !input ) {
      input = RotEnc.getNextInput();
    }

    if( charEdit ) {
      switch( input ) {
        case CW:
          switch( str[cursor] ) { 
            case 33: str[cursor] = 48; break; 
            case 57: str[cursor] = 65; break; 
            case 90: str[cursor] = 97; break;
            case 122: str[cursor] = 32; break;
            default: str[cursor]++; break;
          }
          break;
        case CCW:
          switch( str[cursor] ) { 
            case 32: str[cursor] = 122; break; 
            case 97: str[cursor] = 90; break; 
            case 65: str[cursor] = 57; break;
            case 48: str[cursor] = 33; break;
            default: str[cursor]--; break;
          }
          break;
        case BUTTONPRESS:
          charEdit--;
          break;
      }
    } else {
      switch( input ) {
        case CW:
          if( cursor < TEXT_MAXLEN ) cursor++;
          break;
        case CCW:
          if( cursor > 0 ) cursor--;
          break;
        case BUTTONPRESS:
          if( cursor < TEXT_MAXLEN ) {
          charEdit++;
            if( cursor+1 > str.length() ) {
              for( int i = str.length(); i <= cursor; i++ ) {
                str += " ";
              }
            }
          } else {
            str.trim();
            return str;
          }
          break;
      }
    }
  }

}

void menuSystem() {
  int state = MAINMENU;
  int prevCursor[3] = {0,0,0};
  while( state != -1 ) {
    switch( state ) {
      case MAINMENU:
        state = 1 + menuSelector( mainMenuLabels, "Main Menu", prevCursor[0] );
        prevCursor[0] = state - 1;
        break;
      case DATADISPLAY:
        state = 1 + menuSelector(  )
        switch( menuSelector( gaugeMenuLabels, "Data Display", prevCursor[1] ) ) {
          case 0:
            settings.dataDisplayStyle = menuSelector( displayStyleLabels, "Data Display Style", settings.dataDisplayStyle );
            break;
          case 1:
            PIDSelector( hist.pid );
            prevCursor[1] = 1;
            break;
          case 2:
            state = 0;
            prevCursor[1] = 0;
            break;
        }
        break;
      case OUTPUTCONTROL: {
        prevCursor[0] = OUTPUTCONTROL - 1;
        int n = menuSelector( settings.outputNames, "Output Control", prevCursor[1] );
        if( n == 4 ) { state = MAINMENU; break; };
        switch( menuSelector( outputControlLabels, settings.outputNames[n], prevCursor[2] ) ) {
          case 0: settings.outputNames[n] = textEdit( settings.outputNames[n], "Output Name" ); break;
        }
        } break;
      default:
        state = -1;
        break;
    }
  }
}

void checkAlerts() {
  if( ( alertQueue[0].state != -1 ) && !alertActive ) {
    alertActive = 1;
    alertTime = millis();
  } else if( alertActive && ( millis() - alertTime >= ALERT_TIME ) ) {
    alertActive = 0;
    alertTime = 0;
    for( int i = 0; i < ALERT_MAX-1; i++ ) {
      alertQueue[i] = alertQueue[i+1];
    }
    alertQueue[ALERT_MAX] = { -1, -1 };
    ( alertLast > 0 ) ? alertLast-- : alertLast = 0;
  }

  if( alertActive ) {
    String name = settings.outputNames[alertQueue[0].output];
    String state = stateName[alertQueue[0].state];
    int rectWidth = ( name.length() * 6 ) + 5;
    int x = ( 128 - rectWidth ) / 2;
    int xS = ( 128 - ( state.length() * 6 ) ) / 2;
    display.fillRect( x, 0, rectWidth, 24, BLACK );
    display.drawRect( x, 0, rectWidth, 24, WHITE );
    display.setCursor( x+3, 3 );
    display.println( name );
    display.setCursor( xS, 12 );
    display.println( state );
  }
}

void outputStateAlert( int output, int state ) {
  if( alertLast <= ALERT_MAX ) { alertQueue[alertLast] = { output, state }; alertLast++; }
}