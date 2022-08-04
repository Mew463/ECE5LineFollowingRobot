/* Motor Driver Code UCSD ECE 5 Lab 4*/

// Include the Motor driver library
#include <L298N.h>

// Initialize Motors
// Create one motor instance
L298N Motor1(8, 9, 10); // ENA, IN1, IN2

// Create another motor instance
L298N Motor2(13, 12, 11); // ENB, IN4, IN3

//Set Initial Speed of Motors
int speed = 60

//Set LED Pin
int led_Pin = 13; // can change to another digital pin and connect extra LED to me more easily seen

// A function that commands a specified motor to move towards a given direction at a given speed
void runMotorAtSpeed(L298N *motor, int speed, bool FORWARD) {
  motor->setSpeed(speed); // sets the speed of the motor from arguments
  if (FORWARD)
    motor->forward();
  else
    motor->backward();
}

// setup - runs once
void setup() {
   pinMode(led_Pin, OUTPUT); // set pin mode to output voltage
                             // Gives you a moment before tank actually moves
   for (int i = 0; i < 20; i++) {
      digitalWrite(led_Pin, HIGH); // turn the LED on (HIGH is the voltage level)
      delay(100);                  // wait for 100 milliseconds
      digitalWrite(led_Pin, LOW);  // turn the LED off by making the voltage LOW
      delay(100);                  // wait for 100 milliseconds
   }
}

// loop - loops forever
void loop() {

   // Start Motors in forward direction
   runMotorAtSpeed(Motor1, speed, true);
   runMotorAtSpeed(Motor2, speed, true);
   delay(3000); // let run forward for 3 seconds

   // Start Motors in backward direction
   /** FIX ME: set motor 1 at M1Sp in the BACKWARD direction (HINT: Very similar to forward direction) **/
   /** FIX ME: set motor 2 at M2Sp in the BACKWARD direction **/
   delay(3000); // let run backward for 3 seconds

   // Stop Motors
   /** FIX ME: stop motor 1 by using RELEASE as direction, use M1Sp for speed here **/
   /** FIX ME: stop motor 2 by using RELEASE as direction, use M2Sp for speed here **/
   delay(3000); // stop for 3 seconds
}
