#include <Wire.h>
#include <warman.h>

#define LEADSCREW_SENSOR 2
#define LEADSCREW_DOWN 4
#define LEADSCREW_UP 5
#define BUZZER 6
#define LEFT_MOTOR_DIR 7
#define RIGHT_MOTOR_DIR 8
#define LEFT_MOTOR_SPEED 9
#define RIGHT_MOTOR_SPEED 10
#define BOGEY_SPEED 11

#define UP 1
#define DOWN 2

#define LCD 0x27

typedef volatile struct {
  int rotations = 0;
  unsigned int direction = false;
} LeadscrewData;

SensorData sensor_data;
LeadscrewData leadscrew;


void leadscrew_irq() {
  noInterrupts();
  if (leadscrew.direction == UP)
    leadscrew.rotations++;
  else
    leadscrew.rotations--;
  interrupts();
}

//void leadscrew_irq() {
//  noInterrupts();
//  if (leadscrew.direction == DOWN)
//    leadscrew.rotations--;
//  else
//    leadscrew.rotations++;
//  interrupts();
//}

void setup() {
  pinMode(LEADSCREW_UP, OUTPUT);
  pinMode(LEADSCREW_DOWN, OUTPUT);
  pinMode(LEADSCREW_SENSOR, INPUT);

  attachInterrupt(digitalPinToInterrupt(LEADSCREW_SENSOR), leadscrew_irq, RISING);
  
  Wire.begin();        // join i2c bus (address optional for master)
  Serial.begin(9600);  // start serial for output
  Serial.println("Initialized.");
}

void loop() {
  Wire.requestFrom(8, 6);    // request 6 bytes from slave device #8
  int index = 0;
  while (Wire.available()) { // slave may send less than requested
    ((uint8_t*) &sensor_data)[index] = Wire.read(); // receive a byte as character
    index++;
  }

  if (sensor_data.bump_bogey)
    Serial.println("Bump!");

  if (Serial.available()) {
    int serial;
    serial = Serial.read();
    if (serial == 'u') {
      leadscrew.direction = UP;
      digitalWrite(LEADSCREW_UP, 1);
      delay(2000);
      digitalWrite(LEADSCREW_UP, 0);
    } else if (serial == 'd') {
      leadscrew.direction = DOWN;
      digitalWrite(LEADSCREW_DOWN, 1);
      delay(2000);
      digitalWrite(LEADSCREW_DOWN, 0);
    }
  }

  Serial.print("Leadscrew rotations:\t");
  Serial.println(leadscrew.rotations, DEC);
  Serial.print("Leadscrew position:\t");
  Serial.println(leadscrew.rotations/2*1.25, DEC);

  delay(1000);
}
