#include <stdint.h>

#define TWI_ACK 0xC4   // TWCR value to send ACK after data received
#define TWI_NACK 0x84  // TWCR value to send NACK after data received
#define TWI_WRITE 0    // Write bit (0) in the address
#define TWI_READ 1     // Read bit (1) in the address

void twi_init(void);
void twi_start(void);
void twi_stop(void);
void twi_sla_w(uint8_t addr);
void twi_sla_r(uint8_t addr);
void twi_write(uint8_t data);
uint8_t twi_read(uint8_t ack);
