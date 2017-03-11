
#ifndef triacPIDh
#define triacPIDh

#include<stdint.h>
#include <math.h>


#define desiredTemperature  27.0

#define pidIntervalSecs  60

#define totFactor  4.5   * (60 / pidIntervalSecs)   //  to keep the correction per time the same when changing the step width
														// need not necessarily be the case this way, maybe this factor would be better let away,
														// experiment will show .. 
#define pFactor   -1.1 
#define iFactor  0.005  
#define dFactor -500	


#define itegralTreshold 1.0
#define correctionThreshold  150  * (60 / pidIntervalSecs)

#define csvIntervalSecs  20

#define initialTriacDelayValue   0x0200	

double currentAmpsValue;

//#define printfPid



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
