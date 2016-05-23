/*
 * serial_motor_control.pde
 * -----------------
 * Controls the speed of a motor depending on what is received 
 * through the serial port.
 *
 * http://spacetinkerer.blogspot.com
 */

#define BOGEY 11
#define LEFT_MOTOR_SPEED 9
#define RIGHT_MOTOR_SPEED 10

int input = 0;

void setup() {              
  // initialize the PWM pin #BOGEY as an output.
  pinMode(BOGEY, OUTPUT);
  Serial.begin(9600); 
  pinMode(RIGHT_MOTOR_SPEED, OUTPUT);
  pinMode(LEFT_MOTOR_SPEED, OUTPUT);
  digitalWrite(LEFT_MOTOR_SPEED, 0);
  digitalWrite(RIGHT_MOTOR_SPEED, 0);
  Serial.println("Initialized.");
}

void loop() {
    input = Serial.read();
    if (input == '0'){
      analogWrite(BOGEY, 0);
      Serial.println("Speed is 0!");
    }
    if (input == '1'){
      analogWrite(BOGEY, 64);
      Serial.println("Speed is at 25%");
    }
    if (input == '2'){
      analogWrite(BOGEY, 127);
      Serial.println("Speed is at 50%");
    }
    if (input == '3'){
      analogWrite(BOGEY, 192);
      Serial.println("Speed is at 75%");
    }
    if (input == '4'){
      analogWrite(BOGEY,255);
      Serial.println("Speed is at 100%");
    }
}
