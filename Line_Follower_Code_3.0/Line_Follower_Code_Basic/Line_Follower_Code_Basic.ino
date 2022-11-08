/* ************************************************************************************************* */
// UCSD ECE 5 Lab 4 Code: Line Following Robot with PID 
// V 3.0
// Last Modified 11/8/2022 by MingWei Yeoh
/* ************************************************************************************************* */

/*
   This is code for your PID controlled line following robot.
   The code is divided into different tabs to better divide the code.

   ******      Code Table of Contents      ******

  - Line_Follower_Code_Karcher
   > Declare libraries     - declares global variables so each variable can be accessed from every function
   > Declare Pins          - where the user sets what pin everything is connected to 
   > Settings              - settings that can improve robot functionality and help to debug
   > Setup (Main)          - runs once at beginning when you press button on arduino or when you open serial monitor
   > Loop  (Main)          - loops forever calling on a series of function
   
  - Calibration 
   > Main Calibrate()      - runs calibration function calls and synchronizes calibration state with different led animations
  
  - Helper_Functions
   > setLEDs               - turns on all LEDs in the LED_Pin array on or off
   > Read Potentiometers   - reads each potentiometer
   > Read Photoresistors   - reads each photoresistor
   > Run Motors            - runs motors
   > Calculate Error       - calculate error from photoresistor readings
   > PID Turn              - takes the error and implements PID control
   > determineRogueRobot() - checks each photoresistor. If all of them are reading a dark color we know the robot is off the track
   > Print                 - used for printing information but should disable when not debugging because it slows down program

*/

// Include files needed
#include <L298NX2.h>

// ************************************************************************************************* //
// ****** DECLARE PINS HERE  ****** 

// Taken from LEFT TO RIGHT of the robot ****** Orient yourself so that you are looking from the rear of the robot (photoresistors are farthest away from you, wheels are closest to you)

//                  Left Motors   Right motors 
L298NX2 DriveMotors(  2, 3, 4,      7, 5, 6);
//                 ENA, IN1, IN2, ENB, IN3, IN4

enum side {LEFT, RIGHT};

int LDR_Pin[] = {A8, A9, A10, A11, A12, A13, A14}; // SET PINS CONNECTED TO PHOTORESISTORS // FROM LEFT TO RIGHT OF THE ROBOT, ROBOT IS ORIENTED WHERE PHOTORESISOTRS FARTHEST FROM YOU AND WHEELS ARE CLOSEST TO YOU      

// Potentiometer Pins
const int S_pin = A3; // Pin connected to Speed potentiometer
const int P_pin = A2; // Pin connected to P term potentiometer
const int I_pin = A1; // Pin connected to I term potentiometer
const int D_pin = A0; // Pin connected to D term potentiometer
                                                                 
int led_Pins[] = {41,43,45,47,49,51,53};  // LEDs to indicate what part of calibration you're on and to illuminate the photoresistors

// ************************************************************************************************* //
// Change Robot Settings here

#define PRINTALLDATA        0  // Prints ALL the data, Could be useful for debugging =)
#define NOMINALSPEED        30 // This is the base speed for both motors, can also be increased by using potentiometers

// ************************************************************************************************* //
// Variable declarations

//Variables for PID Calculations
int MxRead, MxIndex, CriteriaForMax;
int leftHighestPR, highestPResistor, rightHighestPR;
float AveRead, WeightedAve;

int M1SpeedtoMotor, M2SpeedtoMotor;

int SpRead = 0; // speed increase
int kPRead = 0; // proportional gain
int kIRead = 0; // integral gain
int kDRead = 0; // derivative gain

// Variables for Calibration
float Mn[20]; 
float Mx[20];
float LDRf[20];
int LDR[20];    
int rawPResistorData[20];  
int totalPhotoResistors = sizeof(LDR_Pin) / sizeof(LDR_Pin[0]);  
int numLEDs = sizeof(led_Pins) / sizeof(led_Pins[0]);    

// For Motor Control
int Turn, M1P = 0, M2P = 0;
float error, lasterror = 0, sumerror = 0;
float kP, kI, kD;

// ************************************************************************************************* //
// setup - runs once
void setup() {
  Serial.begin(9600);                            // For serial communication set up

  for (int i = 0; i < numLEDs; i++)
    pinMode(led_Pins[i], OUTPUT);                // Initialize all LEDs to output
 
  for (int i = 0; i < totalPhotoResistors; i++)
    pinMode(LDR_Pin[i], INPUT_PULLUP);           // Initialize all Photoresistors to INPUT_PULLUP
  
  Calibrate();                                   // Calibrate black and white sensing

  ReadPotentiometers();                          // Read potentiometer values (Sp, P, I, & D)
  
} // end setup()

