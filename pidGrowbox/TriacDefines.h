

// TriacDefines.h contains global and some local defines, but not all of the local defines
// also it contains many of the global data definition and their persistent storage on eeprom

// Yoar Attention please, yoar attention: set Brown out level on 2.7V at least
// otherwise errors on the eeprom could be observed at startup

#ifndef TriacDefines_H
	#define TriacDefines_H


	#include <inttypes.h>

	#define JT_F_CPU 11095200	
	
//	#define printCsvData     //  if printCsvValues is set, only csv values must be printed out
	
	#define maxSecsPossible  0x7FFF
	#define triggerDelayMaxTcnt0 810     // works properly with 50 Hz Ac and 11 Mhz
	
	#define inductiveLoad   1      // to be set to 0 or 1
	#define triacTriggerLength   0      // delay approx ( n * 2.5 us ) + 5.5    at 11.0592  mhz
	#define delayBetweenTriacTriggers  16  //  in timer 2 tcnt values , prescaler at   128
	#define measuredRepetitionIntervalus  200;      //  approx   ( 1s  /(11.0592 E+6  /128) ) * delayBetweenTriacTriggers    [us]

// takes approx  2 us at 10 Mhz with -o0 (no opts. !) ,with all  call/ret, push N Pop

void info_printf( char *emsg, ...);

void delayEmptyProc (); 

uint8_t  hygrosenseMsgCnt;

void delay6pnt2d5us(unsigned int n);   // description in TriacDefines.c


int8_t fatalErrorOccurred;

void initDefines();

#endif
