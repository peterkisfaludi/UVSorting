// 
// I2C/TWI implementation for AVR microcontrollers.
//

#ifndef I2C_H
  #define I2C_H
  
  #define MAX_TRIES 5
  #define I2C_START 0
  #define I2C_DATA  1
  #define I2C_STOP  2

  void i2c_init (void);
  unsigned char i2c_transmit(unsigned char);
  int i2c_writebyte(unsigned int, unsigned int, int);
  int i2c_readbyte(unsigned int, unsigned int, unsigned int*);

#endif
