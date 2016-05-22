#include <Wire.h>
#include <warman.h>

#define LEFT_BUMP 2
#define BOGEY_BUMP 3
#define RIGHT_BUMP 3

SensorData sensor_data;

void left_bump_irq() {
  if (digitalRead(LEFT_BUMP) == LOW) {
    sensor_data.bump_left = true;
  } else {
    sensor_data.bump_left = false;
  }
}

void right_bump_irq() {
  if (digitalRead(RIGHT_BUMP) == LOW) {
    sensor_data.bump_right = true;
    sensor_data.bump_bogey = true;
  } else {
    sensor_data.bump_right = false;
    sensor_data.bump_bogey = false;
  }
}

void right_unbump_irq() {
  
}

void setup() {
  pinMode(LEFT_BUMP, INPUT_PULLUP);
  pinMode(RIGHT_BUMP, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(LEFT_BUMP), left_bump_irq, CHANGE);
  attachInterrupt(digitalPinToInterrupt(RIGHT_BUMP), right_bump_irq, CHANGE);
  
  Wire.begin(8);                // join i2c bus with address #8
  Wire.onRequest(requestEvent); // register event
  Serial.begin(9600);  // start serial for output
  Serial.println("Slaved device initialized.");
}

void loop() {
  if (sensor_data.bump_bogey) {
    Serial.println("Bump!");
  }
  delay(100);
}

// function that executes whenever data is requested by master
// this function is registered as an event, see setup()
void requestEvent() {
  Wire.write((uint8_t*) &sensor_data, sizeof(sensor_data));
}

