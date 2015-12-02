// Software 16-bit PWM using Output Compare in Normal Mode on timer0 or timer2 (8-bit)
//
// Made mainly for Arduino, but works fine as C code by substituting pinMode(), 
// digitalWrite(), delay() functions with their commented alternatives.
// Supports to two different servos, it's possible to have multiple cloned servos (not implemented) 
// Based on Alex's work http://aeroquad.com/showthread.php?322-analogWrite-50Hz-servo-OK-!
#include "softservo.h"

ISR (TIMER2_COMPA_vect) { 
  // update servo0
  servoUpdate(0);
}

ISR (TIMER2_COMPB_vect) {   
  // update servo1
  servoUpdate(1);
}

void initServo() {
  TCCR2A = 0; // normal counting mode
  // changing the prescale factor on Timer0 TCCR0B will affect functions millis(), micros(), delay()
  //TCCR0B |= 1<<CS02; // prescaling 64 - not needed, default 64
  TIMSK2 |= 1<<OCIE2A | 1<<OCIE2B; // enable Output Compare Match A and B vector
  pinMode(servoPins[0], OUTPUT); // set pin 12 to output
  pinMode(servoPins[1], OUTPUT); // set pin 13 to output
  //DDRB |= (1<<4); // set pin 12 to output
  //DDRB |= (1<<5); // set pin 13 to output
}

void setServoPos(int servo, int pos) {
  servoPos[servo] = pos; 
}

static void servoUpdate(int servo) {
  // prescaler is set by default to 64 on Timer0 for arduino specific functions
  // Duemilanove: 16000000 Hz / 64 => 4 microseconds for a counter step
  // 256 steps = 1 counter cycle = 1024 microseconds
  // algorithm loop strategy:
  //   pulse high
  //   do nothing for <servo value> or x1000 microseconds
  //   pulse down
  //   do nothing for 1000-<servo value> microseconds
  //   do nothing for 16+<servo value factor> counter cycles
  // the total cycle lasts exactly 20ms => 50Hz
  static uint8_t stateServo[2] = {0, 0};
  static uint8_t servoCount[2] = {0, 0};
  static uint8_t servoLen[2] = {0, 0};
  static uint8_t servoFac[2] = {0, 0};

  if (stateServo[servo] == 0) {
    digitalWrite(servoPins[servo], HIGH);
    //if (servo == 0) PORTB |= (1<<4); // set pin 12 high, positive pulse
    //else if (servo == 1) PORTB |= (1<<5); // set pin 13 high, positive pulse 
    
    servoFac[servo] = servoPos[servo]/1000; // will be int factor 0-3
    //servoFac[servo] = map(servoPos[servo], 0, 3000, 0, 3);
    servoLen[servo] = servoFac[servo];
    
    if (servoFac[servo] > 0) {
      // val over 1000
      ocrCount = 250; // 1000ms
      stateServo[servo] = 1;
    } else {
      // val is 0-999
      ocrCount = servoPos[servo] / 4;
      stateServo[servo] = 2;
    }
  } else if (stateServo[servo] == 1) {
    if (servoLen[servo] > 1) {
      // val is over 2000, loop through
      ocrCount = 250; // 1000ms
      servoLen[servo]--;
    } else {
      // remaining val is x000-x999
      //ocrCount = (servoPos[servo]-(servoFac[servo]*1000) +((servoFac[servo]+1)*20)) / 4; // hack to get past 1k, 2k border skip: +((servoFac[servo]+1)*20)
      // less cpu intensive
      ocrCount = (servoPos[servo]+80) / 4;
      stateServo[servo] = 2;
    }
  } else if (stateServo[servo] == 2) {
    digitalWrite(servoPins[servo], LOW);
    //if (servo == 0) PORTB &= ~(1<<4); // set the pin 12 state to low
    //else if (servo == 1) PORTB &= ~(1<<5); // set the pin 13 state to low

    // subtract to get the inverse of the remaining servo val to fill the low state, use below for exact 50Hz
    //ocrCount = (1000-(servoPos[servo]-(servoFac[servo]*1000))) / 4;     
    // less cpu intensive, try this, frequency will only drift +/-2Hz
    ocrCount = 127;
    stateServo[servo] = 3;
    
  } else if (stateServo[servo] == 3) {
    // fill the low space to get 20ms
    servoCount[servo] = 17-servoFac[servo]; // 920microseconds + (17-servofactor x 1000 microseconds) = time to keep state low
    ocrCount = 230; // fudge for perfect 50hz
    stateServo[servo] = 4;
    
  } else if (stateServo[servo] == 4) {
    // fill the low space to get 20ms
    if (servoCount[servo] > 0) servoCount[servo]--;
    else stateServo[servo] = 0;
    ocrCount = 250;
  }
  if (servo == 0) OCR2A += ocrCount;
  else if (servo == 1) OCR2B += ocrCount;
}


