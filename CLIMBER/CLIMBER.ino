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
int torq_left = D_TORQ_DEFAULT_1;
int torq_right = D_TORQ_DEFAULT_2;
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
  // LED
  pinMode(PIN_LED_RED1, OUTPUT);
  pinMode(PIN_LED_RED2, OUTPUT);
  pinMode(PIN_LED_GREEN, OUTPUT);
  // Init IMU
  IMU_init();
  // Init EDF
  EDF_init();

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
  // printdata();
  readSensors();
  // LED
  LED_rst();
  if (state == WAIT || state == EDF)
    digitalWrite(PIN_LED_RED1, HIGH);
  if ((state == EDF) && (edf_val == EDF_MAX))
    digitalWrite(PIN_LED_RED2, HIGH);
  if (state == CLEAN)
    digitalWrite(PIN_LED_GREEN, HIGH);
  if (state == STOP)
    LED_flash();
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

void LED_rst()
{
  digitalWrite(PIN_LED_RED1, LOW);
  digitalWrite(PIN_LED_RED2, LOW);
  digitalWrite(PIN_LED_GREEN, LOW);
}

void LED_flash()
{
  digitalWrite(PIN_LED_RED1, HIGH);
  delay(100);
  digitalWrite(PIN_LED_RED1, LOW);
  digitalWrite(PIN_LED_RED2, HIGH);
  delay(100);
  digitalWrite(PIN_LED_RED2, LOW);
  digitalWrite(PIN_LED_GREEN, HIGH);
  delay(100);
  digitalWrite(PIN_LED_GREEN, LOW);
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
  roll_scaled = map(abs(roll), 0, 180, 0, 90);
  if (roll_scaled>ROLL_FLAT)
  {
    compensateRight();
  }
  else if (roll_scaled<ROLL_FLAT)
  {
    compensateLeft();
  }
  // else
  // {
  //   // Reset torq variables when level
  //   torq_left = D_TORQ_DEFAULT;
  //   torq_right = D_TORQ_DEFAULT;
  // }
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
  torq_left = D_TORQ_DEFAULT_1;
  torq_right = D_TORQ_DEFAULT_2;
}

/**
 * Correct veering to right. Decrease left wheel speed and increase right wheel
 * speed.
 */
void compensateRight()
{
  changeTorq(1, &torq_left, -1);
  changeTorq(2, &torq_right, 1);
}

/**
 * Correct veering to left. Decrease right wheel speed and increase left wheel
 * speed.
 */
void compensateLeft()
{
  changeTorq(1, &torq_left, 1);
  changeTorq(2, &torq_right, -1);
}

/**
 * Change PWM within the boundaries of 0-254
 */
void changeTorq(int id, int *torque, int dir)
{
  if (id == 1)
  {
    if (dir<0 && (*torque)>D_TORQ_MIN_1) (*torque)--;
    if (dir>0 && (*torque)<D_TORQ_MAX_1) (*torque)++;
  }
  if (id == 2)
  {
    if (dir<0 && (*torque)>D_TORQ_MIN_2) (*torque)--;
    if (dir>0 && (*torque)<D_TORQ_MAX_2) (*torque)++;
  }
}


