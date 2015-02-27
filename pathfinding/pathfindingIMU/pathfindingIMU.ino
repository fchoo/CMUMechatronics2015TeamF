/**
*
* This code includes software based on MinIMU-9-Arduino-AHRS by Pololu.
*
* Key functions include:
* AHRS_Init() - Initialize the AHRS
* Read_AHRS() - Update roll, pitch, yaw angles in AHRS
* Get_Roll()  - Get Roll angle in AHRS
* Get_Pitch()  - Get Pitch angle in AHRS
* Get_Yaw()  - Get Yaw angle in AHRS
*
**/

// Include libraries
  /* None for now */

// Analog Pins
#define irPin 0            // analog pin  for IR sensor readings

// Digital Pins
#define startButton 8      // digital pin for start button, high (1) == off, low (0) == on
#define STATUS_LED 13

// Fixed constants  
#define loopCount 12       // loop count for averaging IR readings
#define startDebounceDelay 500
#define moveDelay 0
#define LEDduration 1000
#define turnForwardDuration 1000
#define turnDist 15
#define stopDist 15

/* Mode selection for turns:
 * 0 - pitch (IMU is vertical)
 * 1 - yaw   (IMU is horizontal - flat on ground)
 * 2 - pitch, debug mode (print statements on Serial)
 * 3 - yaw, debug mode
 */
#define pitchYaw 1 // 

// Initialize global variables
boolean startReading = false;
boolean start = false;
boolean lastLap = false;
int irVal, curYaw, curPitch, aftAngle;
unsigned long beforeTime, afterTime, lastStartTime, curStartTime;
float dist;
String curDir = "up";  // starting position - up. other possible values include "left", "right", and "down"
int initYaw = -361;

int doF = 5;           // degrees of freedom (error margin of angle, in degrees);
int leftRightTurn = 1; // next turn: left == 0, next turn: right == 1;
                       // starting from bottom left corner, turn right first
boolean initialStartString = false;

void setup() {
  
  // Set and define pins
  pinMode(irPin, INPUT);
  pinMode(startButton, INPUT);
  pinMode (STATUS_LED,OUTPUT);
  
  // Open serial port
  Serial.begin(115200); 

  // Initialization and calibration of IMU  
  digitalWrite(STATUS_LED,LOW); // OFF - Calibrating
  AHRS_Init();
  digitalWrite(STATUS_LED,HIGH); // ON - Calibration done
}

void loop() {
  
  // Start "button"
  startReading = digitalRead(startButton);
  if (startReading == true) {
    if (start == false) {
      Serial.println("START");
      start = true;
      initYaw = -361;
    }
    
    // Get readings from IMU and IR sensor
    readIMU();
    readIRsensor();
    
    if (dist < turnDist) {
      if (lastLap) moveStop();
      else if ((leftRightTurn == 0) && (pitchYaw == 0)) uTurnLeftPitch();
      else if ((leftRightTurn == 1) && (pitchYaw == 0)) uTurnRightPitch();
      
      else if ((leftRightTurn == 0) && (pitchYaw == 1)) uTurnLeftYaw();
      else if ((leftRightTurn == 1) && (pitchYaw == 1)) uTurnRightYaw();
      leftRightTurn ^= 1;
    }
    else moveForward();
  }
  
  else {       // startReading == false
    Serial.println("STOP");
    start = false;
  }
}

/************************************************
 *** Helper Functions ***************************
 ************************************************/

void readIRsensor() {
  irVal = 0;
  for (int i = 0; i < loopCount; i++) {
    irVal += analogRead(irPin);
  }
  irVal /= loopCount;

  dist = 12343.85 * pow(irVal, -1.15);  // linearize the value
//  Serial.print("dist = ");
//  Serial.println(dist);
}

void readIMU() {
  Read_AHRS();
  if (pitchYaw == 0) curPitch = abs(Get_Pitch());
  else if (pitchYaw == 1) curYaw = abs(Get_Yaw());
  else if (pitchYaw == 2) {
    curPitch = abs(Get_Pitch());
    Serial.print("curPitch = ");
    Serial.println(curPitch);  
  }
  else if (pitchYaw == 3) {
    curYaw = abs(Get_Yaw());
    Serial.print("curYaw = ");
    Serial.println(curYaw);  
  }
}

int getYaw() {
  Read_AHRS();
  int i = Get_Yaw();
  if (i < 0) i += 360;
//  Serial.print("getYaw rets ");
//  Serial.println(i);
  return i;
}

void moveForward() {
}

void moveBackward() {
  Serial.println("BACK");
}

void moveStop() {
  Serial.println("STOP");
  while (true);
}

void uTurnLeftYaw() {
  Serial.println("u turn left");
  Serial.println("PAUSE");
  leftTurnYaw(90);   // turn left based on yaw by 90 deg
  
  readIRsensor();
  if (dist < stopDist) lastLap = true;
  delay(moveDelay);
  delay(turnForwardDuration);
  Serial.println("PAUSE");
  leftTurnYaw(90);   // turn left by 90 deg to complete u-turn
}

void uTurnLeftPitch() {
  Serial.println("u turn left");
  leftTurnPitch(90);   // turn left based on pitch by 90 deg
  
  readIRsensor();
  if (dist < stopDist) lastLap = true;
  delay(moveDelay);
  delay(turnForwardDuration);
  
  leftTurnPitch(90); 
}

void uTurnRightYaw() {
  Serial.println("u turn right");
  Serial.println("PAUSE");
  rightTurnYaw(90);   // turn right based on yaw by 90 deg
  
  readIRsensor();
  if (dist < stopDist) lastLap = true;
  delay(moveDelay);
  delay(turnForwardDuration);
  
  Serial.println("PAUSE");
  rightTurnYaw(90);   // turn right by 90 deg to complete u-turn
}

void uTurnRightPitch() {
  rightTurnPitch(90);   // turn right based on pitch by 90 deg
  
  readIRsensor();
  if (dist < stopDist) lastLap = true;
  delay(moveDelay);
  delay(turnForwardDuration);
  
  rightTurnPitch(90); 
}

void leftTurnYaw(int desiredDeltaAngle) {  
  int curAngle = getYaw();
  int initAngle = curAngle;
  
  int deltaAngle = 0; // no change in angle initially
  while (deltaAngle < (desiredDeltaAngle-doF)) {
    // wheels turn left here
    Read_AHRS();
    curAngle = getYaw();
    deltaAngle = abs(initAngle - curAngle);
    if (deltaAngle > 180) deltaAngle = 360 - deltaAngle;
  }
  Serial.println("LEFT");
  
  Serial.print("turned left by ");
  Serial.println(desiredDeltaAngle);
}

void leftTurnPitch(int angle) {
}

void rightTurnYaw(int desiredDeltaAngle) {
  int curAngle = getYaw();
  int initAngle = curAngle;
  
  int deltaAngle = 0; // no change in angle initially
  while (deltaAngle < (desiredDeltaAngle-doF)) {
    // wheels turn left here
    Read_AHRS();  
    curAngle = Get_Yaw();
    deltaAngle = (abs(curAngle - initAngle))%360;
//    Serial.println(deltaAngle);
    if (deltaAngle > 180) deltaAngle = 360 - deltaAngle;
  }
  Serial.println("RIGHT");
  
  Serial.print("turned right by ");
  Serial.println(desiredDeltaAngle);
}

void rightTurnPitch(int angle) {
}

// code ends here
