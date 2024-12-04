#include <stdint.h>

int
shift_debounce(uint8_t volatile *PIN, uint8_t pin)
{
	
	//initialize a shift register
	uint8_t shift_register = 0x55;

	while(1) {

		shift_register = (shift_register << 1) | (((*PIN >> pin)) & 1);
		if (shift_register == 255) break;
		else if (shift_register == 0) break;
	}
	
	return ((shift_register >> 7) & 1);
}

