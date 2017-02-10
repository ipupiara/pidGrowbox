
#ifndef triacPIDh
#define triacPIDh

#include<stdint.h>
#include <math.h>

#define pidIntervalSecs  60
#define desiredTemperature  17.0

// Select 'double' or 'float' here:
typedef double real;


void InitPID();

void resetPID();

void printPIDState();

void onEntryIdlePID();

void onTriacIdleSecondTick_PID();

real Update(real error);

void calcNextTriacDelay();

void onPidStep();


#endif
