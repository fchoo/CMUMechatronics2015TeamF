/**
* EDFCalibration.ino - Finding boundaries for PWM controls
* Copyright (c) 2015 Foo Lai Choo. All right reserved.
*
* This program steps up to the max pwm values with a delay of .5s for each step.
*
**/
// EDF Control
#define PWM_MIN 125
#define PWM_MAX 235
#define PWM_DELAY 100 // .5s
#define PWM_STEPSIZE 1
// Pin Definition
// #define PIN_LED 13
#define PIN_EDF 11 // Digital PWM pin for EDF.

// Variables
int pwm_value = PWM_MIN;
int cmd, man_value;
long pwm_timer = 0; // pwm_timer for PWM stepping

void setup()
{
  Serial.begin(115200);
  // pinMode(PIN_LED, OUTPUT);  // Status LED
  pinMode(PIN_EDF, OUTPUT);
  // digitalWrite(PIN_LED, HIGH); // On - Initializing

  analogWrite(PIN_EDF, 0);
  delay(PWM_DELAY);
  analogWrite(PIN_EDF, PWM_MIN);

  // digitalWrite(PIN_LED, LOW); // Off - Initialization done
}

void loop() //Main Loop
{
  if (Serial.available()>0) // Read cmd
  {
    cmd = getSerial();
    if (cmd == 's') // Stepping to a certain value
    {
      man_value = getSerial(); // Get user input for pwm
      while (pwm_value < man_value)
        step_PWM(1);
      while (pwm_value > man_value)
        step_PWM(-1);
    }
    else if (cmd == 'p')
    {
      pwm_value = getSerial(); // Get user input for pwm
      Serial.print("PWM_Value: ");
      Serial.println(pwm_value);
      analogWrite(PIN_EDF, pwm_value);
    }
  }
  if (cmd == 'r') // Stepping it continuously
    step_PWM(1);
}

void step_PWM(int dir)
{
  if((millis()-pwm_timer)>=PWM_DELAY) // step at 1/PWM_DELAY Hz
  {
    // digitalWrite(PIN_LED, HIGH);
    if (dir>0 && pwm_value<PWM_MAX) pwm_value+= PWM_STEPSIZE;
    if (dir<0 && pwm_value>PWM_MIN) pwm_value-= PWM_STEPSIZE;
    analogWrite(PIN_EDF, pwm_value); // Send PWM value to ESC
    pwm_timer = millis(); // Update timer

    Serial.print("PWM_Value: ");
    Serial.println(pwm_value);
    // digitalWrite(PIN_LED, LOW);
  }
}

int getSerial()
{
  int serialData = 0;
  int aChar = 0;
  while (aChar != '/')
  {
    aChar = Serial.read();
    if (aChar >= '0' && aChar <= '9')
      serialData = serialData * 10 + aChar - '0';
    else if (aChar >= 'a' && aChar <= 'z')
      serialData = aChar;
  }
  return serialData;
}
