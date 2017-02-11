
#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "TriacIntr.h"
#include "TriacDefines.h"
#include "triacPID.h"


int16_t remainingTriacTriggerDelayCounts;

int16_t triacTriggerTimeTcnt0;

int16_t secondsDurationTimerRemaining;

int16_t secondsInDurationTimer;

//int16_t amtInductiveRepetitions;

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

#define ocra0ValueMax 0XFC  // still to be defined


void startTimer0()
{
	
	#warning: "todo test if timer 0 (before was timer 2 !) works correct in atmega128"
	//TCCR2B = 0b00000101  ; // CTC on CC2A , set clk / 128, timer 2 started
	
	TCCR0 |=  (1 <<  CS00) |  (1 <<  CS02) ;    //    (1 <<  CS01) |
}

void stopTimer0()
{
	//TCCR2B = 0b00000000  ;  // CTC, timer stopped
	//TIMSK2  = 0x00;
	//TIFR2 = (1<< OCF2A);    // cleared by writing a "logic" one to the flag
	
	TCCR0 &=  (1 <<  CS00) |  (1 <<  CS01) | (1 <<  CS02) ;     // (0 prescaler )  timer stopped
}


void setTcnt0AndOcr0(int16_t newTcnt0Val,int16_t newOcr0)
{
	
	// timer must be stopped to set tcnt, because else, on an 
	// unprotected set, the timer itself could interfere with the *non double buffered feature" write access.
	// resulting in a more or less random set value.
	// stop timer0
	int8_t tccr0Stack; 
	tccr0Stack = TCCR0;
	stopTimer0();		
	TCNT0 = newTcnt0Val;		
	if (newOcr0 == (TCNT0 + 1)) {++ newOcr0; }  // .... updating avoids triggering of next clock cycle, but needs overnext.
	OCR0 = newOcr0;  
	TCCR0 = tccr0Stack  ; // set previous value, restart timer0 if it was running
}

void setTriacTriggerDelayValues()
{
	if (remainingTriacTriggerDelayCounts < ocra0ValueMax) {		
		setTcnt0AndOcr0 (0, remainingTriacTriggerDelayCounts);
		triacTriggerTimeTcnt0 += remainingTriacTriggerDelayCounts;
		remainingTriacTriggerDelayCounts = 0;
	} else {
		remainingTriacTriggerDelayCounts -= ocra0ValueMax;
		setTcnt0AndOcr0(0, ocra0ValueMax);
		triacTriggerTimeTcnt0 +=  ocra0ValueMax;
	}
}


void startTriacTriggerDelay( int16_t delayDuration)  // must run protected between cli and sei
{
	if (delayDuration <= 0) { 
		delayDuration = 1;   // just a very short duration, but one that will happen in future
	}
	remainingTriacTriggerDelayCounts = delayDuration;
	setTriacTriggerDelayValues();
	startTimer0();		
}

void setTriacFireDuration(int16_t durationTcnt0)
{
	cli();
	if (durationTcnt0 < triggerDelayMaxTcnt0) {
		if (durationTcnt0 > 0) {
			triacFireDurationTcnt0 = durationTcnt0;}
		else {
			triacFireDurationTcnt0 = 0;
		}
	} else {
		triacFireDurationTcnt0 = triggerDelayMaxTcnt0;
	}
	sei();
}

//void calcAmtInductiveRepetitions(int16_t triacFireDurTcnt0)
//{
	//if ( inductiveLoad)  {
		//float amtInductiveRepetitionsF = 0.0;
		//float triacFireDurationTcnt2F = triacFireDurTcnt0;
////		amtInductiveRepetitions = ((triacFireDurationTcnt2 * ( 1  /(11.0592e+6  /128) )) * 1.0e+6  ) /  measuredRepetitionIntervalus; 
		//amtInductiveRepetitionsF = (triacFireDurationTcnt2F * 11.63  )  /  measuredRepetitionIntervalus; 
		//// always cut off modulo part when converting to int
		//amtInductiveRepetitions = amtInductiveRepetitionsF;   // tobe  debugged
	//} else {
		//amtInductiveRepetitions = 1;
	//}
//}

ISR( TIMER0_COMP_vect)
{
	cli();
	if (remainingTriacTriggerDelayCounts <= 0) {
		PORTD |= 0x10;
		delay6pnt2d5us(triacTriggerLength);   // approx 5 us of triac trigger , try later half or even less, measured 7 with oscilloscope
		PORTD &= ~0x10;			// handled synchronous
		if ((triacTriggerTimeTcnt0 >= triggerDelayMaxTcnt0) ) {    //|| (amtInductiveRepetitions <= 0) 
			stopTimer0();
		} else {
			startTriacTriggerDelay(delayBetweenTriacTriggers);
//			--amtInductiveRepetitions;
		}
	} else {
		setTriacTriggerDelayValues();
	}
	sei();	
}

