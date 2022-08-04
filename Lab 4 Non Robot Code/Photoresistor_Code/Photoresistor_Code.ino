/* Photoresistor Code UCSD ECE 5 Lab 4 */
/* Variables for Light Sensors*/

#define totalPhotoResistors 7 // Defining how many photoresistors are used, modify this if more or less are used

// Initialize Photo Resistor Array
int LDR[totalPhotoResistors]; // Since there are 7 photoresistors, we will number them from 0 to 6 (recall array index start with 0)

void setup() {
  Serial.begin(9600); // For serial communication set up
}

void loop() {

  /**FIX ME: Which function should we call here to read photo resistors? **/ (); // Read photoresistors and map to 0-100 based on calibration

  /**FIX ME: Which function should we call here to print out our values? **/ (); // Print values to serial monitor
}

// ************************************************************************************************* //
// function to read photo resistors
void ReadPhotoResistors() {
  int photoResistorCounter = 0;
  // looping through analog pins A8 to A14 and storing their values into our LDR array
  for (byte pin = A8; pin <= A14; pin++) {
    LDR[photoResistorCounter] = /** FIX ME: which function to put here to read analog pins? (hint: refer to 10.1 code) **/ (pin);
    photoResistorCounter++;
    delay(2);
  }
}

// ************************************************************************************************* //
// function to print values of interest
void Print() {
  for (int i = 0; i < totalPhotoResistors; i++) {
    // Printing the photo resistor reading values one by one
    Serial.print(LDR[i]);
    Serial.print(" ");
  }
  Serial.println(); // This just prints a new line, allowing the next set of readings to be on a new line

  delay(200); //just here to slow down the output for easier reading if desired
}
