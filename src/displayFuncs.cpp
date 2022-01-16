#include <vector>
#include <Arduino.h>
#include <Adafruit_SSD1306.h>
#include <OBD2.h>
#include "rotEnc.h"
#include "globals.h"
#include "gauges.h"
#include "displayFuncs.h"

alert_t alertQueue[ALERT_MAX]  = { { NULL, -1  } };;
int alertLast = 0;
unsigned long int alertTime = 0;
int alertActive = 0;

int menuSelector( std::vector< String > items, String title, int cursor = 0 ) { 
  display.setTextSize(1);
  int titleCursor = 64 - ( ( title.length() * 6 ) / 2 );
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

int numberSelector( int min, int max, String title, int number ) {
  display.setTextSize(1);
  int titleCursor = 64 - ( ( title.length() * 6 ) / 2 );
  if( number < min ) number = min;
  int cursor = 0;
  int cursorX = 20;
  int digitEdit = 0;
  int input = 0;

  while( 1 ){
    cursorX = 50 + ( cursor * 6 );
    display.clearDisplay();
    display.setCursor(titleCursor, 0);
    display.print( title );
    String rangeString = "Range: " + String(min) + " to " + String(max);
    display.setCursor( 64-( rangeString.length() * 3), 16 );
    display.print( rangeString );
    if( number >= 0 ) {
      display.setCursor( 50, 50 );
      display.printf( "%04d", number );
    } else {
      display.setCursor( 44, 50 );
      display.printf( "%05d", number );
    }
    if( cursor == 4 ) display.setCursor( 100, 50 ), display.print( "|OK|"); else display.setCursor( 106, 50 ), display.print( "OK");

    display.setCursor( cursorX, 58 );
    if( cursor < 4 ) ( digitEdit ) ? display.printf( "T" ) : display.printf( "^" );
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

int PIDSelector( int cursor ) { // Displays the supplied list of PIDs in a list with a selection cursor
  display.setTextSize(1);
  while(1) {
    display.clearDisplay();
    display.setCursor(0,24);
    ( cursor > -1 ) ? ( settings.supportedPIDs[cursor] ) ? display.print("*") : display.print(">") : display.print("-");
    uint8_t textCursor;
    uint8_t textCursorAdd;
    uint8_t numDisplay;
    switch( cursor ) {
      case -1: 
        textCursor = 24;
        textCursorAdd = 32;
        numDisplay = 4;
        break;
      case 0 ... 2:
        textCursor = 16 - ( 8 * cursor );
        textCursorAdd = 24 - ( 8 * cursor );
        numDisplay = 5 + cursor;
        break;
      default:
        textCursorAdd = 0;
        numDisplay = 8;
        break;
    }
    if( cursor < 3 ) {
      display.setCursor( 7, textCursor );
      display.print("Back");
    }

    for( int i = 0; i < numDisplay; i++ ){
      uint8_t PID_n = ( cursor - ( numDisplay - 5 ) ) + i;
      if( PID_n <= masterPID_n ) {
        textCursor = textCursorAdd + ( i * 8 );
        display.setCursor( 0, textCursor );
        ( settings.supportedPIDs[PID_n] ) ? display.print( "*" ) : display.print(" ");
        display.print( OBD2.pidName( masterPIDList[PID_n] ) );
      }
    }
    display.display();

    int input = -1;
    while( input == -1 ){
      input = RotEnc.getNextInput();
    }

    switch( input ){
      case CCW:
        if( cursor > -1 ) cursor--;
        break;
      case CW:
        if( cursor < masterPID_n ) cursor++;
        break;
      case BUTTONPRESS:
        return cursor;
    }
  }
}

String textEdit( String str, String title ){
  display.setTextSize(1);
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
    switch( cursor ) {
      case -1: display.setCursor( 0, 50 ); display.print( "|CLEAR|" ); display.setCursor( 110, 50 ); display.print( "OK" ); break;
      case TEXT_MAXLEN: display.setCursor( 104, 50 ); display.print( "|OK|" ); display.setCursor( 6, 50 ); display.print( "CLEAR" ); break;
      default: ( charEdit ) ? display.printf( "T" ) : display.printf( "^" ); display.setCursor( 110, 50 ); display.print( "OK" ); display.setCursor( 6, 50 ); display.print( "CLEAR" ); break;
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
          if( cursor < TEXT_MAXLEN ) cursor++; else cursor = -1;
          break;
        case CCW:
          if( cursor > -1 ) cursor--; else cursor = TEXT_MAXLEN;
          break;
        case BUTTONPRESS:
          if( cursor == -1 ) { str = ""; }
          else if( cursor < TEXT_MAXLEN ) {
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
  display.setTextSize(1);
  int state = MAINMENU;
  uint8_t prevCursor[3] = {0,0,0};
  uint8_t ctrlNum = 0;
  while( state != -1 ) {
    switch( state ) {
      case MAINMENU:
        state = 1 + menuSelector( mainMenuLabels, "Main Menu", prevCursor[0] );
        if( state == 4 ) state = -1;
        prevCursor[0] = state - 1;
        break;
      case DATADISPLAY:
        state = DATADISPLAY_STYLE + menuSelector( gaugeMenuLabels, "Data Display", prevCursor[1] );
        prevCursor[1] = state - DATADISPLAY_STYLE;
        if( state == 9 ) state = 0, prevCursor[1] = 0;
        break;
      case LOGGING_FREQUENCY:
        settings.loggingFreq = numberSelector( 100, 5000, "Frequency in mS", settings.loggingFreq );
        state = MAINMENU;
        break;
      case OUTPUTCONTROL: {
        std::vector<String> _outControl;
        _outControl.reserve(4);
        for( auto& o : settings.outControl ) {
          _outControl.push_back( o.name );
        }
        state = OUTPUTCONTROL_MENU + menuSelector( _outControl, "Output Control", prevCursor[1] );
        prevCursor[1] = state - OUTPUTCONTROL_MENU;
        if( state == OUTPUTCONTROL_MENU + 4 ) state = MAINMENU, prevCursor[1] = 0;
        break; }


      case DATADISPLAY_STYLE:
        settings.dataDisplayStyle = menuSelector( displayStyleLabels, "Data Display Style", settings.dataDisplayStyle );
        state = DATADISPLAY;
        break;
      case DATADISPLAY_DATA: {
        uint8_t c = 0;
        uint8_t PIDs_n;
        ( settings.dataDisplayStyle ) ? PIDs_n = settings.dataDisplayStyle : PIDs_n = 1;
        while( c != PIDs_n ){
          std::vector<String> _PIDs;
          _PIDs.reserve( PIDs_n );
          for( int i = 0; i < PIDs_n; i++ ) {
            _PIDs.push_back(OBD2.pidName( masterPIDList[settings.dataDisplayPIDs[i]] ));
          }
          c = menuSelector( _PIDs, "Data Display PIDs", c );
          if( c < PIDs_n ) {
            int p = PIDSelector();
            if( p > -1 ) settings.dataDisplayPIDs[c] = p, xyPlotter.newData( PID );
          } else {
            break;
          }
        }
        state = DATADISPLAY;
        break; }
      case DATADISPLAY_GRAPHRANGEH: {
        int old = settings.graphRangeH;
        settings.graphRangeH = numberSelector( settings.graphRangeL, 9999, "Graph Upper Limit", settings.graphRangeH );
        if( settings.graphRangeH != old ) xyPlotter.newData( RANGE );
        state = DATADISPLAY;
        break; }
      case DATADISPLAY_GRAPHRANGEL: {
        int old = settings.graphRangeL;
        settings.graphRangeL = numberSelector( -40, settings.graphRangeH, "Graph Lower Limit", settings.graphRangeL );
        if( settings.graphRangeL != old ) xyPlotter.newData( RANGE );
        state = DATADISPLAY;
        break; }

      case OUTPUTCONTROL_MENU ... OUTPUTCONTROL_MENU+3: {
        ctrlNum = state - OUTPUTCONTROL_MENU;
        uint8_t c = menuSelector( outputControlLabels, settings.outControl[ctrlNum].name, prevCursor[2] );
        prevCursor[2] = c;
        if( c == 3 ) { state = OUTPUTCONTROL; prevCursor[2] = 0; break; }
        if( c ) {
          state = (OUTPUTCONTROL_MENU+3) + c;
        } else {
          String title = "Output ";
          title = title + ( ctrlNum + 1);
          settings.outControl[ctrlNum].name = textEdit( settings.outControl[ctrlNum].name, title );
        }
        break; }

      case OUTPUTCONTROL_PIDS: {
        uint8_t c = 0;
        while( c != 3 ){
          std::vector<String> _PIDs;
          _PIDs.reserve(3);
          _PIDs.push_back( OBD2.pidName( masterPIDList[settings.outControl[ctrlNum].PIDs[0]] ) );
          _PIDs.push_back( OBD2.pidName( masterPIDList[settings.outControl[ctrlNum].PIDs[1]] ) );
          _PIDs.push_back( OBD2.pidName( masterPIDList[settings.outControl[ctrlNum].PIDs[2]] ) );
          c = menuSelector( _PIDs, settings.outControl[ctrlNum].name, c );
          if( c < 3 ) {
            uint8_t p = PIDSelector();
            if( p > -1 ) settings.outControl[ctrlNum].PIDs[c] = p;
          } else {
            state = ctrlNum + 9;
            break;
          }
        }
        break; }

      case OUTPUTCONTROL_CTRLLOGIC: {
        
      break; }
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
    alertQueue[ALERT_MAX] = { NULL, -1 };
    ( alertLast > 0 ) ? alertLast-- : alertLast = 0;
  }

  if( alertActive ) {
    String name = alertQueue[0].output->name;
    String state = stateName[alertQueue[0].state];
    int rectWidth;
    ( name.length() > 3 ) ? rectWidth = ( name.length() * 6 ) + 5 : rectWidth = 29;
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

void outputStateAlert( OutputControllerClass *output, int state ) {
  if( alertLast <= ALERT_MAX ) { alertQueue[alertLast] = { output, state }; alertLast++; }
}

void loadPIDs() {
  if( !OBDConnected ) { return; }
  for (int pid = 0; pid < masterPID_n; pid++) {
    settings.supportedPIDs[pid] = OBD2.pidSupported( masterPIDList[pid]);
  }
}