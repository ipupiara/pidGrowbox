
#include <avr/io.h>
#include <stdio.h>

#include "TriacIntr.h"
#include "TriacDefines.h"
#include "triacPID.h"


int16_t remainingTriacTriggerDelayCounts;

int16_t triacTriggerTimeTcnt2;

int16_t secondsDurationTimerRemaining;

int16_t secondsInDurationTimer;

int16_t amtInductiveRepetitions;

int16_t getSecondsDurationTimerRemaining()
{
	int16_t res;
	cli();
	res = secondsDurationTimerRemaining;
	sei();
	return res;
}

int16_t getSecondsInDurationTimer()
{
	int16_t res;
	cli();
	res = secondsInDurationTimer;
	sei();
	return res;
}

#define ocra2aValueMax 0XFC  // still to be defined

void setTcnt2AndOcra2a(int16_t newTcnt2Val,int16_t newOcra2a)
{
	//
	//// timer must be stopped to set tcnt, because else, on an 
	//// unprotected set, the timer itself could interfere with the *non double buffered feature" write access.
	//// resulting in a more or less random set value.
	//int8_t tccr2bStack;
	//tccr2bStack = TCCR2B;
	//TCCR2B = 0b00000000  ;  // CTC, timer stopped		
	//if (TCNT2 != newTcnt2Val) {  // dont set if not needed , because  .....
		//TCNT2 = newTcnt2Val;	
		//if (newOcra2a == (TCNT2 + 1)) {++ newOcra2a; }  // .... updating avoids triggering of next clock cycle, but needs overnext.
	//}
	//OCR2A = newOcra2a;  
	//TCCR2B = tccr2bStack  ; // set previous value
}

void setTriacTriggerDelayValues()
{
	if (remainingTriacTriggerDelayCounts < ocra2aValueMax) {		
		setTcnt2AndOcra2a (0, remainingTriacTriggerDelayCounts);
		triacTriggerTimeTcnt2 += remainingTriacTriggerDelayCounts;
		remainingTriacTriggerDelayCounts = 0;
	} else {
		remainingTriacTriggerDelayCounts -= ocra2aValueMax;
		setTcnt2AndOcra2a(0, ocra2aValueMax);
		triacTriggerTimeTcnt2 +=  ocra2aValueMax;
	}
}

void startTimer2()
{
	//TIFR2 = 0x00;
	//TIMSK2   = 0b00000010;  //  Output Compare A Match Interrupt Enable
	//TCCR2B = 0b00000101  ; // CTC on CC2A , set clk / 128, timer 2 started
}

void stopTimer2()
{
	//TCCR2B = 0b00000000  ;  // CTC, timer stopped
	//TIMSK2  = 0x00;
	//TIFR2 = (1<< OCF2A);    // cleared by writing a "logic" one to the flag
}


void startTriacTriggerDelay( int16_t delayDuration)  // must run protected between cli and sei
{
	if (delayDuration <= 0) { 
		delayDuration = 1;   // just a very short duration, but one that will happen in future
	}
	remainingTriacTriggerDelayCounts = delayDuration;
	setTriacTriggerDelayValues();
	startTimer2();		
}

void setTriacFireDuration(int16_t durationTcnt2)
{
	cli();
	if (durationTcnt2 < triggerDelayMaxTcnt2) {
		if (durationTcnt2 > 0) {
			triacFireDurationTcnt2 = durationTcnt2;}
		else {
			triacFireDurationTcnt2 = 0;
		}
	} else {
		triacFireDurationTcnt2 = triggerDelayMaxTcnt2;
	}
	sei();
}

