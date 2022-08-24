/* Motor Driver Code UCSD ECE 5 Lab 4*/

// Include the Motor driver library
#include <L298NX2.h>

enum side {LEFT, RIGHT};

// Initialize Motors

// Taken from LEFT TO RIGHT of the robot ****** Orient yourself so that you are looking from the rear of the robot (photoresistors are farthest away from you, wheels are closest to you)
// Create in stance of motors

//                  Left Motors   Right motors 
L298NX2 DriveMotors(  2, 3, 4,      7, 5, 6);
//                 ENA, IN1, IN2, ENB, IN3, IN4

//Set Initial Speed of Motors
int speed = 60;

// A function that commands a specified motor to move towards a given direction at a given speed
void runMotorAtSpeed(side _side, int speed) {

  if (_side == LEFT) {
    DriveMotors.setSpeedA(abs(speed));
    if (speed > 0)                // swap direction if speed is negative
      DriveMotors.forwardA();           // sets the direction of the motor from arguments
    else
      DriveMotors.backwardA();          // sets the direction of the motor from arguments
  }
  if (_side == RIGHT) {
    DriveMotors.setSpeedB(abs(speed));
    if (speed > 0)                // swap direction if speed is negative
      DriveMotors.forwardB();           // sets the direction of the motor from arguments
    else
      DriveMotors.backwardB();          // sets the direction of the motor from arguments
  }
}

// setup - runs once
void setup() {
  delay(1500); // gives you a moment before tank actually moves 
}

// loop - loops forever
void loop() {

  // Start Motors in forward direction
  runMotorAtSpeed(LEFT, speed);
  runMotorAtSpeed(RIGHT, speed);
  delay(3000); // let run forward for 3 seconds

  // Start Motors in backward direction
  runMotorAtSpeed(LEFT, -speed); /** FIX ME: set motor 1 at M1Sp in the BACKWARD direction (HINT: Very similar to forward direction) **/
  runMotorAtSpeed(RIGHT, -speed); /** FIX ME: set motor 2 at M2Sp in the BACKWARD direction **/
  
  delay(3000); // let run backward for 3 seconds

  // Stop Motors
  runMotorAtSpeed(LEFT, 0); /** FIX ME: stop motor 1 **/
  runMotorAtSpeed(RIGHT, 0); /** FIX ME: stop motor 2 **/
  delay(3000); // stop for 3 seconds
}
