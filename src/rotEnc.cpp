#include "rotEnc.h"

unsigned long int in1Debounce = 0;
unsigned long int in2Debounce = 0;
unsigned long int buttonDebounce = 0;

void in1_ISR() {
  if( millis() - in1Debounce > 5 ) { RotEnc.input1Handler(); }
  in1Debounce = millis();
}

void in2_ISR() {
  if( millis() - in2Debounce > 5 ) { RotEnc.input2Handler(); }
  in2Debounce = millis();
}

void button_ISR() {
  if( millis() - buttonDebounce > 100 ) { RotEnc.buttonHandler(); }
  buttonDebounce = millis();
}

RotaryEncoder::RotaryEncoder() {
  // Allocate rotary encoder pins
  pinMode( ROTENC_IN1, INPUT_PULLUP );
  pinMode( ROTENC_IN2, INPUT_PULLUP );
  pinMode( ROTENC_BUTTON, INPUT_PULLUP );
}

void RotaryEncoder::begin() {
  // Attach interrupts
  attachInterrupt( digitalPinToInterrupt( ROTENC_IN1 ), in1_ISR, CHANGE);
  attachInterrupt( digitalPinToInterrupt( ROTENC_IN2 ), in2_ISR, CHANGE);
  attachInterrupt( digitalPinToInterrupt( ROTENC_BUTTON ), button_ISR, FALLING);
}

void RotaryEncoder::input1Handler() {
        noInterrupts();
        if( digitalRead( ROTENC_IN1 ) != digitalRead( ROTENC_IN2 )) {
            queueLast++;
            queue[queueLast] = CCW;
            firstInt = 0;
        }
        interrupts();
    }

void RotaryEncoder::input2Handler() {
        noInterrupts();
        if( digitalRead( ROTENC_IN2 ) != digitalRead( ROTENC_IN1 ) ) {
            queueLast++;
            queue[queueLast] = CW;
            firstInt = 0;
        }
        interrupts();
    }

void RotaryEncoder::buttonHandler() {
    queueLast++;
    queue[queueLast] = BUTTONPRESS;
}

int RotaryEncoder::getNextInput() {
    switch( queueLast ) {
        case -1: return 0;
        break;
        case 0: { int n = queue[queueLast]; queue[queueLast] = 0; queueLast--; return n; }
        break;
        default: 
            int n = queue[queueLast];
            for( int i = 0; i < queueLast; i++ ) { 
                queue[i] = queue[i+1];
            }
            queue[queueLast] = 0;
            queueLast--;
            return n;
            break;
    }
}

RotaryEncoder RotEnc;