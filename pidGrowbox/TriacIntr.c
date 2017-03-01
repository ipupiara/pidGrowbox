
#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "TriacIntr.h"
#include "TriacDefines.h"
#include "triacPID.h"

int16_t remainingTriacTriggerDelayCounts;

int16_t amtInductiveRepetitions;

int16_t inductiveRepetitionsCounter;

int8_t withinZeroCross;

int16_t getSecondsRemainingInDurationTimer()
{
	int16_t res;
	cli();
	res = secondsRemainingInDurationTimer;
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
	TIFR  &= ~(1 << OCF0);    // clear interrupt flags if it should ever have been set by any reason...
	if (withinZeroCross == 0) {
		TIMSK   |=  (1<<OCIE0) ;  //  Output Compare A Match Interrupt Enable
		TCCR0 |=  (1 <<  CS00) |  (1 <<  CS02) ;    //    (1 <<  CS01) |
	}

}

void stopTimer0()
{
	//TCCR2B = 0b00000000  ;  // CTC, timer stopped
	//TIMSK2  = 0x00;
	//TIFR2 = (1<< OCF2A);    // cleared by writing a "logic" one to the flag
	
	TCCR0 &=  ~((1 <<  CS00) |  (1 <<  CS01) | (1 <<  CS02)) ;     // (0 prescaler )  timer stopped
	TIMSK   &=  ~(1<<OCIE0) ;  //  Output Compare A Match Interrupt disable
	TIFR  &= ~(1 << OCF0);		// clear interrupt flags
		

}


void setOcr0Delay(int16_t newOcr0)
{
#warning "urgent todo, check all the call stacks if timer stop start cli and sei is set ok so that no data/thread violation can occur"	
	// timer0 must be stopped before running this method (was the case per 17 Feb 2017
	// timer must be stopped to set tcnt, because else, on an 
	// unprotected set, the timer itself could interfere with the *non double buffered feature" write access.
	// resulting in a more or less randomly set value.
//	stopTimer0();
	if (newOcr0 <= 1) { newOcr0 = 2; }  // .... updating avoids triggering of next clock cycle, but needs overnext.
	TCNT0 = 0;
	OCR0 = newOcr0;  
}

void setTriacTriggerDelayValues()
{
	if (remainingTriacTriggerDelayCounts < ocra0ValueMax) {		
		setOcr0Delay ( remainingTriacTriggerDelayCounts);
		remainingTriacTriggerDelayCounts = 0;
	} else {
		setOcr0Delay( ocra0ValueMax);
		remainingTriacTriggerDelayCounts -= ocra0ValueMax;
	}
}


void startTriacTriggerDelay( int16_t delayDuration)  
{
	cli();
	stopTimer0();
	if (delayDuration <= 0) { 
		delayDuration = 1;   // just a very short duration, but one that will happen in future
	}
	remainingTriacTriggerDelayCounts = delayDuration;
	setTriacTriggerDelayValues();	
	startTimer0();
	sei();
}

void calcAmtInductiveRepetitions(int16_t tFDurationTcnt0)
{
	if ( inductiveLoad == 1)  {
		floatType amtInductiveRepetitionsF = 0.0;
		floatType tFDurationTcnt0F = tFDurationTcnt0;
		//		amtInductiveRepetitions = ((triacFireDurationTcnt2 * ( 1  /(11.0592e+6  /128) )) * 1.0e+6  ) /  measuredRepetitionIntervalus;
//		float tcnt0TickDurationUs  = 128 / 11.0592e+6; 
//		float calculatedRepetitionInterval = delayBetweenTriacTriggers * tcnt0TickDurationUs + 5;   // trigger takes approx 3- 5us
//                  measured was 200  and calculated was 190 what is quite ok. measured should be a bit higher than calculated 
//                  for all the loss during stop and start of timer  , etc.....
		amtInductiveRepetitionsF = (tFDurationTcnt0F * 11.63  )  /  measuredRepetitionIntervalus;
		// always cut off modulo part when converting to int
		amtInductiveRepetitions = amtInductiveRepetitionsF;   
#warning "tobe  debugged"
	} else {
		amtInductiveRepetitions = 1;
	}
}

