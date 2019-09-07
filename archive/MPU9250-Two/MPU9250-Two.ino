/**
 * Sample program for the MPU9250 using SPI
 *
 * Sample rate of the AK8963 magnetometer is set at 100Hz. 
 * There are only two options: 8Hz or 100Hz so I've set it at 100Hz
 * in the library. This is set by writing to the CNTL1 register
 * during initialisation.
 *
 * Copyright (C) 2015 Brian Chen
 *
 * Open source under the MIT license. See LICENSE.txt.
 */

#include <SPI.h>
#include "MPU9250.h"
#include <Servo.h>
#include "Quaternion.h"
#include "HX711.h" //This library can be obtained here http://librarymanager/All#Avia_HX711


#define calibration_factor -7050.0 //This value is obtained using the SparkFun_HX711_Calibration sketch
#define LOADCELL_DOUT_PIN  PB10
#define LOADCELL_SCK_PIN  PB11
HX711 scale;

#define SPI_CLOCK 8000000  // 8MHz clock works.

#define SS_PIN1   PA4
#define SS_PIN2   PA3
#define LED      PB12
#define SERVO_PIN PA2

#define MODE_SENSE 1
#define MODE_REHAB 2

int mode = -1; 
#define WAITFORINPUT(){            \
	while(!Serial1.available()){};  \
	while(Serial1.available()){     \
		mode = Serial1.read();             \
	};                             \
}                                  \

MPU9250 mpu1(SPI_CLOCK, SS_PIN1);
MPU9250 mpu2(SPI_CLOCK, SS_PIN2);

Servo myservo; // create servo object to control a servo

// global constants for 9 DoF fusion and AHRS (Attitude and Heading Reference System)
float GyroMeasError = PI * (40.0f / 180.0f);   // gyroscope measurement error in rads/s (start at 40 deg/s)
float GyroMeasDrift = PI * (0.0f  / 180.0f);   // gyroscope measurement drift in rad/s/s (start at 0.0 deg/s/s)
// There is a tradeoff in the beta parameter between accuracy and response speed.
// In the original Madgwick study, beta of 0.041 (corresponding to GyroMeasError of 2.7 degrees/s) was found to give optimal accuracy.
// However, with this value, the LSM9SD0 response time is about 10 seconds to a stable initial quaternion.
// Subsequent changes also require a longish lag time to a stable output, not fast enough for a quadcopter or robot car!
// By increasing beta (GyroMeasError) by about a factor of fifteen, the response time constant is reduced to ~2 sec
// I haven't noticed any reduction in solution accuracy. This is essentially the I coefficient in a PID control sense; 
// the bigger the feedback coefficient, the faster the solution converges, usually at the expense of accuracy. 
// In any case, this is the free parameter in the Madgwick filtering and fusion scheme.
float beta = sqrt(3.0f / 4.0f) * GyroMeasError;   // compute beta
float zeta = sqrt(3.0f / 4.0f) * GyroMeasDrift;   // compute zeta, the other free parameter in the Madgwick scheme usually set to a small or zero value
#define Kp 2.0f * 5.0f // these are the free parameters in the Mahony filter and fusion scheme, Kp for proportional feedback, Ki for integral
#define Ki 0.0f

float pitch1, yaw1, roll1, pitch2, yaw2, roll2;
float deltat = 0.0f, sum = 0.0f;        // integration interval for both filter schemes
float qone[4] = {1.0f, 0.0f, 0.0f, 0.0f};    // vector to hold quaternion
float qtwo[4] = {1.0f, 0.0f, 0.0f, 0.0f};    // vector to hold quaternion
float qthe[4] = {1.0f, 0.0f, 0.0f, 0.0f};
uint32_t lastUpdate = 0, firstUpdate = 0; // used to calculate integration interval
uint32_t Now = 0;        // used to calculate integration interval
int pos = 0, change = 0;

Quaternion qRef;

