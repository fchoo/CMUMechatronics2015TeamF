

/*======================================
=            Serial Control            =
======================================*/

int getSerial()
{
  int serialData = 0;
  int aChar = 0;
  while (aChar != '/')
  {
    aChar = Serial.read();
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
    if (cmd == 'j') // Switch between joystick and serial
    {
      isJoyStick = !isJoyStick;
      if (isJoyStick) isPathfind = false;
      Serial.print("[INFO] Mode: ");
      if (isJoyStick) Serial.println("JoyStick");
      else Serial.println("Serial");
    }
    else if (cmd == 'z') // Activate pathfinding
    {
      isPathfind = !isPathfind;
      if (isPathfind) isJoyStick = false;
      Serial.print("[INFO] Mode: ");
      if (isPathfind) Serial.println("Pathfinding");
      else Serial.println("Serial");
    }
    else if (cmd == 'r')
    {
      state = LEFTU_NEXT;
      encoderFlag = false;
      irFlag = false;
      targetDist = 0;
      n_tick = 0;
    }
  }
  if (cmd == 'a') // counter clockwise
  {
    // Serial.println("[INFO] Move left");
    moveLeft();
  }
  else if (cmd == 'd') // clockwise
  {
    // Serial.println("[INFO] Move right");
    moveRight();
  }
  else if (cmd == 'w') // forward
  {
    // Serial.println("[INFO] Move forward");
    moveForward();
  }
  else if (cmd == 's') // backwards
  {
    // Serial.println("[INFO] Move back");
    moveBack();
  }
  else if (cmd == 'q') // stop motor
  {
    // Serial.println("[INFO] Stop");
    stop();
  }
}
