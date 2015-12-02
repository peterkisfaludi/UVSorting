//
// Sorting machine - functions
//
#include "functions.h"

//do an echo with the ultrasound sensor
int ping(){
  pinMode(DISTANCESENSOR_PIN,OUTPUT);
  pinMode(DISTANCESENSOR_PIN, OUTPUT); // Switch signalpin to output
  digitalWrite(DISTANCESENSOR_PIN, LOW); // Send low pulse 
  delayMicroseconds(2); // Wait for 2 microseconds
  digitalWrite(DISTANCESENSOR_PIN, HIGH); // Send high pulse
  delayMicroseconds(5); // Wait for 5 microseconds
  digitalWrite(DISTANCESENSOR_PIN, LOW); // Holdoff
  pinMode(DISTANCESENSOR_PIN, INPUT); // Switch signalpin to input
  digitalWrite(DISTANCESENSOR_PIN, HIGH); // Turn on pullup resistor
  // please note that pulseIn has a 1sec timeout, which may
  // not be desirable. Depending on your sensor specs, you
  // can likely bound the time like this -- marcmerlin
  // echo = pulseIn(ultraSoundSignal, HIGH, 38000)
  int echo = pulseIn(DISTANCESENSOR_PIN, HIGH); //Listen for echo
  int ultrasoundValue = (echo / 58.138); //convert to CM
  return ultrasoundValue;
}

int readDistanceSensor()
{
  // read the sensor
  int sensorreading = ping();
  return sensorreading;
}

int getTravelTime(ItemColor prevcolor, ItemColor nextcolor) {
  // carousel drop area is a circle starting at one end with: red, green, blue, orange, yellow, purple/unknown
  int time = 0;
  int traveldiff = 0;

  // how far does the carousel need to move
  traveldiff = abs(prevcolor-nextcolor);
  
  // servo specs says 1.56s for a full 360degree rotation, given 6 positions, makes 1.56s/6=260ms to move past one position
  time = traveldiff*travelpercup;
  
  return time;
}

void moveFeedWheel(FeedWheelState action)
{
  // feed wheel positions/speeds: stop, start, slow, reverse
  int wheelpos_value[4] = {1440, 1000, 1350, 1550};

  if (action == stopwheel) {
    setFeedWheelPos(wheelpos_value[0]);
  } else if (action == startwheel) {
    setFeedWheelPos(wheelpos_value[1]);
  } else if (action == slowwheel) {
    setFeedWheelPos(wheelpos_value[2]);
  } else if (action == reversewheel) {
    setFeedWheelPos(wheelpos_value[3]);    
  }
}

void moveCarousel(ItemColor color) {
  // drop positions for items: red, green, blue, orange, yellow, purple/unknown
  //hs322 - int itempos_value[6] = {700, 1650, 1000, 1250, 1650, 1950};
  int itempos_value[6] = {1985, 1795, 1605, 1385, 1215, 1035};

  //setCarouselPos(itempos_value[nextcolor]);

  if (color == red) {
    setCarouselPos(itempos_value[0]);
  } else if (color == green) {
    setCarouselPos(itempos_value[1]);
  } else if (color == blue) {
    setCarouselPos(itempos_value[2]);
  } else if (color == orange) {
    setCarouselPos(itempos_value[3]);
  } else if (color == yellow) {
    setCarouselPos(itempos_value[4]);
  } else {
    setCarouselPos(itempos_value[5]);
  }
}

void setFeedWheelPos(int val)
{
    // Set Pulse width to value
    setServoPos(0, val);
}

void setCarouselPos(int val)
{
    // Set Pulse width to value
    setServoPos(1, val);
}

void displayItemColor(ItemColor itemcolor)
{
  uint8_t rgbvalue[3] = {0, 0, 0};

  // set the analog adc values to illuminate the RGB LED: red, green, blue, orange, yellow, purple/unknown, off
  if (itemcolor == red) {
    rgbvalue[0] = 255;
    rgbvalue[1] = 0;
    rgbvalue[2] = 0;
  } else if (itemcolor == green) {
    rgbvalue[0] = 0;
    rgbvalue[1] = 255;
    rgbvalue[2] = 0;
  } else if (itemcolor == blue) {
    rgbvalue[0] = 0;
    rgbvalue[1] = 0;
    rgbvalue[2] = 255;
  } else if (itemcolor == orange) {
    rgbvalue[0] = 255;
    rgbvalue[1] = 75;
    rgbvalue[2] = 0;
  } else if (itemcolor == yellow) {
    rgbvalue[0] = 255;
    rgbvalue[1] = 200;
    rgbvalue[2] = 200;
  } else if (itemcolor == white) {
    rgbvalue[0] = 255;
    rgbvalue[1] = 255;
    rgbvalue[2] = 255;
  } else if (itemcolor == purple) {
    rgbvalue[0] = 150;
    rgbvalue[1] = 0;
    rgbvalue[2] = 255;
  } else {
    rgbvalue[0] = 0;
    rgbvalue[1] = 0;
    rgbvalue[2] = 0;
  }
  
   //analogWrite(RED_PIN, rgbvalue[0]);
   //analogWrite(GREEN_PIN, rgbvalue[1]);
   //analogWrite(BLUE_PIN, rgbvalue[2]);
}

