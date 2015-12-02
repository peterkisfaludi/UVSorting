//
// TIMER START
//
#include "timer.h"

SIGNAL(TIMER1_COMPA_vect)
{
  Timer t;

  OCR1A += 16000;
  millisecs_since_start++;
  t = first;
  while (t != NULL) {
    if (t->counter != 0) {
      t->counter--;
      if (t->counter == 0) t->expired = 1;
    }
    t = t->next;
  }
}

static void init_timer(void) {
  if (initialized) return;
  millisecs_since_start = 0;
  //initialiser timer 1
  cli();
  TCCR1A = 0; // normal mode
  OCR1A = 16000;
  TCCR1B = (1 << CS10); // prescaler 1
  TIMSK1 = (1 << OCIE1A);
  TIFR1 = (1 << OCF1A);
  TCNT1 = 0;
  initialized = 1;
  sei();
}

void startTiming() {
  if (initialized) return;
  init_timer();
}

static volatile unsigned long end_time, curr_time;
static volatile unsigned int time_to_next_tick;

void delayMilli(unsigned long milliseconds) {
  unsigned char oldSreg = SREG;
  cli();
  time_to_next_tick = (OCR1A - TCNT1) / 16;
  curr_time = millisecs_since_start;
  SREG = oldSreg;
  end_time = millisecs_since_start + milliseconds;
  do {
    oldSreg = SREG;
    cli();
    curr_time = millisecs_since_start;
    SREG = oldSreg;
  }
  while (curr_time < end_time);
  if (time_to_next_tick > 985) time_to_next_tick = 985;
  delayMicro(985 - time_to_next_tick);
}

static volatile int count;

void delayMicro(unsigned int microseconds) {
  if (microseconds < 2) return;
  if (microseconds > 4096) microseconds = 4098;
  if (microseconds == 2) {
    count = 0;
    count = 5;
    return;
  } else if (microseconds == 3) {
    count = 0;
    count++;
    count = 0;
    count = 5;
    return;
  } else if (microseconds == 4) count = (microseconds - 2) * 16 - 11;
  else if (microseconds > 4) count = (microseconds - 3) * 16 - 14;
  unsigned char oldSreg = SREG;
  cli();
  OCR1B = TCNT1 + count;
  TIFR1 = (1 << OCF1B);
  SREG = oldSreg;
  while (!(TIFR1 & (1 << OCF1B)));
}

unsigned long long timeSinceStart() {
  static unsigned int addition;
  static unsigned long millis;
  unsigned char oldSreg = SREG;
  cli();
  addition = TCNT1 - (OCR1A - 16000);
  millis = millisecs_since_start;
  SREG = oldSreg;
  return (unsigned long long)(millis * 1000 + addition / 16);

}

Timer newTimer(void) {
  Timer p, t_p;

  if (!initialized) init_timer();
  // allocate
  t_p = (Timer) malloc(sizeof(timer_type));
  if (t_p == NULL) return NULL;
  // initialize node
  t_p->next = NULL;
  t_p->counter = 0;
  t_p->expired = 0;
  cli();
  // append new node to link
  if (first == NULL) // first node
  first = t_p;
  else {
    p = first;
    while (p->next != NULL)
    p = p->next;
    p->next = t_p;
  }
  sei();
  return t_p;
}

void deleteTimer(Timer t) {
  Timer p;
  if (t == NULL) return;
  unsigned char oldSreg = SREG;
  cli();
  if (t == first) first = t->next;
  else {
    //find previous
    p = first;
    while (p->next != t)
    p = p->next;
    // link previous to next
    p->next = t->next;
  }
  free(t);
  SREG = oldSreg;
}

void startTimer(unsigned long millisecs, Timer t) {
  t->counter = millisecs;
  t->expired = 0;
}

int timeout(Timer t) {
  int to;
  unsigned char oldSreg = SREG;
  cli();
  to = t->expired;
  if (to) t->expired = 0;
  SREG = oldSreg;
  return to;
}

