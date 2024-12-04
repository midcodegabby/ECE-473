/* These are based on the bens_board schematic */
#define CS1 PB0
#define CS1_DDR DDRB
#define CS1_PORT PORTB
#define CS1_PIN PINB

#define SCL PB1
#define SCL_DDR DDRB
#define SCL_PORT PORTB
#define SCL_PIN PINB

#define SI PB2
#define SI_DDR DDRB
#define SI_PORT PORTB
#define SI_PIN PINB

#define A0 PF1
#define A0_DDR DDRF
#define A0_PORT PORTF
#define A0_PIN PINF

#define RST PF0
#define RST_DDR DDRF
#define RST_PORT PORTF
#define RST_PIN PINF

#define LED PC7
#define LED_DDR DDRC
#define LED_PORT PORTC
#define LED_PIN PINC

/* These are the ST7565R command values taken from the data sheet (p. 41) */
#define DISP_OFF 0xAE
#define DISP_ON 0xAF

#define DISP_START_LINE_SET(addr) (0x40 | ((addr) & 0x3f))

#define PAGE_ADDR_SET(addr) (0xB0 | ((addr) & 0x0f))
#define COL_ADDR_SET_UPPER(addr) (0x10 | ((addr) & 0x0f))
#define COL_ADDR_SET_LOWER(addr) (0x00 | ((addr) & 0x0f))

#define ADC_SEL_NORMAL 0xA0
#define ADC_SEL_REVERSE 0xA1

#define DISP_NORMAL 0xA6
#define DISP_REVERSE 0xA1

#define DISP_ALL_PTS_OFF 0xA4
#define DISP_ALL_PTS_ON 0xA5

#define BIAS_9 0xA2
#define BIAS_7 0xA3

#define READ_MODIFY_WRITE 0xE0
#define READ_MODIFY_WRITE_END 0xEE

#define RESET 0xE2

#define COM_OUT_NORMAL 0xC0
#define COM_OUT_REVERSE 0xC8
#define PWR_CTRL_SET(mode) (0x28 | ((mode) & 0x07))
#define VREG_RATIO(mode) (0x20 | ((mode) & 0x07))

#define VOL_MODE_SET_ENABLE 0x81
#define VOL_MODE_SET(mode) ((mode) & 0x3f)

#define SLEEP_MODE_SLEEP 0xAC
#define SLEEP_MODE_NORMAL 0xAD
#define SLEEP_MODE_APPLY 0x00

#define BOOSTER_RATIO_SET_ENABLE 0xF8
#define BOOSTER_RATIO_SET(val) ((val) & 0x03)

#define NOP 0xE3
#define TEST 0xF0
