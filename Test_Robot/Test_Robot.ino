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
#include <Stepper.h>
#include <Servo.h>
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
// #define PIN_IR
#define PIN_JOYX A0 // joystick X axis
#define PIN_JOYY A1 // joystick Y axis

// EDF Control
#define PWM_MIN 120 //133 for actual serial
#define PWM_MAX 240
#define PWM_DELAY 50 // .05s
#define PWM_STEPSIZE 1

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

int cmd; // for Serial

boolean isJoyStick;
boolean isKilled;

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
}

void loop() {
  checkKill(); // Check if kill switch is hit
  // Only execute program if not killed
  if (!isKilled)
  {
    serialControl(); // Serial control
    if (isJoyStick)
      joyStickControl(); // Joystick control
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
  // irVal = 0;

  // for (int i = 0; i < loopCount; i++)
  //   irVal += analogRead(inputIR);

  // irVal /= loopCount;
  // irDist = 12343.85 * pow(irVal, -1.15); // Linearizing eqn, accuracy +- 5%
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
    cmd = getSerial();
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
  else if (cmd == 'j') // Switch between joystick and serial
  isJoyStick = !isJoyStick;
}

/*========================================
=            Joystick Control            =
========================================*/

void joyStickControl()
{
  // // read and scale the two axes:
  int xReading = readAxis(PIN_JOYX);
  int yReading = readAxis(PIN_JOYY);
  //For RIGHT Joystick control
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
  reads an axis (0 or 1 for x or y) and scales the
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
  Serial.println(man_value);
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

