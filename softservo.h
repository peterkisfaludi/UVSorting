//
// Software 16-bit PWM using Output Compare in Normal Mode on timer0 or timer2 (8-bit)
//

#ifndef SOFTSERVO_H
  #define SOFTSERVO_H
  
  #if ARDUINO >= 100
   #include "Arduino.h"
  #else
   #include "WProgram.h"
  #endif
    
  #if !defined(SERVO0_PIN)
   #define SERVO0_PIN 12
  #endif
  
  #if !defined(SERVO1_PIN)
   #define SERVO1_PIN 13
  #endif

  void initServo();
  void setServoPos(int, int);
  static void servoUpdate(int);
  
  // globals for servoUpdate() and interrupt vector
  static uint8_t ocrCount = 0;
  static int servoPos[2] = {1500, 1500};
  static const uint8_t servoPins[2] = {SERVO0_PIN, SERVO1_PIN};
#endif
