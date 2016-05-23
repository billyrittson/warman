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

#define BEGIN 0
#define FIRST_TURN 1
#define DRIVE_TO_PIPE 2
#define LIFTING 3
#define BOGEY_DRIVE 4
#define LOWERING 5
#define UNHOOKING 6
#define DRIVE_TO_END 7
#define FINAL_TURN 8
#define DRIVE_TO_FINISH 9
#define DONE 10

#define EXTEND 0
#define RETRACT 1

#define CCW 0
#define CW 1

#define FORWARDS 0
#define BACKWARDS 1

#define MAX_SPEED 255

#define LCD 0x27

LiquidCrystal_I2C Lcd(0x27, 20, 4);

SensorData sensor_data;
volatile int count = 0;
char wait = true;
char state = BEGIN;

void wheel_speed(int speed) {
  Serial.print("Setting both wheels to speed ");
  Serial.println(speed, DEC);
}

void bogey_soft_start() {
  Serial.println("Ramping bogey speed up...");
}

void bogey_stop() {
  Serial.println("Stopping bogey.");
}

void drive(int time, char direction) {
  Serial.print("Driving for ");
  Serial.print(time, DEC);
  Serial.print("ms, in the ");
  if (direction == FORWARDS) {
    Serial.println("forwards direction.");
  } else if (direction == BACKWARDS) {
    Serial.println("backwards direction.");
  }
}

void turn(int time, char direction) {
  Serial.print("Turning for ");
  Serial.print(time, DEC);
  Serial.print("ms, in the ");
  if (direction == CCW) {
    Serial.println("counter-clockwise direction.");
  } else if (direction == CW) {
    Serial.println("clockwise direction.");
  }
}

void leadscrew(int time, char direction) {
  if (direction == EXTEND) {
    Serial.print("Extending");
  } else if (direction == RETRACT) {
    Serial.print("Retracting");
  }
  Serial.print("leadscrew for ");
  Serial.print(time, DEC);
  Serial.println("ms.");
}

void celebrate() {
  Serial.println("Course completed!");
}

void setup() {
  pinMode(RIGHT_MOTOR_SPEED, OUTPUT);
  pinMode(LEFT_MOTOR_SPEED, OUTPUT);
  pinMode(RIGHT_MOTOR_DIR, OUTPUT);
  pinMode(LEFT_MOTOR_DIR, OUTPUT);
  
  pinMode(LEADSCREW_UP, OUTPUT);
  pinMode(LEADSCREW_DOWN, OUTPUT);
  pinMode(LEADSCREW_SENSOR, INPUT_PULLUP);
  pinMode(BUTTON, INPUT_PULLUP);
  
  Wire.begin();        // join i2c bus (address optional for master)
//  Lcd.init();
//  Lcd.backlight();
  Serial.begin(9600);  // start serial for output
  Serial.println("Initialized.");
}

void loop() {
  if (!wait) {
    Serial.print("In main loop. State:\t");
    Serial.println(state, DEC);
    
    // check for new data from the sensor board.
    Wire.requestFrom(8, 6);
    int index = 0;
    while (Wire.available()) { // slave may send less than requested
      Serial.println("Wire data available. Processing...");
      ((uint8_t*) &sensor_data)[index] = Wire.read(); // receive a byte as character
      index++;
    }

    Serial.print("Switching on state ");
    Serial.println(state, DEC);
    switch (state) {
      case BEGIN:
        drive(2000, FORWARDS);
        leadscrew(2000, EXTEND);
        state = FIRST_TURN;
        break;
      case FIRST_TURN:
        turn(1000, CCW);
        state = DRIVE_TO_PIPE;
        break;
      case DRIVE_TO_PIPE:
        wheel_speed(MAX_SPEED);
        if (sensor_data.bump_bogey) {
          wheel_speed(0);
          state = LIFTING;
        }
        break;
      case LIFTING:
        leadscrew(2000, RETRACT);
        state = BOGEY_DRIVE;
        break;
      case BOGEY_DRIVE:
        bogey_soft_start();
        while (state == BOGEY_DRIVE) {
          if (sensor_data.bogey_dist < 200) {
            bogey_stop();
            state = LOWERING;
          }
        }
        break;
      case LOWERING:
        leadscrew(2000, EXTEND);
        state = UNHOOKING;
        break;
      case UNHOOKING:
        drive(2000, BACKWARDS);
        leadscrew(2000, RETRACT);
        state = DRIVE_TO_END;
        break;
      case DRIVE_TO_END:
        drive(2000, FORWARDS);
        state = FINAL_TURN;
        break;
      case FINAL_TURN:
        turn(1000, CW);
        state = DRIVE_TO_FINISH;
        break;
      case DRIVE_TO_FINISH:
        drive(500, FORWARDS);
        state = DONE;
        break;
      case DONE:
        celebrate();
        break;
    }

    delay(1000);
  } else {
    while (wait) {
      wait = digitalRead(BUTTON);
    }
    Serial.println("GO!");
  }
}
