//
// Sorting machine - feedwheel monitor process
//

void feedWheelMonitor() {
  static int init = 0;
  static enum {waiting, readsensor} state;
  static int distancevalue = 0;
  static int distancereading = 0;
  static int distancebefore = 0;
  static Timer t = newTimer();

  // initialize
  if(!init) {
    //Serial.println("Initializing Feed Wheel Monitor Process");
    state = readsensor;
    init = 1;
  }

  // switch the program into the proper state
  switch (state) {
  case waiting:
    if (timeout(t)) {
      state = readsensor;
    }
    break;
  case readsensor:
    distancevalue = readDistanceSensor();
    // use map to create regions of valid zones, filter noise
    // in position reads: 300 (~8mm distance between sensor and item hole end)
    // out of position reads: 900 (~2mm)
    distancereading = map(distancevalue, fardistance, fardistance, 0, 1);

    if (distancereading < distancebefore) {
      Serial.print("Monitor: In position/item located, value: ");
      Serial.print(distancereading);
      Serial.print(", ");
      Serial.println(distancevalue);
      inposition = 1;
      outofposition = 0;
      distancebefore = distancereading;
      
    } else if (distancereading > distancebefore) {
      Serial.print("Monitor: Out of position, value: ");
      Serial.print(distancereading);
      Serial.print(", ");
      Serial.println(distancevalue);
      inposition = 0;
      outofposition = 1;
      distancebefore = distancereading;
    } else {
      // inbetween waiting for something to happen, wheel spinning
    }
 
    startTimer(waitdistancesensor, t);
    state = waiting;
    break;
  }
}


