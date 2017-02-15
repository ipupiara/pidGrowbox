
#include <stdio.h>
#include <stdarg.h>
#include <avr/interrupt.h>
#include "TriacDefines.h"

//#include "TriacIntr.h"



void info_printf( char *emsg, ...)
{
	va_list ap;
	
#ifndef printCsvData
	printf( emsg, ap);
#endif
	
	va_end(ap);
}



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


