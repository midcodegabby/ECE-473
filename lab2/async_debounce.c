#include <stdint.h>
void async_debounce(uint8_t volatile *PIN, uint8_t pin, int8_t *counter)
{
	//if PD6 is 1 (off)
	if (((*PIN >> 6) & 1) == 1) {
		if (*counter < 0) *counter = 0;
		else *counter += 1;
	}

	//if PD6 is 0 (on)
	else {
		if (*counter > 0) *counter = 0;
		else *counter -= 1;
	}

	//set counter to 64/-64
	if (*counter >= 64) *counter = 64;
	       
	else if (*counter <= -64) *counter = -64;
}
