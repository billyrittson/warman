#include <TimerOne.h>

/*
USB Pinout (Left to Right, USB symbol up)
4: GND
3: Clk
2: Data
1: Vcc
*/

#include "ps2.h"

#define FRONT_OFFSET 27.5 // distance from the front axle line to front mouse sensor, in mm
#define REAR_OFFSET 127.5 // distance from front axle line to rear mouse sensor, in mm
#define SCALE_FACTOR 50.0 // counts per mm

#define PRINTIC
#define DEBUG

PS2 mouseR(13,12);
PS2 mouseF(11,10);

union MouseStatusBits {
  struct {
    unsigned char always0:1;
    unsigned char mode:1;
    unsigned char enable:1;
    unsigned char scaling:1;
    unsigned char always02:1;
    unsigned char leftbtn:1;
    unsigned char middlebtn:1;
    unsigned char rightbtn:1;
  } flags;
  char raw;
};

typedef struct {
  union MouseStatusBits status;
  char resolution;
  char samplerate;
} MouseStatus;

union MouseDataBits {
  struct {
    unsigned char yoverflow:1;
    unsigned char xoverflow:1;
    unsigned char ysign:1;
    unsigned char xsign:1;
    unsigned char always1:1;
    unsigned char middlebtn:1;
    unsigned char rightbtn:1;
    unsigned char leftbtn:1;
  } flags;
  char raw;
};

typedef struct {
  union MouseDataBits stat;
  char x = 0;
  char y = 0;
} MouseData;

typedef struct {
  struct {
    float x = 0;
    float y = 0;
  } front;
  struct {
    float x = 0;
    float y = 0;
  } rear;
  struct {
    float x = 0;
    float y = 0;
    float theta = 0;
  } robot;
} Position; 

MouseData front_data;
MouseData rear_data;

Position position;

void getStatus(PS2 &mouse){
  MouseStatus status;
  int resolution;

  mouse.write(0xE9); // send status request

  mouse.read(); // get ack byte
  status.status.raw = mouse.read();
  status.resolution = mouse.read();
  status.samplerate = mouse.read();

  Serial.print("Mode:");
  Serial.println(status.status.flags.mode ? "remote" : "stream");
  Serial.print("Data reporting:");
  Serial.println(status.status.flags.enable ? "enabled" : "disabled");
  Serial.print("Scaling factor:");
  Serial.println(status.status.flags.scaling ? "2x" : "1x");
  Serial.print("Resolution:");
  switch(status.resolution) {
    case 00 :
      resolution = 1;
    case 01 :
      resolution = 2;
    case 02 :
      resolution = 4;
    case 03 :
      resolution = 8;
  }
  Serial.print(resolution, DEC);
  Serial.println(" counts/mm");
  Serial.print("Sample rate:");
  Serial.println(status.samplerate, DEC);

}

void read_mice() {
  mouseF.mouse_pos(front_data.stat.raw,front_data.x,front_data.y); //query front mouse
  mouseR.mouse_pos(rear_data.stat.raw,rear_data.x,rear_data.y); //query rear mouse
  
  position.front.x += front_data.x/SCALE_FACTOR;
  position.front.y += front_data.y/SCALE_FACTOR;
  position.rear.x += rear_data.x/SCALE_FACTOR;
  position.rear.y += rear_data.y/SCALE_FACTOR;
}

void findIC(MouseData &front, MouseData &rear, float &x, float &y){
  // let's check edge cases.
  // straight forwards:
  if (!front.x & !rear.x) {// both must be true, otherwise we might be pivoting on a sensor
    Serial.println("Going straight forwards. Or stationary.");
    x = 0;
    y = 0;
    return;
  }
  // straight sideways:
  if (!front.y & !rear.y) {// in this case, i think it's impossible for only one to be moving, but let's be thorough
    Serial.println("Going straight sideways, wtf?");
    x = 0;
    y = 0;
    return;
  }
  // if we get here, we might be rotating. find the slopes of the vectors:
  // REMEMBER: to find the IC we are finding the intersection of the n-lines, not the motion vectors
  // this means we must REVERSE x and y values!
  float front_slope = (float)front.x/(float)front.y;
  Serial.print("Front slope=");
  Serial.println(front_slope, DEC);
  if (front_slope != front_slope) { //this is how we check for NaN
    Serial.print("We ended up with a NaN for front slope. x was ");
    Serial.print(front.x, DEC);
    Serial.print(", y was ");
    Serial.println(front.y, DEC);
  }
  if (!isfinite(front_slope)) {
    Serial.print("We ended up with a inf for front slope. x was ");
    Serial.print(front.x, DEC);
    Serial.print(", y was ");
    Serial.println(front.y, DEC);
  }
  float rear_slope = (float)rear.x/(float)rear.y;
  Serial.print("Rear slope=");
  Serial.println(rear_slope, DEC);
  // if the slopes are identical, we are translating, not rotating
  if (front_slope == rear_slope){
    Serial.println("Slopes equal. Translation occuring.");
    return;
  }
  Serial.println(rear_slope, DEC);
  //if we make it this far, we are actually rotating!!!
  // find x
  x = (REAR_OFFSET-FRONT_OFFSET)/(front_slope-rear_slope);
  // find y
  y = front_slope*x+FRONT_OFFSET;
}

void setup(){
  Serial.begin(9600);
  while(!Serial); 

  mouseF.mouse_init();
  mouseR.mouse_init();

  Timer1.initialize(100000); //time is in MICROseconds!!! (10^-9)
  Timer1.attachInterrupt(read_mice);
}

void loop(){
  #ifdef PRINTMOTION
  //print basic motion data to serial.
  Serial.println(statF, BIN);
  Serial.print("Front:");
  Serial.print("\tx=");
  Serial.print(front_x_posn, DEC);
  Serial.print("\ty=");
  Serial.print(position.front.y, DEC);
  Serial.print("\tRear:");
  Serial.print("\tx=");
  Serial.print(rear_x_posn, DEC);
  Serial.print("\ty=");
  Serial.print(position.rear.y, DEC);
  
  Serial.println();
  #endif

  #ifdef PRINTIC
  //find the IC and print to serial.
  float x, y;
  findIC(front_data, rear_data, x, y);
  Serial.print("Instant center position: x=");
  Serial.print(x, DEC);
  Serial.print("\ty=");
  Serial.print(y, DEC);
  Serial.println();
  #endif

  #ifdef SERIALMONITOR
  //print only one part of the motion data to serial; to be used with the serial plotter.
  Serial.println(rear_y_posn, DEC);
  #endif

  delay(1000);
}
