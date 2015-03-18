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
       2 |---------| 1
              |
  -90         |           90
              |
       4 |---------| 3
            (back)

             180
*/

// Assigned pin numbers for motors
#define driveMotor1_1 32
#define driveMotor1_2 33
#define driveMotor2_1 34
#define driveMotor2_2 35
#define driveMotor3_1 30
#define driveMotor3_2 31
#define driveMotor4_1 36
#define driveMotor4_2 37
#define PIN_EDF 3 // Digital PWM pin for EDF.

// Assigned FSR input pins
#define inputFSR1 12
#define inputFSR2 13
#define inputFSR3 14
#define inputFSR4 15

// Assigned encoder input pins
#define inputENC1 16
#define inputENC2 17
#define inputENC3 18
#define inputENC4 19

// Assigned input pins for IR sensor and IMU
#define inputIR 0

// Define number of readings sensors take in
#define loopCount 12

// Define IR sensor distance threshold (in cm) before turning
#define irThresh 15

// Define power of motors
#define power 255

// EDF Control
#define PWM_MIN 191
// #define PWM_MAX 220
#define PWM_DELAY 100 // .5s
#define PWM_STEPSIZE 1

// Define turn duration (used only before integration with encoders/IMU)
//#define turnDuration 2000
int turnDuration = 1650;
int turnLeftDur = 1975; // no edf - 1400
int turnRightDur = 1975; // no edf - 1450

// Initiliaze variables
int fsrVal[4];
int IMU[3];
int fsrAvg, nextDir, edfMotorInput;
unsigned long beforeTime, afterTime;
int cmd;

int irVal = 0;
float irDist = 0;
boolean turning = false;
boolean leftTurnNext = false; 

// EDF Variables
int pwm_value = PWM_MIN;
int man_value;
long pwm_timer = 0; // pwm_timer for PWM stepping


void setup()
{
  Serial.begin(115200);

  // Brushed DC Drivetrain motors
  pinMode(driveMotor1_1, OUTPUT);
  pinMode(driveMotor1_2, OUTPUT);
  pinMode(driveMotor2_1, OUTPUT);
  pinMode(driveMotor2_2, OUTPUT);
  pinMode(driveMotor3_1, OUTPUT);
  pinMode(driveMotor3_2, OUTPUT);
  pinMode(driveMotor4_1, OUTPUT);
  pinMode(driveMotor4_2, OUTPUT);
      
  // Brushless DC EDF Motor Callibration
  pinMode(PIN_EDF, OUTPUT);
  // Initialize EDF Motor
  EDF_Init();
      
  // Assign Sensor pins to Arduino
  pinMode(inputFSR1, INPUT_PULLUP);
  pinMode(inputFSR2, INPUT_PULLUP);
  pinMode(inputFSR3, INPUT_PULLUP);
  pinMode(inputFSR4, INPUT_PULLUP);
  pinMode(inputENC1, INPUT_PULLUP);
  pinMode(inputENC2, INPUT_PULLUP);
  pinMode(inputENC3, INPUT_PULLUP);
  pinMode(inputENC4, INPUT_PULLUP);
  pinMode(inputIR, INPUT);
  
  // Initialize IMU
  AHRS_Init();

  Serial.println("Initialization Completed.");
}
  
void EDF_Init()
{
    // Initialize EDF Motor
  analogWrite(PIN_EDF, 0);
  delay(PWM_DELAY);
  analogWrite(PIN_EDF, PWM_MIN);  // Engage EDF Motor
}
/******************************************************************************
 *** Main control loop ********************************************************
 *****************************************************************************/

