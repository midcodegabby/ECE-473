#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <math.h>

#include "lcd/lcd.h"
#include "twi/twi_config.h"
#include "twi/twi.h"

//define a typedef for echoes
typedef enum {
	echo_not_ready,
	echo_start,
	echo_end,
} echo_t;

//global variables:
uint8_t mutex = 1;
uint16_t ultrasonic_count = 0;
volatile echo_t _ECHO_STAGE;

//function that reads temperature of a temperature sensor
int8_t read_temp(uint8_t address){
	
      int8_t temp[] = {0,0};
      
      twi_start();
      twi_sla_r(address); //tell sensor we want to read data
      temp[1] = twi_read(TWI_ACK); 
      temp[0] = twi_read(TWI_NACK);
      twi_stop();
	
      return temp[1];
}

//this function creates a pulse that starts the ultrasonic sensor reading
void start_ultrasonic(void) {
	//pulse must remain high for at least 10us, but we will go with 15us to be safe
	PORTD |= (1 << PD5); //set PD5 High
	_delay_us(15); //delay for 15us
	PORTD &= ~(1 << PD5); //set PD5 Low

	//configure echo pin to trigger interrupt on rising edge:
	TIFR1 |= (1 << ICF1); //clear Input Capture Flag
	TCCR1B |= (1 << ICES1); //echo triggers interrupt on rising edge (1 for rising, 0 for falling)
	
	_ECHO_STAGE = echo_start; //set global to start

}

//interrupt for Echo pin
ISR(TIMER1_CAPT_vect) {
	if (_ECHO_STAGE == echo_start) {
	//if (TCCR1B & (1 << ICES1)) {
		TIFR1 |= (1 << ICF1); //clear Input Capture Flag
		TCCR1B &= ~(1 << ICES1); //echo triggers interrupt on falling edge

		_ECHO_STAGE = echo_end;

		TCNT1 = 0x0000; //initialize timer to 0
	}

	else {
		ultrasonic_count = ICR1; //read Input Capture Register value
		_ECHO_STAGE = echo_not_ready;
	}
}

int
main()
{
  /* Initialize LCD */
  lcd_init();

  /* Turn on LCD */
  lcd_on();

  /* Turn on LCD backlight to full brightness */
  lcd_led_set(0xff);

  /* Flush test message to LCD RAM */
  lcd_clear();

  //old format
  //char buf1[] = "     0C       0C";
  //char buf2[] = "Distance      cm";
  //lcd_text_buffer_t tb3 = {buf1, " Indoor  Outdoor", "", buf2};

  //New format
  char buf0[] = "Outdoor        C";
  char buf1[] = "Indoor         C";
  char buf2[] = "Distance      cm";
  lcd_text_buffer_t tb3 = {buf0, buf1, "", buf2};

  int8_t in_temp = 0;
  int8_t new_in_temp = 0;
  int8_t out_temp = 0;
  int8_t new_out_temp = 0;

  uint16_t old_ultrasonic_count = 0;

  //set up ports for ultrasonic sensor
  DDRD &= ~(1 << PD4); //set ICP1 (echo pin) to input
  DDRD |= (1 << PD5); //set random port, PD5 (trig pin)  for output 

  //do interrupt init for ultrasonic sensor
  TIMSK1 |= (1 << ICIE1); //enable Input Capture Interrupts
  TCCR1B |= (0x3); //set clock to run at 1/8 MHz (1/8 us per cycle)
  sei(); //enable global interrupts

  _ECHO_STAGE = echo_not_ready;

  twi_init();

  for (;;) {
    for (;;) {
      /* Use TWI to get a temperature reading from the on-board temperature sensor 
       * Refer to spec sheet and bens_board schematic for address info
       */

      //begin the ultrasonic reading only if it is not currently in use
      if (_ECHO_STAGE == echo_not_ready) { 
	     start_ultrasonic();
      } 
	      
      
      //this section is for external temp sensor
      new_in_temp = read_temp(T_ADDR);
      new_out_temp = read_temp(L_ADDR);

      //check if there is a diff temp reading
      if ((new_in_temp != in_temp) || (new_out_temp != out_temp) || (ultrasonic_count != old_ultrasonic_count)) {
	in_temp = new_in_temp; //update inside temp
	out_temp = new_out_temp; //update outside temp
	old_ultrasonic_count = ultrasonic_count; //update ultrasonic count
	break;
      }

      else {
        //Don't refresh output if nothing changed 
        _delay_ms(10);
	continue;
      }

    }
   
    //calculate distance
    uint16_t distance = 8*old_ultrasonic_count/58; //cm
    //uint16_t distance = 8*old_ultrasonic_count/147; //in
    
    //use snprintf to put distance into buf2
    snprintf(buf2, 17, "Distance   %d    ", distance);
    snprintf(buf1, 17, "Indoor       %d  ", in_temp);
    snprintf(buf0, 17, "Outdoor      %d  ", out_temp);

    //manually add extra chars so that no overwrites
    buf0[15] = 'C';
    buf1[15] = 'C';

    //cm
    buf2[14] = 'c';
    buf2[15] = 'm';

    //inches
    //buf2[14] = 'i';
    //buf2[15] = 'n';

    //This part is for alt format of temp printings (temps on bottom, in/out on top)
    /*
    //convert both temperature ints (and distance) to strings
    uint8_t in_tmp = in_temp > 0 ? in_temp : -in_temp;
    uint8_t out_tmp = out_temp > 0 ? out_temp : -out_temp;
    
    for (int i = 0; i < 14; ++i) {
      if (out_tmp == 0) {
        buf1[14 - i] = ' ';
      } 
      else {
        buf1[14 - i] = (out_tmp % 10) + '0';
        out_tmp /= 10;
        if (out_tmp == 0 && out_temp < 0) {
          ++i;
          buf1[14 - i] = '-';
        }
      }
    }
    
    for (int i = 0; i < 5; ++i) {
      if (in_tmp == 0) {
        buf1[5 - i] = ' ';
      } 
      else {
        buf1[5 - i] = (in_tmp % 10) + '0';
        in_tmp /= 10;
        if (in_tmp == 0 && in_temp < 0) {
          ++i;
          buf1[5 - i] = '-';
        }
      }
    }

    buf1[6] = 'C';
    */
    
    /* Write string to LCD */
    lcd_flush_text(tb3);
  }
}
