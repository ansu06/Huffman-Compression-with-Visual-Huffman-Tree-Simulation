#ifndef ADAPTIVE_H
#define ADAPTIVE_H

#define MAX_STEPS 1000
#define ALPHA 256

typedef struct {
    int ctx;
    int ch;
    char code[50];
} Step;

void adaptivePPM(char *text);

extern int contextFreq[256][256];

Step* getSteps();
int getStepCount();

#endif