void setTriacFireDuration(uint16_t durationTcnt0)
{
	uint16_t  tfDuration;
	
	if (durationTcnt0 < triggerDelayMaxTcnt0) {
		if (durationTcnt0 > 0) {
			tfDuration = durationTcnt0;}
		else {
			tfDuration = 0;
		}
	} else {
		tfDuration = triggerDelayMaxTcnt0;
	}
	calcAmtInductiveRepetitions(durationTcnt0);
	cli();
	triacFireDurationTcnt0 = tfDuration;
	sei();
}

uint16_t  getTriacFireDuration()
{
	return triacFireDurationTcnt0;
}


ISR( TIMER0_COMP_vect)
{
	cli();
	if (remainingTriacTriggerDelayCounts <= 0) {
		PORTE |= (1<< PORTE6) ;
		sei();				// allow interrupts during delay and do some manual checks during implemention of source code
		delay6pnt2d5us(triacTriggerLength);   // approx 5 us of triac trigger , try later half or even less, measured 7 with oscilloscope
		cli(); 
		PORTE &= ~(1<< PORTE6) ;		// handled synchronous 	
		if  ((inductiveRepetitionsCounter <= 0) || (withinZeroCross == 1) ) {   
			stopTimer0();
		} else {
			--inductiveRepetitionsCounter;
			startTriacTriggerDelay(delayBetweenTriacTriggers);
		}
	} else {
		stopTimer0();
		setTriacTriggerDelayValues();
		startTimer0();
	}	
	sei();
}



// int7 on port PE7
ISR(INT7_vect)
{
	cli();
	EIFR  &=  ~(1 << INTF7);
	if ((PINE & (1 << PINE7)) != 0) {
		EICRB &=  ~((1<< ISC70) |  (1<< ISC71))  ; 
		EICRB  |=   (1<< ISC71)  ;   
		EIFR  &=  ~(1 << INTF7);												
		withinZeroCross = 0;
		if (triacFireDurationTcnt0 > 0)  {
			inductiveRepetitionsCounter = amtInductiveRepetitions;
			startTriacTriggerDelay(  triggerDelayMaxTcnt0 - triacFireDurationTcnt0);
		}		
	} else {
		EICRB &=  ~((1<< ISC70) |  (1<< ISC71))  ; 
		EICRB  |=  (1<< ISC70) |  (1<< ISC71)  ;   
		EIFR  &=  ~(1 << INTF7);
		withinZeroCross = 1;
		stopTimer0();		
	}
	sei();		  
}   

void printfTime()
{
	info_printf("h: %5i m: %29i s: %2i ", hoursCounter, minutesCounter, secondsCounter);  
}

uint32_t overallSeconds() 
{
	uint32_t res = 0;
	res = hoursCounter * 3600 + minutesCounter * 60 + secondsCounter;
	return res;
}

ISR(TIMER1_COMPA_vect)
{
	++ secondsCounter;
	if (secondsCounter == 59) {
		++ minutesCounter;
		secondsCounter = 0;
		if (minutesCounter == 59) {
			++ hoursCounter;
			minutesCounter = 0;
		}
	}
	
	if (secondsRemainingInDurationTimer >= 0) {
		secondsRemainingInDurationTimer --;
		secondsInDurationTimer ++;
		if (secondsRemainingInDurationTimer <= 0) {
			stopDurationTimer();
			durationTimerReachead = 1;
		} 
	}
	runningSecondsTick = 1;
	
}