ISR(INT7_vect)
{
#warning "tobe tested"
	cli();
	if ((PINE & (1 << PINE7)) != 0) {
		EICRB &=  ~((1<< ISC70) |  (1<< ISC71))  ; 
		EICRB  |=   (1<< ISC71)  ;   // falling edge  ( each edge change needs to be programmed in the interrupt ::::----(((((
		EIFR  &=  ~(1 << INTF7);		//  flag might have been set while changing edge mode ??? and would cause an immediate interrupt
										//  if cli would not have set before, resp. unmasking the interrupt			
		stopTimer0();		
	} else {
		EICRB &=  ~((1<< ISC70) |  (1<< ISC71))  ; 
		EICRB  |=  (1<< ISC70) |  (1<< ISC71)  ;   // rising edge  ( each edge change needs to be programmed in the interrupt ::::----(((((
		EIFR  &=  ~(1 << INTF7);
		triacTriggerTimeTcnt0 = 0;
		if (triacFireDurationTcnt0 > 0)  {
			startTriacTriggerDelay(  triggerDelayMaxTcnt0 - triacFireDurationTcnt0);
//			calcAmtInductiveRepetitions(triacFireDurationTcnt0);  better not needs much cpu time  ! :-(   my mistake, sorry
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
	
///////   0-x detector input pint INT7 on PE7  
		
		DDRE  &=  ~(1 << DDE7 ) ;    // input pin


		EICRB   |=  (1<< ISC70) |  (1<< ISC71)  ;   // rising edge  ( each edge needs to be programmed in the interrupt ::::----(((((
		EIFR  &=  ~(1 << INTF7); 
		EIMSK |= (1 << INT7);

// Timer 1 as Duration Timer
	  
		runningSecondsTick = 0;
	  
		TCCR1A = 0x00;  // normal mode or CTC dep.on TCCR1B
		//TCCR1B = 0b00001101  ; // CTC on CC1A , set clk / 1024, timer started

		TCCR1B = 0b00001000  ;  // CTC, timer stopped

		TCCR1C = 0x00; // no Force output compare

		OCR1A = 0x2A30;  // counter top value  , this value at clk/1024 will cause a delay of exact 1 sec at 11.0592 hz
		TCNT1 = 0x00 ;  

		
//		TIMSK  &=  ~(1 << OCIE1A)  ;// disa  Interrupt    since timsk in atmega128 is global for 3 timers
//			TIMSK1   = 0b00000010;  //  Output Compare A Match Interrupt Enable 
			TIMSK |= (1 << OCIE1A)  ;  //  Output Compare A Match Interrupt Enable



// Timer 0 as Triac Trigger Delay Timer
	  
		TCCR0 = (1 << WGM01) ;  // CTC,
		TCCR0 &=  (1 <<  CS00) |  (1 <<  CS01) | (1 <<  CS02) ;     // (0 prescaler )  timer stopped
	  


		OCR0 = ocra0ValueMax;  // counter top value  , just anything for start, will later be set by PID
		TCNT0 = 0x00 ;  
		ASSR = 0x00;
		//		TIMSK2  = 0x00; // disa  Interrupt 
		TIMSK   |=  (1<<OCIE0) ;  //  Output Compare A Match Interrupt Enable 


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
	stopTimer0();
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

#define startChar 0x40
#define stopChar   0x24
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
uint8_t  msgCnt;
uint16_t amtCharRcvd;
uint16_t errMsgCnt;

float   latestTemperature; 
float   latestHumidity;


void getLatestClimateValues(float* pTemp,float* pHum)    // interface to hygrosense, called by user functions
{
	*pTemp = latestTemperature;
	*pHum  = latestHumidity;
}


float getCurrentTemperature() 
{
	return latestTemperature;
}


void onDataReceived()        // called by main application thread to calculate the latest data
{
	char tempS [8];
	char hydS [8];
	memset (tempS,0,sizeof(tempS));
	memset (hydS,0,sizeof(hydS));
#warning: "todo replace strstr by constants values for less interrupt latency"		
		cli();
			if (amtCharRcvd == amtChars)  {
				++msgCnt;
				strncpy(tempS,strstr(rxBuffer,"V01")+3,4);  // if interrupt latency causes problems, strstr can be replace by a constant 
				strncpy(hydS,strstr(rxBuffer,"V02")+3,4);
				rxState = rxIdle;
			}  else  {
				++ errMsgCnt;
			}
		sei();
	
		float temp = atof(tempS);
		temp = temp / 100;
		float hyd = atof (hydS);
		hyd = hyd / 200;
		
		latestTemperature = temp;
		latestHumidity = hyd;

		//		TRACE2("\nreceived: %i  %s\n",amtRcv, buffer);

		//		TRACE2("\nV01: %s %f\n",tempS, temp);
		//		TRACE2("\nV02: %s %f\n",hydS, hyd);
		
	
	
}


ISR (USART1_RX_vect)
{
	uint8_t rxCh;  
	rxCh = UDR1;
	if (rxCh == startChar)  {
		amtCharRcvd = 0;	
		dataReceived = 0;
		rxState = rxReceiving;
	}
	if (rxState == rxReceiving)  {
		++ amtCharRcvd;
		if (amtCharRcvd < rxBufferSz) {
			rxBuffer [amtCharRcvd] = rxCh;
		}
	}
	if ((rxCh == stopChar)  && (amtCharRcvd == amtChars)) {   // no  chars lost 
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
	amtCharRcvd = 0;
	errMsgCnt = 0;
	latestTemperature = 0.00;
	latestHumidity  = 0.00;
}


void initHW()
{
	initInterrupts();
	initUsart2();
}



/////////////////////////////////////////////////   ADC





//#define amtMux 2
#define amtMux 0

typedef struct {
	float tempLow, tempHigh;
	float VLow, VHigh;            // tobe found manually with use of graph printout, could be automated somewhen in future, but now "time to market"

} graphValuesRec ;

//graphValueRec graphValues [amtMux] = {};
graphValueRec graphValues [amtMux] = {{1.0, 2.0,1.1,2.3},{1.23, 2.34, 4.56, 5.67}};	

uint8_t  adcConnection [amtMux] = { };

//uint8_t  adcConnection [amtMux] = { 0, (1 << MUX0) | (1<< MUX1) };

uint16_t  adcCnt;

uint16_t  lastADCVal;
float lastVoltageVal [amtMux];			// kept global for debugging reasons
int8_t  currentMux; 


void initADC()
{
		ADMUX |=  (1 << REFS0) | (1 << REFS1) | (1 << ) ;   //  2.56V ref, use ADC0
//		ADCSRA  |= (1 << ADEN) | (1 << ADIE) | (1 << ADPS0)  | (1 << ADPS1)  | (1 << ADPS2)  ; //  freq 11.05 E+6 / 128 approx. 86 E+3
		ADCSRA  |= (1 << ADEN) | (1 << ADIE) | (1 << ADPS0)  | (1 << ADPS1)  | (1 << ADPS2)  ; //  freq 11.05 E+6 / 128 approx. 86 E+3
		adcTick = 0;
		adcCnt = 0;
		lastADCVal = 0;
		currentMux = -1; 
		lastVoltageVal = 0.0;
}


ISR(ADC_vect)
{
	lastADCVal[currentMux] = ADC;
	++ adcCnt;
	adcTick = 1;
}


uint8_t getADCTemperature(uint8_t  pos, float* result)
{
	uint8_t retVal = 0;
	float res = 0.0;
	float adcVf = 0.0;
	float dTbdV = 0.0;   // dT by dV, kept for debugging
	if (pos < amtMux)  {
		uint16_t  adcV = 0;
		cli();
		adcV = lastADCVal;      //  2 bytes, so explicit mutex needed
		sei();
		adcVf = adcV;
		lastVoltageVal =  ((adcVf * 2.56)  / 1024);
		dTbdV = (tempHigh - tempLow) / (VHigh - VLow);            // responsibility to prevent divison by 0 
															// has tobe done manually when evaluating temperature by voltage graph
		res = tempLow  +   (( lastVoltageVal - VLow)  * dTbdV ) ;
		retVal = 1;
	};
	*result = res;
	return retVal;
}


void startADC()
{
	
}

void startCurrentMux()
{
	
}


void startADCSequence()
{
	if ( amtMux > 0)  {
		currentMux = 0;
		startCurrentMux();
	} 
	
}


int8_t startNextADC ()
{
	int8_t res = 0;
	if (currentMux + 1 < amtMux)  {
		++ currentMux; 
		res = 1;
		startCurrentMux();
	}
	return res;
}

