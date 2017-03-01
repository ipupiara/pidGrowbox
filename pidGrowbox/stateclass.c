
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
	printf("exit FatalError\n");
}

uStInt evFatalErrorChecker(void)
{
	uStInt res = uStIntNoMatch;
	//	printf("check for event in State evStateIdle\n");

	return (res);
}


//bool CHumidityStateClass::inState(const uint32 u32State)
//{
	//Mutex m(&humidEngineMutex);
	//bool res;
	//res = (IN_STATE(CHumidityStateClass, engine, u32State));
	//return res;
//}


uStInt evStateGrowBoxKeepingHumidity(void)
{
//	printf("check for event in State evStateGrowBoxKeepingHumidity\n");
	if (currentEvent->evType == eReset)  {
	//{
		//BEGIN_EVENT_HANDLER(CHumidityStateClass, engine, eStateGrowBoxKeepingHumidity );
			//// No event action.
		//END_EVENT_HANDLER(CHumidityStateClass, engine);
		//
///*  left this as an original example for history states
 //
		//BEGIN_EVENT_HANDLER(CHumidityStateClass, engine, eStateGrowBoxKeepingHumidity | u32WithHistory);
			//// No event action.
		//END_EVENT_HANDLER(CHumidityStateClass, engine);
//*/
		//return (u32HandlingDone);
	}
	return (uStIntNoMatch);
}


uStInt evStateHumidifying(void)
{
//	printf("check for event in State evStateHumidifying\n");
	//if ((event.evType == eValueAssignement) && (CGrowBoxDefines::GetHumidifyingUpperLimit() < event.humidity))
	//{
		//BEGIN_EVENT_HANDLER(CHumidityStateClass, engine, eStateIdle);
			//// No event action.
		//END_EVENT_HANDLER(CHumidityStateClass, engine);
		//return (u32HandlingDone);
	//}
	return (uStIntNoMatch);
}


uStInt evStateIdle(void)
{
//	printf("check for event in State evStateIdle\n");

	//if (event.evType == eValueAssignement) 
	//{	if (CGrowBoxDefines::GetHumidifyingLowerLimit() > event.humidity)
		//{
			//BEGIN_EVENT_HANDLER(CHumidityStateClass, engine, eStateHumidifying);
				//// No event action.
			//END_EVENT_HANDLER(CHumidityStateClass, engine);
			//return (u32HandlingDone);
		//}
		//if (CGrowBoxDefines::GetDryingUpperLimit() < event.humidity)
		//{
			//BEGIN_EVENT_HANDLER(CHumidityStateClass, engine, eStateDrying);
				//// No event action.
			//END_EVENT_HANDLER(CHumidityStateClass, engine);
			//return (u32HandlingDone);
		//}
	//}
	return (uStIntNoMatch);
}


uStInt evStateNonVentilating(void)
{
//	printf("check for event in State evStateNonVentilating\n");

	//if ((event.evType == eVentilationStartTimer) || (event.evType ==  eVentilationButtonPressed))
	//{
		//BEGIN_EVENT_HANDLER(CHumidityStateClass, engine, eStateVentilating);
			//// No event action.
		//END_EVENT_HANDLER(CHumidityStateClass, engine);
		//return (u32HandlingDone);
	//
	//}
	return (uStIntNoMatch);
}


uStInt evStateVentilating(void)
{
//	printf("check for event in State evStateVentilating\n");

	//if ((event.evType == eVentilationStopTimer)  || (event.evType ==  eVentilationStopButtonPressed))
	//{
		//BEGIN_EVENT_HANDLER(CHumidityStateClass, engine, eStateNonVentilating);
			//// No event action.
		//END_EVENT_HANDLER(CHumidityStateClass, engine);
		//return (u32HandlingDone);
	//
	//}
	return (uStIntNoMatch);
}

uStInt evStateDrying(void)
{
//	printf("check for event in State evStateDrying\n");
	//if ((event.evType == eValueAssignement) && (CGrowBoxDefines::GetDryingLowerLimit() > event.humidity))
	//{
		//BEGIN_EVENT_HANDLER(CHumidityStateClass, engine, eStateIdle);
			//// No event action.
		//END_EVENT_HANDLER(CHumidityStateClass, engine);
		//return (u32HandlingDone);
	//}
	return (uStIntNoMatch);
}

/*
// State transition/handling methods
// left as an example of this defEntry method

void CHumidityStateClass::defEntryStateGrowBoxKeepingHumidity(void)
{

//	printf("CHumidityStateClass::defEntryStateGrowBoxKeepingHumidity\n");
}
*/



void entryStateGrowBoxKeepingHumidity(void)
{
	info_printf("CHumidityStateClass::entryStateGrowBoxKeepingHumidity\n");
}


void entryStateHumidifying(void)
{
	info_printf("CHumidityStateClass::entryStateHumidifying\n");
//	owner->startHumidifying();
}


void entryStateIdle(void)
{
	info_printf("CHumidityStateClass::entryStateIdle\n");
}


void entryStateNonVentilating(void)
{
	info_printf("CHumidityStateClass::entryStateNonVentilating\n");
	//owner->startVentilatingStartTimer(CGrowBoxDefines::GetIdleVentilationDelayMinutes());
	//owner->onEntryIdleNotVentilating();
}


void entryStateVentilating(void)
{
	info_printf("CHumidityStateClass::entryStateVentilating\n");
	//owner->startVentilating();
	//owner->startVentilatingStopTimer(CGrowBoxDefines::GetIdleVentilationMinutes());
	//owner->onEntryIdleVentilating();
}

void entryStateDrying(void)
{
	info_printf("CHumidityStateClass::entryStateDrying\n");
//	owner->startDrying();
}

void exitStateGrowBoxKeepingHumidity(void)
{
	//  printf("CHumidityStateClass::exitStateGrowBoxKeepingHumidity\n");
}


void exitStateHumidifying(void)
{
	//  printf("CHumidityStateClass::exitStateHumidifying\n");
//	owner->stopHumidifying();
}


void exitStateIdle(void)
{
	//  printf("CHumidityStateClass::exitStateIdle\n");
}


void exitStateNonVentilating(void)
{
	//  printf("CHumidityStateClass::exitStateNonVentilating\n");
	//owner->stopVentilatingStartTimer();
	//owner->onExitIdleNotVentilating();
}


void exitStateVentilating(void)
{
	//  printf("CHumidityStateClass::exitStateVentilating\n");
	//owner->stopVentilating();
	//owner->stopVentilatingStopTimer();
	//owner->onExitIdleVentilating();
}


void exitStateDrying(void)
{
	//  printf("CHumidityStateClass::exitStateDrying\n");
	//owner->stopDrying();
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


bool processTriacEvent(TStatechart* t,CGrowBoxEvent* ev)
{
	currentEvent = ev;
	return ProcessEvent(t);
}

