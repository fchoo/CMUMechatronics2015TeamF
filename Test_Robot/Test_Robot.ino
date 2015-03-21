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
boolean irFlag, encoderFlag;

// Mode
boolean isJoyStick;
boolean isKilled;
boolean isPathfind;

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
  isKilled = false;

  // Default mode
  isPathfind = true;
  isJoyStick = false;

  // Interrupts initialization
  irFlag = false;
  encoderFlag = false;
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
    Serial.println("KILLED");
    return; // do nothing once killed
  }
  
  // Feedback controls for motor speed
  motorFeedback();

  // Read Sensors
  read_IMU();

  serialControl(); // Serial control
  if (isJoyStick == true)  // Joystick control
    joyStickControl();
  if (isPathfind == true) // Pathfinding control
  {
    updateFlags();
    pathfindingFSM();
  }
}


/*===============================
=            SENSORS            =
===============================*/

void readIR()
/* Function takes "loopCount" number of IR sensor readings and
 * stores the linearized value in float "irDist"
 */
{
  irVal = 0;

  for (int i = 0; i < FILTER_AVG; i++)
    irVal += analogRead(PIN_IR);

  irVal /= FILTER_AVG;
  irDist = 12343.85 * pow(irVal, -1.15); // Linearizing eqn, accuracy +- 5%
}

void checkEncoder()
{
  curDist = n_tick/5000*360/360*21.5;
}

void updateRightTick()
{
  n_tick++;
  rightWheelTicks++;
}

void updateLeftTick()
{
  leftWheelTicks++;
}

/*==================================
=            Locomotion            =
==================================*/

void motorFeedback()
{
  if ((state == LEFTU_NEXT) || (state == RIGHTU_NEXT))
  {
    torq_straight_1 = torq_straight_1;
    torq_straight_2 = torq_straight_2;
  }
}

void motor_init()
{
  //for motors
  pinMode(PIN_MOTOR_1_1,OUTPUT); analogWrite(PIN_MOTOR_1_1,LOW);
  pinMode(PIN_MOTOR_1_2,OUTPUT); analogWrite(PIN_MOTOR_1_2,LOW);
  pinMode(PIN_MOTOR_2_1,OUTPUT); analogWrite(PIN_MOTOR_2_1,LOW);
  pinMode(PIN_MOTOR_2_2,OUTPUT); analogWrite(PIN_MOTOR_2_2,LOW);
}

void moveLeft()
{
  if (DEBUG) Serial.println("[INFO] Moving left.");
  analogWrite(3,TORQ_TURN);
  analogWrite(5,0);
  analogWrite(6,TORQ_TURN);
  analogWrite(9,0);
}

void moveRight()
{
  if (DEBUG) Serial.println("[INFO] Moving right.");
  analogWrite(3,0);
  analogWrite(5,TORQ_TURN);
  analogWrite(6,0);
  analogWrite(9,TORQ_TURN);
}

void moveForward()
{
  if (DEBUG) Serial.println("[INFO] Moving forward.");
  analogWrite(3,0);
  analogWrite(5,torq_straight_1);
  analogWrite(6,torq_straight_2);
  analogWrite(9,0);
}

void moveBack()
{
  if (DEBUG) Serial.println("[INFO] Moving back.");
  analogWrite(3,torq_straight_1);
  analogWrite(5,0);
  analogWrite(6,0);
  analogWrite(9,torq_straight_2);
}

void stop()
{
  if (DEBUG) Serial.println("[INFO] Stopping.");
  analogWrite(3,0);
  analogWrite(5,0);
  analogWrite(6,0);
  analogWrite(9,0);
}

/*===========================
=            EDF            =
===========================*/

void EDF_init()
{
  pinMode(PIN_EDF,OUTPUT);
  // Initialize EDF Motor
  analogWrite(PIN_EDF, 0);
  delay(PWM_DELAY);
  analogWrite(PIN_EDF, PWM_MIN);  // Engage EDF Motor
}

void step_PWM(int dir)
{
  if((millis()-pwm_timer)>=PWM_DELAY) // step at 1/PWM_DELAY Hz
  {
    pwm_value += dir*PWM_STEPSIZE;
    analogWrite(PIN_EDF, pwm_value); // Send PWM value to ESC
    pwm_timer = millis(); // Update timer
  }
}

/*=======================================*
 *           Pathfinding                 *
 *=======================================*/
 
void updateFlags()
{
  // Serial.print("[INFO] State: ");
  // Serial.println(state);
  switch (state)
  {
    // Waiting for U-turn
    case LEFTU_NEXT:
    case RIGHTU_NEXT:
      readIR();
      if (irDist < THRESHOLD_IR) irFlag = true;
      break;
    // In U-turn
    case LEFTU_1:
    case LEFTU_2:
    case LEFTU_3:
    case RIGHTU_1:
    case RIGHTU_2:
    case RIGHTU_3:
      checkEncoder();
      if (targetDist < curDist) encoderFlag = true;
      break;
  }
}

