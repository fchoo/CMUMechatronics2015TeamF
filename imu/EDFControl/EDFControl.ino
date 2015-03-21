/**
* EDFControl.ino - Control EDF fan speed with IMU
* Copyright (c) 2015 Foo Lai Choo.  All right reserved.
*
* This program controls the EDF fan speed based on the angle of inclination the
* robot is experiencing. First, it calculates the suction force required to keep
* the robot adhere to an inclined surface. It then adjust the EDF motor
* accordingly.
*
**/

// Helper functions
#define ToRad(x) ((x)*0.01745329252)  // *pi/180
#define ToDeg(x) ((x)*57.2957795131)  // *180/pi
// Suction Force Calculation
#define COE_FRICTION 0.6
#define F_WEIGHT (3*9.81) // robot weight 3kg
// EDF Control
#define PWM_MIN 191
#define PWM_MAX 220
#define PWM_DELAY 200 // 200ms
#define PWM_STEPSIZE 1
// Pin Definition
#define PIN_LED 13
#define PIN_EDF 3 // Digital PWM pin for EDF.

// Variables
float theta;
float fs;
float fs_old;
int pwm_value = PWM_MIN;
long pwm_timer = 0; // pwm_timer for PWM stepping

void setup()
{
  Serial.begin(115200);
  pinMode(PIN_LED, OUTPUT);  // Status LED
  pinMode(PIN_EDF, OUTPUT);
  digitalWrite(PIN_LED, LOW); // OFF - Initializing

  analogWrite(PIN_EDF, 0);
  delay(500);
  analogWrite(PIN_EDF, PWM_MIN);
  AHRS_Init();

  digitalWrite(PIN_LED, HIGH); // ON - Initialization done
}

void loop() //Main Loop
{
  Read_AHRS();
  calc_fs();
  if ((fs > fs_old) && (fs>0))
    step_PWM(1);
  else if ((fs < fs_old) && (pwm_value>PWM_MIN))
    step_PWM(-1);
}

void step_PWM(int dir)
{
  if((millis()-pwm_timer)>=PWM_DELAY) // step at 1/PWM_DELAY Hz
  {
    if ((dir==1) && (pwm_value < PWM_MAX)) // Increment, limited to max
      pwm_value += PWM_STEPSIZE;
    else if ((dir==-1) && (pwm_value > PWM_MIN)) // Decrement, limited to min
      pwm_value -= PWM_STEPSIZE;
    analogWrite(PIN_EDF, pwm_value); // Send PWM value to ESC
    pwm_timer = millis();
    fs_old = fs; // Update fs
  }
}

void calc_fs()
{
  theta = ToRad(Get_Pitch());
  fs = ((F_WEIGHT*sin(theta)) - (COE_FRICTION*F_WEIGHT*cos(theta)))/COE_FRICTION;
  Serial.print("Pitch: ");
  Serial.print(Get_Pitch());
  Serial.print("|");
  Serial.print(theta);
  Serial.print("   fs: ");
  Serial.print(fs);
  Serial.print("|");
  Serial.print(fs/9.81);
  Serial.print(" PWM: ");
  Serial.println(pwm_value);
}
