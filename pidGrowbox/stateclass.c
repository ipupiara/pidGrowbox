
#include <avr/io.h>
#include <stdio.h>
//#include <iostream.h>
//#include <assert.h>

#include "TStatechart.h"
#include "StateClass.h"
#include "TriacDefines.h"
#include "StateClass.h"
#include "TriacIntr.h"
#include "triacPID.h"
#include "triacUI.h"

extern const uStInt uStIntHandlingDone;
extern const uStInt uStIntNoMatch;

CGrowBoxEvent* currentEvent;



// This defines and names the states the class has.
#warning  attention: sequence must be the same as in xaStates (below)  !!!


// This defines and names the states the class has.
enum eStates
{
	eStateGrowBoxKeepingHumidity,
	eStartState = eStateGrowBoxKeepingHumidity,
	eStateHumidifying,
	eStateIdle,
	eStateNonVentilating,
	eStateVentilating,
	eStateDrying,
	eStateFatalError,
	eNumberOfStates
};


void entryFatalErrorState(void)
{
	info_printf("entry FatalError\n");
	info_printf("**************fatal Error: %s *************************\n",lastFatalErrorString);
	displayFatalError();
}

void exitFatalErrorState(void)
{
	info_printf("exit FatalError\n");
}

uStInt evFatalErrorChecker(void)
{
	uStInt res = uStIntNoMatch;
	info_printf("check for event in State evStateIdle\n");

	return (res);
}



uStInt evStateGrowBoxKeepingHumidity(void)
{
	info_printf("check for event in State evStateGrowBoxKeepingHumidity\n");
	if (currentEvent->evType == eReset)  
	{
		BEGIN_EVENT_HANDLER(PTriacHumidityTemperatureChart, eStateGrowBoxKeepingHumidity );
			// No event action.
		END_EVENT_HANDLER(PTriacHumidityTemperatureChart);
		
///*  left this as an original example for history states
 //
		//BEGIN_EVENT_HANDLER(CHumidityStateClass,   eStateGrowBoxKeepingHumidity | u32WithHistory);
			//// No event action.
		//END_EVENT_HANDLER(CHumidityStateClass );
//*/
		//return (uStIntHandlingDone);
	}
	return (uStIntNoMatch);
}


uStInt evStateHumidifying(void)
{
	info_printf("check for event in State evStateHumidifying\n");
	if ((currentEvent->evType == eValueAssignement) && (GetHumidifyingUpperLimit() < currentEvent->humidity))
	{
		BEGIN_EVENT_HANDLER(PTriacHumidityTemperatureChart,   eStateIdle);
			// No event action.
		END_EVENT_HANDLER(PTriacHumidityTemperatureChart );
		return (uStIntHandlingDone);
	}
	return (uStIntNoMatch);
}


uStInt evStateIdle(void)
{
	info_printf("check for event in State evStateIdle\n");

	if (currentEvent->evType == eValueAssignement) 
	{	if (GetHumidifyingLowerLimit() > currentEvent->humidity)
		{
			BEGIN_EVENT_HANDLER(PTriacHumidityTemperatureChart,   eStateHumidifying);
				// No event action.
			END_EVENT_HANDLER(PTriacHumidityTemperatureChart );
			return (uStIntHandlingDone);
		}
		if (GetDryingUpperLimit() < currentEvent->humidity)
		{
			BEGIN_EVENT_HANDLER(PTriacHumidityTemperatureChart,   eStateDrying);
				// No event action.
			END_EVENT_HANDLER(PTriacHumidityTemperatureChart );
			return (uStIntHandlingDone);
		}
	}
	return (uStIntNoMatch);
}


uStInt evStateNonVentilating(void)
{
	info_printf("check for event in State evStateNonVentilating\n");

	if ((currentEvent->evType == evTimeOutDurationTimer) || (currentEvent->evType ==  eVentilationButtonPressed))
	{
		BEGIN_EVENT_HANDLER(PTriacHumidityTemperatureChart,   eStateVentilating);
			// No event action.
		END_EVENT_HANDLER(PTriacHumidityTemperatureChart );
		return (uStIntHandlingDone);
	
	}
	return (uStIntNoMatch);
}


uStInt evStateVentilating(void)
{
	info_printf("check for event in State evStateVentilating\n");

	if ((currentEvent->evType == evTimeOutDurationTimer)  || (currentEvent->evType ==  eVentilationStopButtonPressed))  {
		BEGIN_EVENT_HANDLER(PTriacHumidityTemperatureChart,   eStateNonVentilating);
			// No event action.
		END_EVENT_HANDLER(PTriacHumidityTemperatureChart );
		return (uStIntHandlingDone);
	
	}
	return (uStIntNoMatch);
}

uStInt evStateDrying(void)
{
	info_printf("check for event in State evStateDrying\n");
	if ((currentEvent->evType == eValueAssignement) && (GetDryingLowerLimit() > currentEvent->humidity))
	{
		BEGIN_EVENT_HANDLER(PTriacHumidityTemperatureChart,   eStateIdle);
			// No event action.
		END_EVENT_HANDLER(PTriacHumidityTemperatureChart );
		return (uStIntHandlingDone);
	}
	return (uStIntNoMatch);
}

