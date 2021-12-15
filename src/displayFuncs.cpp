#include <map>
#include <Arduino.h>

int menuSelector( std::map<String, int> options ) { // Displays the supplied map of options with a cursor
  int cursor = 0;

  std::map<String, int>::iterator iter = options.begin();

  while(1) {
    display.clearDisplay();
    display.setCursor(0,24);
    display.print("-");
    switch( cursor ) {
      case 0: {
        int i = 0;
        while( iter != options.end() && i < 5 ) {
            int textCursor = 24 + ( i * 8 );
            display.setCursor( 7, textCursor );
            display.print( iter->first );
            iter++;
            i++;
        }
        display.display();
        break; }
      case 1: {
        int i = 0;
        while( iter != options.end() && i < 5 ) {
            int textCursor = 16 + ( i * 8 );
            display.setCursor( 7, textCursor );
            display.print( iter->first );
            iter++;
            i++;
        }
        display.display();
        break; }
      case 2: {
        int i = 0;
        while( iter != options.end() && i < 5 ) {
            int textCursor = 8 + ( i * 8 );
            display.setCursor( 7, textCursor );
            display.print( iter->first );
            iter++;
            i++;
        }
        for( int i = 0; i < 7; i++ ){
          if( numActivePids >= i ){ 
            int textCursor = 8 + ( i * 8 );
            display.setCursor( 7, textCursor );
            display.print( OBD2.pidName(pids[(cursor-2)+i]) );
          }
        }
        display.display();
        break;
      default:
        for( int i = 0; i < 8; i++ ){
          if( numActivePids >= ((cursor-3)+i) ){
            int textCursor = ( i * 8 );
            display.setCursor( 7, textCursor );
            display.print( OBD2.pidName(pids[(cursor-3)+i]) );
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
        if( cursor < numActivePids ) cursor++;
        break;
      case BUTTONPRESS:
        return cursor;
    }
  }
}

void menuSystem() {
    int ex = 0;
    while( !ex ) {

    }
}