void loop() 
{
  
  // Get feedback from FSR array and IMU
  Read_AHRS();
  readFSR();
  // TO DO: use filter on FSR/IMU feedback (optional?)
  
  // Based on FSR/IMU readings, call EDF control code to modify EDF's input
  // edfControl();
  // analogWrite(edfMotor, edfMotorInput);
  
  // read IR sensor readings and call pathfinding algorithm
  readIR();
  pathfind();

  // Serial comm to control robot
  if (Serial.available()>0) // Read cmd
  {
    cmd = getSerial();
    if (cmd == 'a') // counter clockwise
      turn(-90);
    else if (cmd == 'd') // clockwise
      turn(90);
    else if (cmd == 'w') // forward
      drive(0);
    else if (cmd == 's') // backwards
      drive(1);
    else if (cmd == 'q') // stop motor
      stopMotors();
    else if (cmd == 'p') // Stepping to a certain value
    {
      man_value = getSerial();
      while (pwm_value < man_value)
        step_PWM(1);
      while (pwm_value > man_value)
        step_PWM(-1);
    }
    else if (cmd == 'm')
      EDF_Init();
  }

  // Print IMU readings
  // printdata();

  // TO DO: ramp EDF input to max suction force while turning (ramp slowly?)
  // switch (nextDir)
  // {
  //   case 1: // next direction == forward
  //     drive(0);
  //     break;
  //   case 2: // next direction == left
  //     edfMotorInput = 255;
  //     turn(-90);
  //     break;
  //   case 3: // next direction == right
  //     edfMotorInput = 255;
  //     turn(90);
  //     break;
  //   case 4:  // next direction == reverse/backward
  //     drive(1);
  //     break;
  // }
}
  
/******************************************************************************
 *** Helper functions *********************************************************
 *****************************************************************************/

void readFSR() 
/* Function takes "loopCount" number of FSR readings across the 4 FSRs and 
 * stores the averaged value in int "fsrAvg"
 */
{
  // Reset FSR array readings
  fsrVal[0] = 0;
  fsrVal[1] = 0;
  fsrVal[2] = 0;
  fsrVal[3] = 0;
  
  for (int i = 0; i < loopCount; i++) 
  {
    fsrVal[0] += analogRead(inputFSR1);
    fsrVal[1] += analogRead(inputFSR2);
    fsrVal[2] += analogRead(inputFSR3);
    fsrVal[3] += analogRead(inputFSR4);
  }

  for (int fsrNum = 0; fsrNum < 4; fsrNum++)
    fsrVal[fsrNum] /= loopCount;

  fsrAvg = (fsrVal[0] + fsrVal[1] + fsrVal[2] + fsrVal[3]) / 4;
}

void readIR() 
/* Function takes "loopCount" number of IR sensor readings and 
 * stores the linearized value in float "irDist"
 */
{
  irVal = 0;

  for (int i = 0; i < loopCount; i++)
    irVal += analogRead(inputIR);

  irVal /= loopCount;
  irDist = 12343.85 * pow(irVal, -1.15); // Linearizing eqn, accuracy +- 5%      
}

// TO DO: Add in U-Turn to code. Currently only turning when obstacle detected
// TO DO: Change to flood-fill design to navigate around obstacles 
void pathfind()
/* Function utilizes IR sensor and detect if the robot should turn. 
 * Current implementation is of a lawnmower design.
 */
{
  if (irDist < irThresh)
  {
    if (leftTurnNext) 
      {
        nextDir = 2; // next direction == left
        leftTurnNext = false; // next turn == right
      }
    else
      {
        nextDir = 3; // next direction == right
        leftTurnNext = true; // next turn == left
      } 
  }

  else nextDir = 1; // next direction == straight ahead 
}

void edfControl() 
/* Function stores an int value between 190 - 255 based on the FSR/IMU readings
 * in "edfMotorInput"
 */
{
  // If FSR reading is too low OR if IMU pitch > 90 degrees, speed up EDF
  // Note: EDF operates between values between 190-255

  // TO DO: write control loop for EDF using FSR & IMU readings
  edfMotorInput = 191;
}

