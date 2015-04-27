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

// Analog and Digital I/O Pin Assignment
#define rightDriveMotor1   9  // up
#define rightDriveMotor2   10
#define leftDriveMotor1    5  // up
#define leftDriveMotor2    6

#define rightLinAct1       3  // contract
#define rightLinAct2       2
#define leftLinAct1        12 // contract
#define leftLinAct2        13

#define rightWindowSwitch  9
#define leftWindowSwitch   10
#define frameSwitch        11
#define startSwitch        12
#define IMU                13

// Constants
#define linActPWM 255

// Obstacles ?
boolean obstacle = true;

// initialize some variables
int rightWindowSwitchVal, leftWindowSwitchVal, frameSwitchVal, startSwitchVal; 
float roll, pitch;

void setup() {
  
  // declare output pins
  pinMode(rightDriveMotor1, OUTPUT);
  pinMode(rightDriveMotor2, OUTPUT);
  pinMode(leftDriveMotor1, OUTPUT);
  pinMode(leftDriveMotor2, OUTPUT);
  pinMode(rightLinAct1, OUTPUT);
  pinMode(rightLinAct2, OUTPUT);
  pinMode(leftLinAct1, OUTPUT);
  pinMode(leftLinAct2, OUTPUT);
 
  // declare input pins
  pinMode(rightWindowSwitch, INPUT_PULLUP);
  pinMode(leftWindowSwitch, INPUT_PULLUP);
  pinMode(frameSwitch, INPUT_PULLUP);
  pinMode(startSwitch, INPUT_PULLUP);
  
  // start Serial connection process
  Serial.begin(115200);
}

void loop() {
  // test some stuff
  
  // ----- POWER ON -----
  // right side
  analogWrite(rightLinAct1, 0);
  analogWrite(rightLinAct2, 0); 
  analogWrite(rightDriveMotor1, 0);
  analogWrite(rightDriveMotor2, 150);
  // left side
  analogWrite(leftLinAct1, 0);
  analogWrite(leftLinAct2, 0); 
  analogWrite(leftDriveMotor1, 0);
  analogWrite(leftDriveMotor2, 150);
  delay(2000); 
  
  // ----- POWER OFF -----
  // right side
  analogWrite(rightLinAct1, 0);
  analogWrite(rightLinAct2, 0); 
  analogWrite(rightDriveMotor1, 0);
  analogWrite(rightDriveMotor2, 0);
  // left side
  analogWrite(leftLinAct1, 0);
  analogWrite(leftLinAct2, 0); 
  analogWrite(leftDriveMotor1, 0);
  analogWrite(leftDriveMotor2, 0);
  delay(500);
  
  
  

}
