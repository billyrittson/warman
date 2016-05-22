#include <LiquidCrystal_I2C.h>

int leadscrew_up = 5;
int leadscrew_down = 4;
int buzzer = 6;
int left_motor_dir = 7;
int right_motor_dir = 8;
int left_motor_speed = 9;
int right_motor_speed = 10;
int bogey_pwm = 11;
int bogey_dir = 12;

int lcd_address = 0;

unsigned int speed = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(right_motor_speed, OUTPUT);
  pinMode(leadscrew_down, OUTPUT);
  pinMode(leadscrew_up, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  if (Serial.available()) {
    int serial;
    serial = Serial.read();
    if (serial == 'u') {
      digitalWrite(leadscrew_up, 1);
      delay(2000);
      digitalWrite(leadscrew_up, 0);
    } else if (serial == 'd') {
      digitalWrite(leadscrew_down, 1);
      delay(2000);
      digitalWrite(leadscrew_down, 0);
    }
  }
}
