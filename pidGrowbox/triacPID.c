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
real m_kTot, m_kP, m_kI, m_kD, m_stepTime, m_inv_stepTime, m_prev_error, m_error_thresh, m_integral;

real corrCarryOver;     // carry amount if correction in float gives zero correction in int

uint8_t idleTickCnt;
real pidError;




void InitializePID(real kpTot,real kpP, real ki, real kd, real error_thresh, real step_time)
{
    // Initialize controller parameters
	// PN 3.Oct 2011, added m_kP for better setting of proportional factor only
	// though these 4 factors will be linearly dependent
	m_kP   = kpP;
    m_kTot = kpTot;
    m_kI = ki;
    m_kD = kd;
    m_error_thresh = error_thresh;

    // Controller step time and its inverse
    m_stepTime = step_time;
    m_inv_stepTime = 1 / step_time;

    // Initialize integral and derivative calculations
    m_integral = 0;
    m_started = 0;
	
//	 updateGradAmps();

	 corrCarryOver = 0;
}

#define correctionThreshold  30


real pVal, dVal, iVal;

real nextCorrection(real error)
{	
	real res;
		
    if (fabs(error) < m_error_thresh)
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
	if (res > correctionThreshold) {
		res = correctionThreshold;
	} else if (res < -1*correctionThreshold) {
		res = -1* correctionThreshold;
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
//	InitializePID(real kpTot, real kpP, real ki, real kd, real error_thresh, real step_time);  
#warning "todo check all the signs and values :-)" 
	InitializePID( -0.45, 1.1, 0.05, 0.5, 2, pidIntervalSecs);
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
//	calcNextTriacDelay();
}

void printCsvHeader()
{
	info_printf("printCsvHeader\n");
#ifdef printCsvData
	printf("time,temp_inBox,triacFireDuration,pVal,iVal,dVal\n");
	printf("seconds,°C,triacTx,real,real,real\n");
#endif
}

void printCsvValues()
{
	info_printf("printCsvValues\n");
#ifdef printCsvData
	printf("%d,%5.1f,%d,%f,%f,%f, %f\n",overallSeconds(),getCurrentTemperature(),getTriacFireDuration(),pVal,iVal,dVal,desiredTemperature);
#endif
}