void initInterrupts()
{
	
///////   0-x detector input pint INT7 on PE7  
		
		DDRE  &=  ~(1 << DDRE7 ) ;    // input pin
		DDRE |=  (1 << DDRE6);		// ouput pin, triac trigger

		EICRB   |=  (1<< ISC70) |  (1<< ISC71)  ;   // rising edge  ( each time, edge needs to be programmed in the interrupt ::::----(((((
		EIFR  &=  ~(1 << INTF7); 
		EIMSK |= (1 << INT7);
#warning "todo start int7 only after initilization of timer0....."		

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
		TCCR0 &=  ~((1 <<  CS00) |  (1 <<  CS01) | (1 <<  CS02)) ;     // (0 prescaler )  timer stopped
	  


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
	cli();
	EIMSK = 0x00;				// stop external interrupt
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

void startSecondTick()
{
//	TIMSK1   = 0b00000010;  //  Output Compare A Match Interrupt Enable
	TCCR1B = 0b00001101  ; // CTC on CC1A , set clk / 24, timer started
	secondsCounter = 0;
	minutesCounter = 0;
	hoursCounter = 0;
}

void startDurationTimer(int16_t secs)
{
	cli();   // may not be interrupted by secondTick
	durationTimerReachead = 0;
	secondsRemainingInDurationTimer = secs;
	secondsInDurationTimer = 0;
	sei();
	
//	TIMSK1   = 0b00000010;  //  Output Compare A Match Interrupt Enable 
//	TCCR1B = 0b00001101  ; // CTC on CC1A , set clk / 24, timer started 
}

void stopDurationTimer()
{
//	TCCR1B = 0b00001000 ;  // CTC, timer stopped
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


#define startChar 0x40
#define stopChar   0x24
#define amtChars  66
#define rxBufferSz  100
char rxBuffer [rxBufferSz];

enum rxStates {
	rxIdle,
	rxReceiving,
	rxReceived,
};

uint8_t  rxState;
uint8_t  rxCurrentPos;
uint16_t amtCharRcvd;
uint16_t errMsgCnt;

floatType   latestTemperature; 
floatType   latestHumidity;


void getLatestClimateValues(floatType* pTemp,floatType* pHum)    // interface to hygrosense, called by user functions
{
	*pTemp = latestTemperature;
	*pHum  = latestHumidity;
}


floatType getCurrentTemperature() 
{
	return latestTemperature;
}

floatType getCurrentHumidity()
{
	return latestHumidity;
}


char * reallyWorkingStrstr(const char *inStr, const char *subStr)
{
	char firstSubChar;
	size_t len;      
	firstSubChar = *subStr++;
	if (!firstSubChar)
	return (char *) inStr;	// Trivial empty string case

	len = strlen(subStr);
	do {
		char currentInChar;

		do {
			currentInChar = *inStr++;
			if (!currentInChar)
			return (char *) 0;
		} while (currentInChar != firstSubChar);
	} while (strncmp(inStr, subStr, len) != 0);
	
	return (char *) (inStr - 1);
}

char * v01Pos;
char *  v02Pos;

void onDataReceived()        // called by main application thread to calculate the latest data
{
	char tempS [5];
	char hydS [5];
	uint8_t validMsg = 0;

	memset (tempS,0,sizeof(tempS));
	memset (hydS,0,sizeof(hydS));
			
	cli();
		if ((rxState = rxReceived) && ( amtCharRcvd == amtChars))  {      // some valid message check
			validMsg = 1;
			if (v01Pos == 0) { 
				v01Pos = reallyWorkingStrstr((char* )&rxBuffer,"V01");
			}
			if (v02Pos == 0) { 
				v02Pos = reallyWorkingStrstr((char*)&rxBuffer,"V02");
			}
			++hygrosenseMsgCnt;
			strncpy(tempS,v01Pos+3,4);  
			strncpy(hydS,v02Pos+3,4);
			rxState = rxIdle;
		}  else  {
			++ errMsgCnt;
		}
	sei();
	if (validMsg != 0)  {
		char* endP = tempS+3;
		floatType temp = strtoul(tempS ,&endP,0x10) ;
		temp = temp / 100;
		endP = hydS + 3;
		floatType hyd = strtoul(hydS ,&endP,0x10) ;
		hyd = hyd / 200;
		
		latestTemperature = temp;
		latestHumidity = hyd;	
	}
}


ISR (USART1_RX_vect)
{
	uint8_t rxCh;  
	rxCh = UDR1;
	if (rxCh == startChar)  {
		amtCharRcvd = 0;	
		dataReceived = 0;
		rxBuffer [amtCharRcvd] = rxCh;
		rxState = rxReceiving;
	} else
	if (rxState == rxReceiving)  {
		++ amtCharRcvd;
		if (amtCharRcvd < rxBufferSz) {
			rxBuffer [amtCharRcvd] = rxCh;
		}
		if (rxCh == stopChar) {   // no  chars lost 
			rxState = rxReceived;
			dataReceived = 1;
		}
	}
}

void initUsart2()
{
	
	rxState = rxIdle;
	dataReceived = 0;
	amtCharRcvd = 0;
	errMsgCnt = 0;
	latestTemperature = 0.00;
	latestHumidity  = 0.00;
	hygrosenseMsgCnt = 0;
	v01Pos = (char *) 0;
	v02Pos = (char *) 0;
		
	// Set baud rate
	 uint16_t UBRR = 143;    // baud 4800   fixed on hygrosense sensor
		
	UBRR1H = (unsigned char)(UBRR>>8);
	UBRR1L = (unsigned char)UBRR;
	
	//   (UCSR0A & 0b11111100) ;  nothing to do on A-reg	
	// Enable receiver and transmitte                    
		
	UCSR1B |=  (1<<RXEN1) | (1<< RXCIE1) ;               //     |(1<<TXEN0);
	//	UCSR0B = 0b00011000;  // rx compl intr ena - tx compl intr ena - dreg empty intr ena - rx ena - tx ena - sz2 (size bit 2)  - 9. bit rx - 9. tx

// no change needed:	UCSR0C = 0b00000110; // "00" async usart - "00" disa parity - 1 (or 2) stop bit - sz1 - sz0 (set t0 8 bit) - clk polarity (sync only)

}


void initHW()
{
	initInterrupts();
	initUsart2();
	startSecondTick();
}



/////////////////////////////////////////////////   ADC

// approximative temperature measuring. the absolute value of temperature is not so important, but most important is stability in temperature



//#define amtMux 2
#define amtMux 1

typedef struct {
	floatType tempLow, tempHigh;
	floatType VLow, VHigh;            // tobe found manually with use of graph printout, could be automated some when in future, but now "time to market"

} graphValuesRec ;

graphValuesRec graphValues [amtMux] = {{1.0, 2.0, 1.1, 2.3}};
//graphValuesRec graphValues [amtMux] = {{1.0, 2.0, 1.1, 2.3},{1.23, 2.34, 4.56, 5.67}};	
	

uint8_t  adcConnection [amtMux] = {0x00 };

//uint8_t  adcConnection [amtMux] = { 0, (1 << MUX0) | (1<< MUX1) };
floatType adcRefVoltage[amtMux] = {adcRefVoltage5} ;

uint16_t  adcCnt;

uint16_t  lastADCVal [amtMux];
floatType debugLastVoltageVal [amtMux];			// a local variable kept global exclusively for debugging reasons
int8_t  currentMuxPos; 


void initADC()
{
//		ADMUX =  (1 << REFS0) | (1 << REFS1)  ;   //  2.56V ref, use ADC0, with external capacitor
		ADMUX =   (1 << REFS0);      // use AVcc with external capacitor, with ADC0
//		ADCSRA  |= (1 << ADEN) | (1 << ADIE) | (1 << ADPS0)  | (1 << ADPS1)  | (1 << ADPS2)  ; //  freq 11.05 E+6 / 128 approx. 86 E+3
		ADCSRA  = ((1 << ADIE) |(1 << ADEN) |1 << ADPS0)  | (1 << ADPS1)  | (1 << ADPS2)  ; // aden,  freq 11.05 E+6 / 128 approx. 86 E+3
		adcTick = 0;
		adcCnt = 0;
		memset(lastADCVal,0,sizeof(lastADCVal));
		memset(debugLastVoltageVal,0,sizeof(debugLastVoltageVal));
		currentMuxPos = 0; 
}


ISR(ADC_vect)
{
	lastADCVal[currentMuxPos] = ADC;
	++ adcCnt;
	adcTick = 1;
//	ADCSRA &= ~((1 << ADIE) | (1<<ADEN)); //disable further adc since we use only single start adc and usually change adcmux pos what needs
										// aden to get 0 for better synchronisation
}

int16_t adcValue(uint8_t  pos)
{
	uint16_t res = 0;
	if ((pos >= 0) && (pos < amtMux))  {
		cli();
		res = adcValue(pos);
		sei();
	}
	return res;
}

uint8_t getADCTemperature(uint8_t  pos, floatType* result)
{
	uint8_t retVal = 0;
	floatType res = 0.0;
	floatType adcVf = 0.0;
	floatType dTbdV = 0.0;   // dT by dV, kept for debugging
	if (pos < amtMux)  {
//		uint16_t  adcV = 0;
//		adcV = adcValue(pos);      //  2 bytes, so explicit mutex needed
		adcVf = adcVoltage(pos);
		debugLastVoltageVal[pos] =  adcVoltage(pos);
		dTbdV = (graphValues[pos].tempHigh - graphValues[pos].tempLow ) / (graphValues[pos].VHigh - graphValues[pos].VLow);            
																// responsibility to prevent divison by 0 
															// has tobe done manually when evaluating temperature by voltage graph
		res = graphValues[pos].tempLow  +   (( adcVf - graphValues[pos].VLow)  * dTbdV ) ;
		retVal = 1;
	};
	*result = res;
	return retVal;
}


void startCurrentMux()
{
//	ADCSRA &= ~((1 << ADIE) | (1<<ADEN));
	
	ADMUX &= ~ ((1<< MUX0) | (1<< MUX1) |(1<< MUX2) |(1<< MUX3) |(1<< MUX4) );    // set all mux to 0
	ADMUX |= adcConnection[currentMuxPos]; 
	
//	ADCSRA |= (1 << ADIE) | (1<<ADEN) | (1<< ADSC);
	ADCSRA |=  (1<< ADSC);

}


void startADCSequence()
{
	if ( amtMux > 0)  {
		currentMuxPos = 0;
		startCurrentMux();
	} 
	
}


int8_t startNextADC ()
{
	int8_t res = 0;
	if (currentMuxPos + 1 < amtMux)  {
		++ currentMuxPos; 
		res = 1;
		startCurrentMux();
	}
	return res;
}



floatType adcVoltage(uint8_t  pos)      // tobe called outside interrupts
{
	int16_t VHex;
	double   VFl;

	VFl = 0.0;

	VHex = adcValue(pos);
	VFl = (VHex * adcRefVoltage[pos]) / 0x03FF;
	
	return VFl;
}


// debug method used for testing triac triggering behaviour
int16_t getTriacDelayValueFromADC(uint8_t pos)
{
	int16_t res = 0;
	int16_t adcV = adcValue(pos);
	floatType adcF = adcV * 1.0;  // tobe tested
	floatType maxDelay = triggerDelayMaxTcnt0 ;  // tobe tested
	
	floatType  adcFactor = adcF / 1023.0;
	
	floatType resF = adcFactor * maxDelay;
	res = (int16_t)  resF ; // tobe tested maybe needs to use conversion methods
	
	return res;
}

void startHumidifying()
{
	
}

void stopHumidifying()
{
	
}

void startVentilating()
{
	
}

void stopVentilating()
{
	
}

void startDrying()
{
	
}

void stopDrying()
{
	
}
