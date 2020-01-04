

#ifndef TriacIntr_H
	#define TriacIntr_H

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
#include "TriacDefines.h"

#define controlheating   // only was used when heating and humidifying needed same relais-port

#define adcRefVoltage5   5.0
#define adcRefVoltad2d5  2.56

#define GETTimeValues  uint16_t hrs; uint8_t mins; uint8_t secs; getTimeValues(&hrs,&mins,&secs);
	
uint8_t durationTimerReachead;
uint8_t runningSecondsTick;

uint16_t secondsRemainingInDurationTimer;

uint16_t secondsInDurationTimer;

uint16_t triacFireDurationTcnt;   // centi-millis-secs, not exactly but approximate, PID will handle the rest

int8_t heatingIsOn;

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
uint8_t  dataReceivedUart1;
uint8_t onDataReceivedUart1IsValid();

int8_t adcTick;

uint8_t minutesCounter, secondsCounter;
uint16_t hoursCounter;


void initADC();
uint16_t adcValue(uint8_t pos);
floatType adcVoltage(uint8_t  pos);
uint8_t getADCTemperature(uint8_t  pos, floatType* result);
void startADCSequence();
int8_t startNextADC ();
int16_t getTriacFireDurationFromADC(uint8_t pos);
uint16_t getLastAdcValue(uint8_t  pos);
void startSecondTick();
int16_t getSecondsDurationTimerRemaining();
int16_t getSecondsRemainingInDurationTimer();
uint32_t overallSeconds();
void getTimeValues(uint16_t* hrs, uint8_t* mins, uint8_t* secs);

void startHumidifying();
void stopHumidifying();
void startVentilating();
void stopVentilating();
void startDrying();
void stopDrying();

uint16_t addToOutUart0(char* txt, uint16_t len);

#endif
