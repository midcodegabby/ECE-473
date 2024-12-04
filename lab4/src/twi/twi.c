#include <avr/io.h>

#include "twi_config.h"

#include "twi.h"

#include <avr/io.h>
#include <util/delay.h>

/* Initialize TWI for 100 kHz operation
 *
 * Note: Don't use the TWI prescaler; it over-complicates things. Just use the
 * TWI bitrate register for setting clock rate
 */
void
twi_init(void)
{
	//set up clock for I2C (100KHz)
	TWBR = 72;
}

/* Send TWI START condition */
void
twi_start(void)
{
	TWCR = (1 << TWINT)|(1 << TWSTA)|(1 << TWEN);
	while (!(TWCR & (1 << TWINT))); //wait for start condition
}

/* Send TWI STOP condition */
void
twi_stop(void)
{
	TWCR = (1 << TWINT)|(1 << TWEN)|(1 << TWSTO);
}

/* Write data to TWI: this is a write read */
void
twi_write(uint8_t data)
{
	TWDR = data; //put data into data reg
	TWCR = (1 << TWINT)|(1 << TWEN); //start write transaction
	while (!(TWCR & (1 << TWINT))); //wait for ACK/NACK reception
}

/* Read data from TWI with ACK or NACK
 *
 * e.g. twi_read(TWI_ACK)
 *
 * Read the temperature sensor spec sheet for an explanation of why would use
 * ACK or NACK when reading! 
 */
uint8_t
twi_read(uint8_t ack)
{
	TWCR = ack; //start read transaction
	while (!(TWCR & (1 << TWINT))); //wait for data to be put into the data reg
	return TWDR;
}

/* Slave Address + Write (SLA+W) 
 *
 * Input: 7-bit slave address
 *
 * Initiates writing to a slave device at address addr
 */
void
twi_sla_w(uint8_t addr)
{
  // Note: one-liner function in terms of twi_write(...)
  twi_write(addr); //W = 0
}

/* Slave Address + Read (SLA+W) 
 *
 * Input: 7-bit slave address
 *
 * Initiates reading from a slave device at address addr
 */
void
twi_sla_r(uint8_t addr)
{
  // Note: one-liner function in terms of twi_write(...)
  twi_write((addr | 1)); //R = 1
}
