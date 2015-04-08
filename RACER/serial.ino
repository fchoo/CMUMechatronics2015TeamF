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
  int serialData = 0;
  int aChar = 0;
  while (aChar != '/')
  {
    aChar = Serial.read();
    Serial.println(aChar);
    if (aChar >= '0' && aChar <= '9')
      serialData = serialData * 10 + aChar - '0';
    else if (aChar >= 'a' && aChar <= 'z')
      serialData = aChar;
  }
  return serialData;
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