// ************************************************************************************************* //
// loop - runs/loops forever
void loop() {

  ReadPotentiometers(); // Read potentiometers

  ReadPhotoResistors(); // Read photoresistors 

  CalcError();          // Calculates error

  PID_Turn();           // PID Control and Output to motors to turn

  RunMotors();        // Runs motors
  
  if (PRINTALLDATA)     // If PRINTALLDATA Enabled, Print all the data
    Print();         
  
} // end loop()

// ************************************************************************************************* //
// function to calibrate

void Calibrate() {

  int numberOfMeasurements = 15;                // set number Of Measurements to take

  CalibrateHelper(numberOfMeasurements, false); // White Calibration

  setLeds(0);                                   // Turn off LEDs to indicate user to calibrate other color
  delay(2000);
  
  CalibrateHelper(numberOfMeasurements, true);  // Black Calibration

  Serial.print("Black Vals:  ");
  for (int i = 0; i < totalPhotoResistors; i++)
    Serial.print(String(Mx[i]) + " ");          // Print the Black values that will be used by the robot
  Serial.println();

  Serial.print("White Vals:  ");
  for (int i = 0; i < totalPhotoResistors; i++)
    Serial.print(String(Mn[i]) + " ");          // Print the White values that will be used by the robot
  Serial.println();

  Serial.print("Delta Vals:  ");
  for (int i = 0; i < totalPhotoResistors; i++)
    Serial.print(String(Mx[i] - Mn[i]) + " ");  // Print the Difference between the White and Black valuess
  Serial.println();

  setLeds(1);                                   // Turn LEDs on
  delay(2000);

} // end Calibrate()

void CalibrateHelper(int numberOfMeasurements, boolean ifCalibratingBlack) {
  
  if (ifCalibratingBlack)
    Serial.println("\nCalibrating Black");
  else
    Serial.println("\nCalibrating White");
  if (ifCalibratingBlack) {               // Indicate that calibration is starting
    for (int i = 0; i < 4; i++) {
      setLeds(1); // turn the LEDs on
      delay(250); // wait
      setLeds(0); // turn the LEDs off
      delay(250); // wait 
    }
  }

  setLeds(1);
  delay(250);

  for (int i = 0; i < numberOfMeasurements; i++) {
    for (int pin = 0; pin < totalPhotoResistors; pin++) {
      LDRf[pin] = LDRf[pin] + (float)analogRead(LDR_Pin[pin]);
      delay(2);
    }
    Serial.print(". ");
  }
  for (int pin = 0; pin < totalPhotoResistors; pin++) {
    if (ifCalibratingBlack) {                                   // updating cooresponding array based on if we are calibrating black or white
      Mx[pin] = round(LDRf[pin] / (float)numberOfMeasurements); // take average and store for black
    }
    else {
      Mn[pin] = round(LDRf[pin] / (float)numberOfMeasurements); // take average and store for white
    }
    LDRf[pin] = 0.0;
  }

  Serial.println(" Done!");
  setLeds(0);
  delay(250);
}

// Set all LEDs to a certain brightness
void setLeds(int x) {
  for (int i = 0; i < numLEDs; i++)
    digitalWrite(led_Pins[i], x);
}

// ************************************************************************************************* //
// function to read and map values from potentiometers
void ReadPotentiometers() {
  // Call on user-defined function to read Potentiometer values
  SpRead = ReadPotentiometerHelper(S_pin, 0, 1023, 0, 100); // We want to read a potentiometer for S_pin with resolution from 0 to 1023 and potentiometer range from 0 to 100.
  kPRead = ReadPotentiometerHelper(P_pin, 0, 1023, 0, 100); // We want to read a potentiometer for P_pin with resolution from 0 to 1023 and potentiometer range from 0 to 100.
  kIRead = ReadPotentiometerHelper(I_pin, 0, 1023, 0, 100); // We want to read a potentiometer for I_pin with resolution from 0 to 1023 and potentiometer range from 0 to 100.
  kDRead = ReadPotentiometerHelper(D_pin, 0, 1023, 0, 100); // We want to read a potentiometer for D_pin with resolution from 0 to 1023 and potentiometer range from 0 to 100.

} // end ReadPotentiometers()

int ReadPotentiometerHelper(int pin, int min_resolution, int max_resolution, int min_potentiometer, int max_potentiometer) {
  return map(analogRead(pin), min_resolution, max_resolution, min_potentiometer, max_potentiometer);
}

