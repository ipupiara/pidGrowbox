/*
 * pidGrowbox.c
 *
 * Created: 27.01.2017 17:31:55
 *  Author: ixchel
 */ 


#include <avr/io.h>
#include <stdio.h>
#include <string.h>
#include "TriacDefines.h"
#include "TriacIntr.h"
#include <avr/wdt.h>
#include "triacPID.h"
#include "StateClass.h"
#include "triacUI.h"
#include "twi_master.h"


#ifdef UseStdOutForUsart0 

void USART_Init( unsigned int baud )
{
	// Set baud rate
	
	UBRR0H = (unsigned char)(baud>>8);
	UBRR0L = (unsigned char)baud;
	// Enable receiver and transmitter
	UCSR0A =  (UCSR0A & 0b11111100) ;
	
	UCSR0B=    (1<<TXEN0);
	//	UCSR0B = 0b00011000;  // rx compl intr ena - tx compl intr ena - dreg empty intr ena - rx ena - tx ena - sz2 (size bit 2)  - 9. bit rx - 9. tx

	UCSR0C = 0b00000110; // "00" async usart - "00" disa parity - 1 (or 2) stop bit - sz1 - sz0 (set t0 8 bit) - clk polarity (sync only)
}


static int uart_putchar(char c, FILE *stream);

static FILE mystdout = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);  // first opened will be stdout and errout

//stdout = fdevopen((FILE *)uart_putchar, NULL, 0);

//static int uart_putchar(char c, FILE *stream)
//{
//#warning tobe tested whenever this code should be used again		
	//if (c == '\n')
	//{
		//uart_putchar('\r',&mystdout);
	//}
	//while (!(UCSR0A & (1<<UDRE0)));
	//UDR0 = c;
	//return 0;
//}

#endif

void setWatchdogTimerOn()
{
	uint8_t val = ((1<<WDP0) | (1<<WDP1)| (1<<WDP2));
	wdt_enable(val );
	//wdt_reset();
	///* Write logical one to WDCE and WDE */
	//WDTCR |= (1<<WDCE) | (1<<WDE);
	///* Turn off WDT */
	//WDTCR = ((1<<WDP0) | (1<<WDP1)| (1<<WDP2) | (1 << WDE));
}

void setWatchdogTimerOff()
{
	//	wdt_enable(0x4);
	wdt_reset();
	/* Write logical one to WDCE and WDE */
	WDTCSR |= (1<<WDCE) | (1<<WDE);
	/* Turn off WDT */
	WDTCSR = ((1<<WDP0) | (1<<WDP1)| (1<<WDP2) );
}

//for comments see datasheet of AtMega128A on chapter "System Control and Reset"  -> "WatchDog Timer" and ->  "Register Description"

void resetWatchDog()
{
	wdt_reset();
}


uint16_t   pidIntervalCounter;   
uint16_t   csvIntervalCounter ;
uint16_t   secsCounter ;
    
CGrowBoxEvent ev;	
	    
		
int main(void)
{
	setWatchdogTimerOn();   // set this one off during debugging !
//	setWatchdogTimerOff();

#ifdef  UseStdOutForUsart0

	//	USART_Init( 143 );   // baud 4800 at 11.0592 mhz, single uart speed
	//	USART_Init( 71 );   // baud 9600 at 11.0592 mhz, single uart speed
	USART_Init (11 );   // baud 57.6k  at 11.0592 mhz, single uart speed
	stdout = &mystdout;
// #else  initHW will init circular buffer
#endif
	
	initDefines();
	initHW();
	
	info_printf("\nSTARTUP after initHW() pidGrowbox together with stateGrowbox\n");
	
	initPID();
//	initADC();
	printCsvHeader();
	
//	startStateCharts();    // currently only needed for ventilate
	
//	startDurationTimer(maxSecsPossible  );

	csvIntervalCounter = 0;
	pidIntervalCounter = 0;
	secsCounter = 0;
	
	info_printf("\nSTARTUP DONE pidGrowbox together with stateGrowbox\n");

	
    while(1)
    {
		resetWatchDog();
		// while(1) {}   // for testing watchdog
	
 		if (dataReceivedUart1 == 1)  {
 			dataReceivedUart1 = 0;
 			if (onDataReceivedUart1IsValid() > 0) {
				//displayMeasuredValues(getCurrentHumidity(),getCurrentTemperature());
				ev.evType = eValueAssignement;
				ev.humidity = getCurrentHumidity(); 
				ev.temperature = getCurrentTemperature();
//				processTriacEvent(PTriacHumidityChart, &ev);
			 }
 		}
		if (runningSecondsTick == 1)  {
			runningSecondsTick = 0;
			++secsCounter;

 			++ pidIntervalCounter;
 			if (pidIntervalCounter >=  pidIntervalSecs)  {
 				pidIntervalCounter = 0;				
// 				onPidStep();
 			}
			 
 			++csvIntervalCounter;
 			if (csvIntervalCounter >= csvIntervalSecs)   {
 				csvIntervalCounter = 0;
 				printCsvValues();
 			}
//			ev.evType = eSecondsTick;
//			processTriacEvent(PGrowboxI2CChart, &ev);
			 
// 			startADCSequence();
		}   
 		if (adcTick == 1)  {
 			adcTick = 0;
 			startNextADC();
//			debugSetTriacDelayValueFromAdc();
//			displayDebugVoltageNTriggerDelay();
 		}
		if (durationTimerReachead == 1) {
			durationTimerReachead = 0;
			ev.evType = eTimeOutDurationTimer;
//			processTriacEvent(PTriacHumidityChart, &ev);
		} 
		if (twiDataReceived == 1) {
			twiDataReceived = 0;
//			ev.evType = eTWIDataReceived;
//			processTriacEvent(PGrowboxI2CChart, &ev);
		}
	}
}