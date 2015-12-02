//
// Sorting machine - feedwheel process
//
#include "colorsensor.h"
#include "softservo.h"

void feedWheel() {
  static int init = 0;
  static enum {standby, feeditem, waitformovement, waitreverse, waitforfeed, performanalysis, analyzecolor, releaseitem, dropitem} state;
  static FeedWheelState wheelaction = none;
  static int waitpreemptmove = 0;
  static int colorsamples = 0;
  static int detectedrgbcolors[4] = {0, 0, 0, 0};
  static ItemColor founditemcolors[3] = {unknown, unknown, unknown};
  static ItemColor finalcolor = unknown;
  static Timer t = newTimer();
  static int testcolor = 0;
  double detectedhsv[3];
  static int conseqcount = 0;
  
  // initialize
  if(!init) {
    //Serial.println("Initializing Feed Wheel Process");
    state = feeditem;

    init = 1;
  }

  // switch the program into the proper state
  switch (state) {
  case standby:
    Serial.println("");
    Serial.println("Feed Wheel: Entering standby mode, type S to start processing");
    while ((char)Serial.read() != 'S');
    //Serial.println("Feed Wheel: Received command to start feed wheel");
    Serial.println("");
    state = feeditem;
    break;
  case feeditem:
    // move feed wheel to release an item and load a new item
    wheelaction = startwheel;
    moveFeedWheel(wheelaction);

    // update status indicator
    statusindicator = blinking;
    //statuslight = white;
    
    // use timer to detect blockage if no movment signal has been received
    startTimer(900, t);

    Serial.println("Feed Wheel: Moving, awaiting confirmation");
    state = waitformovement;
    break;
  case waitformovement:
    // wait for feedwheelmonitor to indicate the wheel is moving
    if (outofposition) {
       // clear status
       outofposition = 0;
 
       // sensor detected change from empty hole to wheel wall
      Serial.println("Feed Wheel: Moving, waiting to be in position");
      state = waitforfeed;
    } else if (timeout(t)) {
      // try to detect blockage, reverse wheel
      wheelaction = reversewheel;
      moveFeedWheel(wheelaction);
      startTimer(300, t);
      Serial.println("Feed Wheel: Detected blockage, reversing wheel");
      state = waitreverse;
    }
    break;
  case waitreverse:
    if (timeout(t)) {
      // clear any movement signals, begin from stach
      inposition = 0;
      outofposition = 0;
      
      state = feeditem;
    }
    break;
  case waitforfeed:
    // wait for feedwheelmonitor to indicate a new empty hole and stop the wheel
    if (inposition) {
      // clear status
      inposition = 0;

      // sensor detected change from wheel wall to hole, move slow to allow sensor to perform measurements
      Serial.println("Feed Wheel: Stopping, in position to load and analyze new item");
      wheelaction = slowwheel;
      moveFeedWheel(wheelaction);
      startTimer(1, t);
      statusindicator = steady;

      state = performanalysis;
    }
    break;
  case performanalysis:
    // analyze the item, detect color three times while wheel is rotating slightly

    if (timeout(t)) {
      if (colorsamples < 3) {
        getColors(detectedrgbcolors);
        rgbToHSV((double)detectedrgbcolors[0], (double)detectedrgbcolors[1], (double)detectedrgbcolors[2], detectedhsv);
        if (detectedhsv[0] > 352) detectedhsv[0] = 0; // red discontinuity
        founditemcolors[colorsamples] = determineColor(detectedhsv);
        Serial.print("Feed Wheel: Performed analysis number #");
        Serial.print(colorsamples+1);
        Serial.print(": ");
        Serial.print(colortext[founditemcolors[colorsamples]]);
/*        Serial.print(" - (rgb) ");
        Serial.print(detectedrgbcolors[0]);
        Serial.print(" ");
        Serial.print(detectedrgbcolors[1]);
        Serial.print(" ");
        Serial.print(detectedrgbcolors[2]);
        Serial.print(" (");
        Serial.print(detectedrgbcolors[3]);
        Serial.print(") hsv: ");
*/
        Serial.print(" hsv: ");
        Serial.print(detectedhsv[0]);
        Serial.print(" ");
        Serial.print(detectedhsv[1]);
        Serial.print(" ");
        Serial.println(detectedhsv[2]);

        //statuslight = founditemcolors[colorsamples];
        startTimer(waitcolorreadings, t);
        colorsamples++;
      } else {
        // reset sample count and stop wheel
        colorsamples = 0;
        wheelaction = stopwheel;
        Serial.println("Feed Wheel: All samples collected");
        moveFeedWheel(wheelaction);
        state = analyzecolor;
      }
    }
    break;
  case analyzecolor:
    // analyze the final item color
    prevcolor = nextcolor;

    // if any two measurements are equal, then conclude that the item color is detected
    if (founditemcolors[0] == founditemcolors[1]) finalcolor = founditemcolors[0];
    else if (founditemcolors[1] == founditemcolors[2]) finalcolor = founditemcolors[1];
    else if (founditemcolors[0] == founditemcolors[2]) finalcolor = founditemcolors[2];
    else finalcolor = unknown;
    
    // for testing
    /*
    testcolor += random(0,6);
    if (testcolor > 6) testcolor = 0;
    finalcolor = (ItemColor)testcolor;
    */
    
    Serial.print("Feed Wheel: Analysis found item color: ");
    Serial.println(colortext[finalcolor]);

    statuslight = finalcolor;
    
    // update global variable to let carousel know a new item has been analyzed and ready to be released
    nextcolor = finalcolor;

    // update status indicator
    statuslight = nextcolor;

    // if three unknown is detected consequently, feed is probably empty, standby
    if (conseqcount < conseqempty) {
      // emtpy buckets register as yellow or purple/unknown, use this to an advantage, given there won't be <conseqcount> sequence of those colors
      if (finalcolor == unknown || finalcolor == yellow) {
        conseqcount++;
      } else {
        conseqcount = 0;
      }
      state = releaseitem;
    } else {
      conseqcount = 0;
      state = standby;
      //wheelaction = reversewheel;
      //moveFeedWheel(wheelaction);
      //startTimer(1000, t);
      //Serial.println("Feed Wheel: No items in feed tube, reversing wheel");
      //state = waitreverse;
    }
    break;
  case releaseitem:
    // wait for carousel to register the new color, should be instant
    if (!carouseldelivering) {
      if (carouselmoving) {
        // clear status
        carouselmoving = 0;
  
        // time to wait before carousel is just on the way to be in pos, purpose for preemptive release of item
        waitpreemptmove = getTravelTime(prevcolor, nextcolor);
        if ((waitpreemptmove -= preempttime) <= 0) waitpreemptmove = 1; // set low wait time, release immediately
  
        Serial.print("Feed Wheel: Waiting for carousel, holding back item (ms): ");
        Serial.println(waitpreemptmove);
        startTimer(waitpreemptmove, t);
        state = dropitem;
      } else if (nextcolor == prevcolor) {
        // color is the same, no need to wait for the carousel to move, release immediately
        colorduplicate = 1;
        Serial.println("Feed Wheel: Already positioned, releasing item immediately and loading new");
        Serial.println("");
        state = feeditem;
      }
    }
    break;
  case dropitem:
    // to optimize operation, start to release item from feed wheel just before carousel is arriving to pos
    if (timeout(t)) {
      // move feed wheel to release and load a new item, simultaneously
      Serial.println("Feed Wheel: Releasing item and loading new, repeating loop");
      Serial.println("");
      state = feeditem;
    }
    break;
  }
}