// ************************************************************************************************* //
// function to start motors using nominal speed + speed addition from potentiometer
void RunMotors() {
  setLeds(1);

  M1SpeedtoMotor = min(NOMINALSPEED + SpRead + M1P, 255); // limits speed to 255
  M2SpeedtoMotor = min(NOMINALSPEED + SpRead + M2P, 255); // remember M1Sp & M2Sp is defined at beginning of code (default 60)
  
  runMotorAtSpeed(LEFT, M2SpeedtoMotor); // run right motor 
  runMotorAtSpeed(RIGHT, M1SpeedtoMotor); // run left motor
  
} // end RunMotors()

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

// ************************************************************************************************* //
// Function to read photo resistors and map from 0 to 100
void ReadPhotoResistors() {
  for (int Li = 0; Li < totalPhotoResistors; Li++) 
    LDR[Li] = map(analogRead(LDR_Pin[Li]), Mn[Li], Mx[Li], 0, 100);

  for (int i = 0; i < totalPhotoResistors; i++)
    rawPResistorData[i] = analogRead(LDR_Pin[i]);

} // end ReadPhotoResistors()

// ************************************************************************************************* //
// Calculate error from photoresistor readings
void CalcError() {
  MxRead = -99;
  AveRead = 0.0;
  for (int i = 0; i < totalPhotoResistors; i++) { // This loop goes through each photoresistor and stores the photoresistor with the highest value to the variable 'highestPResistor'
    if (MxRead < LDR[i]) {
      MxRead = LDR[i];
      MxIndex = -1 * (i - 3);
      highestPResistor = (float)i;
    }
    AveRead = AveRead + (float)LDR[i] / (float)totalPhotoResistors;
  }
  CriteriaForMax = 2.5;

  if (MxRead > CriteriaForMax * AveRead) { // Make sure that the highestPResistor is actually "seeing" a line. What happens if there is no line and we take the photoresistor that happens to have the highest value?

    // Next we assign variables to hold the index of the left and right Photoresistor that has the highest value, though we have to make sure that we aren't checking a Photoresistor that doesn't exist.
    // Ex: To the left of the left most photoresistor or the right of the right most photoresistor
    if (highestPResistor != 0)
      leftHighestPR = highestPResistor - 1;
    else
      leftHighestPR = highestPResistor;

    if (highestPResistor != totalPhotoResistors - 1)
      rightHighestPR = highestPResistor + 1;
    else
      rightHighestPR = highestPResistor;

    // Next we take the percentage of "line" each of our left, middle, and right photoresistors sees and then we take the average, which is our error calculation
    float numerator = (float)(LDR[leftHighestPR] * leftHighestPR) + (float)(LDR[highestPResistor] * highestPResistor) + (float)(LDR[rightHighestPR] * rightHighestPR);
    float denominator = (float)LDR[leftHighestPR] + (float)LDR[highestPResistor] + (float)LDR[rightHighestPR];

    WeightedAve = ((float)numerator) / denominator;

    error = (WeightedAve - totalPhotoResistors/2);
  }
  
} // end CalcError()

// ************************************************************************************************* //
// PID Function
void PID_Turn() {
  kP = (float)kPRead * 1.;    // each of these scaling factors can change depending on how influential you want them to be
  kI = (float)kIRead * 0.001;
  kD = (float)kDRead * 0.01;

  Turn = error * kP + sumerror * kI + (error - lasterror) * kD; // PID!!!!!!!!!!!!!

  if (sumerror > 5)   // prevents integrator wind-up
    sumerror = 5;
  else if (sumerror < -5)
    sumerror = -5;

  if (error == 0)     // Reset sumerror if line is centered
    sumerror = 0;

  if (Turn < 0) {
    M1P = -Turn;       // One motor becomes slower and the other faster
    M2P = Turn;
  }
  else if (Turn > 0) {
    M1P = -Turn;
    M2P = Turn;
  }
  else {
    M1P = 0;
    M2P = 0;
  }

  lasterror = error;
  sumerror = sumerror + error;

} // end PID_Turn()

// ************************************************************************************************* //
// function to print values of interest
void Print() {
  Serial.print(" Sp: " + String(SpRead) + " P: " + String(kP) + " I: " + String(kI) + " D: " + String(kD) + "  PResistor Val : "); // Prints PID settings

  for (int i = 0; i < totalPhotoResistors; i++) { // Printing the photo resistor reading values one by one
    Serial.print(rawPResistorData[i]);
    Serial.print(" ");
  }

  Serial.print(" Error: " + String(error));      // this will show the calculated error (-3 through 3)

  Serial.println("  LMotor:  " + String(M1SpeedtoMotor) + "  RMotor:  " + String(M2SpeedtoMotor));    // This prints the arduino output to each motor so you can see what the values are (0-255)

  delay(100);                                    // just here to slow down the output for easier reading. Don't comment out or else it'll slow down the processor on the arduino

} // end Print()

