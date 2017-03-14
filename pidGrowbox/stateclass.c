
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
enum eHumidifyingStates
{
	eStateGrowBoxKeepingHumidity,
	eHumidifyingStartState = eStateGrowBoxKeepingHumidity,
	eStateHumidityControlRunning,
	eStateHumidifying,
	eStateIdle,
	eStateNonVentilating,
	eStateVentilating,
	eStateDrying,
	eStateFatalError,
	eNumberOfHumidifyingStates
};


uStInt evFatalErrorChecker(void)
{
	uStInt res = uStIntNoMatch;
	info_printf("check for event in State evStateIdle\n");

	return (res);
}


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
	
	if (currentEvent->evType == eFatalError)
	
	{
		BEGIN_EVENT_HANDLER(PTriacHumidityTemperatureChart, eStateFatalError );
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



void entryStateGrowBoxKeepingHumidity(void)
{
	info_printf("CHumidityStateClass::entryStateGrowBoxKeepingHumidity\n");
}

void exitStateGrowBoxKeepingHumidity(void)
{
	info_printf("CHumidityStateClass::exitStateGrowBoxKeepingHumidity\n");
}


uStInt evStateHumidityControlRunning(void)
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

void entryStateHumidityControlRunning(void)
{
	info_printf("CHumidityStateClass::entryStateHumidityControlRunning\n");
}


void exitStateHumidityControlRunning(void)
{
	info_printf("CHumidityStateClass::exitStateHumiditiyControlRunning\n");
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


void entryStateHumidifying(void)
{
	info_printf("CHumidityStateClass::entryStateHumidifying\n");
	startHumidifying();
}


void exitStateHumidifying(void)
{
	info_printf("CHumidityStateClass::exitStateHumidifying\n");
	stopHumidifying();
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


void entryStateIdle(void)
{
	info_printf("CHumidityStateClass::entryStateIdle\n");
}


void exitStateIdle(void)
{
	info_printf("CHumidityStateClass::exitStateIdle\n");
}



uStInt evStateNonVentilating(void)
{
	info_printf("check for event in State evStateNonVentilating\n");

	if ((currentEvent->evType == eTimeOutDurationTimer) || (currentEvent->evType ==  eVentilationButtonPressed))
	{
		BEGIN_EVENT_HANDLER(PTriacHumidityTemperatureChart,   eStateVentilating);
			// No event action.
		END_EVENT_HANDLER(PTriacHumidityTemperatureChart );
		return (uStIntHandlingDone);
	
	}
	return (uStIntNoMatch);
}


void entryStateNonVentilating(void)
{
	info_printf("CHumidityStateClass::entryStateNonVentilating\n");
	startDurationTimer(GetIdleVentilationDelayMinutes() * 60);
}




void exitStateNonVentilating(void)
{
	info_printf("CHumidityStateClass::exitStateNonVentilating\n");
	stopDurationTimer();
}


uStInt evStateVentilating(void)
{
	info_printf("check for event in State evStateVentilating\n");

	if ((currentEvent->evType == eTimeOutDurationTimer)  || (currentEvent->evType ==  eVentilationStopButtonPressed))  {
		BEGIN_EVENT_HANDLER(PTriacHumidityTemperatureChart,   eStateNonVentilating);
			// No event action.
		END_EVENT_HANDLER(PTriacHumidityTemperatureChart );
		return (uStIntHandlingDone);
	
	}
	return (uStIntNoMatch);
}


void entryStateVentilating(void)
{
	info_printf("CHumidityStateClass::entryStateVentilating\n");
	startVentilating();
	startDurationTimer(GetIdleVentilationMinutes() * 60);
}

void exitStateVentilating(void)
{
	info_printf("CHumidityStateClass::exitStateVentilating\n");
	stopVentilating();
	stopDurationTimer();
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


void entryStateDrying(void)
{
	info_printf("CHumidityStateClass::entryStateDrying\n");
	startDrying();
}


void exitStateDrying(void)
{
	info_printf("CHumidityStateClass::exitStateDrying\n");
	stopDrying();
}


/*
// State transition/handling methods
// left as an example of this defEntry method

void CHumidityStateClass::defEntryStateGrowBoxKeepingHumidity(void)
{

	info_printf("CHumidityStateClass::defEntryStateGrowBoxKeepingHumidity\n");
}
*/




#ifndef  sdccNULL 

#define tfNull 0

#else

t_fvoid  tfNull;

#endif


#ifdef  sdccNULL

tfNull = (t_fvoid ) NULL;

#endif 

// attention: sequence must be the same as above enum eStates

xStateType xaHumidifyingStates[eNumberOfHumidifyingStates] = {
		{eStateGrowBoxKeepingHumidity,
			-1,
			eStateHumidityControlRunning,
			0,
			evStateGrowBoxKeepingHumidity,
			NULL,  
			entryStateGrowBoxKeepingHumidity,
			exitStateGrowBoxKeepingHumidity},
			
				{eStateHumidityControlRunning,
					eStateGrowBoxKeepingHumidity,
					eStateIdle,
					0,
					evStateHumidityControlRunning,
					NULL,
					entryStateHumidityControlRunning,
				exitStateHumidityControlRunning},

/* name						*/	{eStateHumidifying,
	/* parent					*/	eStateGrowBoxKeepingHumidity,
	/* default_substate			*/	0,
									0,     //(  keep history)
	/* event-checking func		*/	evStateHumidifying,
	/* default state entry func	*/	NULL,
	/* entering state func		*/	entryStateHumidifying,
/* exiting state func		*/		exitStateHumidifying},

/* name						*/	{eStateIdle,
	/* parent					*/	eStateGrowBoxKeepingHumidity,
	/* default_substate			*/	eStateVentilating,
									0,
	/* event-checking func		*/	evStateIdle,
	/* default state entry func	*/	NULL,
	/* entering state func		*/	entryStateIdle,
/* exiting state func		*/		exitStateIdle},

/* name						*/	{eStateNonVentilating,
	/* parent					*/	eStateIdle,
	/* default_substate			*/	-1,
									0,
	/* event-checking func		*/	evStateNonVentilating,
	/* default state entry func	*/	NULL,
	/* entering state func		*/	entryStateNonVentilating,
/* exiting state func		*/		exitStateNonVentilating},

/* name						*/	{eStateVentilating,
	/* parent					*/	eStateIdle,
	/* default_substate			*/	-1,
									0,
	/* event-checking func		*/	evStateVentilating,
	/* default state entry func	*/	NULL,
	/* entering state func		*/	entryStateVentilating,
/* exiting state func		*/		exitStateVentilating},

/* name						*/	{eStateDrying,
	/* parent					*/	eStateGrowBoxKeepingHumidity,
	/* default_substate			*/	-1,
									0,
	/* event-checking func		*/	evStateDrying,
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





/***   I2C states  ********/

// This defines and names the states the class has.
enum eI2CStates
{
	eStateGrowboxI2C,
	eI2CStartState = eStateGrowboxI2C,
	eStateI2CIdle,
	eStateI2CWaitForResponse,
	eNumberOfI2CStates
};



void entryGrowboxI2C(void)
{
	info_printf("::entryStateGrowboxI2C\n");
}



uStInt evI2CIdleChecker(void)
{
//	info_printf("check for event in State evStateI2CIdle\n");

//	if ((currentEvent->evType == eTimeOutDurationTimer) || (currentEvent->evType ==  eVentilationButtonPressed))
//	{
//		BEGIN_EVENT_HANDLER(PTriacHumidityTemperatureChart,   eStateVentilating);
			// No event action.
//		END_EVENT_HANDLER(PTriacHumidityTemperatureChart );
//		return (uStIntHandlingDone);
	
//	}
	return (uStIntNoMatch);
}


void entryStateI2CIdle(void)
{
//	info_printf("::entryStateI2CIdle\n");
}




void exitStateI2CIdle(void)
{
//	info_printf("::exitStateI2CIdle\n");
}


uStInt evI2CWaitForResponseChecker(void)
{
//	info_printf("check for event in State evStateI2CWaitForResponse\n");

//	if ((currentEvent->evType == eTimeOutDurationTimer) || (currentEvent->evType ==  eVentilationButtonPressed))
//	{
//		BEGIN_EVENT_HANDLER(PTriacHumidityTemperatureChart,   eStateVentilating);
			// No event action.
//		END_EVENT_HANDLER(PTriacHumidityTemperatureChart );
//		return (uStIntHandlingDone);
	
//	}
	return (uStIntNoMatch);
}


void entryStateI2CWaitForResponse(void)
{
//	info_printf("::entryStateI2CWaitForResponse\n");
}




void exitStateI2CWaitForResponse(void)
{
//	info_printf("::exitStateI2CWaitForResponse\n");
}




xStateType xaI2CStates[eNumberOfI2CStates] = {
	{	eStateGrowboxI2C,
	 	-1,
	 	eStateI2CIdle,             // default child
	 	0,				// keep history
	 	tfNull,
	 	tfNull,				// default entry ?
	 	entryGrowboxI2C,
	 	tfNull
	}	,
		 
	{	eStateI2CIdle,
		eStateGrowboxI2C,
		-1,             // default child
		0,				// keep history
		evI2CIdleChecker,
		tfNull,				// default entry ?
		entryStateI2CIdle,
		exitStateI2CIdle
	}	,
			  
	{	eStateI2CWaitForResponse,
		eStateGrowboxI2C,
		-1,             // default child
		0,				// keep history
		evI2CWaitForResponseChecker,
		tfNull,				// default entry ?
		entryStateI2CWaitForResponse,
		exitStateI2CWaitForResponse
	}				  
	
};



void startStateCharts()
{

 	PTriacHumidityTemperatureChart = & STriacHumidityTemperatureChart; 
	createTStatechart (& STriacHumidityTemperatureChart, xaHumidifyingStates, eNumberOfHumidifyingStates, eHumidifyingStartState);
	
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

