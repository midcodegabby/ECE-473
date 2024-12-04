#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>

#include <avr/io.h>
#include <util/delay.h>

#include "lcd_config.h"
#include "lcd.h"

#define ARRAY_LEN(x) (sizeof(x) / sizeof *(x))

//note: _BV(X) = (1 << X)
/* Initialize SPI interface
 *
 * Read the Atmel spec sheet and LCD driver spec sheet
 */
static void spi_init(void) {
	//set output pins to required values:
	DDRB |= (1 << CS1)|(1 <<SCL)|(1<<SI); //set PB2:0 to output -> MOSI, SCK, SS
	DDRC |= (1 << LED); //set PC7 to output -> PWM from TC4 output
	DDRF |= (1 << RST)|(1 << A0); //set PF1:0 to output -> LCD_A0, RST
	
	//initialize SPCR (spi control reg): SPI EN, SPI master, and SCK = Fosc/16
	SPCR |= (1 << SPE)|(1<<MSTR)|(1 << SPR0); 
	
	//activate chip select; SPI is active
	PORTB &= ~(1 << CS1);
	
	//reset lcd -> PF0 = 0 to reset, then keep at 1
	PORTF &= ~(1 << RST); //may need to keep this low for longer
	_delay_us(3);
	PORTF |= (1 << RST); 
	_delay_us(3);
}

/* Initialize the LED backlight.
 *
 * Look at Timer/Counter4 and the schematic */
static void led_init(void) {
	//OCW4A clear on comp match, set when TCNT = 0x00, FAST PWM
	TCCR4A |= (1 << COM4A0)|(1 << PWM4A);
	TCCR4B |= (1 << CS42)|(1 << CS41)|(1 << CS40); //Clock prescaler 1/64

	lcd_led_set(0x00); //set duty cycle to 0: led off.
}

/* Write to the LCD via spi */
static void spi_write(uint8_t c) {
	//only do CS1 stuff if there are multiple devices.
	//turn on SPI chip select
	PORTB &= ~(1 << CS1);

	//put data into the SPDR
	SPDR = c; 

	while (!(SPSR & (1 << SPIF)));  //wait for transfer to finish

	//turn off SPI chip select
	PORTB |= (1 << CS1);
	
}

/* Interpret SPI bytes as commands */
static void lcd_cmd(void) {
      	A0_PORT &= ~_BV(A0);
}

/* Interpret SPI bytes as image data */
static void lcd_data(void) {
	A0_PORT |= _BV(A0);
}

/* Initializes the LCD and Backlight,
 * but does not turn it on yet
 *
 * Review the LCD spec sheet for correct
 * initialization routine.
 */
void lcd_init(void) {
	spi_init();
	led_init();
	lcd_reset();

	//send commands to the LCD
	lcd_cmd();
	spi_write(PWR_CTRL_SET(0x7)); //all voltage stuff on
	spi_write(VREG_RATIO(0x1)); //mode = 0
}

/* Turn the display on */
void lcd_on() {

	lcd_cmd(); //make spi byte a command
	spi_write(DISP_ON); //turn on LCD
}

/* Turn the display off */
void lcd_off() {
	lcd_cmd(); //make spi byte a command
	spi_write(DISP_OFF); //turn off LCD
}

/* Set the backlight (LED) PWM duty cycle
 *
 * Review schematics... :) 
 */
void lcd_led_set(uint8_t level) {
	//set OCR4A to level
	OCR4A = level;
}

/* Sets LCD volume (contrast) */
void lcd_volume_set(uint8_t level) {
	lcd_cmd();
	spi_write(VOL_MODE_SET_ENABLE); //mode = 0
	spi_write(VOL_MODE_SET(level)); 
}

/* Resets LCD; different from blanking/clearing */
void lcd_reset(void) {
	lcd_cmd(); //make spi byte a command
	spi_write(RESET);
}

void lcd_sleep(void) {
	lcd_cmd();
	spi_write(SLEEP_MODE_SLEEP);
	spi_write(SLEEP_MODE_APPLY);
}

void lcd_wake(void) {
	lcd_cmd();
	spi_write(SLEEP_MODE_NORMAL);
	spi_write(SLEEP_MODE_APPLY);
}

/* Scrolls the LCD by setting the display start line */
void lcd_scroll(uint8_t y) {
	lcd_cmd();
	spi_write(DISP_START_LINE_SET(y));
}

/* Fills the LCD with the specified byte value */
void lcd_fill(uint8_t c) {
  for (size_t y = 0; y < LCD_PAGE_COUNT; y++) {
    lcd_cmd();
    spi_write(PAGE_ADDR_SET(y));
    spi_write(COL_ADDR_SET_UPPER(0));
    spi_write(COL_ADDR_SET_LOWER(0));

    lcd_data();
    for (size_t x = 0; x < LCD_COLUMN_COUNT; x++) {
      spi_write(c);
    }
  }
}

/* Clears the LCD */
void lcd_clear(void) {
  lcd_fill(0x00);
}

/* Dumps a text buffer to the LCD
 *
 * Text buffer = array of 8 strings
 *
 * Converts each character to a glyph for
 * printing
 */
void lcd_flush_text(lcd_text_buffer_t const buf) {
  char c;
  //set line 
  for (size_t y = 0; y < LCD_PAGE_COUNT; y++) {
	  lcd_cmd();
	  spi_write(PAGE_ADDR_SET(y));
	  spi_write(COL_ADDR_SET_UPPER(0));
	  spi_write(COL_ADDR_SET_LOWER(0));
	  
	  //send data for each page
	  lcd_data();

	  //convert buf to chars for each page/string in the buffer; stop at null terminator
	  for (uint8_t x = 0; buf[y][x] != '\0'; x++) {
	  	c = buf[y][x];
	  
		  glyph_t const *g = ascii_to_glyph(c);
		  for (uint8_t col = 0; col < 8; col++) {
		    if (col < GLYPH_WIDTH) spi_write(g->cols[col]);
		    else spi_write(0);
		  }
  	  }
  }
}

/* Dumps a pixel buffer to the LCD
 *
 * Pixel buffer = raw pixel data
 */
void lcd_flush_pixels(lcd_pixel_buffer_t const buf) {

  for (size_t y = 0; y < LCD_PAGE_COUNT; ++y) {
    lcd_cmd();
    spi_write(PAGE_ADDR_SET(y));
    spi_write(COL_ADDR_SET_UPPER(0));
    spi_write(COL_ADDR_SET_LOWER(0));

    lcd_data();
    for (size_t x = 0; x < LCD_COLUMN_COUNT; ++x) {
      uint8_t c = buf[y][x];
      spi_write(c);
    }
  }
}
