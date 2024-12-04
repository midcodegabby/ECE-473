#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <avr/io.h>
#include <util/delay.h>
#include <math.h>

#include "lcd/lcd.h"
#include "twi/twi_config.h"
#include "twi/twi.h"

int8_t read_temp(uint8_t address){
	
      int8_t temp[] = {0,0};

      //twi_start();
      //twi_sla_w(address); //tell external sensor we want to write 
      //twi_write(0x00); //set pointer to temp register
      
      twi_start();
      twi_sla_r(address); //tell sensor we want to read data
      temp[1] = twi_read(TWI_ACK); 
      temp[0] = twi_read(TWI_NACK);
      twi_stop();
	
      return temp[1];
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

  /* Allocate a new buffer for storing temperature string */
  //char buf[] = "      0 deg C";
  //lcd_text_buffer_t tb2 = {buf};

  char buf[] = "     0C       0C";
  lcd_text_buffer_t tb3 = {buf,"", " Indoor  Outdoor"};

  int8_t in_temp = 0;
  int8_t new_in_temp = 0;
  int8_t out_temp = 0;
  int8_t new_out_temp = 0;

  twi_init();

  for (;;) {
    for (;;) {
      /* Use TWI to get a temperature reading from the on-board temperature sensor 
       * Refer to spec sheet and bens_board schematic for address info
       */
      
      //this section is for external temp sensor
      new_in_temp = read_temp(T_ADDR);
      new_out_temp = read_temp(L_ADDR);

      //check if there is a diff temp reading
      if ((new_in_temp != in_temp) || (new_out_temp != out_temp)) {
	in_temp = new_in_temp; //update inside temp
	out_temp = new_out_temp; //update outside temp
	break;
      }

      else {
        //Don't refresh output if nothing changed 
        _delay_ms(10);
	continue;
      }

    }
   
    
    //convert both temperature ints to strings
    uint8_t in_tmp = in_temp > 0 ? in_temp : -in_temp;
    uint8_t out_tmp = out_temp > 0 ? out_temp : -out_temp;
    
    
    for (int i = 0; i < 14; ++i) {
      if (out_tmp == 0) {
        buf[14 - i] = ' ';
      } 
      else {
        buf[14 - i] = (out_tmp % 10) + '0';
        out_tmp /= 10;
        if (out_tmp == 0 && out_temp < 0) {
          ++i;
          buf[14 - i] = '-';
        }
      }
    }
    
    for (int i = 0; i < 5; ++i) {
      if (in_tmp == 0) {
        buf[5 - i] = ' ';
      } 
      else {
        buf[5 - i] = (in_tmp % 10) + '0';
        in_tmp /= 10;
        if (in_tmp == 0 && in_temp < 0) {
          ++i;
          buf[5 - i] = '-';
        }
      }
    }

    buf[6] = 'C';
    
    
   
    /*
    //combined version?
    for (int i = 0; i < 14; ++i) {
      if (out_tmp == 0) {
        buf[14 - i] = ' ';
      } 
      else {
        buf[14 - i] = (out_tmp % 10) + '0';
        out_tmp /= 10;
        if (out_tmp == 0 && out_temp < 0) {
          ++i;
          buf[14 - i] = '-';
        }
      }
    }
    */


    /*
    //inside temp testing
    uint8_t tmp = in_temp > 0 ? in_temp : -in_temp;
    for (int i = 0; i < 6; ++i) {
      if (tmp == 0) {
        buf[6 - i] = ' ';
      } 
      else {
        buf[6 - i] = (tmp % 10) + '0';
        tmp /= 10;
        if (tmp == 0 && in_temp < 0) {
          ++i;
          buf[6 - i] = '-';
        }
      }
    }
    */

    /*
    //outside temp testing
    uint8_t tmp = out_temp > 0 ? out_temp : -out_temp;
    for (int i = 0; i < 6; ++i) {
      if (tmp == 0) {
        buf[6 - i] = ' ';
      } 
      else {
        buf[6 - i] = (tmp % 10) + '0';
        tmp /= 10;
        if (tmp == 0 && out_temp < 0) {
          ++i;
          buf[6 - i] = '-';
        }
      }
    }
    */

    /* Write string to LCD */
    lcd_flush_text(tb3);
  }
}
