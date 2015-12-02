//
// Sorting machine - functions
//

#ifndef SORTINGFUNC_H
  #define SORTINGFUNC_H

  #include "timer.h"
  #include "colorsensor.h"

  // data types
  typedef enum {none, startwheel, stopwheel, slowwheel, reversewheel} FeedWheelState;
  typedef enum {waiting, steady, blinking} StatusState;
  char* colortext[] = {"red", "green", "blue", "orange", "yellow", "purple"};
  
  // global system variables
  // feed wheel process -> carousel process
  ItemColor nextcolor = unknown;
  ItemColor prevcolor = unknown;
  int colorduplicate = 0;
  // carousel process -> feed wheel process
  int carouselmoving = 0;
  int carouseldelivering = 0;
  // feed wheel monitor -> feed wheel process  
  int outofposition = 0;
  int inposition = 0;
  // feed wheel -> status process
  StatusState statusindicator = waiting;
  ItemColor statuslight = unknown; // same as off

  // servo pins
  #if !defined(SERVO0_PIN)
   #define SERVO0_PIN FEEDHWHEEL_PIN
  #endif
  
  #if !defined(SERVO1_PIN)
   #define SERVO1_PIN CAROUSEL_PIN
  #endif
  
  int readDistanceSensor();
  int getTravelTime(ItemColor prevcolor, ItemColor);
  void moveFeedWheel(FeedWheelState);
  void moveCarousel(ItemColor);
  void setCarouselPos (int);
  void setFeedWheelPos (int);
  void displayItemColor(ItemColor);
#endif
