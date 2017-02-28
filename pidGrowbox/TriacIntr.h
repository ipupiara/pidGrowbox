

#ifndef TriacIntr_H
	#define TriacIntr_H

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
	
int8_t durationTimerReachead;
int8_t runningSecondsTick;


void startDurationTimer(int16_t secs);
int16_t getSecondsRemainingInDurationTimer();
int16_t getSecondsInDurationTimer();

void stopDurationTimer();

void initHW();
void setTriacFireDuration(uint16_t cmsecs);
uint16_t  getTriacFireDuration();
void startTriacRun();
void stopTriacRun();


void getLatestClimateValues(float* pTemp,float* pHum);    // interface to hygrosense, called by user functions
float getCurrentTemperature();
float getCurrentHumidity();
float getCurrentTemperatureVoltage();
uint8_t  dataReceived;
void onDataReceived();

int8_t adcTick;

uint8_t minutesCounter, secondsCounter;
uint16_t hoursCounter;


void initADC();
uint8_t getADCTemperature(uint8_t  pos, float* result);
void startADCSequence();
int8_t startNextADC ();
int16_t getTriacDelayValueFromADC(uint8_t pos);
uint16_t getLastAdcValue(uint8_t  pos);
void startSecondTick();
uint32_t overallSeconds();
#endif
