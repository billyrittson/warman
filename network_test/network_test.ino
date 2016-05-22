#include <Wire.h>
#include <warman.h>

SensorData sensor_data;
int index = 0;

void setup() {
  Wire.begin();        // join i2c bus (address optional for master)
  Serial.begin(9600);  // start serial for output
  Serial.println("Initialized.");
}

void loop() {
  Wire.requestFrom(8, 6);    // request 6 bytes from slave device #8
  index = 0;
  while (Wire.available()) { // slave may send less than requested
    ((uint8_t*) &sensor_data)[index] = Wire.read(); // receive a byte as character
    index++;
  }

  if (sensor_data.bump_bogey)
    Serial.println("Bump!");

  delay(100);
}
