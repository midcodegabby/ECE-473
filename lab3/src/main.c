#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <avr/io.h>
#include <util/delay.h>

#include "lcd/lcd.h"

lcd_text_buffer_t text_buffer = {
    "hello world!", "1", "2", "3", "4", "5", "6", "Goodbye!"};

lcd_pixel_buffer_t pixel_buffer;  

int
main()
{

  /* Initialize LCD */
  lcd_init();

  /* Turn on LCD */
  lcd_on();

  /* Turn on LCD backlight to full brightness */
  lcd_led_set(0xFF);

  //lcd_fill(0x01);
  _delay_ms(1000);

  /* Flush test message to LCD RAM */
  lcd_clear();
  lcd_flush_text(text_buffer);
  _delay_ms(1000);

  /* Scroll through test message */
  for (uint8_t i = 0; i <= 0x20; ++i) {
    _delay_ms(100);
    lcd_scroll(i);
  }
  _delay_ms(1000);

  /* Reset the LCD */
  lcd_scroll(0);
  lcd_clear();

  /* Reduce LCD backlight to 25% */
  lcd_led_set(64);

  /* Erase the text buffer */
  for (uint8_t i = 0; i < 8; ++i) {
    text_buffer[i] = 0;
  }

  /* Allocate a new buffer for storing counter string */
  char buf[6] = {0};
  text_buffer[0] = buf;

  for (uint16_t count = 0;; ++count) {
    /* Convert count to a string */
    uint16_t tmp = count;
    for (uint8_t i = 0; i < 6; ++i) {
      buf[5 - i] = tmp % 10 + '0';
      tmp /= 10;
    }
    /* Write string to LCD */
    lcd_flush_text(text_buffer);
    lcd_led_set(count % 255);
    _delay_ms(100);
  }
}