void calcAmtInductiveRepetitions(int16_t triacFireDurationTcnt2)
{
	if ( inductiveLoad)  {
		float amtInductiveRepetitionsF = 0.0;
		float triacFireDurationTcnt2F = triacFireDurationTcnt2;
//		amtInductiveRepetitions = ((triacFireDurationTcnt2 * ( 1  /(11.0592e+6  /128) )) * 1.0e+6  ) /  measuredRepetitionIntervalus; 
		amtInductiveRepetitionsF = (triacFireDurationTcnt2F * 11.63  )  /  measuredRepetitionIntervalus; 
		// always cut off modulo part when converting to int
		amtInductiveRepetitions = amtInductiveRepetitionsF;   // tobe  debugged
	} else {
		amtInductiveRepetitions = 1;
	}
}
//
//ISR(TIMER2_COMPA_vect)
//{
	//cli();
	//if (remainingTriacTriggerDelayCounts <= 0) {
		//PORTD |= 0x10;
		//delay6pnt2d5us(triacTriggerLength);   // approx 5 us of triac trigger , try later half or even less, measured 7 with oscilloscope
		//PORTD &= ~0x10;			// handled synchronous
		//if ((triacTriggerTimeTcnt2 >= triggerDelayMaxTcnt2) || (amtInductiveRepetitions <= 0)  ) {
			//stopTimer2();
		//} else {
			//startTriacTriggerDelay(delayBetweenTriacTriggers);
			//--amtInductiveRepetitions;
		//}
	//} else {
		//setTriacTriggerDelayValues();
	//}
	//sei();	
//}

ISR(INT0_vect)
{
	cli();
	if ((PIND & 0x04) != 0) {
		stopTimer2();		
	} else {
		triacTriggerTimeTcnt2 = 0;
		if (triacFireDurationTcnt2 > 0)  {
			startTriacTriggerDelay(  triggerDelayMaxTcnt2 - triacFireDurationTcnt2);
			calcAmtInductiveRepetitions(triacFireDurationTcnt2);
		}
	}
	sei();		  
}   


ISR(TIMER1_COMPA_vect)
{
	secondsDurationTimerRemaining --;
	secondsInDurationTimer ++;
	if (secondsDurationTimerRemaining <= 0) {
		stopDurationTimer();
		durationTimerReachead = 1;
	} else {
		runningSecondsTick = 1;
	}
}

void initInterrupts()
{
// Ext. Interrupt

		DDRD &= ~0x04;		// set PortD pin 2 as input for trigger Ext Int 0
		PORTD &=  ~0x04;   // without pullup 

		PORTD &= ~0x10; 		// done also before setting DDR to avoid eventual accidental triac trigger
		DDRD |= 0x10;			// set Portd pin 04 be Triac output
		PORTD &= ~0x10; 		// and initialize with 0-value

		PORTD &= ~0x08; 		
		DDRD |= 0x08;			// set Portd pin 03 to be completionAlarm
		PORTD &= ~0x08; 		// and initialize with 0-value



		EICRA = 0x01;   // both, fall/rise edge trigger    
		EIMSK = 0x00;   

// Timer 1 as Duration Timer
	  //
			//runningSecondsTick = 0;
	  //
		//TCCR1A = 0x00;  // normal mode or CTC dep.on TCCR1B
		////TCCR1B = 0b00001101  ; // CTC on CC1A , set clk / 1024, timer started
//
		//TCCR1B = 0b00001000  ;  // CTC, timer stopped
//
		//TCCR1C = 0x00; // no Force output compare
//
		//OCR1A = 0x2A30;  // counter top value  , this value at clk/1024 will cause a delay of exact 1 sec
		//TCNT1 = 0x00 ;  
//
		//TIMSK1  = 0x00; // disa  Interrupt 
		////		TIMSK1   = 0b00000010;  //  Output Compare A Match Interrupt Enable 
//
//
//
//// Timer 2 as Triac Trigger Delay Timer
	  //
		//TCCR2A = 0b00000010;  //  CTC 
//
		////TCCR2B = 0b00000101  ; // CTC on CC0A , set clk / 128, timer started
//
		//TCCR2B = 0b00000000  ;  // CTC, timer stopped
		//ASSR = 0x00;
//
		//OCR2A = ocra2aValueMax;  // counter top value  , just anything for start, will later be set by PID
		//TCNT2 = 0x00 ;  
//
		//TIMSK2  = 0x00; // disa  Interrupt 
		////		TIMSK2   = 0b00000010;  //  Output Compare A Match Interrupt Enable 


		sei();  // start interrupts if not yet started
}

void startTriacRun()
{
//	resetPID();
//	startAmpsADC();
	EIFR = 0x00;
	EIMSK = 0x01;  				// start external interrupt (zero pass detection)
}

