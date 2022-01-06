// RotEnc.h - Simple Rotary Encoder Library - Aaron Jones
#ifndef ROTENC_H
#define ROTENC_H

#include <Arduino.h>

#define ROTENC_IN1    PB3
#define ROTENC_IN2    PB4
#define ROTENC_BUTTON PB5

#define CW 1
#define CCW 2
#define BUTTONPRESS 3

#define LONGPRESSMS 800

extern unsigned long int in1Debounce;
extern unsigned long int in2Debounce;
extern unsigned long int buttonDebounce;

void in1_ISR();
void in2_ISR();
void button_ISR();

class RotaryEncoder{
  int firstInt = 0;
  int queue[5] = {0};
  int queueLast = -1;

  int downMillis;

public:
  RotaryEncoder();
  void begin();
  void input1Handler();
  void input2Handler();
  void buttonHandler();
  int getNextInput();
};

extern RotaryEncoder RotEnc;

#endif