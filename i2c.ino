// 
// I2C/TWI implementation for AVR microcontrollers.
//

#include <avr/io.h>
#include <util/twi.h>

#include "i2c.h"

void i2c_init (void)
{    
    /* Initial TWI Peripheral */
    TWSR = 0x00; // Select Prescaler of 1
    
    // SCL frequency = 16000000 / (16 + 2 * TWBR * 1) = 100 khz
    TWBR = 0x48; // 72 Decimal

	// Enable twi module
	TWCR = (1 << TWEN);
}

unsigned char i2c_transmit(unsigned char type)
{
    switch(type) {
        case I2C_START:    // Send Start Condition
            TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
            break;
        case I2C_DATA:     // Send Data
            TWCR = (1 << TWINT) | (1 << TWEN);
            break;
        case I2C_STOP:     // Send Stop Condition
            TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
            return 0;
    }
    
    // Wait for TWINT flag set in TWCR Register
    while (!(TWCR & (1 << TWINT)));

    // Return TWI Status Register, mask the prescaler bits (TWPS1,TWPS0)
    return (TWSR & 0xF8);
}

int i2c_writebyte(unsigned int i2c_addr, unsigned int reg_addr, int data)
{
    unsigned char n = 0;
    unsigned char twi_status;
    char r_val = -1;
        
i2c_retry:
    if (n++ >= MAX_TRIES) return r_val;
    
    // Transmit Start Condition
    twi_status=i2c_transmit(I2C_START);
    
    // Check the TWI Status
    if (twi_status == TW_MT_ARB_LOST) goto i2c_retry;
    if (!((twi_status == TW_START) || (twi_status == TW_REP_START))) goto i2c_quit;

    
    // Send slave address (SLA_W)
    TWDR = (i2c_addr << 1) | TW_WRITE;
    
    // Transmit I2C Data
    twi_status=i2c_transmit(I2C_DATA);

    // Check the TWSR status
    if ((twi_status == TW_MT_SLA_NACK) || (twi_status == TW_MT_ARB_LOST)) goto i2c_retry;
    if (twi_status != TW_MT_SLA_ACK) goto i2c_quit;
    

    // Send the register address of interest to the device
    TWDR = reg_addr;
    
    // Transmit I2C Data
    twi_status=i2c_transmit(I2C_DATA);
    
    // Check the TWSR status
    if (twi_status != TW_MT_DATA_ACK) goto i2c_quit;
    

    // Put data into data register and start transmission
    TWDR = data;
    
    // Transmit I2C Data
    twi_status=i2c_transmit(I2C_DATA);
    
    // Check the TWSR status
    if (twi_status != TW_MT_DATA_ACK) goto i2c_quit;

    
    // TWI Transmit Ok
    r_val=1;
    
i2c_quit:
    // Transmit I2C Data
    twi_status=i2c_transmit(I2C_STOP);
    return r_val;
}

int i2c_readbyte(unsigned int i2c_addr, unsigned int reg_addr, unsigned int *data)
{
    unsigned char n = 0;
    unsigned char twi_status;
    char r_val = -1;
        
i2c_retry:
    if (n++ >= MAX_TRIES) return r_val;
    
    // Transmit Start Condition
    twi_status=i2c_transmit(I2C_START);
    
    // Check the TWI Status
    if (twi_status == TW_MT_ARB_LOST) goto i2c_retry;
    if (!((twi_status == TW_START) || (twi_status == TW_REP_START))) goto i2c_quit;

    
    // Send slave address (SLA_W)
    TWDR = (i2c_addr << 1) | TW_WRITE;
    
    // Transmit I2C Data
    twi_status=i2c_transmit(I2C_DATA);
    
    // Check the TWSR status, wait for ACK
    if ((twi_status == TW_MT_SLA_NACK) || (twi_status == TW_MT_ARB_LOST)) goto i2c_retry;
    if (twi_status != TW_MT_SLA_ACK) goto i2c_quit;
    
    
    // Send the register address of interest to the device
    TWDR = reg_addr;
    
    // Transmit I2C Data
    twi_status=i2c_transmit(I2C_DATA);
    
    // Check the TWSR status, wait for ACK
    if (twi_status != TW_MT_DATA_ACK) goto i2c_quit;
    
    
    // Send new start Condition
    twi_status=i2c_transmit(I2C_START);
    
    // Check the TWSR status
    if (twi_status == TW_MT_ARB_LOST) goto i2c_retry;
    if ((twi_status != TW_START) && (twi_status != TW_REP_START)) goto i2c_quit;
    
    
    // Send slave address
    TWDR = (i2c_addr << 1) | TW_READ;
    
    // Transmit I2C Data
    twi_status=i2c_transmit(I2C_DATA);  
    
    // Check the TWSR status, wait for ACK
    if ((twi_status == TW_MR_SLA_NACK) || (twi_status == TW_MR_ARB_LOST)) goto i2c_retry;
    if (twi_status != TW_MR_SLA_ACK) goto i2c_quit;
    
    
    // Read device data
    twi_status=i2c_transmit(I2C_DATA);
    if (twi_status != TW_MR_DATA_NACK) goto i2c_quit;
    
    // Get the Data
    *data=TWDR;
    r_val=1;
    
i2c_quit:
    // Send Stop Condition
    twi_status=i2c_transmit(I2C_STOP);
    return r_val;
    
}
