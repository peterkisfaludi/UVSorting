//
// UV Sorting machine v1.0
// A modulized system for sorting UV painted objects into 3 different bins based its color
//
#include "functions.h"

// settings
// pins for the servos, any digital will work
#define FEEDHWHEEL_PIN 12
#define CAROUSEL_PIN 13

// parallax distance sensor
#define DISTANCESENSOR_PIN 5
// UV pin
#define UV_PIN 6

// time values
// distance check: how long to wait between distance sensor checks
const int waitdistancesensor = 10;
// color sampling: time to wait between color sensor readings (3x readings performed)
const int waitcolorreadings = 30;
// color determining: radius of the confidence interval, 2 stddev 95%, 3 stddev 99%
const int colordeterminestddev = 2;
// release: travel time needed to travel past one cup, e.g. 1560ms/6 = 260ms travel time per cup 
const int travelpercup = 230; // s125 1t 1560/6=260ms - hs322 75ms
// release: try to preempt an item release before carousel is in position to gain performance
const int preempttime = 1*travelpercup;
// delivery: how long to stay put waiting for the item to roll into the bin
const int waitinpos = 400; // 750 quiet

// standby: how many consequent/unkown analysis should run by before stopping the wheel, 4 is a full rotation
const int conseqempty = 15;
// distance sensor trigger values for feed wheel positioning, betweeen 50-750
const int fardistance = 75;

void setup() {
  // initialize serial communication:
  Serial.begin(115200);  
  Serial.println("Sorting Machine v1.0 - Welcome!");
  Serial.println("");

  // setup counter registers for servos
  initServo();

}

void loop() {
  // main loop of processes
  feedWheel();
  sortCarousel();
  feedWheelMonitor();
  statusIndicator();
}

