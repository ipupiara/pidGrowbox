
#include <avr/io.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include "TriacIntr.h"
#include "TriacDefines.h"
#include "TriacKeyPad.h"



int8_t lastCharPressed;

#define IntrMsk  PCMSK2
#define PCICRPos  PCIE2
#define PCINTVECT  PCINT2_vect   

//#define printKeybState

#define keypadSignalDelayFaktor  8

// #define counting

#ifdef counting
  int64_t pcintCnt;
#endif

int8_t lastValueZero; 


#ifdef counting
	void testIncCnt()
	{
		pcintCnt++;
		if (pcintCnt == INT64_MAX)  {
			sprintf((char *) &lastFatalErrorString,"pcintCnt overflow\n");
			fatalErrorOccurred = 1;
		}
	}
#endif


int16_t  getKeybIntCnt()
{
	int16_t res= 0;
#ifdef counting
	cli();	
	res = pcintCnt;
	sei();
#endif	
	return res;
}

void onTriacIdleSecondTick_Keyb()
{
#ifdef printKeybState
	int16_t secs;
	secs = getSecondsDurationTimerRemaining();
	if ((secs & 0x001f) == 0x0000) {
		int16_t res = getKeybIntCnt();
		info_printf("kybIntCnt: %i lastCh: %X lastV0: %X\n",res , lastCharPressed, lastValueZero);
	}
#endif
}



void initUsartInput()
{
	// USART_init()  has already set most of usart setup, so only a few has to be done
	UCSR0B |= (1<< RXCIE0) | (1<<RXEN0) ;
	
}

void initKeyPad()
{
	lastCharPressed = 0;
	lastValueZero = 1;
#ifdef counting	
	pcintCnt = 0;
#endif	
	

	info_printf("jtagDebugKeyboardMode\n");
	initUsartInput();


	sei(); //  enable all interrupts if not yet done so
}

ISR(USART0_RX_vect)
{
	int8_t rxCh;
	rxCh = UDR0;
	
#ifdef counting
testIncCnt();
#endif	

	switch (rxCh)
	{
		case '1' :	{
			lastCharPressed = kp1;
			break;
		}
		case '2' : {
			lastCharPressed = kp2;
			break;
		}
		case '3' : {
			lastCharPressed = kp3;
			break;
		}
		case '4' :	{
			lastCharPressed = kp4;
			break;
		}
		case '5' : {
			lastCharPressed = kp5;
			break;
		}
		case '6' : {
			lastCharPressed = kp6;
			break;
		}
		case '7' :	{
			lastCharPressed = kp7;
			break;
		}
		case '8' : {
			lastCharPressed = kp8;
			break;
		}
		case '9' : {
			lastCharPressed = kp9;
			break;
		}
		case '0' : {
			lastCharPressed = kp0;
			break;
		}
		case '#' : {
			lastCharPressed = kpNum;
			break;
		}		
		case '*' : {
			lastCharPressed = kpAst;
			break;
		}
		case 'A' : {
			lastCharPressed = kpF1;
			break;
		}
		case 'B' : {
			lastCharPressed = kpF2;
			break;
		}
		case 'R' : {
			lastCharPressed = kpStop;
			break;
		}
		case 'G' : {
			lastCharPressed = kpStart;
			break;
		}
		default : {
			lastCharPressed = 0;
			break;
		}
	}
	
	
}



int8_t keyEntered()  // with interrupts
{
	int8_t res;
	cli();  
	res = lastCharPressed;
	lastCharPressed = 0;
	sei();
	return res;
}

