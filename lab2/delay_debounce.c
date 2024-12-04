#include <stdint.h>

#include "delay_cycles.h"

int
delay_debounce(uint8_t volatile *PIN, uint8_t pin, uint16_t cycles)
{

	if ((*PIN) & (1 << pin)) return 1; //button not pressed

	else {
		//delay by cycles
		delay_cycles(cycles);

		if (!((*PIN >> pin) & 1)) return 0; //button has been pressed and is still pressed
	  	else return 1; //button did not remain pressed
	}
}
