#include <avr/io.h>
#include <stdio.h>
#include "TriacIntr.h"
#include "triacUI.h"
#include "st7565r.h"
#include "TriacDefines.h"
#include "triacPID.h"


void displayVoltage()
{
	double   VFl;
	char buffer [13];
	
	VFl = adcVoltage(0);
	
	snprintf((char*)&buffer,sizeof(buffer),"%5.2fV/%4iD",VFl,getTriacFireDuration());

	lcd_goto(3, 6);
	lcd_write_str((char*)&buffer);
}

void displayDebugVoltageNTriggerDelay()
{
	double   VFl;
	char buffer [16];
	int16_t adc =  adcValue(0);

	VFl = adcVoltage(0);

	snprintf((char*)&buffer,sizeof(buffer),"%5.2fV %4iA %3iD",VFl,adc, getTriacFireDuration());

	lcd_Line1();
	lcd_write_str((char*)&buffer);

}


void displayFatalError()
{
	lcd_clrscr();
	lcd_write_str("fatal error");
	lcd_Line1();
	lcd_write_str(lastFatalErrorString);
}




void clr_scr()
{
	lcd_clrscr();
}


void displayCountDown()
{
	int16_t secondsRem = getSecondsRemainingInDurationTimer();
	int32_t minRem  = secondsRem / 60; ;        //(250 us on 1Mhz Simulator)
	int16_t mRem = minRem;
	int8_t secsRem	= secondsRem - (minRem * 60);  // subtraction + multiply by 60 faster than division
													// 25 us on 1 Mhz Sim 2 
	char buffer [8];
	snprintf((char*)&buffer,sizeof(buffer),"%3im%02i",mRem,secsRem);

	lcd_goto(1,9);
	lcd_write_str((char*)&buffer);
}

void displayInDurationTimerSince()
{
	int16_t secondsRel = getSecondsInDurationTimer();
	int32_t minRel  = secondsRel / 60; ;        //(250 us on 1Mhz Simulator)
	int16_t mRel = minRel;
	int8_t secsRel	= secondsRel - (minRel * 60);  // subtraction + multiply by 60 faster than division
													// 25 us on 1 Mhz Sim 2 
	char buffer [8];
	sprintf((char*)&buffer,"%3im%02i",mRel,secsRel);

	lcd_goto(1,6);
	lcd_write_str((char*)&buffer);
}


void displayTriacRunning()
{
	lcd_clrscr();
	lcd_write_str("Running ");
	lcd_goto(3,0);
	lcd_write_str("R stop");
}


void initUI()
{
	info_printf("initUI\n");
}


void displayMeasuredValues(floatType hum, floatType temp)
{
	char buffer [20];


	snprintf((char*)&buffer,sizeof(buffer),"h: %5.1f t:%5.1f ",hum,temp);

	
	lcd_goto(0,0);
	lcd_write_str((char*)&buffer);
}

