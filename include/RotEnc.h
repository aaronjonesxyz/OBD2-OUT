// RotEnc.h - Rotary Encoder Library - Aaron Jones
#ifndef ROTENC_H
#define ROTENC_H

#define ROTENC_IN1    PB3
#define ROTENC_IN2    PB4
#define ROTENC_BUTTON PB5

#define CW 1
#define CCW 2
#define BUTTONPRESS 3

#define LONGPRESSMS 800

class RotaryEncoder {
    int firstInt = 0;
    int queue[5] = { 0 };
    int queueLast = -1;

    int downMillis;

    public:

    void input1Handler() {
        noInterrupts();
        if( digitalRead( ROTENC_IN1 ) != digitalRead( ROTENC_IN2 )) {
            queueLast++;
            queue[queueLast] = CCW;
            firstInt = 0;
        }
        interrupts();
    }

    void input2Handler() {
        noInterrupts();
        if( digitalRead( ROTENC_IN2 ) != digitalRead( ROTENC_IN1 ) ) {
            queueLast++;
            queue[queueLast] = CW;
            firstInt = 0;
        }
        interrupts();
    }

    void buttonHandler() {
        queueLast++;
        queue[queueLast] = BUTTONPRESS;
    }

    int getNextInput() {
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
};

RotaryEncoder RotEnc;

#endif