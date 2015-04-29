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
 * This is the overall code used for the final design of RACER. It takes in
 * sensor readings from the inertial measurement unit (IMU), force-senstiive
 * resistor (FSR), infrared (IR) sensor, and uses these readings to navigate
 * the RACER robot around the window to clean it.
 *
 *****************************************************************************/

// Libraries used for RACER
#include "config.h"
#include "IOpins.h"

// Variables for EDF
int edf_1_val = EDF_MIN;
int edf_2_val = EDF_MIN;
long edf_1_utime = 0; // last update time for edf 1
long edf_2_utime = 0; // last update time for edf 2
int edf_id;

// IR variables
float irDist = 0;
float irArray [FILTER_SAMPLE];   // array for holding raw sensor values for sensor1
float irRawData, irSmoothData;  // variables for sensor1 data

// IMU variable
float roll, pitch;
Heading curDir, pastDir;

// Turning variables
int targetTime = 0;

// Variables for Motors
int torq_straight_1 = TORQ_DEFAULT;
int torq_straight_2 = TORQ_DEFAULT;

// Debounce button for rst imu
int rstIMU_value_cur = LOW;
int rstIMU_value_old = LOW;
long rstIMU_timer;
int rstIMU_state;

// FSM
State state = LEFTU_NEXT;
boolean isLastLap = false;

// Mode
boolean isPathfind = false;

void setup()
{
  Serial.begin(115200);
  LED_init();
  // Activate indicators
  digitalWrite(PIN_RED1, HIGH);
  digitalWrite(PIN_RED2, HIGH);
  // Switches
  pinMode(PIN_IMUSW, INPUT);
  pinMode(PIN_PATHFINDSW, INPUT);
  pinMode(PIN_EDFSW, INPUT);
  // Initialize components
  motor_init();
  EDF_init();
  IMU_init();
  // Deactivate indicators
  digitalWrite(PIN_RED1, LOW);
  digitalWrite(PIN_RED2, LOW);
}

void loop()
{
  // printdata();
  // Check all switches
  checkEDFSW();
  checkPathfindSW();
  checkRstIMUBut();
  // Indicators
  LEDcontrol();
  // Read Sensors
  readIMU();
  updateAngles();
  readIR();
  // Pathfinding
  if (isPathfind)
  {
    pathfindingFSM();
    motorFeedback();
  }
}

/*============================
=            Misc            =
============================*/

void LEDcontrol()
{
  LED_rst(); // Reset all LEDS

  if (isPathfind) // Pathfinding mode
  {
    if (state == LEFTU_NEXT ||
        state == RIGHTU_NEXT||
        state == LEFTU_2    ||
        state == RIGHTU_2)
    {
      digitalWrite(PIN_GREEN1, HIGH);
      digitalWrite(PIN_GREEN2, HIGH);
    }
    // left U turn
    else if (state == LEFTU_1 ||
             state == LEFTU_3)
    {
      digitalWrite(PIN_GREEN1, HIGH);
    }
    // right U turn
    else if (state == RIGHTU_1 ||
             state == RIGHTU_3)
    {
      digitalWrite(PIN_GREEN2, HIGH);
    }
    // last lap
    else if (state == LAST_LAP)
    {
      digitalWrite(PIN_RED1, HIGH);
      digitalWrite(PIN_RED2, HIGH);
      digitalWrite(PIN_GREEN1, HIGH);
      digitalWrite(PIN_GREEN2, HIGH);
    }
    // stop
    else if (state == STOP)
    {
      LED_flash();
    }
  }
  else // Idle state
  {
    digitalWrite(PIN_BLUE, HIGH);
    // EDF at max
    if (edf_1_val == EDF_MAX && edf_2_val == EDF_MAX)
      digitalWrite(PIN_RED1, HIGH);
  }
}

/**
 * Once EDF SW is off, switch off EDF
 */
void checkEDFSW()
{
  // Debounce push button
  if (digitalRead(PIN_EDFSW) == LOW)
    EDF_rst();
  else
  {
    if (edf_1_val<EDF_MAX) // Step EDF 1 to max
      step_PWM(1,1);
    if (edf_2_val<EDF_MAX) // Step EDF 2 to max
      step_PWM(2,1);
    analogWrite(PIN_EDF_1, edf_1_val);
    analogWrite(PIN_EDF_2, edf_2_val);
  }
}

/**
 * Once pathfind switch is hit, activate pathfinding mode
 */
void checkPathfindSW()
{
  if (digitalRead(PIN_PATHFINDSW) == LOW)
  {
    rstPathfind();
    stop();
  }
  else
    isPathfind = true;
}

void checkRstIMUBut()
{
  // Debounce push button
  rstIMU_value_cur = digitalRead(PIN_IMUSW);
  if (rstIMU_value_cur != rstIMU_value_old)
    rstIMU_timer = millis();
  if ((millis() - rstIMU_timer) > DEBOUNCE_DELAY) // filter out noise
  {
    // button is held long enough, recalibrate IMU if it has not been calibrated
    if (rstIMU_value_cur == HIGH && rstIMU_state == LOW)
    {
        // Clear all indicator
        LED_rst();
        // Reset IMU
        digitalWrite(PIN_RED1, HIGH);
        digitalWrite(PIN_RED2, HIGH);
        rstIMU_state = !rstIMU_state;
        rstIMU();
        digitalWrite(PIN_RED1, LOW);
        digitalWrite(PIN_RED2, LOW);
    }
    // button has been released long enough, reset reset state
    if (rstIMU_value_cur == LOW && rstIMU_state == HIGH)
      rstIMU_state = !rstIMU_state;
    // ignore case where button is pressed and imu has been reset before
  }
  rstIMU_value_old = rstIMU_value_cur; // Update old reading
}


/*=====================================
=            LED Functions            =
=====================================*/

void LED_init()
{
  pinMode(PIN_RED1, OUTPUT);
  pinMode(PIN_RED2, OUTPUT);
  pinMode(PIN_BLUE, OUTPUT);
  pinMode(PIN_GREEN1, OUTPUT);
  pinMode(PIN_GREEN2, OUTPUT);
  LED_rst();
}

void LED_rst()
{
  digitalWrite(PIN_RED1, LOW);
  digitalWrite(PIN_RED2, LOW);
  digitalWrite(PIN_BLUE, LOW);
  digitalWrite(PIN_GREEN1, LOW);
  digitalWrite(PIN_GREEN2, LOW);
}

void LED_flash()
{
  digitalWrite(PIN_GREEN1, HIGH);
  delay(100);
  digitalWrite(PIN_GREEN1, LOW);
  digitalWrite(PIN_GREEN2, HIGH);
  delay(100);
  digitalWrite(PIN_GREEN2, LOW);
  digitalWrite(PIN_BLUE, HIGH);
  delay(100);
  digitalWrite(PIN_BLUE, LOW);
  digitalWrite(PIN_RED1, HIGH);
  delay(100);
  digitalWrite(PIN_RED1, LOW);
  digitalWrite(PIN_RED2, HIGH);
  delay(100);
  digitalWrite(PIN_RED2, LOW);
}
