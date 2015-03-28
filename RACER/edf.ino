
/*===========================
=            EDF            =
===========================*/

void EDF_init()
{
  pinMode(PIN_EDF,OUTPUT);
  // Initialize EDF Motor
  analogWrite(PIN_EDF, 0);
  delay(PWM_DELAY);
  analogWrite(PIN_EDF, PWM_MIN);  // Engage EDF Motor
}

void step_PWM(int dir)
{
  if((millis()-pwm_timer)>=PWM_DELAY) // step at 1/PWM_DELAY Hz
  {
    if (dir>0 && pwm_value<PWM_MAX) pwm_value++;
    if (dir<0 && pwm_value>PWM_MIN) pwm_value--;
    analogWrite(PIN_EDF, pwm_value); // Send PWM value to ESC
    pwm_timer = millis(); // Update timer
  }
}

/**
 * Simple feedback where edf is activated once the robot is vertical
 */
void edfFeedback()
{
  if (isVert)
  {
    if (pwm_value<PWM_MAX) // step pwm to max
      step_PWM(1);
    else
      analogWrite(PIN_EDF, pwm_value); // once max, continue sending pwm values
  }
  else // disengage edf
    step_PWM(-1);
}
