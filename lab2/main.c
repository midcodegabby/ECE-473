#include <avr/io.h>

#include "delay_debounce.h"
#include "shift_debounce.h"
#include "async_debounce.h"

int
main()
{
  /* Set up Port B for LED output */
  DDRB = 0xff;
  //PORTB = 0x00; //set to off
  PORTB = 0b10100000; //set to off

  /* Set up Port D for Input */
  DDRD = 0x00;
  PORTD = 0xff; /* Pull-up pins */

  uint8_t pd5_state = 1;
  uint8_t prevpd5_state = 1;
  int8_t pd6_cnt = 0;
  uint8_t pd6_state = 0;
  uint8_t pd7_state = 1;
  uint8_t prevpd7_state = 1;
  uint16_t cycles = 10;

  for (;;) {

	//update the previous states
	prevpd5_state = pd5_state;
	prevpd7_state = pd7_state;
	
	//get the current pd5 state; 0 = on, 1 = off	
	pd5_state = shift_debounce(&PIND, 5); //debounce PD5  
	 
	//use if statement to check for rising edge only then toggle PB5
	if ((pd5_state != prevpd5_state) && (prevpd5_state == 0)) PORTB ^= (1 << 5); 

    	async_debounce(&PIND, 6, &pd6_cnt); //debounce PD6
	
	//get the current pd7 state
	pd7_state = delay_debounce(&PIND, 7, cycles);

	//use if statement to check for rising edge only then toggle PB7
	if ((pd7_state != prevpd7_state) && (prevpd7_state == 0)) PORTB ^= (1 << 7); 

    	if (pd6_cnt > 32 && pd6_state == 0) { //check if pd6_cnt is over 32
      		pd6_state = 1; // 1 is on
      		PORTB ^= (1 << 6);
	}

      	//check other condition, where pd6_cnt is below 32 and the leds are on
     	else if (pd6_cnt < 32 && pd6_state == 1) {
      		pd6_state = 0; // 0 is off
    	}
  }
}