void pathfindingFSM()
{
  switch (state)
  {
    case LEFTU_NEXT: // move forward, left u-turn next
      moveForward();
      if (irFlag)
      {
        state = LEFTU_1;
        irFlag = false;
        setTargetDist(DIST_TURN90);
        stop();
      }
      break;
    case LEFTU_1: // left turn
      moveLeft();
      if (encoderFlag)
      {
        state = LEFTU_2;
        encoderFlag = false;
        setTargetDist(DIST_UFOR);
        stop();
      }
      break;
    case LEFTU_2: // straight
      moveForward();
      if (encoderFlag)
      {
        state = LEFTU_3;
        encoderFlag = false;
        setTargetDist(DIST_TURN90);
        stop();
      }
      break;
    case LEFTU_3: // left turn
      moveLeft();
      if (encoderFlag)
      {
        state = RIGHTU_NEXT;
        encoderFlag = false;
        stop();
      }
      break;
    case RIGHTU_NEXT: // move forward, right u-turn next
      moveForward();
      if (irFlag)
      {
        state = RIGHTU_1;
        irFlag = false;
        setTargetDist(DIST_TURN90);
        stop();
      }
      break;
    case RIGHTU_1: // right turn
      moveRight();
      if (encoderFlag)
      {
        state = RIGHTU_2;
        encoderFlag = false;
        setTargetDist(DIST_UFOR);
        stop();
      }
      break;
    case RIGHTU_2: // straight
      moveForward();
      if (encoderFlag)
      {
        state = RIGHTU_3;
        encoderFlag = false;
        setTargetDist(DIST_TURN90);
        stop();
      }
      break;
    case RIGHTU_3: // right turn
      moveRight();
      if (encoderFlag)
      {
        state = LEFTU_NEXT;
        encoderFlag = false;
        stop();
      }
      break;
  }
}

void setTargetDist(float dist)
{
  targetDist = dist;
  n_tick = 0;
}

/*======================================
=            Serial Control            =
======================================*/

int getSerial()
{
  int serialData = 0;
  int aChar = 0;
  while (aChar != '/')
  {
    aChar = Serial.read();
    if (aChar >= '0' && aChar <= '9')
      serialData = serialData * 10 + aChar - '0';
    else if (aChar >= 'a' && aChar <= 'z')
      serialData = aChar;
  }
  return serialData;
}

void serialControl()
{
  // Serial comm to control robot
  if (Serial.available()>0) // Read cmd
  {
    cmd = getSerial();
    if (cmd == 'j') // Switch between joystick and serial
    {
      isJoyStick = !isJoyStick;
      if (isJoyStick) isPathfind = false;
      Serial.print("[INFO] Mode: ");
      if (isJoyStick) Serial.println("JoyStick");
      else Serial.println("Serial");
    }
    else if (cmd == 'z') // Activate pathfinding
    {
      isPathfind = !isPathfind;
      if (isPathfind) isJoyStick = false;
      Serial.print("[INFO] Mode: ");
      if (isPathfind) Serial.println("Pathfinding");
      else Serial.println("Serial");
    }
    else if (cmd == 'r')
    {
      state = LEFTU_NEXT;
      encoderFlag = false;
      irFlag = false;
      targetDist = 0;
      n_tick = 0;
    }
  }
  if (cmd == 'a') // counter clockwise
    moveLeft();
  else if (cmd == 'd') // clockwise
    moveRight();
  else if (cmd == 'w') // forward
    moveForward();
  else if (cmd == 's') // backwards
    moveBack();
  else if (cmd == 'q') // stop motor
    stop();
}

/*========================================
=            Joystick Control            =
========================================*/

void joyStickControl()
{
  // read and scale the two axes:
  int xReading = readAxis(PIN_JOYX);
  int yReading = readAxis(PIN_JOYY);
  // For RIGHT Joystick control
  if(yReading>4)
    moveLeft();
  else if(yReading<-4)
    moveRight();
   else if(xReading>4)
    moveBack();
  else if(xReading<-4)
    moveForward();
  else
    stop();

  delay(JOY_DELAY);
  POT2PWM();
}

/**
 * Reads an axis (0 or 1 for x or y) and scales the
 * analog input range to a range from 0 to <range>
 */
int readAxis(int thisAxis) {
  // read the analog input:
  int reading = analogRead(thisAxis);

  // map the reading from the analog input range to
  // the output range
  reading = map(reading, 0, 1023, 0, JOY_RANGE);

  // if the output reading is outside from the
  // rest position threshold,  use it:
  int distance = reading - JOY_CENTER;

  if (abs(distance) < JOY_THRESHOLD) {
    distance = 0;
  }

  // return the distance for this axis:
  return distance;
}

/**
 * Use POT values to control EDF speed
 */
void POT2PWM()
{
  potValue = analogRead(PIN_POT);
  man_value = map(potValue, 0, 1023, PWM_MIN, PWM_MAX);
  if (pwm_value < man_value)
    step_PWM(1);
  if (pwm_value > man_value)
    step_PWM(-1);
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

