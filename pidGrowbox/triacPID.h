
#ifndef triacPIDh
#define triacPIDh

#include<stdint.h>
#include <math.h>

#define pidIntervalSecs  60
#define desiredTemperature  27.0
#define csvIntervalSecs  20

#define initialTriacDelayValue   0x0110	

//#define printfPid
//#define printfAmps


// Select 'double' or 'float' here:
typedef float real;


void initPID();

void resetPID();

void printCsvHeader();
void printCsvValues();

real Update(real error);

void calcNextTriacDelay();

void onPidStep();


#endif
