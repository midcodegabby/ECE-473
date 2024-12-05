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
uint16_t ultrasonic_count = 0;
volatile echo_t _ECHO_STAGE;

volatile uint8_t seconds = 0;
volatile uint8_t minutes = 0;
volatile uint8_t hours = 0;

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
	PORTD |= (1 << PD2); //set PD5 High
	_delay_us(15); //delay for 15us
	PORTD &= ~(1 << PD2); //set PD5 Low

	//configure echo pin to trigger interrupt on rising edge:
	TIFR1 |= (1 << ICF1); //clear Input Capture Flag
	TCCR1B |= (1 << ICES1); //echo triggers interrupt on rising edge (1 for rising, 0 for falling)
	
	_ECHO_STAGE = echo_start; //set global to start

}

int shift_debounce(volatile uint8_t *PIN, uint8_t pin)
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

    

//interrupt for Echo pin
ISR(TIMER1_CAPT_vect) {
	if (_ECHO_STAGE == echo_start) {
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

//interrupt for OVR match for TCNT3
ISR(TIMER3_COMPA_vect) {
	//modify globals
	if (seconds == 59) {
		seconds = 0;
		
		if (minutes == 59) {
			minutes = 0;

			if (hours == 23) {
				hours = 0;
			}
			else {
				hours += 1;
			}
		}
		else {
			minutes += 1;
		}
	}
	else {
		seconds += 1;
	}

	TCNT3 = 0x0;
}

int
main()
{
  /* Initialize LCD */
  lcd_init();

  /* Turn on LCD */
  lcd_on();

  /* Turn on LCD backlight to full brightness */
  //lcd_led_set(0xff);

  /* Flush test message to LCD RAM */
  lcd_clear();

  //New format
  char buf0[] = "Outdoor        C";
  char buf1[] = "Indoor         C";
  char buf2[] = "Distance      cm";
  char buf3[] = "                ";
  lcd_text_buffer_t tb3 = {buf0, buf1, buf2, buf3};

  int8_t in_temp = 0;
  int8_t new_in_temp = 0;
  int8_t out_temp = 0;
  int8_t new_out_temp = 0;

  uint16_t old_ultrasonic_count = 0;

  uint16_t old_distance = 0;
  uint16_t new_distance = 0;
  int16_t delta_distance = 0;

  uint8_t brightness = 0;

  //set up ports for ultrasonic sensor
  DDRD &= ~(1 << PD4); //set ICP1 (echo pin) to input
  DDRD |= (1 << PD2); //set random port, PD2 (trig pin)  for output 

  //do interrupt init for ultrasonic sensor
  TIMSK1 |= (1 << ICIE1); //enable Input Capture Interrupts
  TCCR1B |= (0x3); //set clock to run at 1/8 MHz (1/8 us per cycle)

  _ECHO_STAGE = echo_not_ready;

  //set up TCNT3 for clock
  TCCR3B |= (1 << CS32); //set clock prescaler to 1/256
  OCR3A = 0x7A12; 	//1 second per hit
  TIMSK3 |= (1 << OCIE3A); //enable OCRA interrupt 

  //button interface setup
  PORTD |= (1 << PD7)|(1 << PD6)|(1 << PD5); //pull-ups
  DDRD &= ~(1 << PD7)|(1 << PD6)|(1 << PD5); //inputs

  volatile uint8_t pd7_state = 1;
  volatile uint8_t prevpd7_state = 1;
  volatile uint8_t pd6_state = 1;
  volatile uint8_t prevpd6_state = 1;
  volatile uint8_t pd5_state = 1;
  volatile uint8_t prevpd5_state = 1;

  sei(); //enable global interrupts

  twi_init();

  for (;;) {
    for (;;) {
      /* Use TWI to get a temperature reading from the on-board temperature sensor 
       * Refer to spec sheet and bens_board schematic for address info
       */
     
      prevpd7_state = pd7_state;
      prevpd6_state = pd6_state;
      prevpd5_state = pd5_state;
      
	
      pd7_state = shift_debounce(&PIND, 7);
      pd6_state = shift_debounce(&PIND, 6);
      pd5_state = shift_debounce(&PIND, 5);

      if ((pd7_state != prevpd7_state) && (prevpd7_state == 0)) {
	if (hours <= 22) {
	  hours += 1;
	}
	else {
	  hours = 0;
	}
      }

      if ((pd6_state != prevpd6_state) && (prevpd6_state == 0)) {
	if (minutes <= 58) {
	  minutes += 1;
	}
	else {
          minutes = 0;
	  
	  if (hours == 23) {
	    hours = 0;
	  }
          else {
	    hours += 1;
	  }
	}
      }

      if ((pd5_state != prevpd5_state) && (prevpd5_state == 0)) {
	if (brightness == 0xFF) {
	  brightness = 0;
	  lcd_led_set(brightness);
	}
        
	else {
	  brightness += 15;
	  lcd_led_set(brightness);
        }
      }

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
    new_distance = 8*old_ultrasonic_count/58; //cm

    old_distance = new_distance; //update old_distance

    //uint16_t distance = 8*old_ultrasonic_count/147; //in
    
    //use snprintf to put distance into buf2
    snprintf(buf3, 17, "%d :%d :%d       ", hours, minutes, seconds);
    snprintf(buf2, 17, "Distance   %d    ", new_distance);
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

    /* Write string to LCD */
    lcd_flush_text(tb3);
  }
}
