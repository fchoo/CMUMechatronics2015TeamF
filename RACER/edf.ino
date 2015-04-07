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
  delay(PWM_DELAY);
  analogWrite(PIN_EDF_1, PWM_MIN);  // Engage EDF Motor
  analogWrite(PIN_EDF_2, PWM_MIN);  // Engage EDF Motor
}

void step_PWM(int edf_id, int dir)
{
  if (edf_id == 1)
  {
    if((millis()-pwm_1_utime)>=PWM_DELAY) // step at 1/PWM_DELAY Hz
    {
      if (dir>0 && pwm_1_val<PWM_MAX) pwm_1_val++;
      if (dir<0 && pwm_1_val>PWM_MIN) pwm_1_val--;
      analogWrite(PIN_EDF_1, pwm_1_val); // Send PWM value to ESC
      pwm_1_utime = millis(); // Update timer
    }
  }
  else
  {
    if((millis()-pwm_2_utime)>=PWM_DELAY) // step at 1/PWM_DELAY Hz
    {
      if (dir>0 && pwm_2_val<PWM_MAX) pwm_2_val++;
      if (dir<0 && pwm_2_val>PWM_MIN) pwm_2_val--;
      analogWrite(PIN_EDF_2, pwm_2_val); // Send PWM value to ESC
      pwm_2_utime = millis(); // Update timer
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
    if (pwm_1_val<PWM_MAX)
      step_PWM(1,1);
    else
      analogWrite(PIN_EDF_1, pwm_1_val); // once max, continue sending pwm values
    // Step EDF 2 to max
    if (pwm_2_val<PWM_MAX)
      step_PWM(2,1);
    else
      analogWrite(PIN_EDF_2, pwm_2_val); // once max, continue sending pwm values
  }
  else // disengage edf once horizontal
  {
    step_PWM(1,-1);
    step_PWM(2,-1);
  }
}
