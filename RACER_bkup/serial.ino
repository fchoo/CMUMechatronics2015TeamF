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
  Serial.println("i: Set RACER to idle");
  Serial.println("\tt: Diagnostic mode");
  Serial.println("Pathfinding control(s)");
  Serial.println("\tr: Reset pathfinding states");
  Serial.println("Sensor control(s)");
  Serial.println("\tr: Reset IMU");
  Serial.println("Motor Control(s)");
  Serial.println("\tp: Step EDF to a value.");
  Serial.println("\t\tFormat: \'p/edf_id/edf_val/\'");
  Serial.println("\t\t\tedf_id: 1) Front EDF, 2) Back EDF, 3) Both EDFs");
  Serial.println("\t\t\tedf_val: 0-255");
  Serial.println("\tw: Move forward");
  Serial.println("\ta: Turn left");
  Serial.println("\ts: Move backward");
  Serial.println("\td: Turn right");
  Serial.println("\tq: Stop\n");
  Serial.println("h: Print serial instructions");
  Serial.println("-------------------------------------------------------------------------");
}

void serialControl()
{
  // Serial comm to control robot
  if (Serial.available()>0) // Read cmd
  {
    cmd = getSerial();
    if (cmd == 'r') // Reset pathfinding state
    {
      state = LEFTU_NEXT;
      isLastLap = false;
      rstIMU();
    }
    else if (cmd == 'p') // Stepping to a certain value
    {
      edf_id = getSerial(); // get id
      edf_mval = getSerial(); // get value
      edf_mval = (edf_mval < EDF_MIN) ? EDF_MIN : edf_mval; // set mval to min
      // set pwm value
      if (edf_id == 1)
        edf_1_mval = edf_mval;
      else if (edf_id == 2)
        edf_2_mval = edf_mval;
      else if (edf_id == 3)
      {
        edf_1_mval = edf_mval;
        edf_2_mval = edf_mval;
      }
    }
    else if (cmd == 't') // Diagnostic mode
    {
      isDiagnostic = true;
      printDiagInst();
      test_id = 0;
    }
    else if (cmd == 'h') // print instruction
      printSerialInst();
  }

  // Stepping EDF
  // EDF 1
  if (edf_1_val < edf_1_mval)
    step_PWM(1,1);
  if (edf_1_val > edf_1_mval)
    step_PWM(1,-1);
  // EDF 2
  if (edf_2_val < edf_2_mval)
    step_PWM(2,1);
  if (edf_2_val > edf_2_mval)
    step_PWM(2,-1);

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
