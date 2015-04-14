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
int edf_1_val = EDF_MIN;
int edf_2_val = EDF_MIN;
int edf_1_mval, edf_2_mval;
long edf_1_utime = 0; // last update time for edf 1
long edf_2_utime = 0; // last update time for edf 2
int edf_id;

// Pump Variables
long pump_utime;
int pump_state;

// IR variables
float irVal = 0;
float irDist = 0;
float turnMinVal = 0;

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

// Debounce button for rst imu
int rstIMU_value_cur;
int rstIMU_value_old;
long rstIMU_timer;
int rstIMU_state;

// Serial
int cmd = 'q';
int test_id;

// FSM
State state;
boolean irFlag = false;

// Mode
boolean isKilled = false;
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
  // kill switch
  pinMode(PIN_KILL, INPUT);
  // Interrupts initialization
  // attachInterrupt(0, updateLeftTick, RISING);
  attachInterrupt(4, updateRightTick, RISING);
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
  if (isDiagnostic)
  {
    LED_rst();
    digitalWrite(PIN_RED1, LOW);
    digitalWrite(PIN_RED2, LOW);
    diagnosticCheck();
  }
  else {
    checkKillSW(); // Check if kill switch is hit
    LEDcontrol(); // Set indicators
  }
  if (isKilled)
    return; // do nothing once killed

  checkPathfindSW();
  checkRstIMUBut();

  // Read Sensors
  readIMU();
  updateAngles();
  checkVertical();
  checkHeading();
  readIR();

  // Read controls
  serialControl(); // Serial control
  if (isPathfind == true)
  {
    pathfindingFSM();
    pump_control();
  }

  // Feedback controls
  // edfFeedback();
  // motorFeedback();

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
  // Serial.print("[INFO] State: ");
  // Serial.print(StateStrings[state]);
  // Serial.print(" Vertical: ");
  // Serial.print(isVert);
  // Serial.print(" Heading: ");
  // Serial.print(HeadingStrings[curDir]);
  // Serial.print(" Roll: ");
  // Serial.print(roll);
  // Serial.print(" Pitch: ");
  // Serial.print(pitch);
  // Serial.print(" IRdist ");
  // Serial.println(irDist);
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
  if (isPathfind == true)
  {
    if (state == LEFTU_NEXT || state == RIGHTU_NEXT)
    {
      digitalWrite(PIN_GREEN1, HIGH);
      digitalWrite(PIN_GREEN2, HIGH);
    }
    // left U turn
    else if (state == LEFTU_1 ||
             state == LEFTU_2 ||
             state == LEFTU_3)
    {
      digitalWrite(PIN_GREEN1, HIGH);
    }
    // right U turn
    else if (state == RIGHTU_1 ||
             state == RIGHTU_2 ||
             state == RIGHTU_3)
    {
      digitalWrite(PIN_GREEN2, HIGH);
    }
    // stop
    else if (state == STOP)
    {
      digitalWrite(PIN_RED1, HIGH);
      digitalWrite(PIN_RED2, HIGH);      
    }
  }
  // BLUE LED
  if (isKilled)
    digitalWrite(PIN_BLUE, LOW);
  else
    digitalWrite(PIN_BLUE, HIGH);
  // RED LED
  // TODO: set to stop state
}

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
    cmd = 'q'; // stop all cmd
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
  if (digitalRead(PIN_PATHFIND) == LOW)
  {
    rstPathfind();
    stop();
  }
  else
  {
    isPathfind = true;
  }
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
        // TODO: Remove leds
        digitalWrite(PIN_RED1, HIGH);
        digitalWrite(PIN_RED2, HIGH);
        rstIMU_state = !rstIMU_state;
        rstIMU();
        digitalWrite(PIN_RED1, LOW);
        digitalWrite(PIN_RED2, LOW);
        Serial.println("[INFO] Resetting IMU");
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
  analogWrite(PIN_EDF_1, 0);
  analogWrite(PIN_EDF_2, 0);
  edf_1_val = EDF_MIN;
  edf_2_val = EDF_MIN;
}

void rstPathfind()
{
  isPathfind = false;
  state = LEFTU_NEXT;
}

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
