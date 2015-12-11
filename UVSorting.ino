//
// UV Sorting machine v1.0
// A modulized system for sorting UV painted objects into 3 different bins based its color
//

#include <Wire.h>
#include "Adafruit_TCS34725.h"

//color sensor
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);
uint16_t clear, red, green, blue;
float hue, saturation, value;

//servos
#include <Servo.h>
// servo1 = gate
#define GATESERVO_PIN 12
Servo gateServo;
int gateServoPos = 0;
// servo2 = bin
#define BINSERVO_PIN  13
Servo binServo;
int binServoPos = 0;

// parallax distance sensor
#define DISTANCESENSOR_PIN 5
const int nearDistanceCm = 10; //TODO check with Iman

// UV light source pin
#define UV_PIN 6

void setup() {
  // initialize serial communication
  Serial.begin(9600);
  Serial.println("UV Sorting Machine v1.0 - Welcome!");
  Serial.println("");

  // initialize I2C for color sensor
  if (tcs.begin()) {
    Serial.println("Found TCS34725 sensor");
  } else {
    Serial.println("No TCS34725 found ... check your connections");
    while (1);
  }

  //initialize proximity sensor
  digitalWrite(DISTANCESENSOR_PIN, HIGH); // Turn on pullup resistor

  //initialize servos
  gateServo.attach(GATESERVO_PIN);
  binServo.attach(BINSERVO_PIN);

  //close gate intially
  gateServo.write(0);

  // UV light source
  pinMode(UV_PIN, OUTPUT);

}

void loop() {
  //wait for object to be detected
  getDistanceCm();
  //while(getDistanceCm() > nearDistanceCm);
  
  //wait for user closing the door -> container dark
  delay(5000); //TODO check with Iman

  //turn on UV LED
  digitalWrite(UV_PIN, HIGH);

  //read color
  readColor();

  classifyColor();

  //based on color, rotate bin = carousel = servo2
  //TODO color reading
  binServo.write(90);

  //open gate = servo1
  gateServo.write(180);

  //wait for object to disappear
  //while(getDistanceCm() <= nearDistanceCm);
  getDistanceCm();

  //close gate = servo1
  gateServo.write(0);

  //turn off UV light
  digitalWrite(UV_PIN, LOW);
}

// distance sensor
//do an echo with the ultrasound sensor
int ping(){
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

int getDistanceCm()
{
  // read the distance sensor
  int sensorreading = ping();
  Serial.print("distance = "); Serial.println(sensorreading);
  return sensorreading;
}

// color sensor
void readColor(){
  //TODO check if this is needed
  //tcs.setInterrupt(true);  // turn off LED

  delay(50);  // takes 50ms to read
  tcs.getRawData(&red, &green, &blue, &clear);
  
  Serial.print("C:\t"); Serial.print(clear);
  Serial.print("\tR:\t"); Serial.print(red);
  Serial.print("\tG:\t"); Serial.print(green);
  Serial.print("\tB:\t"); Serial.print(blue);

  rgbToHSV();

  Serial.print("\tH:\t"); Serial.print(hue);
  Serial.print("\tS:\t"); Serial.print(saturation);
  Serial.print("\tV:\t"); Serial.print(value);  
}

void classifyColor(){  
  
  //blue
  if(hue > 0.66) {
    Serial.println("object is blue");
  } else if (hue > 0.33) {
    Serial.println("object is green");
  } else {
    Serial.println("object is red");
  }
}

//RGB to HSV
void rgbToHSV(){

  //http://www.rapidtables.com/convert/color/rgb-to-hsv.htm
  //https://github.com/ratkins/RGBConverter/blob/master/RGBConverter.cpp
  
  float r = red/255.0;
  float g = green/255.0;
  float b = blue/255.0;
  float Cmax = max(r, max(g, b));
  float Cmin = max(r, max(g, b));
  float delta = Cmax - Cmin;

  const float epsi = 0.0001;
  value = Cmax;
  saturation = abs(Cmax) < epsi ? 0.0 : delta / Cmax;
  if (abs(delta) < epsi) { 
        hue = 0.0; // achromatic
    } else {
        if (abs(Cmax - r) < epsi) {
            hue = (g - b) / delta + (g < b ? 6.0 : 0.0);
        } else if (abs(Cmax - g) < epsi) {
            hue = (b - r) / delta + 2.0;
        } else if (Cmax == b) {
            hue = (r - g) / delta + 4.0;
        }
        hue /= 6.0;
    }
}

