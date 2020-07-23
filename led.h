// LED functions for 4-digit seven segment led

#include <stdint.h>
#include "config.h"

// index into ledtable[]
#define LED_BLANK  10
#define LED_DASH   11
#define LED_TEMP   12

static const uint8_t ledtable[] = {
	// digit to led digit lookup table
	// dp,g,f,e,d,c,b,a
	// negative image
	0b11000000, // 0
	0b11111001, // 1
	0b10100100, // 2
	0b10110000, // 3
	0b10011001, // 4
	0b10010010, // 5
	0b10000010, // 6
	0b11111000, // 7
	0b10000000, // 8
	0b10010000, // 9
	0b11111111, // ' '
	0b10111111, // '-'

	#if CFG_TEMP_UNIT == 'F'
	0b10001110, // F
	#else
	0b11000110, // C
	#endif
};

