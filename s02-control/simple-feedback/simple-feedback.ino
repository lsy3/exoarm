/*
Simple feedback 
*/

#include "HX711.h" //This library can be obtained here http://librarymanager/All#Avia_HX711
#include <Servo.h>

#define LOAD_CALIB_FACTOR -7050.0 //This value is obtained using the SparkFun_HX711_Calibration sketch
#define LOADCELL_DOUT_PIN  PB10
#define LOADCELL_SCK_PIN  PB11

#define MOTOR_PIN PA2
#define MOTOR_MAXPOS 125
#define MOTOR_MINPOS 0

#define FB_SCALE 0.5

#define DELAY 2000

HX711 scale;
Servo myservo; // create servo object to control a servo
int pos = MOTOR_MINPOS; // variable to store the servo position

void setup() {
  Serial.begin(9600);
  Serial.println("Simple feedback demo");

  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(LOAD_CALIB_FACTOR); //This value is obtained by using the SparkFun_HX711_Calibration sketch
  scale.tare();	//Assuming there is no weight on the scale at start up, reset the scale to 0

  pinMode(MOTOR_PIN, OUTPUT);
  myservo.attach(MOTOR_PIN); // attaches the servo on pin 9 to the servo object
  myservo.write(MOTOR_MINPOS);
  delay(DELAY);
}

void loop() {
  pos += FB_SCALE*scale.get_units();

  if(pos < MOTOR_MINPOS) pos = MOTOR_MINPOS;
  else if(pos > MOTOR_MAXPOS) pos = MOTOR_MAXPOS;

  Serial.print("Reading: ");
  Serial.print(scale.get_units(), 1); //scale.get_units() returns a float
  Serial.print(" lbs. Pos: "); //You can change this to kg but you'll need to refactor the calibration_factor
  Serial.println(pos);
  
  myservo.write(pos);
}
