
#ifndef triacPIDh
#define triacPIDh

#include<stdint.h>
#include <math.h>

#define pidIntervalSecs  60
#define desiredTemperature  30.0
#define csvIntervalSecs  20

#define initialTriacDelayValue   0x0200	

double currentAmpsValue;

#define printfPid



// Select 'double' or 'float' here:
typedef floatType real;


void initPID();

void resetPID();

void printCsvHeader();
void printCsvValues();

real Update(real error);

void calcNextTriacDelay();

void onPidStep();

void debugSetTriacDelayValueFromAdc();

#endif
