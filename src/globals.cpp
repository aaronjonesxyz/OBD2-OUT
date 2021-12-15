#include "globals.h"

int activePIDs[96];
std::bitset<96> enabledPids = {0};
int numPids = 0;
int currentPIDValues[96] = {0};

unsigned long int in1Debounce = 0;
unsigned long int in2Debounce = 0;
unsigned long int buttonDebounce = 0;