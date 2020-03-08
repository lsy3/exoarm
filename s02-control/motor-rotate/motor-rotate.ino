#include <Servo.h>

#define MOTOR_PIN 3 // D3
#define MOTOR_MAXPOS 100
#define MOTOR_MINPOS 0
#define DELAY 2000

Servo myservo; // create servo object to control a servo
// twelve servo objects can be created on most boards

int pos = MOTOR_MINPOS; // variable to store the servo position
 
void setup()
{
  pinMode(MOTOR_PIN, OUTPUT);
  myservo.attach(MOTOR_PIN); // attaches the servo on pin 9 to the servo object
  myservo.write(MOTOR_MINPOS);
  delay(DELAY);
}
 
void loop()
{
  for(pos = MOTOR_MINPOS; pos <= MOTOR_MAXPOS; pos+=2) // goes from 0 degrees to 180 degrees
  { // in steps of 1 degree
    myservo.write(pos); // tell servo to go to position in variable 'pos'
    delay(15); // waits 15ms for the servo to reach the position
  }
  delay(DELAY);
  for(pos = MOTOR_MAXPOS; pos>=MOTOR_MINPOS; pos-=2) // goes from 180 degrees to 0 degrees
  {
    myservo.write(pos); // tell servo to go to position in variable 'pos'
    delay(15; // waits 15ms for the servo to reach the position
  }
  delay(DELAY);
}
