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
#include <math.h>
#include "config.h"
#include "IOpins.h"

#define DEBUG true

// Variables for joysticks
int potValue;                 // to control EDF

// Variables for EDF
int pwm_value = PWM_MIN;
int man_value;
long pwm_timer = 0; // pwm_timer for PWM stepping

// IR variables
int irVal = 0;
float irDist = 0;

// IMU variable
float yaw;

// Encoder variables
float targetDist = 0;
float curDist = 0;
float n_tick = 0;
long leftWheelTicks, rightWheelTicks;

// Variables for Motors
int torq_straight_1 = TORQ_DEFAULT;
int torq_straight_2 = TORQ_DEFAULT;

// Serial
int cmd;

// FSM
State state;
boolean irFlag = false;
boolean encoderFlag = false;

// Mode
boolean isJoyStick = false;
boolean isKilled = false;
boolean isPathfind = true;

void setup()
{
  Serial.begin(115200);
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_POT, INPUT);

  motor_init();
  EDF_init();
  IMU_init();

  // kill switch
  pinMode(PIN_KILL, INPUT);

  // Interrupts initialization
  attachInterrupt(0, updateRightTick, RISING);
  attachInterrupt(1, updateLeftTick, RISING);
  state = LEFTU_NEXT;

  Serial.println("[INFO] Initialization Done.");
}

void loop()
{
  checkKill(); // Check if kill switch is hit
  if (isKilled)
  {
    if (DEBUG) Serial.println("[INFO] Killed switch flipped.");
    return; // do nothing once killed
  }

  // Read Sensors
  readIMU();
  yaw = getYaw();

  // Read controls
  serialControl(); // Serial control
  if (isJoyStick == true)  // Joystick control
    joyStickControl();
  if (isPathfind == true) // Pathfinding control
  {
    updateFlags();
    pathfindingFSM();
  }

  // Feedback controls for motor speed
  motorFeedback();
}

/*============================
=            Misc            =
============================*/

/**
 * Once kill switch is hit, switch off all motors and EDF.
 */
void checkKill()
{
  if (digitalRead(PIN_KILL) == HIGH) {
    digitalWrite(PIN_LED, LOW);
    isKilled = true;
    analogWrite(PIN_EDF, 0); // kill edf
    pwm_value = 0; // reset pwm values
    stop(); // kill all motors
    cmd = ' '; // stop all cmd
  }
  else
  {
    digitalWrite(PIN_LED, HIGH);
    isKilled = false;
  }
}

