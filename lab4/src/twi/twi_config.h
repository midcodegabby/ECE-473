#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

/* Taken from ATmega32u4 Datasheet */
#define SCL_DDR DDRD
#define SCL_PORT PORTD
#define SCL_PIN PIND
#define SCL PD0

#define SDA_DDR DDRD
#define SDA_PORT PORTD
#define SDA_PIN PIND
#define SDA PD1

#define F_SCL 100000
#define T_ADDR (0b1001 << 4) 
#define L_ADDR ((0b1001 << 4) | (1 << 1))

#define TWI_START 0xA4 // TWCR value to send START condition
#define TWI_STOP 0x94  // TWCR value to send STOP condition
