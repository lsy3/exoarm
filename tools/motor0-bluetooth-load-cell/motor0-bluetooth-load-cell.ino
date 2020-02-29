/*
 Example using the SparkFun HX711 breakout board with a scale
 By: Nathan Seidle
 SparkFun Electronics
 Date: November 19th, 2014
 License: This code is public domain but you buy me a beer if you use this and we meet someday (Beerware license).
 
 This example demonstrates basic scale output. See the calibration sketch to get the calibration_factor for your
 specific load cell setup.
 
 This example code uses bogde's excellent library: https://github.com/bogde/HX711
 bogde's library is released under a GNU GENERAL PUBLIC LICENSE
 
 The HX711 does one thing well: read load cells. The breakout board is compatible with any wheat-stone bridge
 based load cell which should allow a user to measure everything from a few grams to tens of tons.

 Arduino pin 2 -> HX711 CLK
 3 -> DAT
 5V -> VCC
 GND -> GND
 
 The HX711 board can be powered from 2.7V to 5V so the Arduino 5V power should be fine.
 
*/

#include "HX711.h" //This library can be obtained here http://librarymanager/All#Avia_HX711
#include <Servo.h>

#define calibration_factor -7050.0 //This value is obtained using the SparkFun_HX711_Calibration sketch

#define LOADCELL_DOUT_PIN 5 // D5
#define LOADCELL_SCK_PIN 4 // D4

#define MOTOR_PIN 3 // D3
#define DELAY 1000
#define MOTOR_POS0 45

HX711 scale;
Servo myservo; // create servo object to control a servo

#include <SoftwareSerial.h>
SoftwareSerial Serial1(6, 7); // RX (D6), TX (D7)

void setup() {
  Serial.begin(9600);
  Serial.println("HX711 scale demo");

  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(calibration_factor); //This value is obtained by using the SparkFun_HX711_Calibration sketch
  scale.tare();	//Assuming there is no weight on the scale at start up, reset the scale to 0

  Serial.println("Readings:");

  pinMode(MOTOR_PIN, OUTPUT);
  myservo.attach(MOTOR_PIN); // attaches the servo on pin 9 to the servo object
  myservo.write(MOTOR_POS0);
  Serial1.begin(38400);
  delay(DELAY);
}

void loop() {
  Serial.print("Reading: ");
  Serial.print(scale.get_units(), 1); //scale.get_units() returns a float
  Serial.print(" lbs"); //You can change this to kg but you'll need to refactor the calibration_factor
  Serial.println();

  myservo.write(MOTOR_POS0); // tell servo to go to position in variable 'pos'
  Serial1.println("hello");
  
  delay(10);
}
