
#if !defined(CCtateClassHeader)
#define CCtateClassHeader

#include "TStatechart.h"
#include "TriacDefines.h"

TStatechart	 STriacHumidityTemperatureChart;
TStatechart* PTriacHumidityTemperatureChart;

TStatechart	 SGrowboxI2CChart;
TStatechart* PGrowboxI2CChart;
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
	eCharEntered,
	eF1Pressed,
	ePressed,
	eAstPressed,
	eNumPressed,
	eTimeOutDurationTimer,
	eSecondsTick,
	eStartPressed,
	eStopPressed,
	eAdcTick,
	eTWIDataReceived,
	eFatalError
};


typedef struct  {
	int evType;
	floatType humidity;
	floatType temperature;
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


