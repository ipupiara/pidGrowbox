
#include <stdio.h>
#include <stdarg.h>
#include <avr/interrupt.h>
#include "TriacDefines.h"

//#include "TriacIntr.h"


#define IdleVentilationDelayMinutes   15.0
#define IdleVentilationMinutes   5.0
#define HumidifyingLowerLimit  88.0
#define HumidifyingUpperLimit   90.0
#define DryingUpperLimit      98.0
#define DryingLowerLimit     96.0





void info_printf(const char *emsg, ...)
{
	va_list ap;
	va_start(ap, emsg);
	
#ifndef printCsvData

	printf( emsg, ap);
#endif
	
	va_end(ap);
}


//  Testmethod  ShowVar( "fcsi", 32.4f, 'a', "Test string", 4 ); 
//void ShowVar( char *szTypes, ... ) 
//{
	//va_list vl;
	//int i;
	//
	//va_start( vl, szTypes );
	//
	//for( i = 0; szTypes[i] != '\0'; ++i ) {
		//
		//union Printable_t {
			//int     i;
			//float   f;
			//char    c;
			//char   *s;
		//} Printable;
		//
		//switch( szTypes[i] ) {   
			//case 'i':
				//Printable.i = va_arg( vl, int );
				//printf( "%i\n", Printable.i );
				//break;
			//
			//case 'f':
				//Printable.f = va_arg( vl, double );
				//printf( "%f\n", Printable.f );
				//break;
			//
			//case 'c':
				//Printable.c = va_arg( vl, char );
				//printf_s( "%c\n", Printable.c );
				//break;
			        //
			//case 's':
				//Printable.s = va_arg( vl, char * );
				//printf_s( "%s\n", Printable.s );
				//break;
			        //
			//default:
				//break;
		//}
	//}
	//va_end( vl );
//}
//

#define ownEepromMethods 

#ifndef ownEepromMethods
#include <avr/eeprom.h>
#else

// ATTENTION: use of EEPROM needs BOD Level of at least 2.7 V, otherwise EEPROM memory
// is likely to crash on restore when done at mcu startup

void EEPROM_write(unsigned int uiAddress, unsigned char ucData)
{
	// Wait for completion of previous write 
	while(EECR & (1<<EEWE));
	// Set up address and Data Registers 
	EEAR = uiAddress;
	EEDR = ucData;
	cli();
	
	// Write logical one to EEMPE 
	//EECR |= (1<<EEMPE);
	// Start eeprom write by setting EEPE 
	//EECR |= (1<<EEPE);
	

	// standard EEPROM code does not work under all optimization levels (-o directive of compiler)
    // for timing reasons (max. 4 cpu cycles between the two sbi commands for safety reasons)
	 asm volatile (
     "sbi 0x1f,0x02" "\r\n"
     "sbi 0x1f,0x01" "\r\n"
       );
	sei();
}

unsigned char EEPROM_read(unsigned int uiAddress)
{
	// Wait for completion of previous write 
	while(EECR & (1<<EEWE))
	;
	// Set up address register 
	EEAR = uiAddress;
	// Start eeprom read by writing EERE 
	EECR |= (1<<EERE);
	// Return data from Data Register 
	return EEDR;
}

void eeprom_write_byte (uint8_t *adr, uint8_t val)
{
	uint8_t checkRes = 0;
	uint16_t adre = (uint16_t) adr;
	EEPROM_write(adre,*(&val));
	checkRes = EEPROM_read(adre); 
	if (val != checkRes) {
		info_printf("eeprom stored %X, but check returned %X\n",val,checkRes);
	} else {
		info_printf("reread ok returned %X\n",checkRes);
	}
}

uint8_t eeprom_read_byte (const uint8_t *adr)
{
	unsigned char bu;
	uint16_t adre = (uint16_t) adr;
	bu =  EEPROM_read(adre); 
	return bu;
}

uint16_t eeprom_read_word (uint16_t* adr)
{	
	uint16_t res;
	unsigned char bu;
	uint16_t adre;
	adre = (uint16_t) adr; 

	res = 0;
	bu = EEPROM_read(adre); 
	*(&res) = bu;
	bu = EEPROM_read(adre + 1); 
	*((unsigned char*)&res+1) = bu;

	return res;
}

void eeprom_write_word(uint16_t* adr, uint16_t val)
{
	uint16_t adre;
	adre = (uint16_t) adr; 
	EEPROM_write(adre,*(&val));
	EEPROM_write(adre + 1,*((unsigned char*)&val + 1));
}



#endif



void delayEmptyProc ()
{
}



void delay6pnt2d5us(unsigned int enn)
{
//delay approx ( n * 2.5 us ) + 6    at 11.0592  mhz  
//    6 Plus N Times  2 Dot 5  (  "6pnt2d5"  )

//n        t    tested on Simulator 2  (PN 28. May 2011)

//0		6
//1		8 - 9
//2		11
//4		16
//8		25 -26
//16	45   approx 500 cycles

//  accurate enough for our keyboard delays


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"

uint8_t ix;  
	ix= 0;

  while(enn--){
  }
} 

#pragma GCC diagnostic pop

void initDefines()
{

    fatalErrorOccurred = 0;
}



floatType  GetIdleVentilationDelayMinutes()
{
	return IdleVentilationDelayMinutes;
}

floatType  GetIdleVentilationMinutes()
{
	return IdleVentilationMinutes;
}

floatType  GetHumidifyingLowerLimit()
{
	return HumidifyingLowerLimit;
}

floatType GetHumidifyingUpperLimit()
{
	return HumidifyingUpperLimit;
}

floatType  GetDryingUpperLimit()
{
	return DryingUpperLimit;
}
floatType  GetDryingLowerLimit()
{
	return DryingLowerLimit;
}
