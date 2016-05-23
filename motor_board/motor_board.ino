#include <LiquidCrystal_I2C.h>
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
#define BUTTON 13

#define UP 1
#define DOWN 2
#define WAIT false

#define LCD 0x27

typedef volatile struct {
  int rotations = 0;
  unsigned int direction = false;
} LeadscrewData;

LiquidCrystal_I2C Lcd(0x27, 20, 4);

SensorData sensor_data;
LeadscrewData leadscrew;
volatile int count = 0;
char wait = true;

void leadscrew_irq() {
  count++;
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
  pinMode(RIGHT_MOTOR_SPEED, OUTPUT);
  pinMode(LEFT_MOTOR_SPEED, OUTPUT);
  pinMode(RIGHT_MOTOR_DIR, OUTPUT);
  pinMode(LEFT_MOTOR_DIR, OUTPUT);
  
  pinMode(LEADSCREW_UP, OUTPUT);
  pinMode(LEADSCREW_DOWN, OUTPUT);
  pinMode(LEADSCREW_SENSOR, INPUT_PULLUP);
  pinMode(BUTTON, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(LEADSCREW_SENSOR), leadscrew_irq, CHANGE);
  
//  Wire.begin();        // join i2c bus (address optional for master)
//  Lcd.init();
//  Lcd.backlight();
  Serial.begin(9600);  // start serial for output
  Serial.println("Initialized.");
}

void loop() {
//  Wire.requestFrom(8, 6);    // request 6 bytes from slave device #8
//  int index = 0;
//  while (Wire.available()) { // slave may send less than requested
//    ((uint8_t*) &sensor_data)[index] = Wire.read(); // receive a byte as character
//    index++;
//  }
//  while (wait == true){
//    wait = digitalRead(BUTTON);
//    if (wait == false)
//      Serial.println("Go!");
//    delay(100);
//  }
//
//  if (wait == false){
//    digitalWrite(RIGHT_MOTOR_DIR, 0);
//    digitalWrite(LEFT_MOTOR_DIR, 0);
//    digitalWrite(RIGHT_MOTOR_SPEED, 1);
//    digitalWrite(LEFT_MOTOR_SPEED, 1);
//  }
//  
//  if (sensor_data.bump_bogey) {
//    digitalWrite(LEFT_MOTOR_SPEED, 0);
//    digitalWrite(RIGHT_MOTOR_SPEED, 0);
//    wait = true;
//    Serial.println("Bump!");
//  }

  if (Serial.available()) {
    int serial;
    serial = Serial.read();
    if (serial == 'u') {
      Serial.println("Going up.");
      count = 0;
      delay(100);
      digitalWrite(LEADSCREW_UP, 1);
      delay(2000);
      digitalWrite(LEADSCREW_UP, 0);
      leadscrew.rotations += count;
    } else if (serial == 'd') {
      Serial.println("Going down.");
      count = 0;
      delay(100);
      digitalWrite(LEADSCREW_DOWN, 1);
      delay(2000);
      digitalWrite(LEADSCREW_DOWN, 0);
      leadscrew.rotations -= count;
    }
  }

  Serial.print("Rotations\t");
  Serial.println(count, DEC);
//  Serial.print("Total leadscrew rotations:\t");
//  Serial.println(leadscrew.rotations, DEC);
//  Serial.print("Leadscrew position:\t");
//  Serial.println(leadscrew.rotations*1.25, DEC);

//  char buffer[7];
//  itoa(count,buffer,10);
//  for (int i = 0; i<7; i++)
//    Lcd.write(buffer[i]);

  delay(1000);
}
