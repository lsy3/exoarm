/*
Simple feedback 
*/

#include <Servo.h>

#define MOTOR_PIN PB1
#define MOTOR_MAXPOS 125
#define MOTOR_MINPOS 0

#define K_P 0.6
#define K_I 0.0
#define K_D 0.1

#define DELAY 1000

Servo myservo; // create servo object to control a servo
int pos = MOTOR_MINPOS; // variable to store the servo position

double error, new_error, int_error, diff_error;
int btBuf;

void setup() {
  Serial.begin(9600);
  Serial.println("Simple feedback demo");
  Serial1.begin(9600);

  error = 0.0;
  int_error = 0.0;
  diff_error = 0.0;
  pinMode(MOTOR_PIN, OUTPUT);
  myservo.attach(MOTOR_PIN); // attaches the servo on pin 9 to the servo object
  myservo.write(MOTOR_MINPOS);
  delay(DELAY);
}

void loop() {  
  btBuf = Serial1.readString().toInt();
  if(btBuf <= 0) return;
  
  pos = btBuf;

  if(pos < MOTOR_MINPOS) pos = MOTOR_MINPOS;
  else if(pos > MOTOR_MAXPOS) pos = MOTOR_MAXPOS;
  Serial.println(pos);
  
  myservo.write(pos);
}