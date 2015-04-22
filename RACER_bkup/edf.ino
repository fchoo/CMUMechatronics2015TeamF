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
 * EDF related functions. Includes initization code, stepping function and
 * feedback controls.
 *
 *****************************************************************************/

void EDF_init()
{
  pinMode(PIN_EDF_1,OUTPUT);
  pinMode(PIN_EDF_2,OUTPUT);
  // Initialize EDF Motor
  analogWrite(PIN_EDF_1, 0);
  analogWrite(PIN_EDF_2, 0);
  delay(EDF_DELAY);
  analogWrite(PIN_EDF_1, EDF_MIN);  // Engage EDF Motor
  analogWrite(PIN_EDF_2, EDF_MIN);  // Engage EDF Motor
}

void EDF_rst()
{
  // Initialize EDF Motor
  delay(EDF_DELAY);
  analogWrite(PIN_EDF_1, 0);
  analogWrite(PIN_EDF_2, 0);
  delay(EDF_DELAY);
  analogWrite(PIN_EDF_1, EDF_MIN);  // Engage EDF Motor
  analogWrite(PIN_EDF_2, EDF_MIN);  // Engage EDF Motor
}

void step_PWM(int edf_id, int dir)
{
  if (edf_id == 1)
  {
    if((millis()-edf_1_utime)>=EDF_DELAY) // step at 1/EDF_DELAY Hz
    {
      if (dir>0 && edf_1_val<EDF_MAX) edf_1_val+=2;
      if (dir<0 && edf_1_val>EDF_MIN) edf_1_val-=2;
      analogWrite(PIN_EDF_1, edf_1_val); // Send PWM value to ESC
      edf_1_utime = millis(); // Update timer
    }
  }
  else if (edf_id == 2)
  {
    if((millis()-edf_2_utime)>=EDF_DELAY) // step at 1/EDF_DELAY Hz
    {
      if (dir>0 && edf_2_val<EDF_MAX) edf_2_val+=2;
      if (dir<0 && edf_2_val>EDF_MIN) edf_2_val-=2;
      analogWrite(PIN_EDF_2, edf_2_val); // Send PWM value to ESC
      edf_2_utime = millis(); // Update timer
    }
  }
}

/**
 * Simple feedback where edf is activated once the robot is vertical
 */
void edfFeedback()
{
  if (isVert)
  {
    // Step EDF 1 to max
    if (edf_1_val<EDF_MAX)
      step_PWM(1,1);
    else
      analogWrite(PIN_EDF_1, edf_1_val); // once max, continue sending pwm values
    // Step EDF 2 to max
    if (edf_2_val<EDF_MAX)
      step_PWM(2,1);
    else
      analogWrite(PIN_EDF_2, edf_2_val); // once max, continue sending pwm values
  }
}