void stopTriacRun()
{
	EIMSK = 0x00;				// stop external interrupt
	cli();
	stopTimer2();
	sei();
//	stopAmpsADC();
}


/*
int16_t  valueFrom6Bit2Complement(int16_t adcV)
{
	if (adcV & 0x0200) {
		adcV--;
		adcV = ~(adcV | 0xFC00);
		adcV = - adcV;
	}
	return adcV;
}

int16_t diffADCValue()
{  
	int16_t res;
	res = ampsADCValue();
	res = valueFrom6Bit2Complement(res);
	return res;
}
*/

void startDurationTimer(int16_t secs)
{
	durationTimerReachead = 0;
	secondsDurationTimerRemaining = secs;
	secondsInDurationTimer = 0;
	
//	TIMSK1   = 0b00000010;  //  Output Compare A Match Interrupt Enable 
	TCCR1B = 0b00001101  ; // CTC on CC1A , set clk / 24, timer started 
}

void stopDurationTimer()
{
	TCCR1B = 0b00001000 ;  // CTC, timer stopped
//	TIMSK1 = 0x00;
	
}
/*
void setCompletionAlarmOff()
{
	PORTD &= ~0x08; 		
}

void setCompletionAlarmOn()
{
	PORTD |= 0x08; 	
}

void toggleCompletionAlarm()
{  
	if (PORTD & 0x08) {
		setCompletionAlarmOff();
	} else {
		setCompletionAlarmOn();
	}
}
*/


/////////////   USART 2 ///////////////////

#warning: "needs  more work: below constants are tobe debugged"

#define startChar 123
#define stopChar   321
#define amtChars  68
#define rxBufferSz  100
char rxBuffer [rxBufferSz];

enum rxStates {
	rxIdle,
	rxReceiving,
	rxReceived,
};

uint8_t  rxState;
uint8_t  rxCurrentPos;
uint16_t msgCnt;

float   latestTemperature; 
float   latestHumidity;


void getLatestClimateValues(float* pTemp,float* pHum)    // interface to hygrosense, called by user functions
{
	*pTemp = latestTemperature;
	*pHum  = latestHumidity;
}


void onDataReceived()        // called by main application thread to calculate the latest data
{
	++msgCnt;
	cli();
	//  extact data bytes to local buffer outside message
	rxState = rxIdle;	
	sei();
	//  calculate values out of local buffer
}


ISR (USART1_RX_vect)
{
	uint8_t rxCh;  
	rxCh = UDR1;
	if (rxCh == startChar)  {
		msgCnt = 0;	
		dataReceived = 0;
		rxState = rxReceiving;
	}
	if (rxState == rxReceiving)  {
		++ msgCnt;
		if (msgCnt < rxBufferSz) {
			rxBuffer [msgCnt] = rxCh;
		}
	}
	if ((rxCh == stopChar)  && (msgCnt == amtChars)) {   // no  chars lost 
		rxState = rxReceived;
		dataReceived = 1;
	}
}

void initUsart2()
{
	// Set baud rate
	 uint16_t UBRR = 143;    // baud 4800   fixed on hygrosense sensor
		
	UBRR0H = (unsigned char)(UBRR>>8);
	UBRR0L = (unsigned char)UBRR;
	
	//   (UCSR0A & 0b11111100) ;  nothing to do on A-reg	
	// Enable receiver and transmitte                    
		
	UCSR0B |=  (1<<RXEN0) | (1<< RXCIE1) ;               //     |(1<<TXEN0);
	//	UCSR0B = 0b00011000;  // rx compl intr ena - tx compl intr ena - dreg empty intr ena - rx ena - tx ena - sz2 (size bit 2)  - 9. bit rx - 9. tx

// no change needed:	UCSR0C = 0b00000110; // "00" async usart - "00" disa parity - 1 (or 2) stop bit - sz1 - sz0 (set t0 8 bit) - clk polarity (sync only)

	rxState = rxIdle;
	dataReceived = 0;
	msgCnt = 0;
	latestTemperature = 0.00;
	latestHumidity  = 0.00;
}


void initHW()
{
	initInterrupts();
	initUsart2();
}

