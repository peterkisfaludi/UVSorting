//
// Sorting machine - sorting carousel process
//
#include "softservo.h"

void sortCarousel() {
  static int init = 0;
  static enum {waitingfornewpos, movetopos, waitingforpos, stayinginpos} state;
  static ItemColor currentcolor = unknown;
  static int waitfornewpos = 0;
  static Timer t = newTimer();

  // initialize
  if(!init) {
    //Serial.println("Initializing Carousel Process");
    nextcolor = unknown;
    state = movetopos;
    
    init = 1;
  }

  // switch the program into the proper state
  switch (state) {
  case waitingfornewpos:
    // same color as previous item, duplicate color, stay put and wait for item to roll into bin
    if (colorduplicate) {
      // clear status
      colorduplicate = 0;

      startTimer(waitinpos, t);
      carouseldelivering = 1;
      state = stayinginpos;
     }
    
    // new color detected by feed wheel, move carousel into position
    if (currentcolor != nextcolor) {
      // update status to let feedwheel know it has received the notification of a new item and started moving
      Serial.print("Carousel: Different color detected, carousel is moving to new position, bin: ");
      Serial.println(colortext[nextcolor]);
      carouselmoving = 1;
      currentcolor = nextcolor;

      state = movetopos;
    }
    break;
  case movetopos:
      // rotate the carousel to the new position determined by the new detected color
      waitfornewpos = getTravelTime(prevcolor, nextcolor);
      if (!waitfornewpos) waitfornewpos = 1;
      
      Serial.print("Carousel: Waiting for carousel to move to position, takes (ms): ");
      Serial.println(waitfornewpos);
      startTimer(waitfornewpos, t);
      moveCarousel(currentcolor);
      
      state = waitingforpos;
  case waitingforpos:
    // wait for the carousel to move into position (no external feedback, only timer)
    if (timeout(t)) {
     Serial.print("Carousel: Carousel is now in position, bin: ");
     Serial.println(colortext[currentcolor]);
     
     startTimer(waitinpos, t);
     carouseldelivering = 1;
     state = stayinginpos;
    }
    break;
  case stayinginpos:
    // when in position, stay put to finish the delivery (rolling) of the item to the bin 
    if (timeout(t)) {
      // clear status
      carouseldelivering = 0;
      state = waitingfornewpos;
    }
    break;
  }
}


