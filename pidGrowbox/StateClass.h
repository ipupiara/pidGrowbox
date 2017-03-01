
#if !defined(CCtateClassHeader)
#define CCtateClassHeader

#include "TStatechart.h"
#include "TriacDefines.h"

TStatechart	 STriacHumidityTemperatureChart;
TStatechart* PTriacHumidityTemperatureChart;
//
//
//enum eEventTypes
//{
	//eReset,
	//eValueAssignement,
	//eVentilationStartTimer,
	//eVentilationStopTimer,
	//eVentilationButtonPressed,
	//eVentilationStopButtonPressed
//};

enum eEventTypes
{
	eReset,
	eValueAssignement,
	eVentilationStartTimer,
	eVentilationStopTimer,
	eVentilationButtonPressed,
	eVentilationStopButtonPressed,
	evCharEntered,
	evF1Pressed,
	evF2Pressed,
	evAstPressed,
	evNumPressed,
	evTimeOutDurationTimer,
	evSecondsTick,
	evStartPressed,
	evStopPressed,
	evAdcTick,
	evFatalError
};


typedef struct  {
	int evType;
	floatType humidity;
	union {
		int8_t keyCode;
		struct {			// currently not in use
			floatType   voltage;  
			int8_t  potiPos;
			int8_t  jobType;
		} zeroAdjustingState;
	}  evData;
} CGrowBoxEvent ;




void startStateCharts();


void stopStateCharts();


bool processTriacEvent(TStatechart* tStCh,CGrowBoxEvent* ev);




#endif