void setup() {
	Serial.begin(115200);
  Serial1.begin(9600);
	pinMode(LED, OUTPUT);
	digitalWrite(LED, HIGH);

	SPI.begin();

	Serial.println("Press any key to continue");
	WAITFORINPUT();

	mpu1.init(true);
  mpu2.init(true);
	uint8_t wai = mpu1.whoami();
	if (wai == 0x71){
		Serial.println("Successful connection1");
	}
	else{
		Serial.print("Failed connection1: ");
		Serial.println(wai, HEX);
	}

	uint8_t wai_AK8963 = mpu1.AK8963_whoami();
	if (wai_AK8963 == 0x48){
		Serial.println("Successful connection to mag1");
	}
	else{
		Serial.print("Failed connection to mag1: ");
		Serial.println(wai_AK8963, HEX);
	}

  wai = mpu2.whoami();
  if (wai == 0x71){
    Serial.println("Successful connection2");
  }
  else{
    Serial.print("Failed connection2: ");
    Serial.println(wai, HEX);
  }

  wai_AK8963 = mpu2.AK8963_whoami();
  if (wai_AK8963 == 0x48){
    Serial.println("Successful connection to mag2");
  }
  else{
    Serial.print("Failed connection to mag2: ");
    Serial.println(wai_AK8963, HEX);
  }

	mpu1.calib_acc();
	mpu1.calib_mag();
  mpu2.calib_acc();
  mpu2.calib_mag();

	Serial1.println("Choose mode:");
  Serial1.print(MODE_SENSE); Serial1.print(" or not in option: sense, ");
  Serial1.print(MODE_REHAB); Serial1.println(": rehab");
	WAITFORINPUT();
  mode = (int)(mode-'0');
  Serial.print(mode); Serial1.print(mode);
  if (mode == MODE_REHAB) {
    myservo.attach(SERVO_PIN);
    pos = 0;
    myservo.write(pos);

    for(int i=0; i<500; i++) {
      updateOrientation();
      delay(1);
    }
    qRef = Quaternion(qone) * Quaternion(qtwo).conj();

    scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
    scale.set_scale(calibration_factor); //This value is obtained by using the SparkFun_HX711_Calibration sketch
    scale.tare();  //Assuming there is no weight on the scale at start up, reset the scale to 0
  
    Serial.println("Chosen rehab mode");
    Serial1.println("Chosen rehab mode");
  } else {
    mode = MODE_SENSE;
    Serial.println("Chosen sense mode");
    Serial1.println("Chosen sense mode");
    delay(2000);
  }
}


void loop() {
  updateOrientation();
  
  float angle1[3], angle2[3], angle3[3];
  
  calcYawPitchRoll(qone, angle1);
  Quaternion qtwo2 = (qRef * Quaternion(qtwo));
  qtwo2.to_array(qthe);
  calcYawPitchRoll(qthe, angle2);
  Quaternion UA_q_FA = Quaternion(qone).conj() * qRef * Quaternion(qtwo);
  UA_q_FA.to_euler_angles(angle3);
  
  Serial.print("Orientation: ");
  Serial1.print("Orientation: ");
  for(int i=0; i<3; i++) {
    Serial.print(angle1[i], 2);
    Serial.print(" ");
    Serial1.print(angle1[i], 2);
    Serial1.print(" ");
  }
  for(int i=0; i<3; i++) {
    Serial.print(angle2[i], 2);
    Serial.print(" ");
    Serial1.print(angle2[i], 2);
    Serial1.print(" ");
  }
//  for(int i=0; i<3; i++) {
//    Serial.print(angle3[i], 2);
//    Serial.print(" ");
//    Serial1.print(angle3[i], 2);
//    Serial1.print(" ");
//  }
//  Serial.println(); Serial1.println();
  
  if (mode == MODE_SENSE) {
    delay(5);
    Serial.println(); Serial1.println();
  } else if (mode == MODE_REHAB) {
    change = (int)scale.get_units();
    if(pos < 0) {
      change = 0;
      pos = 0;
    } else if(pos > 135) {
      change = 0;
      pos = 135;
    }
    pos += change;
    myservo.write(pos); // tell servo to go to position in variable 'pos'
  	delay(1);
    Serial.println(scale.get_units()); Serial1.println(scale.get_units());
  }
}

void updateOrientation() {
    // various functions for reading
  // mpu.read_mag();
  // mpu.read_acc();
  // mpu.read_gyro();
  Now = micros();
  deltat = ((Now - lastUpdate)/1000000.0f); // set integration time by time elapsed since last filter update
  lastUpdate = Now;
  
  mpu1.read_all();
//  Serial.print(mpu.gyro_data[0]);   Serial.print('\t');
//  Serial.print(mpu.gyro_data[1]);   Serial.print('\t');
//  Serial.print(mpu.gyro_data[2]);   Serial.print('\t');
//  Serial.print(mpu.accel_data[0]);  Serial.print('\t');
//  Serial.print(mpu.accel_data[1]);  Serial.print('\t');
//  Serial.print(mpu.accel_data[2]);  Serial.print('\t');
//  Serial.print(mpu.mag_data[0]);    Serial.print('\t');
//  Serial.print(mpu.mag_data[1]);    Serial.print('\t');
//  Serial.print(mpu.mag_data[2]);    Serial.print('\t');
//  Serial.println(mpu.temperature);

  
  MadgwickQuaternionUpdate(qone, mpu1.accel_data[0], mpu1.accel_data[1], mpu1.accel_data[2], 
                           mpu1.gyro_data[0]*PI/180.0f, mpu1.gyro_data[1]*PI/180.0f, mpu1.gyro_data[2]*PI/180.0f, 
                           mpu1.mag_data[1], mpu1.mag_data[0], mpu1.mag_data[2]);
  delay(5);
  mpu2.read_all();
  MadgwickQuaternionUpdate(qtwo, mpu2.accel_data[0], mpu2.accel_data[1], mpu2.accel_data[2], 
                           mpu2.gyro_data[0]*PI/180.0f, mpu2.gyro_data[1]*PI/180.0f, mpu2.gyro_data[2]*PI/180.0f, 
                           mpu2.mag_data[1], mpu2.mag_data[0], mpu2.mag_data[2]);
}
