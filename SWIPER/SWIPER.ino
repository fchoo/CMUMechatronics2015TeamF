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

// DEBUG variables
#define DEBUG 1
#define MOVE_DELAY 2000
#define STOP_DELAY 500
boolean moveState = false;
long delay_timer = 0;

// Switches variables
int rightWinSWVal, leftWinSWVal, frameSWVal, startSWVal;
// IMU variable
float roll;
// Variables for Motors
int torq_left = D_TORQ_DEFAULT;
int torq_right = D_TORQ_DEFAULT;

void setup() {
  // declare output pins
  pinMode(PIN_RIGHTMOTOR_1, OUTPUT);
  pinMode(PIN_RIGHTMOTOR_2, OUTPUT);
  pinMode(PIN_LEFTMOTOR_1, OUTPUT);
  pinMode(PIN_LEFTMOTOR_2, OUTPUT);
  pinMode(PIN_RIGHTLACT_1, OUTPUT);
  pinMode(PIN_RIGHTLACT_2, OUTPUT);
  pinMode(PIN_LEFTLACT_1, OUTPUT);
  pinMode(PIN_LEFTLACT_2, OUTPUT);

  // declare input pins
  pinMode(PIN_RIGHTWIN_SW, INPUT_PULLUP);
  pinMode(PIN_LEFTWIN_SW, INPUT_PULLUP);
  pinMode(PIN_FRAME_SW, INPUT_PULLUP);
  pinMode(PIN_START_SW, INPUT_PULLUP);

  // Init IMU
  IMU_init();
  // start Serial connection process
  Serial.begin(115200);
}

void loop() {

  // Read & Update sensor values
  readSensors();
  // Check (or wait) if start switch to trigger, then begin the operating sequence
  if ( startSWVal == 1 )
  {
    // Check if the cleaning pad is in contact with the window --> extend linear Actuators if not
    checkWindowContact(rightWinSWVal, leftWinSWVal);
    // Drive up the window and stop when it reaches the top
    drive();
  }
  // DEBUG MODE
  if (DEBUG)
  {
    if (moveState) // Time to move!
    {
      motorPID();
      moveUp();
      if ((millis()-delay_timer)>=MOVE_DELAY)
      {
        moveState = false;
        delay_timer = millis();
      }
    }
    else // Time to stop!
    {
      stop();
      if ((millis()-delay_timer)>=STOP_DELAY)
      {
        moveState = true;
        delay_timer = millis();
      }
    }
  }
}


void readSensors()
{
  // Read switches
  rightWinSWVal = digitalRead(PIN_RIGHTWIN_SW);
  leftWinSWVal = digitalRead(PIN_LEFTWIN_SW);
  frameSWVal = digitalRead(PIN_FRAME_SW);
  startSWVal = digitalRead(PIN_START_SW);
  // Read IMU
  readIMU();
  roll = getRoll();
}

/**
 * Check if robot is in contact with the window
 */
void checkWindowContact( int rightWinSWVal, int leftWinSWVal)
{
  if ( rightWinSWVal == 0 ) // check right switch
  {
    // extend linear actuator arm until switch is triggered (until pad is touching window)
    analogWrite(PIN_RIGHTLACT_1, LACT_TORQ_PWM);
    analogWrite(PIN_RIGHTLACT_2, 0);
  }
  else
  {
    // hold linear actuator position
    analogWrite(PIN_RIGHTLACT_1, 0);
    analogWrite(PIN_RIGHTLACT_2, 0);
  }

  if ( leftWinSWVal == 0 ) // check left switch
  {
    // extend linear actuator arm until switch is triggered (until pad is touching window)
    analogWrite(PIN_LEFTLACT_1, LACT_TORQ_PWM);
    analogWrite(PIN_LEFTLACT_2, 0);
  }
  else
  {
    // hold linear actuator position
    analogWrite(PIN_LEFTLACT_1, 0);
    analogWrite(PIN_LEFTLACT_2, 0);
  }
}

void drive()
{
  // check if cleaning pad is making contact on both sides of the window
  if (rightWinSWVal == 1 && leftWinSWVal == 1 && PIN_FRAME_SW == 0 )
  {
    // Adjust motor speed of each side to level out the bar
    motorPID();
    moveUp();
  }
  else
    stop();
}

void motorPID()
{
  if (round(roll)>ROLL_FLAT)
    compensateRight();
  else if (round(roll)<ROLL_FLAT)
    compensateLeft();
}

void moveUp()
{
    // right side
    analogWrite(PIN_RIGHTMOTOR_1, torq_right);
    analogWrite(PIN_RIGHTMOTOR_2, 0);
    // left side
    analogWrite(PIN_LEFTMOTOR_1, torq_left);
    analogWrite(PIN_LEFTMOTOR_2, 0);
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
  changeTorq(&torq_left, -TORQ_STEP);
  changeTorq(&torq_right, TORQ_STEP);
}

/**
 * Correct veering to left. Decrease right wheel speed and increase left wheel
 * speed.
 */
void compensateLeft()
{
  changeTorq(&torq_left, TORQ_STEP);
  changeTorq(&torq_right, -TORQ_STEP);
}

/**
 * Change PWM within the boundaries of 0-254
 */
void changeTorq(int *torque, int step)
{
  if ((step<0 && (*torque)>D_TORQ_MIN) || (step>0 && (*torque)<D_TORQ_MAX))
    (*torque)+=step;
}
