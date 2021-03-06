//-*- C -*-

#include <Math.h>
#include <Servo.h>

//
// CONSTANTS
//

// multiply by degrees to get radians
//
const double RADIANS_PER_DEGREE = 0.0174532925;

const int LEFT_LIMIT = 40;
const int RIGHT_LIMIT = 130;

// milliseconds to wait between servo movement: when servo is gently oscillating
const int GENTLE_DELAY = 30;

// milliseconds to wait between servo movement: when servo is shaking back & forth
const int SHAKE_DELAY = 10;

// maximum servo movement per iteration
const int ANGLE_INCREMENT = 3;

// milliseconds to wait at one end point before reversing rotation
const int TURN_AROUND_DELAY = 10;

// number of degrees to shake servo on each side of center
const int SHAKE_OFFSET_ANGLE = 25;

//
// GLOBALS
//

Servo myservo;  // create servo object to control a servo
                // a maximum of eight servo objects can be created

int currAngle = 90;

void setup()
{
  myservo.attach(9);   // attaches the servo on pin 9 to the servo object
  myservo.write(currAngle);

  // initialize digital pin 13 as an output.
  pinMode(13, OUTPUT);

  Serial.begin(9600);  // open the serial port at 9600 bps:

  delay(3000);         // wait long enough to open Serial console
}

void loop()
{
  // randomly determine which behavior
  switch (random(2)) {
    case 0:
      currAngle = oscillateGently(currAngle, random(5, 10));
      break;

    case 1:
      currAngle = shakeServo(currAngle, random(4));
      break;
  }

  // wait 5 seconds until next loop
  delay(5000);
}

int oscillateGently(int angle, int numOscillations)
{
  for (int ii = 0; ii < numOscillations ; ii += 1) {
    delay(TURN_AROUND_DELAY);
    //  Serial.println("------------------------------------------------------------");
  
    digitalWrite(13, HIGH);   // turn the LED on (HIGH is the voltage level)
    angle = turnServo(myservo, angle, random(angle, RIGHT_LIMIT - SHAKE_OFFSET_ANGLE) + 1, GENTLE_DELAY);
  
    delay(TURN_AROUND_DELAY);
    //  Serial.println("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
  
    digitalWrite(13, LOW);    // turn the LED off by making the voltage LOW
    angle = turnServo(myservo, angle, random(SHAKE_OFFSET_ANGLE, angle), GENTLE_DELAY);
  }

  return angle;
}

int turnServo(Servo servo, int startAngle, int endAngle, int delayTime)
{
  int angle = startAngle;

  // turn right
  if (startAngle < endAngle) {
    while (angle < endAngle)
    {
      //      Serial.print("RIGHT:\t");
      //      Serial.println(angle);

      servo.write(angle);         // tell servo to go to position in variable 'angle'
      delay(delayTime);           // waits for the servo to reach the position

      angle += calcDeltaAngle(angle, startAngle, endAngle, ANGLE_INCREMENT);
    }
  }
  // turn left
  else if (startAngle > endAngle) {
    while (angle > endAngle)
    {
      //      Serial.print("LEFT:\t");
      //      Serial.println(angle);

      servo.write(angle);         // tell servo to go to position in variable 'angle'
      delay(delayTime);           // waits for the servo to reach the position

      // note decrement instead of increment
      angle -= calcDeltaAngle(angle, endAngle, startAngle, ANGLE_INCREMENT);
    }
  }
  // don't turn
  else {
    Serial.print("NO OP:\t");
    Serial.println(angle);
  }

  return angle;
}

// Rotate servo 'numShakes' back & forth, starting at 'startAngle'.
// Returns final angle of head
//
int shakeServo(int startAngle, int numShakes)
{
  int rightEnd = startAngle + SHAKE_OFFSET_ANGLE;
  int leftEnd = startAngle - SHAKE_OFFSET_ANGLE;

  turnServo(myservo, startAngle, rightEnd, SHAKE_DELAY);
  turnServo(myservo, rightEnd, leftEnd, SHAKE_DELAY);
  for (int ii = 0; ii < numShakes ; ii++)
  {
    turnServo(myservo, leftEnd, rightEnd, SHAKE_DELAY);
    turnServo(myservo, rightEnd, leftEnd, SHAKE_DELAY);
  }
  turnServo(myservo, leftEnd,  startAngle, SHAKE_DELAY);

  return startAngle;
}

int calcDeltaAngle(int angle, int startAngle, int endAngle, int maxAngleIncrement)
{
  // normal half circle is 180 degrees
  const double wholeRange = 180.0;

  // amount to rotate head depends on where it is: more in the
  // middle, & less at the ends. AKA sine wave.
  int currRange = endAngle - startAngle;
  double rangeRatio = wholeRange / currRange;
  int offsetAngle = angle - startAngle;
  double normalizedOffset = rangeRatio * offsetAngle;

  double rad = normalizedOffset * RADIANS_PER_DEGREE;
  double sineValue = sin(rad);
  int increAngle = ceil(0.1 + maxAngleIncrement * sineValue);

  /*
  Serial.print(angle);
  Serial.print("\t");
  Serial.print(increAngle);
  Serial.println("");
  */

  return increAngle;
}


