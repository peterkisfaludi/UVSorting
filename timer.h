//
// TIMER DECLARE
//
// The controller for the Arduino Mega series is the Atmel AVR ATmega1280 or the ATmega2560. 
// Also identical only differs in memory size. These controllers have 6 timers. Timer 0, 
// timer1 and timer2 are identical to the ATmega168/328. The timer3, timer4 and timer5 are 
// all 16bit timers, similar to timer1.

#ifndef TIMER_H
  #define TIMER_H

  #include <avr/io.h>
  #include <stdlib.h>
  #include <avr/interrupt.h>
  
  typedef struct timer {
    struct timer * next;
    volatile unsigned long counter;
    volatile int expired;
  }
  timer_type, * Timer;
  Timer newTimer(void);
  void deleteTimer(Timer);
  void startTimer(unsigned long, Timer);
  int timeout(Timer);
  void startTiming();
  unsigned long long timeSinceStart(); // returns 64 bit integer!
  void delayMilli(unsigned long);
  void delayMicro(unsigned int);
  
  static Timer first = NULL;
  static int initialized = 0;
  static volatile unsigned long millisecs_since_start = 0;
#endif
