
/*===============================
=            SENSORS            =
===============================*/
void updateAngles()
{
  roll = getRoll();
  pitch = getPitch();
}

boolean isVeeringLeft()
{
  if (isVert && pitch < THR_PITCH_LEFT)
    return true;
  else
    return false;
}

boolean isVeeringRight()
{
  if (isVert && pitch > THR_PITCH_RIGHT)
    return true;
  else
    return false;
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

void checkEncoder()
{
  curDist = n_tick/5000*360/360*21.5;
}

/**
 * ISR routines for encoders
 */

void updateRightTick()
{
  n_tick++;
  rightWheelTicks++;
}

void updateLeftTick()
{
  leftWheelTicks++;
}
