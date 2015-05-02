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
  pinMode(PIN_EDF,OUTPUT);
  // Initialize EDF Motor
  analogWrite(PIN_EDF, 0);
  delay(EDF_DELAY);
  analogWrite(PIN_EDF, EDF_MIN);  // Engage EDF Motor
}

void EDF_rst()
{
  edf_val = EDF_MIN;
  analogWrite(PIN_EDF, edf_val);
}

void step_PWM(int dir)
{
  if((millis()-edf_utime)>=EDF_DELAY) // step at 1/EDF_DELAY Hz
  {
    if (dir>0 && edf_val<EDF_MAX) edf_val+=2;
    if (dir<0 && edf_val>EDF_MIN) edf_val-=2;
    edf_utime = millis(); // Update timer
    analogWrite(PIN_EDF, edf_val);
  }
}
