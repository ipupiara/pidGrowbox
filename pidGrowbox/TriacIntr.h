

#ifndef TriacIntr_H
	#define TriacIntr_H

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
#include "TriacDefines.h"

#define adcRefVoltage5   5.0
#define adcRefVoltad2d5  2.56
	
int8_t durationTimerReachead;
int8_t runningSecondsTick;

int16_t secondsRemainingInDurationTimer;

int16_t secondsInDurationTimer;

uint16_t triacFireDurationTcnt0;   // centi-millis-secs, not exactly but approximate, PID will handle the rest



void startDurationTimer(int16_t secs);
int16_t getSecondsRemainingInDurationTimer();
int16_t getSecondsInDurationTimer();

void stopDurationTimer();

void initHW();
void setTriacFireDuration(uint16_t cmsecs);
uint16_t  getTriacFireDuration();
void startTriacRun();
void stopTriacRun();

void getLatestClimateValues(floatType* pTemp,floatType* pHum);    // interface to hygrosense, called by user functions
floatType getCurrentTemperature();
floatType getCurrentHumidity();
floatType getCurrentTemperatureVoltage();
uint8_t  dataReceived;
void onDataReceived();

int8_t adcTick;

uint8_t minutesCounter, secondsCounter;
uint16_t hoursCounter;


void initADC();
int16_t adcValue(uint8_t pos);
floatType adcVoltage(uint8_t  pos);
uint8_t getADCTemperature(uint8_t  pos, floatType* result);
void startADCSequence();
int8_t startNextADC ();
int16_t getTriacDelayValueFromADC(uint8_t pos);
uint16_t getLastAdcValue(uint8_t  pos);
void startSecondTick();
int16_t getSecondsDurationTimerRemaining();
int16_t getSecondsRemainingInDurationTimer();
uint32_t overallSeconds();
void printfTime();
#endif
