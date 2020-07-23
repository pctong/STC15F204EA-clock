/*---------------------------------------------------------------------------------*/
/* --- STC MCU International Limited -------------------------------------
*/
/* --- STC 15 Series MCU A/D Conversion Demo -----------------------
*/
/* --- Mobile: (86)13922805190 --------------------------------------------
*/
/* --- Fax: 86-755-82944243 -------------------------------------------------*/
/* --- Tel: 86-755-82948412 -------------------------------------------------
*/
/* --- Web: www.STCMCU.com --------------------------------------------
*/
/* If you want to use the program or the program referenced in the  ---*/
/* article, please specify in which data and procedures from STC    ---
*/
/*----------------------------------------------------------------------------------*/
#include <stc12.h>
#include <stdint.h>

#define _nop_ __asm nop __endasm;

/*Define ADC operation const for ADC_CONTR*/
#define ADC_POWER   0x80            //ADC power control bit
#define ADC_FLAG    0x10            //ADC complete flag
#define ADC_START   0x08            //ADC start control bit
#define ADC_SPEEDLL 0x00             //540 clocks
#define ADC_SPEEDL  0x20            //360 clocks
#define ADC_SPEEDH  0x40            //180 clocks
#define ADC_SPEEDHH 0x60            //90 clocks

/*----------------------------
Initial ADC sfr
----------------------------*/
void InitADC(uint8_t chan)
{
	P1ASF |= 1 << chan;             //enable channel ADC function
	ADC_RES = 0;                    //Clear previous result
	ADC_CONTR = ADC_POWER | ADC_SPEEDLL;
}

/*----------------------------
Get ADC result - 10 bit
----------------------------*/
uint16_t getADCResult(uint8_t chan)
{
	ADC_CONTR = ADC_POWER | ADC_SPEEDHH | ADC_START | chan;
	_nop_;       //Must wait before inquiry
	while (!(ADC_CONTR & ADC_FLAG));  //Wait complete flag
	ADC_CONTR &= ~ADC_FLAG;           //Close ADC
	return  ADC_RES << 2 | (ADC_RESL & 0b11) ;  //Return ADC result
}

