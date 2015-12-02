//
// COLOR LIGHT SENSOR DECLARE aka. colorsensor.h
//
// Pin layout:
//  Sensor  -> Arduino controller
//   - LED  -> LEDPIN
//   - 3V3  -> 3V3
//   - GND  -> GND
//   - SCL  -> SCL (hardwired)
//   - SDA  -> SDA (hardwired)
//
#ifndef COLORSENSOR_H
  #define COLORSENSOR_H
    
  // I2C address for the device. Note: only 7-bit address, excl. LSB for read/write
  #define I2C_ADDR 0x74

  // I2C PINS
  // Classic Arduinos
  #if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega328__)|| defined(__AVR_ATmega168__) 
    #define SDA_PIN 27 // Arduino silkscreen pin A4
    #define SCL_PIN 28 // Arduino silkscreen pin A5
  // Megas
  #elif defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
    #define SDA_PIN 44 // Arduino silkscreen pin 20
    #define SCL_PIN 43 // Arduino silkscreen pin 21
  // Leonardo
  #elif defined(__AVR_ATmega32U4__)
    #define SDA_PIN 19 // Arduino silkscreen pin 2 (PWM)
    #define SCL_PIN 18 // Arduino silkscreen pin 3 (PWM)
  #endif
  
  // LED pin
  #ifndef LEDLIGHT_PIN
    #define LEDLIGHT_PIN 11
  #endif 
  
  // Register addresses
  #define REG_CAP_RED     0x06
  #define REG_CAP_GREEN   0x07
  #define REG_CAP_BLUE    0x08
  #define REG_CAP_CLEAR   0x09
  
  #define REG_INT_RED_LO  0x0A
  #define REG_INT_RED_HI  0x0B
  #define REG_INT_GREEN_LO  0x0C
  #define REG_INT_GREEN_HI  0x0D
  #define REG_INT_BLUE_LO   0x0E
  #define REG_INT_BLUE_HI   0x0F
  #define REG_INT_CLEAR_LO  0x10
  #define REG_INT_CLEAR_HI  0x11
  
  #define REG_DATA_RED_LO   0x40
  #define REG_DATA_RED_HI   0x41
  #define REG_DATA_GREEN_LO 0x42
  #define REG_DATA_GREEN_HI 0x43
  #define REG_DATA_BLUE_LO  0x44
  #define REG_DATA_BLUE_HI  0x45
  #define REG_DATA_CLEAR_LO 0x46
  #define REG_DATA_CLEAR_HI 0x47

  typedef enum {red, green, blue, orange, yellow, purple, unknown, white} ItemColor;

  void colorCalibrate (void);
  void getColors (int[]);
  ItemColor determineColor(double detectedhsv[]);
  void rgbToHSV (double, double, double, double[]);

#endif
