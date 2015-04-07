

/*========================================
=            Joystick Control            =
========================================*/

void joyStickControl()
{
  // read and scale the two axes:
  int xReading = readAxis(PIN_JOYX);
  int yReading = readAxis(PIN_JOYY);
  // For RIGHT Joystick control
  if(yReading>4)
    moveLeft();
  else if(yReading<-4)
    moveRight();
   else if(xReading>4)
    moveBack();
  else if(xReading<-4)
    moveForward();
  else
    stop();

  delay(JOY_DELAY);
  POT2PWM();
}

/**
 * Reads an axis (0 or 1 for x or y) and scales the
 * analog input range to a range from 0 to <range>
 */
int readAxis(int thisAxis) {
  // read the analog input:
  int reading = analogRead(thisAxis);

  // map the reading from the analog input range to
  // the output range
  reading = map(reading, 0, 1023, 0, JOY_RANGE);

  // if the output reading is outside from the
  // rest position threshold,  use it:
  int distance = reading - JOY_CENTER;

  if (abs(distance) < JOY_THRESHOLD) {
    distance = 0;
  }

  // return the distance for this axis:
  return distance;
}

/**
 * Use POT values to control EDF speed
 */
void POT2PWM()
{
  potValue = analogRead(PIN_POT);
  edf_1_mval = map(potValue, 0, 1023, EDF_MIN, EDF_MAX);
  if (edf_1_val < edf_1_mval)
    step_PWM(1, 1);
  if (edf_1_val > edf_1_mval)
    step_PWM(1, -1);
}
