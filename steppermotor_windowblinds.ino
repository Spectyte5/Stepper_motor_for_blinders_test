// Sketch to control a 28BYJ-48 stepper motor with ULN2003 driver board and Arduino UNO. 

// Include the Arduino Stepper.h library:
#include <Stepper.h>

// Define number of steps per rotation:
int steps_per_rev = 2048;


const int UPPER_MAX_POS = 20;
const int LOWER_MAX_POS = -20;

const int UP_CODE = (int)'u' + (int)'p';
const int STOP_CODE = (int)'s' + (int)'t';
const int DOWN_CODE = (int)'d' + (int)'n';
int code = STOP_CODE;

int steps = 0;
int step_level = 0;

// Stepper(ammountofsteps, pin1, pin2, pin3, pin4);
Stepper int_stepper = Stepper(steps_per_rev, 8, 10, 9, 11);

void setup() {
  // Set the speed to 5 rpm:
  int_stepper.setSpeed(50);
  
  // Begin Serial communication at a baud rate of 9600:
  Serial.begin(9600);
}
char cmd[4] = {0,0,0,0};


void loop() {
  // Read command
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

  switch (code){
   case UP_CODE:
     // Step one revolution in one direction:
      //steps_per_rev = 2048;
      //Serial.println("clockwise");
      steps = 2;
     break;
     
     case DOWN_CODE:
      // Step one revolution in the other direction:
        //Serial.println("counterclockwise");
        steps = -2;
        break;
        
     case STOP_CODE:
     steps = 0;
     
     //Serial.println("stop");
     break;
  default:
  
  break;
 }
 int_stepper.step(steps);
 step_level += steps;
 delay(10);

  if (step_level >= UPPER_MAX_POS){
    code = STOP_CODE;
  }
} 
