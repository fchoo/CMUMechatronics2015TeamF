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
 * LAST REVISION: 03/11/2015
 *
 * This is the overall code used for the final design of RACER. It takes in
 * sensor readings from the inertial measurement unit (IMU), force-senstiive
 * resistor (FSR), infrared (IR) sensor, and uses these readings to navigate
 * the RACER robot around the window to clean it.
 *
 *****************************************************************************/

// Libraries used for RACER
#include <math.h>

/*
  Assigned motor numbers for RACER
              0

           (front)
              |
              |
  -90  1 |---------| 2  90
              |
              |
            (back)

             180
*/

// Assigned pin numbers
// For Motors
#define PIN_MOTOR_1_1 3
#define PIN_MOTOR_1_2 5
#define PIN_MOTOR_2_1 6
#define PIN_MOTOR_2_2 9
#define PIN_EDF 10  // Digital PWM pin for EDF.
#define PIN_POT 2
#define PIN_KILL 4
#define PIN_IR A5
#define PIN_JOYX A0 // joystick X axis
#define PIN_JOYY A1 // joystick Y axis

// EDF Control
#define PWM_MIN 120 //133 for actual serial
#define PWM_MAX 240
#define PWM_DELAY 50 // .05s
#define PWM_STEPSIZE 1

// Define number of readings sensors take in
#define AVGFILTER_NUM 12

// Define IR sensor distance threshold (in cm) before turning
#define THRESHOLD_IR 15

// Motor
#define MOTOR_TORQ 200

// Variables for joysticks
int potValue;                 // to control EDF

// Parameters for joysticks
int joy_range = 12;               // output range of X or Y movement
int responseDelay = 5;        // response delay of the mouse, in ms
int threshold = joy_range/4;      // resting threshold
int center = joy_range/2;         // resting position value

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

int cmd; // for Serial


int state = 10; 
/*
  1 - Move forward; left U-turn next
  2 - Left turn
  3 - Straight ahead
  4 - Left turn
  5 - Move forward; right U-turn next
  6 - Right turn
  7 - Straight ahead
  8 - Right turn
  9 - Move forward; last lap
  10 - STOP
 */

boolean isJoyStick, isPathfind;
boolean isKilled;
boolean irFlag = false;
boolean encoderFlag = false;

void setup() {
  Serial.begin(9600);

  //for motors
  pinMode(PIN_POT, INPUT);
  pinMode(PIN_MOTOR_1_1,OUTPUT); analogWrite(PIN_MOTOR_1_1,LOW);
  pinMode(PIN_MOTOR_1_2,OUTPUT); analogWrite(PIN_MOTOR_1_2,LOW);
  pinMode(PIN_MOTOR_2_1,OUTPUT); analogWrite(PIN_MOTOR_2_1,LOW);
  pinMode(PIN_MOTOR_2_2,OUTPUT); analogWrite(PIN_MOTOR_2_2,LOW);

  //for EDF
  pinMode(PIN_EDF,OUTPUT);
  EDF_Init();

  // kill switch
  pinMode(PIN_KILL, INPUT);
  isJoyStick = true; // default to joystick
  isKilled = false;
  attachInterrupt(0, updateTick, RISING);
}

void loop() {
  checkKill(); // Check if kill switch is hit
  
  // Only execute program if not killed
  if (!isKilled)
  {
    serialControl(); // Serial control
    if (isJoyStick)
      joyStickControl(); // Joystick control
    if (isPathfind) 
      state = 1;
      updateFlags();
      updateState();
  }
}


/*===============================
=            SENSORS            =
===============================*/

void updateFlags()
{
  switch (state) 
  {
    case 1:
    case 5: 
            readIR();
            if (irDist < THRESHOLD_IR) irFlag = true;
            break();
    case 2:
    case 3:
    case 4:
    case 6:
    case 7:
    case 8:
            checkEncoder();
            if (targetDist < curDist) encoderFlag = true;
            break;
  }
}

void readIR()
/* Function takes "loopCount" number of IR sensor readings and
 * stores the linearized value in float "irDist"
 */
{
  irVal = 0;

  for (int i = 0; i < AVGFILTER_NUM; i++)
    irVal += analogRead(PIN_IR);

  irVal /= AVGFILTER_NUM;
  irDist = 12343.85 * pow(irVal, -1.15); // Linearizing eqn, accuracy +- 5%
}

