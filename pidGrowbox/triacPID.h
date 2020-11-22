
#ifndef triacPIDh
#define triacPIDh

#include<stdint.h>
#include <math.h>


#define desiredTemperature  27.0

#define pidIntervalSecs  60

#define totFactor  10   * (60 / pidIntervalSecs)   //  to keep the correction per time the same when changing the step width
														// need not necessarily be the case this way, maybe this factor would be better let away,
														// experiment will show .. 
#define pFactor   -2.5 
#define iFactor  -0.0025  
#define dFactor -500	


#define itegralTreshold 0.7
#define integralTimeDiminuision    0.9
#define correctionThreshold  150  * (60 / pidIntervalSecs)

#define csvIntervalSecs  20
//#define csvIntervalSecs  1   // just for debugging 

#define initialTriacDelayValue   0x0200	

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

real getTwaTemperature();

real getTwaAbsTemperatureDiff();


#endif
