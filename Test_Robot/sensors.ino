
/*===============================
=            SENSORS            =
===============================*/

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

void updateRightTick()
{
  n_tick++;
  rightWheelTicks++;
}

void updateLeftTick()
{
  leftWheelTicks++;
}
