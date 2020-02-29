/*
Simple feedback 
*/

#include "HX711.h" //This library can be obtained here http://librarymanager/All#Avia_HX711
#include <Servo.h>
#include <SoftwareSerial.h>
SoftwareSerial Serial1(6, 7); // RX (D6), TX (D7)

#define LOAD_CALIB_FACTOR -7050.0 //This value is obtained using the SparkFun_HX711_Calibration sketch
#define LOADCELL_DOUT_PIN 5 // D5
#define LOADCELL_SCK_PIN 4 // D4

#define MOTOR_PIN 3 // D3
#define MOTOR_MAXPOS 125
#define MOTOR_MINPOS 0

#define CMD_DELAY 100 // mm

#define K_P 0.6
#define K_I 0.0
#define K_D 0.1

#define DELAY 1000

HX711 scale;
Servo myservo; // create servo object to control a servo
int pos = MOTOR_MINPOS; // variable to store the servo position

double error, new_error, int_error, diff_error;
unsigned long time;

void setup() {
  Serial.begin(9600);
  Serial.println("Simple feedback demo");
  Serial1.begin(38400);
    
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(LOAD_CALIB_FACTOR); //This value is obtained by using the SparkFun_HX711_Calibration sketch
  scale.tare();	//Assuming there is no weight on the scale at start up, reset the scale to 0

  error = 0.0;
  int_error = 0.0;
  diff_error = 0.0;
  pinMode(MOTOR_PIN, OUTPUT);
  myservo.attach(MOTOR_PIN); // attaches the servo on pin 9 to the servo object
  myservo.write(MOTOR_MINPOS);
  time = millis();
  delay(DELAY);
}

void loop() {
  new_error = scale.get_units();
  int_error += error;
  diff_error = new_error - error;
  error = new_error;

  pos += K_P*error + K_D*diff_error + K_I*int_error;

  if(pos < MOTOR_MINPOS) pos = MOTOR_MINPOS;
  else if(pos > MOTOR_MAXPOS) pos = MOTOR_MAXPOS;

  Serial.print("Reading: ");
  Serial.print(scale.get_units(), 1); //scale.get_units() returns a float
  Serial.print(" lbs. Pos: "); //You can change this to kg but you'll need to refactor the calibration_factor
  Serial.println(pos);
  if(millis()-time > CMD_DELAY) {
    Serial1.println(pos);
    time = millis();
  }
  myservo.write(pos);
}
