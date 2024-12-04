#include <avr/io.h>
#include <util/delay.h>
#ifndef F_CPU
#define F_CPU 8000000UL
#endif

int
main()
{
  DDRB = 0xff;
  for (;;) {
    //PORTB = 0xFF;
    PORTB = 0b10101010;
    _delay_ms(500);
    PORTB = 0x00;
    _delay_ms(500);
  }
}
