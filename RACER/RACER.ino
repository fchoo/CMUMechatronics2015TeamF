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

#define DEBUG false

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
float roll, pitch;
int horzDur = 0;
boolean isVert = false;
Heading curDir;

// Encoder variables
float targetDist = 0;
float curDist = 0;
float leftWheelTicks = 0;
float rightWheelTicks = 0;

// Variables for Motors
int torq_straight_1 = TORQ_DEFAULT;
int torq_straight_2 = TORQ_DEFAULT;

// Serial
int cmd = 'q';

// FSM
State state;
boolean irFlag = false;
boolean encoderFlag = false;

// Mode
boolean isJoyStick = false;
boolean isKilled = false;
boolean isPathfind = false;

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
  attachInterrupt(0, updateLeftTick, RISING);
  attachInterrupt(1, updateRightTick, RISING);
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
  // printdata();
  updateAngles();
  checkVertical();
  checkHeading();

  Serial.print("[INFO] Vertical: ");
  Serial.print(isVert);
  Serial.print(" Heading: ");
  Serial.print(curDir);
  Serial.print(" Roll: ");
  Serial.print(roll);
  Serial.print(" Pitch: ");
  Serial.println(pitch);
  // Read controls
  serialControl(); // Serial control
  if (isJoyStick == true)  // Joystick control
    joyStickControl();

  if (isPathfind == true) // Pathfinding control
  {
    if (pwm_value < 150)
      step_PWM(1);
    if (pwm_value == 150)
    {
      updateFlags();
      pathfindingFSM();
    }
  }
  // Serial.print("[INFO] State: ");
  // Serial.print(state);
  // Serial.print(" targdist ");
  // Serial.print(targetDist);
  // Serial.print(" curDist: ");
  // Serial.print(curDist);
  // Serial.print(" left: ");
  // Serial.print(leftWheelTicks);
  // Serial.print(" right: ");
  // Serial.println(rightWheelTicks);
  // Feedback controls for motor speed
  // Serial.print("[INFO] Left tick: ");
  // Serial.print(leftWheelTicks);
  // Serial.print(" Right tick: ");
  // Serial.print(rightWheelTicks);
  // Serial.print(" Left wheel PWM: ");
  // Serial.print(torq_straight_1);
  // Serial.print(" Right wheel PWM: ");
  // Serial.println(torq_straight_2);
  update_PWM();
  motorFeedback();
  edfFeedback();
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
    // Program State
    digitalWrite(PIN_LED, LOW);
    isKilled = true;
    cmd = ' '; // stop all cmd
    // EDF
    analogWrite(PIN_EDF, 0); // kill edf
    // Locomotion
    pwm_value = PWM_MIN; // reset pwm values
    stop(); // kill all motors
    // Pathfinding variables
    state = LEFTU_NEXT;
    // Encoder variables
    leftWheelTicks = 0;
    rightWheelTicks = 0;
  }
  else
  {
    digitalWrite(PIN_LED, HIGH);
    isKilled = false;
  }
}

