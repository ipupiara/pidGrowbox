

#ifndef TriacIntr_H
	#define TriacIntr_H

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
	
#define initialTriacDelayValue   200	
	
int8_t durationTimerReachead;
int8_t runningSecondsTick;


int16_t triacFireDurationTcnt0;   // centi-millis-secs, not exactly but approximate, PID will handle the rest

void startDurationTimer(int16_t secs);
int16_t getSecondsDurationTimerRemaining();
int16_t getSecondsInDurationTimer();

void stopDurationTimer();

void initHW();
void setTriacFireDuration(int16_t cmsecs);
void startTriacRun();
void stopTriacRun();


void getLatestClimateValues(float* pTemp,float* pHum);    // interface to hygrosense, called by user functions
float getCurrentTemperature();
uint8_t  dataReceived;
void onDataReceived();

#endif