// TO DO: integrate IMU or encoder values to stop turn. Currently using time.
void turn(int turnDegree)
/* Function takes in a number ranging from -179 to 180. This number represents
 * the amount RACER is supposed to turn (in degrees). 
 */
{
  beforeTime = millis(); // turning start time
  afterTime = 0;
  turning = true; // RACER is now turning
  
  if (turnDegree < 0)  // left turn
  {
    while (afterTime < (beforeTime + turnLeftDur)) 
      {
        driveMotorsLeft();
        afterTime = millis();
      }
  }

  else if (turnDegree > 0) // right turn
  {
    while (afterTime < (beforeTime + turnRightDur)) 
      {
        driveMotorsRight();
        afterTime = millis();
      }  
  }

  else 
  // SHOULD NEVER REACH HERE
  {
    while (true) {}
  }

  stopMotors();
  turning = false; // RACER has finished turning
}

void stopMotors() 
{
  analogWrite(driveMotor1_1, LOW);
  analogWrite(driveMotor1_2, LOW);
  analogWrite(driveMotor2_1, LOW);
  analogWrite(driveMotor2_2, LOW);
  analogWrite(driveMotor3_1, LOW);
  analogWrite(driveMotor3_2, LOW);
  analogWrite(driveMotor4_1, LOW);
  analogWrite(driveMotor4_2, LOW); 
}

void driveMotorsLeft()
// Function writes values to the drive motors to turn left
{
  analogWrite(driveMotor1_1, LOW);
  analogWrite(driveMotor1_2, 200);
  analogWrite(driveMotor2_1, LOW);
  analogWrite(driveMotor2_2, 250);
  analogWrite(driveMotor3_1, LOW);
  analogWrite(driveMotor3_2, 200);
  analogWrite(driveMotor4_1, LOW);
  analogWrite(driveMotor4_2, 250); 
} 

void driveMotorsRight()
// Function writes values to the drive motors to turn right
{
  analogWrite(driveMotor1_1, 255);
  analogWrite(driveMotor1_2, LOW);
  analogWrite(driveMotor2_1, 200);
  analogWrite(driveMotor2_2, LOW);
  analogWrite(driveMotor3_1, 255);
  analogWrite(driveMotor3_2, LOW);
  analogWrite(driveMotor4_1, 200);
  analogWrite(driveMotor4_2, LOW); 
} 
  
void drive(int dir)
/* Function takes in a number (0 or 1) to determine if RACER drives forward
 * or backwards.
 */
{
  if (dir == 0) // power all drive motors equally to drive straight forward
  {
    analogWrite(driveMotor1_1, LOW);
    analogWrite(driveMotor1_2, power);
    analogWrite(driveMotor2_1, power);
    analogWrite(driveMotor2_2, LOW);
    analogWrite(driveMotor3_1, LOW);
    analogWrite(driveMotor3_2, power);
    analogWrite(driveMotor4_1, power);
    analogWrite(driveMotor4_2, LOW);     
  }
  else if (dir == 1) // power all drive motors equally to drive backward
  {
    analogWrite(driveMotor1_1, power);
    analogWrite(driveMotor1_2, LOW);
    analogWrite(driveMotor2_1, LOW);
    analogWrite(driveMotor2_2, power);
    analogWrite(driveMotor3_1, power);
    analogWrite(driveMotor3_2, LOW);
    analogWrite(driveMotor4_1, LOW);
    analogWrite(driveMotor4_2, power);     
  }
}

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


void step_PWM(int dir)
{
  if((millis()-pwm_timer)>=PWM_DELAY) // step at 1/PWM_DELAY Hz
  {
    pwm_value += dir*PWM_STEPSIZE;
    analogWrite(PIN_EDF, pwm_value); // Send PWM value to ESC
    pwm_timer = millis(); // Update timer

    Serial.print("PWM_Value: ");
    Serial.println(pwm_value);
  }
}


/******************************************************************************
 *** End of code **************************************************************
 *****************************************************************************/
