#include <avr/io.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <avr/eeprom.h>
#include "TriacDefines.h"
#include "triacPID.h"
#include "TriacIntr.h"


enum adcScopeEnum
{
	nearScope,
	farScope
};


int8_t m_started;
real m_kTot, m_kP, m_kI, m_kD, m_stepTime, m_inv_stepTime, m_prev_error, m_integral_thresh, m_integral, m_correctionTresh, m_goalValue;

real corrCarryOver;     // carry amount if correction in float gives zero correction in int

uint8_t idleTickCnt;
real pidError;


#define useTWA

#ifdef useTWA		// interface to Pid
	void initTWA();
	void addTwaValue(real val);

#endif

void InitializePID(real kpTot,real kpP, real ki, real kd, real error_thresh, real step_time, real corrThresh, real goalVal)
{
    // Initialize controller parameters
	// PN 3.Oct 2011, added m_kP for better setting of proportional factor only
	// though these 4 factors will be linearly dependent
	m_kP   = kpP;
    m_kTot = kpTot;
    m_kI = ki;
    m_kD = kd;
    m_integral_thresh = error_thresh;

    // Controller step time and its inverse
    m_stepTime = step_time;
    m_inv_stepTime = 1 / step_time;
	m_correctionTresh = corrThresh;

    // Initialize integral and derivative calculations
    m_integral = 0;
    m_started = 0;
	
	m_goalValue = goalVal;
	
//	 updateGradAmps();

	 corrCarryOver = 0;
	 
	 #ifdef useTWA
		initTWA();
	 #endif
}




real pVal, dVal, iVal;

real nextCorrection(real error)
{	
	real res;
		
    if (fabs(error) < m_integral_thresh)
         m_integral += m_stepTime * error;
    else  {
		m_integral = 0.0;
	}

    // Compute the error derivative
    real deriv;
    if (!m_started)
    {
        m_started = 1;
        deriv = 0;
    }
    else
        deriv = (error - m_prev_error) * m_inv_stepTime;

    m_prev_error = error;

    // Return the PID controller actuator command
	res = m_kTot*((pVal =m_kP * error) + (iVal = m_kI * m_integral) + (dVal = m_kD * deriv));
	if (res > m_correctionTresh) {
		res = m_correctionTresh;
	} else if (res < -1*m_correctionTresh) {
		res = -1* m_correctionTresh;
	}

#ifdef printfPID
	real errD = error;
	real intD = m_integral;
	real derivD = deriv;
	info_printf("err %f int %f deriv %f \n",errD, intD, derivD);
#endif
    return res;
}

void calcNextTriacDelay()
{  
	real corr;
	int16_t newDelay;
	int16_t corrInt;
	pidError = getCurrentTemperature()  - desiredTemperature ;
	#ifdef useTWA
		addTwaValue(pidError);
	#endif
	corr = nextCorrection(pidError) + corrCarryOver;
	corrInt = corr;     
	corrCarryOver = corr - corrInt;
	newDelay = getTriacFireDuration() + corrInt;
	setTriacFireDuration(newDelay);
#ifdef printfPID
	printPIDState();
	real corrD = corr;
	real carryCorrD = corrCarryOver;
	real ampsD  = currentAmps();
	info_printf("err %6.2f corr %f corrI %i cryOv %f delay %x  amps %f\n",pidError, corrD,corrInt, carryCorrD, newDelay, ampsD); 
#endif
}

void initPID()
{
//	void InitializePID(real kpTot,real kpP, real ki, real kd, real error_thresh, real step_time, real corrThresh, real goalVal)
	InitializePID( totFactor, pFactor, iFactor, dFactor, itegralTreshold, pidIntervalSecs,correctionThreshold, desiredTemperature);
	setTriacFireDuration(initialTriacDelayValue);
}

void resetPID()
{
	corrCarryOver = 0;
 	m_integral =0;
	m_prev_error = 0;
}

void printPIDState()
{
	/*
	int16_t adcAmps;
	float res;
	double resD;
	double gradD = gradAmps;

	adcAmps = 0;

	res = calibLowAmps +  (gradAmps * ((int16_t) adcAmps - (int16_t) calibLowADC  ));
	resD = res;

	info_printf("\nPID State\n");
	info_printf("calLowA %i calHighA %i caLowDelay %i caHiDelay %i\n",calibLowAmps,calibHighAmps, calibLowTriacFireDuration, calibHighTriacFireDuration);
	info_printf("calLowAdc %i caHiAdc %i \n",calibLowADC, calibHighADC);
	info_printf("shows at 0 ADC : %f A  grad %f zeroPotiPos %i\n",resD, gradD,zeroPotiPos);
//	checkEEPOROM();

*/
}


void onPidStep()
{
	info_printf("onPidStep\n");
	calcNextTriacDelay();
}

void printCsvHeader()
{
	info_printf("printCsvHeader\n");
	csv_printf("time,temp_inBox,triacFireDuration,pVal,iVal,dVal,goal_temp\n");
	csv_printf("seconds,�C,triacTx,real,real,real,�C\n");
}

void printCsvValues()
{

	GETTimeValues
	csv_printf("%5d:%02d:%02d,%5.1f,%d,%f,%f,%f, %5.1f\n",hrs,mins,secs,getCurrentTemperature(),getTriacFireDuration(),pVal,iVal,dVal,desiredTemperature);

}

void debugSetTriacDelayValueFromAdc()
{
	int16_t triacDuration = getTriacFireDurationFromADC(0); 
	setTriacFireDuration(triacDuration);
}


///////////////////////// twa   ////////////////////////////




#ifdef useTWA
#define amtTwaValues   20
#define twaStepWidth  3    //  keep every twaStepWidth's value
real twaDifferenceAbs;

void initTWA();
void addTwaValue(real val);

typedef struct  {
	uint8_t amtValues;
	real totalWeight;
	uint16_t totTwaPointsAdded;
	uint8_t currentTwaInterval;
	real twaResult;
} twaStruct;

typedef int someint;
typedef  twaStruct*   PTwaStruct;


real calcTotalWeightLinear(uint8_t amtValues)
{
	real res= 0.0;
	
	
	res = (amtValues * (amtValues -1)) / 2;
	
	return res;
}



void initTWAStruct(PTwaStruct pTwaStruct)
{
		pTwaStruct->totalWeight =  calcTotalWeightLinear(amtTwaValues);
		pTwaStruct->totTwaPointsAdded = 0;
		pTwaStruct->currentTwaInterval = 0;
	
}



void addTwaValue(real val)
{
	
}



void initTWA()
{

}



#endif