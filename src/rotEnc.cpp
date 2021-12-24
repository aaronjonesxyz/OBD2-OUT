#include "rotEnc.h"

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