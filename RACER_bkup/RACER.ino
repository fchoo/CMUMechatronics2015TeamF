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

#define DEBUG 2

// Variables for joysticks
int potValue; // to control EDF

// Variables for EDF
int edf_1_val = EDF_MIN;
int edf_2_val = EDF_MIN;
int edf_mval = EDF_MIN;
int edf_1_mval = EDF_MIN;
int edf_2_mval = EDF_MIN;
long edf_1_utime = 0; // last update time for edf 1
long edf_2_utime = 0; // last update time for edf 2
int edf_id;

// Pump Variables
long pump_utime;
int pump_state;

// IR variables
float irDist = 0;
float irArray [FILTER_SAMPLE];   // array for holding raw sensor values for sensor1
float irRawData, irSmoothData;  // variables for sensor1 data

// IMU variable
float roll, pitch;
int horzDur = 0;
boolean isVert = false;
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

// Serial
int cmd = 'i';
int test_id;

// FSM
State state;
boolean isLastLap = false;

// Mode
boolean isPathfind = false;
boolean isDiagnostic = false;

char* HeadingStrings[5] = { "North", "South", "East", "West", "Turning" };
char* StateStrings[10] = { "LEFTU_NEXT", "LEFTU_1", "LEFTU_2", "LEFTU_3",
                            "RIGHTU_NEXT", "RIGHTU_1", "RIGHTU_2", "RIGHTU_3",
                            "LAST_LAP", "STOP"};

void setup()
{
  Serial.begin(115200);
  pinMode(PIN_LED, OUTPUT);
  LED_init();
  digitalWrite(PIN_RED1, HIGH);
  digitalWrite(PIN_RED2, HIGH);
  // EDF SW
  pinMode(PIN_EDFSW, INPUT);
  state = LEFTU_NEXT;
  // Initialize components
  motor_init();
  EDF_init();
  IMU_init();
  pump_init();

  digitalWrite(PIN_RED1, LOW);
  digitalWrite(PIN_RED2, LOW);
  printSerialInst();
}

void loop()
{
  // if (isDiagnostic)
  // {
  //   LED_rst();
  //   digitalWrite(PIN_RED1, HIGH);
  //   digitalWrite(PIN_RED2, HIGH);
  //   diagnosticCheck();
  // }
  // else {
    LEDcontrol(); // Set indicators
  // }

  checkEDFSW();
  checkPathfindSW();
  if (!isVert)
    checkRstIMUBut();

  // Read Sensors
  readIMU();
  updateAngles();
  checkVertical();
  checkHeading();
  readIR();

  // serialControl(); // Serial control
  if (isPathfind == true)
  {
    pathfindingFSM();
    // pump_control();
    motorFeedback();
  }

  // DEBUG PRINTS
  // if (DEBUG == 0)
  // {
  //   Serial.print("[INFO] State: ");
  //   Serial.print(StateStrings[state]);
  //   Serial.print(" Vertical: ");
  //   Serial.print(isVert);
  //   Serial.print(" Cur_Heading: ");
  //   Serial.print(HeadingStrings[curDir]);
  //   Serial.print(" Past_Heading: ");
  //   Serial.print(HeadingStrings[pastDir]);
  //   Serial.print(" IRdist ");
  //   Serial.println(irDist);
  // }
  // else if (DEBUG == 2) {
  //   printdata();
  // }
}

/*============================
=            Misc            =
============================*/

void LEDcontrol()
{
  // Reset all LEDS
  LED_rst();
  // GREEN LED
  // straight
  if (isPathfind)
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
  // Idle
  if (!isPathfind)
    digitalWrite(PIN_BLUE, HIGH);
  // EDF at max
  if (edf_1_val == EDF_MAX && edf_2_val == EDF_MAX)
    digitalWrite(PIN_RED1, HIGH);
}

/**
 * Once EDF SW is off, switch off EDF
 */
void checkEDFSW()
{
  // Debounce push button
  if (digitalRead(PIN_EDFSW) == LOW)
  {
    edf_1_val = EDF_MIN;
    edf_2_val = EDF_MIN;
  }
  else
  {
    // if (isVert)
    // {
      // Step EDF 1 to max
      if (edf_1_val<EDF_MAX)
        step_PWM(1,1);
      // Step EDF 2 to max
      if (edf_2_val<EDF_MAX)
          step_PWM(2,1);
    // }
  }
  analogWrite(PIN_EDF_1, edf_1_val);
  analogWrite(PIN_EDF_2, edf_2_val);
}

/**
 * Once pathfind switch is hit, activate pathfinding mode
 */
void checkPathfindSW()
{
  if (digitalRead(PIN_PATHFIND) == LOW)
  {
    rstPathfind();
    // rstPump();
    stop();
  }
  else
    isPathfind = true;
}

void checkRstIMUBut()
{
  // Debounce push button
  rstIMU_value_cur = digitalRead(PIN_RSTIMU);
  if (rstIMU_value_cur != rstIMU_value_old)
    rstIMU_timer = millis();
  if ((millis() - rstIMU_timer) > DEBOUNCE_DELAY) // filter out noise
  {
    // button is held long enough, recalibrate IMU if it has not been calibrated
    if (rstIMU_value_cur == HIGH && rstIMU_state == LOW)
    {
        // Clear all indicator
        LED_rst();
        // Reset EDF
        digitalWrite(PIN_RED1, HIGH);
        EDF_rst();
        digitalWrite(PIN_RED1, LOW);
        // Reset IMU
        digitalWrite(PIN_RED2, HIGH);
        rstIMU_state = !rstIMU_state;
        rstIMU();
        digitalWrite(PIN_RED2, LOW);
    }
    // button has been released long enough, reset reset state
    if (rstIMU_value_cur == LOW && rstIMU_state == HIGH)
      rstIMU_state = !rstIMU_state;
    // ignore case where button is pressed and imu has been reset before
  }
  rstIMU_value_old = rstIMU_value_cur; // Update old reading
}

void rstEDF()
{
  edf_1_val = EDF_MIN;
  edf_2_val = EDF_MIN;
  // edf_1_mval = EDF_MIN;
  // edf_2_mval = EDF_MIN;
  analogWrite(PIN_EDF_1, edf_1_val);
  analogWrite(PIN_EDF_2, edf_2_val);
}

void rstPump()
{
  pump_state = 0;
  analogWrite(PIN_PUMP, 0);
}

void rstPathfind()
{
  isPathfind = false;
  state = LEFTU_NEXT;
  isLastLap = false;
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