/*
// State transition/handling methods
// left as an example of this defEntry method

void CHumidityStateClass::defEntryStateGrowBoxKeepingHumidity(void)
{

	info_printf("CHumidityStateClass::defEntryStateGrowBoxKeepingHumidity\n");
}
*/



void entryStateGrowBoxKeepingHumidity(void)
{
	info_printf("CHumidityStateClass::entryStateGrowBoxKeepingHumidity\n");
}


void entryStateHumidifying(void)
{
	info_printf("CHumidityStateClass::entryStateHumidifying\n");
	startHumidifying();
}


void entryStateIdle(void)
{
	info_printf("CHumidityStateClass::entryStateIdle\n");
}


void entryStateNonVentilating(void)
{
	info_printf("CHumidityStateClass::entryStateNonVentilating\n");
	startDurationTimer(GetIdleVentilationDelayMinutes());
}


void entryStateVentilating(void)
{
	info_printf("CHumidityStateClass::entryStateVentilating\n");
	startVentilating();
	startDurationTimer(GetIdleVentilationMinutes());
}

void entryStateDrying(void)
{
	info_printf("CHumidityStateClass::entryStateDrying\n");
	startDrying();
}

void exitStateGrowBoxKeepingHumidity(void)
{
	info_printf("CHumidityStateClass::exitStateGrowBoxKeepingHumidity\n");
}


void exitStateHumidifying(void)
{
	info_printf("CHumidityStateClass::exitStateHumidifying\n");
	stopHumidifying();
}


void exitStateIdle(void)
{
	info_printf("CHumidityStateClass::exitStateIdle\n");
}


void exitStateNonVentilating(void)
{
	info_printf("CHumidityStateClass::exitStateNonVentilating\n");
	stopDurationTimer();
}


void exitStateVentilating(void)
{
	info_printf("CHumidityStateClass::exitStateVentilating\n");
	stopVentilating();
	stopDurationTimer();
}


void exitStateDrying(void)
{
	info_printf("CHumidityStateClass::exitStateDrying\n");
	stopDrying();
}




/***   Temperature  ********/

// This defines and names the states the class has.
enum eTemperatureStates
{
	eStateGrowBoxTemperature,
	eTemperatureStartState = eStateGrowBoxTemperature,
	eStateTemperatureIdle,
	eStateHeating,
	eStateCooling,
	eNumberOfTemperatureStates
};



#ifndef  sdccNULL 

#define tfNull 0

#else

t_fvoid  tfNull;

#endif 

// attention: sequence must be the same as above enum eStates

xStateType xaStates[eNumberOfStates] = {
		{eStateGrowBoxKeepingHumidity,
			-1,
			eStateIdle,
			eStateGrowBoxKeepingHumidity,
			NULL,  
			entryStateGrowBoxKeepingHumidity,
			exitStateGrowBoxKeepingHumidity},

/* name						*/	{eStateHumidifying,
	/* parent					*/	eStateGrowBoxKeepingHumidity,
	/* default_substate			*/	-1,
	/* event-checking func		*/	eStateHumidifying,
	/* default state entry func	*/	NULL,
	/* entering state func		*/	entryStateHumidifying,
/* exiting state func		*/		exitStateHumidifying},

/* name						*/	{eStateIdle,
	/* parent					*/	eStateGrowBoxKeepingHumidity,
	/* default_substate			*/	eStateNonVentilating,
	/* event-checking func		*/	eStateIdle,
	/* default state entry func	*/	NULL,
	/* entering state func		*/	entryStateIdle,
/* exiting state func		*/		exitStateIdle},

/* name						*/	{eStateNonVentilating,
	/* parent					*/	eStateIdle,
	/* default_substate			*/	-1,
	/* event-checking func		*/	eStateNonVentilating,
	/* default state entry func	*/	NULL,
	/* entering state func		*/	entryStateNonVentilating,
/* exiting state func		*/		exitStateNonVentilating},

/* name						*/	{eStateVentilating,
	/* parent					*/	eStateIdle,
	/* default_substate			*/	-1,
	/* event-checking func		*/	eStateVentilating,
	/* default state entry func	*/	NULL,
	/* entering state func		*/	entryStateVentilating,
/* exiting state func		*/		exitStateVentilating},

/* name						*/	{eStateDrying,
	/* parent					*/	eStateGrowBoxKeepingHumidity,
	/* default_substate			*/	-1,
	/* event-checking func		*/	eStateDrying,
	/* default state entry func	*/	NULL,
	/* entering state func		*/	entryStateDrying,
/* exiting state func		*/		exitStateDrying},
	
	{eStateFatalError,
 	eStateGrowBoxKeepingHumidity,
 	-1,
 	0,
 	evFatalErrorChecker,
 	tfNull,
 	entryFatalErrorState,
 	exitFatalErrorState}	 	 	 
};




void startStateCharts()
{

#ifdef  sdccNULL 

	tfNull = (t_fvoid ) NULL;

#endif 

 	PTriacHumidityTemperatureChart = & STriacHumidityTemperatureChart; 
	createTStatechart (& STriacHumidityTemperatureChart, xaStates, eNumberOfStates, eStartState);
	
}


void stopStateCharts()
{
	destructTStatechart(&STriacHumidityTemperatureChart);
}


bool processTriacEvent(TStatechart* tStCh,CGrowBoxEvent* ev)
{
	currentEvent = ev;
	return ProcessEvent(tStCh);
}

