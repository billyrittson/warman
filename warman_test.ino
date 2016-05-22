#include <LiquidCrystal_I2C.h>

typedef struct {
  int dir_pin;
  int speed_pin;
  int raw_speed;
  int raw_direction;
  int speed;
} WheelMotor;

WheelMotor left;

left.dir_pin = 7;
left.speed_pin = 9;
right.dir_pin = 8;
right.speed_pin = 10;

int leadscrew_up = 4;
int leadscrew_down = 5;
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
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
//  analogWrite(left_motor_speed, speed);
  digitalWrite(right_motor_speed, 1);
  speed += 10;
  Serial.println(speed,DEC);
  delay(2000);
}
