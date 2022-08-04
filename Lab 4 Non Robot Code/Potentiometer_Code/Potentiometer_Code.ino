/* Potentiometer Code UCSD ECE 5 Lab 4*/
/* Declare Variables for Potentiometer */
const int S_pin = A0; // proportional - analog pin 0
const int P_pin = A1; // proportional - analog pin 1
const int I_pin = A2; // integral - analog pin 2
const int D_pin = A3; // derivative - analog pin 3
int Sp = 0;           // speed gain coeficient
int kP = 0;           // proportional gain coeficient
int kI = 0;           // integral gain coeficient
int kD = 0;           // derivative gain coeficient

void setup() { /* Setup - runs once (when power is supplied or after reset) */

  Serial.begin(9600); // For serial communication set up
}

void loop() { /* Loop - loops forever (until unpowered or reset) */

  // Call on user-defined function to read Potentiometer values
  Sp = /* FIX ME, replace this comment with actual function name */ (S_pin, 0, 1023, 0, 100);
  kP = /* FIX ME, replace this comment with actual function name */ (P_pin, 0, 1023, 0, 100);
  kI = /* FIX ME, replace this comment with actual function name */ (I_pin, 0, 1023, 0, 100);
  kD = /* FIX ME, replace this comment with actual function name */ (D_pin, 0, 1023, 0, 100);

  Print(); // Call on user-defined function to print values from potentiometers
}

// ************************************************************************************************* //
// function to read and map values from potentiometers
int ReadPotentiometer(int pin, int min_resolution, int max_resolution, int min_potentiometer, int max_potentiometer) {
  return map(analogRead(pin), min_resolution, max_resolution, min_potentiometer, max_potentiometer);
}

// ************************************************************************************************* //
// function to print values of interest
void Print() {

  Serial.print(Sp);
  Serial.print(" ");
  Serial.print(kP);
  Serial.print(" ");
  Serial.print(kI);
  Serial.print(" ");
  Serial.println(kD);

  delay(200); //just here to slow down the output for easier reading if desired
}
