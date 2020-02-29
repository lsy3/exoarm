#include <Servo.h>

#define MOTOR_PIN 3 // D3
#define MOTOR_MAXPOS 125
#define MOTOR_MINPOS 0
#define DELAY 2000

Servo myservo; // create servo object to control a servo
int pos = MOTOR_MINPOS; // variable to store the servo position
 
void setup()
{
  pinMode(MOTOR_PIN, OUTPUT);
  myservo.attach(MOTOR_PIN); // attaches the servo on pin 9 to the servo object
  myservo.write(MOTOR_MINPOS);
  delay(DELAY);
}
 
void loop() {
}
