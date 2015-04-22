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
 * LAST REVISION: 03/20/2015
 *
 * Pump related functions
 *
 *****************************************************************************/

void pump_init()
{
  pinMode(PIN_PUMP,OUTPUT);
  analogWrite(PIN_PUMP,0);
}

void pump_control()
{
  if (pump_state == 0) // pump is off
    // downtime finished
    if (millis()-pump_utime > PUMP_DOWNTIME)
    {
      analogWrite(PIN_PUMP, PUMP_VAL);
      pump_state = !pump_state;
      pump_utime = millis();
    }
  else if (pump_state == 1) // up time
  {
    // Scenarios where pump should be switched off
    // boundaries detected
    if (irDist < THR_IR_PUMP)
      pump_state = !pump_state;
    // robot is U-turning
    else if (state != LEFTU_NEXT || state != RIGHTU_NEXT)
      pump_state = !pump_state;
    // uptime finished
    else if (millis()-pump_utime > PUMP_UPTIME)
      pump_state = !pump_state;
    // if pump is switched off
    if (pump_state == 0)
    {
      analogWrite(PIN_PUMP, 0);
      pump_utime = millis();
    }
  }
}
