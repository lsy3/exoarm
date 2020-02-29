#include <Servo.h>
#include <SoftwareSerial.h>
SoftwareSerial Serial1(6, 7); // RX (D6), TX (D7)

#define MOTOR_PIN 3 // D3
#define MOTOR_MAXPOS 90
#define MOTOR_MINPOS 0
#define DELAY 500

Servo myservo; // create servo object to control a servo
// twelve servo objects can be created on most boards

int pos = MOTOR_MINPOS; // variable to store the servo position
 
void setup()
{
  Serial.begin(9600);
  Serial1.begin(38400);
  Serial1.setTimeout(50);
}
 
void loop()
{
  for(pos = MOTOR_MINPOS; pos <= MOTOR_MAXPOS; pos += 1) // goes from 0 degrees to 180 degrees
  { // in steps of 1 degree
    Serial.println(pos);
    Serial1.println(pos);
    delay(100); // waits 15ms for the servo to reach the position
  }
  delay(DELAY);
  for(pos = MOTOR_MAXPOS; pos>=MOTOR_MINPOS; pos-=1) // goes from 180 degrees to 0 degrees
  {
    Serial.println(pos);
    Serial1.println(pos);
    delay(100); // waits 15ms for the servo to reach the position
  }
  delay(DELAY);
}
