// ************************************************************************************************* //
// function to calibrate

void Calibrate() {

  int numberOfMeasurements = 15;                // set number Of Measurements to take

  CalibrateHelper(numberOfMeasurements, false); // White Calibration

  setLeds(0);                                   // Turn off LEDs to indicate user to calibrate other color
  delay(2000);
  
  CalibrateHelper(numberOfMeasurements, true);  // Black Calibration

  if (!CALIBRATEEVERYTIME)                      // If calibrate every time is enabled skip this next line 
    EEPROMCalibration();                        // Decide whether to recall data from memory or save data to memory. Elminates the need to recalibrate EVERY. SINGLE. TIME.
  
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

void EEPROMCalibration() {
  if (abs(Mn[3] - Mx[3]) > 50) {    // Only write to EEPROM memory if there is a big difference in the middle photoresistor value (like you actually calibrated robot vs left it stationary and didn't calibrate)
    Serial.println("Saving to memory...");
    for (int i = 0; i < totalPhotoResistors*2; i++) {
      setLeds(0);
      if (i < totalPhotoResistors)
        EEPROM.write(i, Mx[i] / 4);
      else
        EEPROM.write(i, Mn[i - totalPhotoResistors] / 4);
      delay(200);
    }
  }
  else {
    Serial.println("Reading from memory...");
    setLeds(1);
    for (int i = 0; i < totalPhotoResistors*2; i++) {
      if (i < totalPhotoResistors)
        Mx[i] = (EEPROM.read(i) * 4);
      else
        Mn[i - totalPhotoResistors] = (EEPROM.read(i) * 4);
      delay(125);
    }
  }
}

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
