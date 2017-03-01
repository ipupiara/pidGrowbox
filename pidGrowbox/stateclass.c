
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

CJoesTriacEvent* currentEvent;



// This defines and names the states the class has.
#warning  attention: sequence must be the same as in xaStates (below)  !!!


//// This defines and names the states the class has.
//enum eStates
//{
	//eStateGrowBoxKeepingHumidity,
	//eStartState = eStateGrowBoxKeepingHumidity,
	//eStateHumidifying,
	//eStateIdle,
	//eStateNonVentilating,
	//eStateVentilating,
	//eStateDrying,
	//eNumberOfStates
//};

enum eStates
{
	eStateJoesTriac,
	eStartState = eStateJoesTriac,
	eStateTriacOperating,
	eStateFatalError,
	eNumberOfStates
};


uStInt evJoesTriacChecker(void)
{
	return (uStIntNoMatch);
}

uStInt evTriacOperatingChecker(void)
{
	uStInt res = uStIntNoMatch;
//	printf("check for event in State evTriacOperating\n");

	if (currentEvent->evType == evFatalError) 
	{	
			BEGIN_EVENT_HANDLER(PJoesTriacStateChart, eStateFatalError);
				// No event action.
			END_EVENT_HANDLER(PJoesTriacStateChart);
			res =  uStIntHandlingDone;
	}
	
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
	printf("exit FatalError\n");
}

uStInt evFatalErrorChecker(void)
{
	uStInt res = uStIntNoMatch;
	//	printf("check for event in State evStateIdle\n");

	return (res);
}



#ifndef  sdccNULL 

#define tfNull 0

#else

t_fvoid  tfNull;

#endif 

// attention: sequence must be the same as above enum eStates

xStateType xaStates[eNumberOfStates] = {
 	{eStateJoesTriac,    // name
 	-1,									//parent
 	eStateTriacOperating,    // default substate
 	0,    // keep history
 	evJoesTriacChecker,    // event checking fu
	tfNull,       // def state entry function
 	tfNull,     //    entering state
 	tfNull},     // exiting state

 	{eStateTriacOperating,
 	eStateJoesTriac,
 	0,
 	0,									
 	evTriacOperatingChecker,
 	tfNull,
 	tfNull,
 	tfNull},


 
	
	{eStateFatalError,
 	eStateJoesTriac,
 	-1,
 	0,
 	evFatalErrorChecker,
 	tfNull,
 	entryFatalErrorState,
 	exitFatalErrorState}	 	 	 
};








/*
void entry...State(void)
{
//	printf("entry I\n");
}

void exit...State(void)
{
//	printf("exit I\n");
}

uStInt ev...Checker(void)
{
//	printf("check for event in State evStateIdle\n");

//	if (currentEvent->evType == eValueAssignement) 
//	{	if (HumidifyingLowerLimit > currentEvent->humidity)
//		{
//			BEGIN_EVENT_HANDLER(PJoesTriacStateChart, eStateHumidifying);
//				// No event action.
//			END_EVENT_HANDLER(PJoesTriacStateChart);
//			return (u32HandlingDone);
//		}
//	}
	return (u32NoMatch);
}

*/

/*

enum eStates
{
	eStateJoesTriac,
	eStartState = eStateJoesTriac,
	eStateAskForCalibration,
	eStateCalibrating,
	eNumberOfStates
};


uStInt evJoesTriacChecker(void)
{
	return (uStIntNoMatch);
}

uStInt evAskForCalibrationChecker(void)
{
	return (uStIntNoMatch);
}

uStInt evCalibratingChecker(void)
{
	return (uStIntNoMatch);
}


#ifndef  sdccNULL 

#define tfNull 0

#else

t_fvoid  tfNull;

#endif 

xStateType xaStates[eNumberOfStates] = {
 	{eStateJoesTriac,    // name
 	-1,									//parent
 	eStateAskForCalibration,    // default substate
 	0,    // keep history
 	evJoesTriacChecker,    // event checking fu
	tfNull,       // def state entry function
 	tfNull,     //    entering state
 	tfNull},     // exiting state

 	{eStateAskForCalibration,
 	eStateJoesTriac,
 	-1,
 	0,									
 	evAskForCalibrationChecker,
 	tfNull,
 	tfNull,
 	tfNull},

 	{eStateCalibrating,
 	eStateJoesTriac,
 	-1,
 	0,
 	evCalibratingChecker,
 	tfNull,
 	tfNull,
 	tfNull},
	 

};

*/


void startStateCharts()
{

#ifdef  sdccNULL 

	tfNull = (t_fvoid ) NULL;

#endif 

 	PJoesTriacStateChart = & SJoesTriacStateChart; 
	createTStatechart (& SJoesTriacStateChart, xaStates, eNumberOfStates, eStartState);
	
}


void stopStateCharts()
{
	destructTStatechart(&SJoesTriacStateChart);
}


bool processTriacEvent(TStatechart* t,CJoesTriacEvent* ev)
{
	currentEvent = ev;
	return ProcessEvent(t);
}

