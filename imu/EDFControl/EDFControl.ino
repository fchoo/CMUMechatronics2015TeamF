/**
* This program controls the EDF fan speed based on the angle of inclination the 
* robot is experiencing. First, it calculates the suction force required to keep 
* the robot adhere to an inclined surface. It then adjust the EDF motor accordingly.
**/

// Helper functions
#define ToRad(x) ((x)*0.01745329252)  // *pi/180
#define ToDeg(x) ((x)*57.2957795131)  // *180/pi
// Suction Force Calculation
#define COE_FRICTION = 0.6
#define F_WEIGHT = 3 // robot weight = 3kg
// EDF Control
#define PWM_MIN = 191;
#define PWM_MAX = 255;
#define PWM_DELAY = 20; // 20ms
#define PWM_STEPSIZE = 5;
// Pin Definition
#define PIN_LED 13
#define PIN_EDF = 3; // Digital PWM pin for EDF.

float pitch;
float fs;
float fs_old;
int pwm_value = PWM_MIN;
long timer = 0; // timer for PWM stepping

void setup()
{
  Serial.begin(115200);
  pinMode(PIN_LED, OUTPUT);  // Status LED
  pinMode(PIN_EDF, OUTPUT);
  digitalWrite(PIN_LED, LOW); // OFF - Initializing

  AHRS_Init();
  analogWrite(PIN_EDF, 0);
  delay(500);
  analogWrite(PIN_EDF, PWM_MIN);
  delay(4000);

  digitalWrite(PIN_LED, HIGH); // ON - Initialization done
}

void loop() //Main Loop
{
  Read_AHRS();
  calc_fs();
  if (fs > fs_old)
    step_PWM();
  Serial.print(fs);
}

void step_PWM()
{
  if((millis()-timer)>=PWM_DELAY)
  {
    if (pwm_value < PWM_MAX)
    {
      pwm_value += PWM_STEPSIZE;
      analogWrite(PIN_EDF, pwm_value);
      timer = millis();
      fs_old = fs; // Update fs
    }
  }
}

void calc_fs()
{
  pitch = ToRad(Get_Pitch());
  fs = ((F_WEIGHT*sin(pitch)) - (COE_FRICTION*F_WEIGHT*cos(pitch)))/COE_FRICTION;
}