#include <Stepper.h>

// Define number of steps per rotation:
int stepsperrev = 2048;
// Max levels for blinder (reaching 1 for delay of 10 ms and speed of 10rpms takes 6s)
//which means limit of 1 allows for 6 full revolutions.
const int upper_max = 1;
const int upper_min = -1;
// up, down, stop modes:
const int up_code=(int)'u'+(int)'p';
const int down_code=(int)'d'+(int)'n';
const int stop_code=(int)'s'+(int)'t';
int code=stop_code;
int steps=0;
//define the command array:
char cmd[4]={0,0,0,0};
// Create stepper object called 'stepper_m', note the pin order:
Stepper stepper_m = Stepper(stepsperrev, 8, 10, 9, 11);


// Wiring:
// Pin 8 to IN1 on the ULN2003 driver
// Pin 9 to IN2 on the ULN2003 driver
// Pin 10 to IN3 on the ULN2003 driver
// Pin 11 to IN4 on the ULN2003 driver

void setup() {
  // Begin Serial communication at a baud rate of 9600:
  Serial.begin(9600);
}

void loop() {
  //do not move at the start of the program
   stepsperrev = 0;
   
  //read command:
if (Serial.available() > 0) {
    delay(10);
    short i=0;
    while(Serial.available() > 0 && i < 4) 
    {
      cmd[i] = Serial.read();
      i++;
    }
  }
code = (int)cmd[0] + int(cmd[1]);

    //stop when max is reached
  if (steps >= upper_max && code!=down_code){
    code = stop_code;
  }
    if (steps <= upper_min && code!=up_code){
    code = stop_code;
    }


  switch (code){
   case up_code:
      steps++;
      stepsperrev = 2048;
     break;
     
     case down_code:
      steps--;
      stepsperrev = -2048;
      break;
        
     case stop_code:
     stepsperrev = 0;
     break;
     
  default:
  break;
 }
 stepper_m.step(stepsperrev);
    // Set the speed to 10 rpm:
  stepper_m.setSpeed(10);
  delay(10);
} 
