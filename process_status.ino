//
// Sorting machine - status led process
//

void statusIndicator() {
  static int init = 0;
  //static enum {waiting, steady, blinking} state;
  static StatusState state;
  static Timer t = newTimer();
  static int blinkingstate = 0;
  static int waitblinkoff = 50; // 150
  static int waitblinkon = 25; // 75
  static ItemColor statuslightbefore = unknown;

  // initialize
  if(!init) {
    //Serial.println("Initializing Status Indicator Process");
    state = steady;

    init = 1;
  }

  // switch the program into the proper state
  switch (state) {
  case waiting:
    // wait for blinking timer to expire
    if (timeout(t)) {
      state = blinking;
    }
    break;
  case steady:
    // keep the status light illumiated
    if (statusindicator != steady) {
      state = statusindicator;
    } else if (statuslight != statuslightbefore) {
      displayItemColor(statuslight);
      statuslightbefore = statuslight;
    }
    break;
  case blinking:
    // switch the status light on and off in a defined period
    if (statusindicator != blinking) {
      state = statusindicator;
    } else {
      if (!blinkingstate) { // off
        displayItemColor(unknown);
        startTimer(waitblinkoff, t);
        blinkingstate = 1;
      } else { // on
        displayItemColor(statuslight);
        startTimer(waitblinkon, t);
        blinkingstate = 0;
      }
      state = waiting;
    }
    break;
  }
}

