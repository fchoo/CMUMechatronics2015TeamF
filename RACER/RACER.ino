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

#define DEBUG false

// Variables for joysticks
int potValue;                 // to control EDF

// Variables for EDF
int pwm_value = PWM_MIN;
int man_value;
long pwm_timer = 0; // pwm_timer for PWM stepping

// IR variables
float irVal = 0;
float irDist = 0;

// IMU variable
float roll, pitch;
int horzDur = 0;
boolean isVert = false;
Heading curDir;

// Encoder variables
float targetDist = 0;
int targetTime = 0;
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
boolean isKilled = false;
boolean isPathfind = false;

char* HeadingStrings[5] = { "North", "South", "East", "West", "Turning" };
char* StateStrings[8] = { "LEFTU_NEXT", "LEFTU_1", "LEFTU_2", "LEFTU_3",
                            "RIGHTU_NEXT", "RIGHTU_1", "RIGHTU_2", "RIGHTU_3"};

void setup()
{
  Serial.begin(115200);
  pinMode(PIN_LED, OUTPUT);

  // Initialize components
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
  checkKillSW(); // Check if kill switch is hit
  if (isKilled)
  {
    if (DEBUG) Serial.println("[INFO] Killed switch flipped.");
    return; // do nothing once killed
  }

  // Read Sensors
  readIMU();
  updateAngles();
  checkVertical();
  checkHeading();
  readIR();

  // Read controls
  serialControl(); // Serial control
  if (isPathfind == true) // Pathfinding control
    pathfindingFSM();

  // Feedback controls
  edfFeedback();
  motorFeedback();

  // DEBUG PRINTS
  // IR
  // Serial.print("[INFO] State: ");
  // Serial.print(state);
  // Serial.print(" dist ");
  // Serial.println(irDist);

  // Encoders
  // Serial.print("[INFO] Left tick: ");
  // Serial.print(leftWheelTicks);
  // Serial.print(" Right tick: ");
  // Serial.println(rightWheelTicks);

  // IMU
  Serial.print("[INFO] State: ");
  Serial.print(StateStrings[state]);
  Serial.print(" Vertical: ");
  Serial.print(isVert);
  Serial.print(" Heading: ");
  Serial.print(HeadingStrings[curDir]);
  Serial.print(" Roll: ");
  Serial.print(roll);
  Serial.print(" Pitch: ");
  Serial.print(pitch);
  Serial.print(" IRdist ");
  Serial.println(irDist);
}

/*============================
=            Misc            =
============================*/

/**
 * Once kill switch is hit, switch off all motors and EDF.
 */
void checkKillSW()
{
  if (digitalRead(PIN_KILL) == HIGH)
  {
    // Program State
    digitalWrite(PIN_LED, LOW);
    isKilled = true;
    cmd = ' '; // stop all cmd
    // Locomotion
    rstEDF();
    rstPathfind();
    stop(); // kill all motors
  }
  else
  {
    digitalWrite(PIN_LED, HIGH);
    isKilled = false;
  }
}

/**
 * Once pathfind switch is hit, activate pathfinding mode
 */
void checkPathfindSW()
{
  if (digitalRead(PIN_PATHFIND) == HIGH)
  {
    // TODO: Switch off some indicator
    rstPathfind();
    stop();
  }
  else
  {
    // TODO: Switch on some indicator
    isPathfind = true;
  }
}

void rstEDF()
{
  analogWrite(PIN_EDF, 0);
  pwm_value = PWM_MIN; // reset pwm values
}

void rstPathfind()
{
  isPathfind = false;
  state = LEFTU_NEXT;
}
