#include <Pushbutton.h>
#include <ZumoMotors.h>
#include <Wire.h>
#include <LSM303.h>

#define CRB_REG_M_2_5GAUSS 0x60 // CRB_REG_M value for magnetometer +/-2.5 gauss full scale
#define CRA_REG_M_220HZ    0x1C // CRA_REG_M value for magnetometer 220 Hz update rate

Pushbutton button(ZUMO_BUTTON);
ZumoMotors motors;
LSM303 compass;

float setPoint = 0;

float Kp = 0.5;
float Ki = 1;
float Kd = 0;

void setup() {
  LSM303::vector<int16_t> running_min = {-5417, 287, 20907}, running_max = {353, 5988, 27306};

  // Initiate the Wire library and join the I2C bus as a master
  Wire.begin();

  // Initiate LSM303
  compass.init();

  // Enables accelerometer and magnetometer
  compass.enableDefault();

  compass.writeReg(LSM303::CRB_REG_M, CRB_REG_M_2_5GAUSS); // +/- 2.5 gauss sensitivity to hopefully avoid overflow problems
  compass.writeReg(LSM303::CRA_REG_M, CRA_REG_M_220HZ);    // 220 Hz compass update rate
  compass.m_min = running_min;
  compass.m_max = running_max;

  Serial.begin(9600);
  Serial.println("Waiting for button press...");
  //button.waitForButton();
  compass.read();
  setPoint = compass.heading();
  Serial.println("Set point:");
  Serial.println(setPoint);
}

float previous = 0;

float pid(float present_value) {
  float error = setPoint - compass.heading();
  float proportional = Kp * error;
  float integral = Ki * (previous + error);
  float derivative = Kd * (previous - error);

  //Serial.println(proportional);
  //Serial.println(integral);
  //Serial.println(derivative);

  previous = error;

  float output = proportional + integral + derivative;
  Serial.println(error);
  return output;
}

void turn(int speed) {
  motors.setSpeeds(speed, -speed);
  delay(10);
  //motors.setSpeeds(0,0);
}

float getHeading() {
  compass.read();
  return compass.heading();
}

float x = 0;
float increment = 10;

void loop() {
  // put your main code here, to run repeatedly:
  float current = getHeading();
  //Serial.println("Current:");
  //Serial.println(current);
  //turn((int) (10 * pid(current)));
  turn(x);
  x += increment;
  if (x == 400) {
    increment = -10;
  } else if (x == -400) {
    increment = 10;
  }
  Serial.println(x);
  delay(100);
}
