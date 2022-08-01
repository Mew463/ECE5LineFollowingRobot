// ************************************************************************************************* //
// sanityCheck - runs once at the end of setup() to see whether robot's photoresistors are misbehaving

int RobotFault = 0;

void sanityCheck() {
  for (int i = 0; i < totalPhotoResistors; i ++) {
    ReadPhotoResistors();
    
    if (rawPResistorData[i] > 1000) {
      Serial.println("Photoresistor A" + String(LDR_Pin[i] - 54) + " has a reading over 1000. It is either unplugged or not working.");
      RobotFault = 1;
    }
    else if (abs(Mx[i] - Mn[i]) < 50) {
      Serial.println("Photoresistor A" + String(LDR_Pin[i] - 54) + " has a difference between black and white of less than 50, consider replacing it or recalibrating.");
      RobotFault = 1;
    } 
    else if (Mx[i] - Mn[i] < 0) {
      Serial.println("Photoresistor A" + String(LDR_Pin[i] - 54) + " has a negative difference value when it should be positive. Calibrate White first and then Black");
      RobotFault = 1;
    }
    else if (abs(Mx[i] - Mn[i]) < 100) 
      Serial.println("WARNING: Photoresistor A" + String(LDR_Pin[i] - 54) + " has a difference between black and white of less than 100, consider adding a light source, raising your photoresistors from the ground, or adding shielding");
  }
  
  if (RobotFault != 0) {
    Serial.println(" ====== Robot Haulted, Error detected ====== ");
    Motor1->setSpeed(0);
    Motor2->setSpeed(0);
    unsigned long prevTime = 0;
    int ledState = 0;
    while (1) {
      if (millis() - prevTime > 500) {
        ledState = !ledState;
        setLeds(ledState);
        prevTime = millis();
      }
      
      #ifdef RGBLEDSTRIP
        currentpal = Error_Animation;
        updateLEDStrip();
        delay(7);
      #endif
    }
  }
  else
    Serial.println(" ====== All Checks Passed, Robot is good to go! ====== ");
}











// Good luck in the competition!  =)
