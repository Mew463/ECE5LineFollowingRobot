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
// See if robot fell off track
int determineRogueRobot() {
  int count = 0;
  for (int i = 0; i < totalPhotoResistors; i++)
    if (rawPResistorData[i] > Mx[i] - (Mx[i] - Mn[i])/2 ) // Check each photoresistor to see whether they are reading a dark color
      count++;

  if (count == totalPhotoResistors)       // If ALL the photoresistors are seeing a "black" line, we know it is going rogue
    rogueRobotCount++;
  else
    rogueRobotCount = 0;

  if (rogueRobotCount > 5){               // Check 5 more times just to be sure
    #ifdef RGBLEDSTRIP
        currentpal = Off_Board_Animation;
    #endif
    return 1;
  }
  else {
    #ifdef RGBLEDSTRIP
        currentpal = LineTracking_Animation;
    #endif
    return 0;
  }
}

void haltMotors() {
  static int ledState = 0;

  runMotorAtSpeed(LEFT, 0);
  runMotorAtSpeed(RIGHT, 0);
  
  static unsigned long prevTime;
  if (millis() - prevTime > 150) {
        ledState = !ledState;
        setLeds(ledState);
        Serial.println("Robot is off the board!");
        prevTime = millis();
  }
}

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
