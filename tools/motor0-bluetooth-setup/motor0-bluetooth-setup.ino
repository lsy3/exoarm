#include <Servo.h>

#define MOTOR_PIN PB1
#define DELAY 1000
#define MOTOR_POS0 0

Servo myservo; // create servo object to control a servo
// twelve servo objects can be created on most boards
 
void setup()
{
  pinMode(MOTOR_PIN, OUTPUT);
  myservo.attach(MOTOR_PIN); // attaches the servo on pin 9 to the servo object
  myservo.write(MOTOR_POS0);
  Serial1.begin(9600);
  delay(DELAY);
}
 
void loop()
{
  myservo.write(MOTOR_POS0); // tell servo to go to position in variable 'pos'
  Serial1.println("hello");
  delay(DELAY);
}
