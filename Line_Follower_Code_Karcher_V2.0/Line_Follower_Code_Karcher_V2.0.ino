/* ************************************************************************************************* */
// UCSD ECE 5 Lab 4 Code: Line Following Robot with PID 
// V 2.0
// Last Modified 8/12/2022 by MingWei Yeoh
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
   > EEPROMCalibration     - eliminates the need to calibrate on startup each time since it takes advantage of the built in memory on the arduino (EEPROM)
   
  - Helper_Functions
   > setLEDs               - turns on all LEDs in the LED_Pin array on or off
   > Read Potentiometers   - reads each potentiometer
   > Read Photoresistors   - reads each photoresistor
   > Run Motors            - runs motors
   > Calculate Error       - calculate error from photoresistor readings
   > PID Turn              - takes the error and implements PID control
   > determineRogueRobot() - checks each photoresistor. If all of them are reading a dark color we know the robot is off the track
   > Print                 - used for printing information but should disable when not debugging because it slows down program

  - Sanity_Check
   > Check for unconnected Photoresistors
   > Check for unsensitive Photoresistors 
   > Check for incorrect calibration order

*/

// Include files needed
#include <Wire.h>
#include <EEPROM.h>
#include <L298NX2.h>
#include <FastLED.h>
#include "LEDAnimations.h"
#include <IBusBM.h>

IBusBM IBus; 


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

#define CALIBRATEEVERYTIME  0  // Set to 0 to take advantage of the built in memory (Turn on robot and dont move it, it will read values from memory). You can still calibrate at any time to store new values.
#define DETECTFALLOFFBOARD  1  // Detects robot falling off the board and immediately stops all motors
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
int rogueRobotCount = 0;

// ************************************************************************************************* //
// setup - runs once
void setup() {
  Serial.begin(9600);                            // For serial communication set up
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness(  BRIGHTNESS );

  IBus.begin(Serial1);  // iBUS connected to pin 19

  for (int i = 0; i < numLEDs; i++)
    pinMode(led_Pins[i], OUTPUT);                // Initialize all LEDs to output
 
  for (int i = 0; i < totalPhotoResistors; i++)
    pinMode(LDR_Pin[i], INPUT_PULLUP);           // Initialize all Photoresistors to INPUT_PULLUP

  fill_rainbow(leds, NUM_LEDS, 0, 3);
  FastLED.show();
  delay(1000);
  static uint8_t starthue = 0;
  for (int x = 0; x < 500; x++) {
    fill_rainbow( leds, NUM_LEDS, ++starthue, 4);
    FastLED.show();
  }

  fill_solid(leds, NUM_LEDS, CRGB ::Green);
  FastLED.show();
  
  Calibrate();                                   // Calibrate black and white sensing

  ReadPotentiometers();                          // Read potentiometer values (Sp, P, I, & D)

  sanityCheck();                                 // Determines Robot Faults

} // end setup()

// ************************************************************************************************* //
// loop - runs/loops forever
void loop() {

  ReadPotentiometers(); // Read potentiometers

  ReadPhotoResistors(); // Read photoresistors 

  CalcError();          // Calculates error

  PID_Turn();           // PID Control and Output to motors to turn

  if (DETECTFALLOFFBOARD && determineRogueRobot())
    haltMotors();       // Only if DETECTFALLOFFBOARD is enabled AND Robot is off the track will we halt the motors.
  else
    RunMotors();        // Runs motors
  
  if (PRINTALLDATA)     // If PRINTALLDATA Enabled, Print all the data
    Print();         

  fill_palette(leds, NUM_LEDS, hue, 255 / NUM_LEDS, currentpal, 255, LINEARBLEND);
  EVERY_N_MILLISECONDS(10) hue++;
  FastLED.show();
  
} // end loop()
