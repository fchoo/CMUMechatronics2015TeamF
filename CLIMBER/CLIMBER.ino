/******************************************************************************
 * (18-578 / 16-778 / 24-778) MECHATRONIC DESIGN
 * TEAM F: SWIPER
 * MEMBERS:
 *          - CHOO, FOO LAI
 *          - EREBOR, TELSON
 *          - FLAREAU, JOSHUA
 *          - KALOUCHE, SIMON
 *          - TAN, NICHOLAS
 *
 * LAST REVISION: 04/23/2015
 *
 *****************************************************************************/
// Header files
#include "config.h"
#include "IOpins.h"

// Shorthand functions
#define round(x) ((x>=0)?(int)(x+0.5):(int)(x-0.5))

// EDF
int edf_val = EDF_MIN;
long edf_utime = 0;

// IMU variable
float roll;
int roll_scaled;
// Variables for Motors
int torq_left = D_TORQ_DEFAULT;
int torq_right = D_TORQ_DEFAULT;
// Switches variables
int SW_cur, SW_old;
State state;

char* StateStrings[4] = { "WAIT", "EDF", "CLEAN", "STOP"};

void setup() {
  // start Serial connection process
  Serial.begin(115200);
  // declare output pins
  pinMode(PIN_RIGHTMOTOR_1, OUTPUT);
  pinMode(PIN_RIGHTMOTOR_2, OUTPUT);
  pinMode(PIN_LEFTMOTOR_1, OUTPUT);
  pinMode(PIN_LEFTMOTOR_2, OUTPUT);
  // declare input pins
  pinMode(PIN_SW, INPUT_PULLUP);
  // Init IMU
  IMU_init();
  // Init EDF
  EDF_init();

  pinMode(PIN_LED, OUTPUT);
  analogWrite(PIN_LED, LOW);
}

void loop() {
  // Serial.print("state: ");
  // Serial.print(StateStrings[state]);
  // Serial.print(" edf: ");
  // Serial.println(edf_val);
  // Serial.print(" roll: ");
  // Serial.print(roll);
  // Serial.print(" Sroll: ");
  // Serial.print(roll_scaled);
  // Serial.print(" leftT: ");
  // Serial.print(torq_left);
  // Serial.print(" rightT: ");
  // Serial.println(torq_right);

  readSensors();
  // if edf is max, activate indicator
  if (edf_val == EDF_MAX)
    analogWrite(PIN_LED, HIGH);
  // ramping EDF to max
  if ((state == EDF) && (edf_val<EDF_MAX))
    step_PWM(1);
  // Check (or wait) if start switch to trigger, then begin the operating sequence
  else if (state == CLEAN)
  {
    motorPID();
    moveUp();
  }
  else
    stop();
}

void readSensors()
{
  // Read switches
  checkSW();
  // Read IMU
  readIMU();
  roll = getRoll();
}

void checkSW()
{
  // Debounce push button
  SW_cur = digitalRead(PIN_SW);
  if (SW_cur != SW_old)
  {
    if (SW_cur == HIGH)
    {
      if (state == WAIT)
        state = EDF;
      else if ((edf_val == EDF_MAX) && (state == EDF))
        state = CLEAN;
      else if (state == CLEAN)
        state = STOP;
    }
    SW_old = SW_cur;
  }
}

void motorPID()
{
  roll_scaled = map(abs(roll), 0, 180, 0, 45);
  if (roll_scaled>ROLL_FLAT)
  {
    compensateRight();
  }
  else if (roll_scaled<ROLL_FLAT)
  {
    compensateLeft();
  }
  else
  {
    // Reset torq variables when level
    torq_left = D_TORQ_DEFAULT;
    torq_right = D_TORQ_DEFAULT;
  }
}

void moveUp()
{
    // right side
    analogWrite(PIN_RIGHTMOTOR_1, 0);
    analogWrite(PIN_RIGHTMOTOR_2, torq_right);
    // left side
    analogWrite(PIN_LEFTMOTOR_1, 0);
    analogWrite(PIN_LEFTMOTOR_2, torq_left);
}

void stop()
{
  analogWrite(PIN_RIGHTMOTOR_1,0);
  analogWrite(PIN_RIGHTMOTOR_2,0);
  analogWrite(PIN_LEFTMOTOR_1,0);
  analogWrite(PIN_LEFTMOTOR_2,0);
  // Reset torq variables
  torq_left = D_TORQ_DEFAULT;
  torq_right = D_TORQ_DEFAULT;
}

/**
 * Correct veering to right. Decrease left wheel speed and increase right wheel
 * speed.
 */
void compensateRight()
{
  changeTorq(&torq_left, -1);
  changeTorq(&torq_right, 1);
}

/**
 * Correct veering to left. Decrease right wheel speed and increase left wheel
 * speed.
 */
void compensateLeft()
{
  changeTorq(&torq_left, 1);
  changeTorq(&torq_right, -1);
}

/**
 * Change PWM within the boundaries of 0-254
 */
void changeTorq(int *torque, int dir)
{
  if (dir<0 && (*torque)>D_TORQ_MIN) (*torque)--;
  if (dir>0 && (*torque)<D_TORQ_MAX) (*torque)++;
}


