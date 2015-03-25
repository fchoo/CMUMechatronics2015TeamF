
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

void update_PWM()
{
  analogWrite(PIN_EDF, pwm_value); // Send PWM value to ESC
}

/**
 * Simple feedback where edf is activated once the robot is vertical
 */
void edfFeedback()
{
  if (isVert)
    step_PWM(1);
  else
    step_PWM(-1);
}
