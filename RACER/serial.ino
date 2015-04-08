/******************************************************************************
 * (18-578 / 16-778 / 24-778) MECHATRONIC DESIGN
 * TEAM F: [RACER]
 * MEMBERS:
 *          - CHOO, FOO LAI
 *          - EREBOR, TELSON
 *          - FLAREAU, JOSHUA
 *          - KALOUCHE, SIMON
 *          - TAN, NICHOLAS
 *
 * LAST REVISION: 04/03/2015
 *
 * Serial Control. Include parsing of serial communication.
 *
 *****************************************************************************/


int getSerial()
{
  boolean negative = false;
  int serialData = 0;
  int aChar = 0;
  while (aChar != '/')
  {
    aChar = Serial.read();
    if (aChar == '-') // Current read is negative
      negative = true;
    else if (aChar >= '0' && aChar <= '9')
      serialData = serialData * 10 + aChar - '0';
    else if (aChar >= 'a' && aChar <= 'z')
      serialData = aChar;
  }
  if (negative)
    serialData *= -1;
  return serialData;
}

void printSerialInst()
{
  Serial.println("****************************** Serial Mode ******************************");
  Serial.println("Serial instruction format is \'*/\'. The commands are as followed:");
  Serial.println("Mode(s)");
  Serial.println("\tz: Pathfinding mode");
  Serial.println("\tt: Diagnostic mode");
  Serial.println("Sensor control(s)");
  Serial.println("\tr: Reset IMU");
  Serial.println("Motor Control(s)");
  Serial.println("\tp: Step EDF to a value. Format: \'p/edf_id/edf_val/\' (edf_id <- [0:1], edf_val <- [0:255]");
  Serial.println("\tw: Move forward");
  Serial.println("\ta: Turn left");
  Serial.println("\ts: Move backward");
  Serial.println("\td: Turn right");
  Serial.println("h: Print serial instructions");
  Serial.println("-------------------------------------------------------------------------");
}

void serialControl()
{
  // Serial comm to control robot
  if (Serial.available()>0) // Read cmd
  {
    cmd = getSerial();
    if (cmd == 'z') // Activate pathfinding
    {
      isPathfind = !isPathfind;
      Serial.print("[INFO] Mode: ");
      if (isPathfind) Serial.println("Pathfinding");
      else Serial.println("Serial");
    }
    else if (cmd == 'r')
    {
      state = LEFTU_NEXT;
      irFlag = false;
      targetDist = 0;
      rstIMU();
    }
    else if (cmd == 'p') // Stepping to a certain value
    {
      edf_id = getSerial(); // get id
      // set pwm value
      if (edf_id == 1)
        edf_1_mval = getSerial();
      else if (edf_id == 2)
        edf_2_mval = getSerial();
      // EDF 1
      while (edf_1_val < edf_1_mval)
        step_PWM(1,1);
      while (edf_1_val > edf_1_mval)
        step_PWM(1,-1);
      // EDF 2
      while (edf_2_val < edf_2_mval)
        step_PWM(2,1);
      while (edf_2_val > edf_2_mval)
        step_PWM(2,-1);
    }
    else if (cmd == 't')
    {
      isKilled = true;
      isDiagnostic = true;
      printDiagInst();
      test_id = 0;
    }
    else if (cmd == 'h')
      printSerialInst();
  }
  // Motor commands
  if (cmd == 'a') // counter clockwise
    moveLeft();
  else if (cmd == 'd') // clockwise
    moveRight();
  else if (cmd == 'w') // forward
    moveForward();
  else if (cmd == 's') // backwards
    moveBack();
  else if (cmd == 'q') // stop motor
    stop();
}
