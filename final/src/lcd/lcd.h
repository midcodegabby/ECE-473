#include "glyph.h"

#ifndef LCD_PAGE_COUNT
#define LCD_PAGE_COUNT 8
#endif

#ifndef LCD_COLUNN_COUNT
#define LCD_COLUMN_COUNT 128
#endif

typedef uint8_t lcd_pixel_buffer_t[LCD_PAGE_COUNT][LCD_COLUMN_COUNT];
typedef char *lcd_text_buffer_t[LCD_PAGE_COUNT];

extern void lcd_init(void);
extern void lcd_on(void);
extern void lcd_off(void);

extern void lcd_led_set(uint8_t level);
extern void lcd_volume_set(uint8_t level);
extern void lcd_reset(void);

extern void lcd_sleep(void);
extern void lcd_wake(void);

extern void lcd_scroll(uint8_t y);

extern void lcd_flush_text(lcd_text_buffer_t const buf);
extern void lcd_flush_pixels(lcd_pixel_buffer_t const buf);


extern void lcd_fill(uint8_t c);
extern void lcd_clear(void);