void checkEncoder()
{
  curDist = n_tick/5000*360/360*21.5; 
}

void updateTick()
{
  n_tick++;
}

/*==================================
=            Locomotion            =
==================================*/

void moveLeft()
{
  analogWrite(3,MOTOR_TORQ);
  analogWrite(5,0);
  analogWrite(6,MOTOR_TORQ);
  analogWrite(9,0);
}
void moveRight()
{
  analogWrite(3,0);
  analogWrite(5,MOTOR_TORQ);
  analogWrite(6,0);
  analogWrite(9,MOTOR_TORQ);
}
void moveForward()
{
  analogWrite(3,0);
  analogWrite(5,MOTOR_TORQ);
  analogWrite(6,MOTOR_TORQ);
  analogWrite(9,0);
}
void moveBack()
{
  analogWrite(3,MOTOR_TORQ);
  analogWrite(5,0);
  analogWrite(6,0);
  analogWrite(9,MOTOR_TORQ);
}
void stop()
{
  analogWrite(3,0);
  analogWrite(5,0);
  analogWrite(6,0);
  analogWrite(9,0);
}

/*===========================
=            EDF            =
===========================*/

void EDF_Init()
{
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

void updateState()
{
  switch (state)
  {
    case 1: // move forward, left u-turn next
            if (irFlag) state = 2; 
            irFlag = false;
            targetDist = 6; n_tick = 0; // reset thresholds
            break;

    case 2: // left turn
            if (encoderFlag) state = 3; 
            encoderFlag = false;
            targetDist = 10; n_tick = 0; // reset thresholds
            break;
    case 3: // straight
            if (encoderFlag) state = 4; 
            encoderFlag = false;
            targetDist = 6; n_tick = 0; // reset thresholds
            break;
    case 4: // left turn
            if (encoderFlag) state = 5;
            encoderFlag = false;
            break;
    case 5: // move forward, right u-turn next
            if (irFlag) state = 6;
            irFlag = false;
            targetDist = 6; n_tick = 0; // reset thresholds            
            break;
    case 6: // right turn
            if (encoderFlag) state = 7;
            encoderFlag = false;
            targetDist = 10; n_tick = 0; // reset thresholds
            break;
    case 7: // straight
            if (encoderFlag) state = 8;
            encoderFlag = false;
            targetDist = 6; n_tick = 0; // reset thresholds
            break;
    case 8: // right turn
            if (encoderFlag) state = 1;
            encoderFlag = false;
            break;
    case 9: break;
    case 10: break;
  }
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
      Serial.print("MODE: ");
      if (isJoyStick) Serial.println("JoyStick");
      else Serial.println("Serial");
    }
    if (cmd == 'z') // Activate pathfinding
    {
      isPathfind = !isPathFind;
      Serial.println("Mode: Pathfinding Algo");
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

  delay(responseDelay);
  POT2PWM();
}

/*
  Reads an axis (0 or 1 for x or y) and scales the
  analog input range to a range from 0 to <range>
 */
int readAxis(int thisAxis) {
  // read the analog input:
  int reading = analogRead(thisAxis);

  // map the reading from the analog input range to the output range:
  reading = map(reading, 0, 1023, 0, joy_range);

  // if the output reading is outside from the
  // rest position threshold,  use it:
  int distance = reading - center;

  if (abs(distance) < threshold) {
    distance = 0;
  }

  // return the distance for this axis:
  return distance;
}

void POT2PWM()
{
  potValue = analogRead(PIN_POT);
  man_value = map(potValue, 0, 1023, PWM_MIN, PWM_MAX);
  while (pwm_value < man_value)
    step_PWM(1);
  while (pwm_value > man_value)
    step_PWM(-1);
}

/*============================
=            Misc            =
============================*/


void checkKill()
{
  if (digitalRead(PIN_KILL) == HIGH) {
    isKilled = true;
    analogWrite(PIN_EDF, 0); // kill edf
    stop(); // kill all motors
  }
}

