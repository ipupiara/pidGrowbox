#ifndef triacUIh
#define triacUIh

#include <inttypes.h>

#define shortCircuitAlarmSupported  1


void clr_scr();

void displayCurrentAmps();

void displayCountDown();

void displayInDurationTimerSince();

void displayTriacRunning();

void initUI();

void displayDebugVoltageNTriggerDelay();

void displayVoltage();

void displayFatalError();

void displayMeasuredValues(floatType hum, floatType temp);


#endif
