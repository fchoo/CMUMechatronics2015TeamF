
/*===============================
=            SENSORS            =
===============================*/
void updateAngles()
{
  roll = getRoll();
  pitch = getPitch();
}

void checkHeading()
{
  if (isVert) // Only set headings when robot is vertical
  {
    if (pitch >= PITCH_N) curDir = NORTH;
    else if (pitch <= PITCH_S) curDir = SOUTH;
    else if (abs(pitch) <= PITCH_EW)
    {
      if (roll >= ROLL_E) curDir = EAST;
      else if (roll <= ROLL_W) curDir = WEST;
    }
    else
      curDir = TURNING;
  }
}

void checkVertical()
{
  if (abs(roll)<THR_ROLL_VERT && pitch < THR_PITCH_VERT)
    // imu is horizontal
    horzDur++;
  else
    // imu is vertical
    horzDur = 0; // reset duration
  // Update vertical flag
  isVert = (horzDur > THR_HORZ_DUR) ? false : true;
}

void readIR()
/* Function takes "loopCount" number of IR sensor readings and
 * stores the linearized value in float "irDist"
 */
{
  irVal = 0;

  for (int i = 0; i < FILTER_AVG; i++)
    irVal += analogRead(PIN_IR);

  irVal /= FILTER_AVG;
  irDist = 12343.85 * pow(irVal, -1.15); // Linearizing eqn, accuracy +- 5%
}

void checkEncoder(int wheelNum)
{
  Serial.println("[INFO] CHECKING ENCODER");
  if (wheelNum == 1)
    curDist = rightWheelTicks/5000*360/360*21.5;
  else if (wheelNum == 2)
    curDist = leftWheelTicks/5000*360/360*21.5;
}

/**
 * ISR routines for encoders
 */

void updateRightTick()
{
  rightWheelTicks++;
}

void updateLeftTick()
{
  leftWheelTicks++;
}
