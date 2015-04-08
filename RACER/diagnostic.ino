void diagnosticCheck()
{
  // Still read all sensors
  readIR();
  readIMU();
  updateAngles();
  // Serial comm to control robot
  if (Serial.available()>0) // Read cmd
  {
    test_id = getSerial();
    if (test_id == 0) printDiagInst();
    else if (test_id == 1)
    {
      leftWheelTicks = 0;
      rightWheelTicks = 0;
    }
    else if (test_id == 4)
      Serial.println("Blinking LEDs in series.");
  }
  if (test_id == -1)
  {
    isKilled = false;
    isDiagnostic = false;
    cmd = 'q';
    test_id = 0;
    printSerialInst();
  }
  if (test_id == 1) testEncoder();
  else if (test_id == 2) testIMU();
  else if (test_id == 3) testIR();
  else if (test_id == 4) testLED();
  else if (test_id == 5) testMotors();
  else if (test_id == 6) testSwitch();
}

void printDiagInst()
{
  Serial.println("****************************** Serial Mode ******************************");
  Serial.println("Serial instruction format is \'*/\'. The test numbering is as followed:");
  Serial.println("1: Encoder Test");
  Serial.println("2: IMU Test");
  Serial.println("3: IR Test");
  Serial.println("4: LED Test");
  Serial.println("5: Motors Test");
  Serial.println("6: Switch Test");
  Serial.println("0: Print Instruction");
  Serial.println("-1: Exit Diagnostic Mode");
  Serial.println("-------------------------------------------------------------------------");
}

void testSwitch()
{
  Serial.print("Kill Switch: ");
  Serial.print(digitalRead(PIN_KILL));
  Serial.print(" Pathfinding Button: ");
  Serial.print(digitalRead(PIN_PATHFIND));
  Serial.print(" IMU Button: ");
  Serial.println(digitalRead(PIN_RSTIMU));
}

// Encoders
void testEncoder()
{
  Serial.print("Encoder Ticks: ");
  Serial.println(rightWheelTicks);
}

void testIMU()
{
  Serial.print(" Roll: ");
  Serial.print(roll);
  Serial.print(" Pitch: ");
  Serial.print(pitch);
  Serial.print(" Yaw: ");
  Serial.println(getYaw());
}

void testIR()
{
  Serial.print("IR Distance: ");
  Serial.println(irDist);
}

void testLED()
{
  // put your main code here, to run repeatedly:
  digitalWrite(PIN_GREEN1, HIGH);
  delay(500);
  digitalWrite(PIN_GREEN2, HIGH);
  delay(500);
  digitalWrite(PIN_BLUE, HIGH);
  delay(500);
  digitalWrite(PIN_RED1, HIGH);
  delay(500);
  digitalWrite(PIN_RED2, HIGH);
  delay(500);
  digitalWrite(PIN_RED1, LOW);
  digitalWrite(PIN_RED2, LOW);
  digitalWrite(PIN_BLUE, LOW);
  digitalWrite(PIN_GREEN1, LOW);
  digitalWrite(PIN_GREEN2, LOW);
  delay(500);
}

void testMotors()
{
  Serial.println("Testing moving forward.");
  moveForward();
  delay(1000);
  stop();
  delay(50);

  Serial.println("Testing moving backward.");
  moveBack();
  delay(1000);
  stop();
  delay(50);

  Serial.println("Testing turning left.");
  moveLeft();
  delay(1000);
  stop();
  delay(50);

  Serial.println("Testing turning right.");
  moveRight();
  delay(1000);
  stop();
  delay(50);
}
