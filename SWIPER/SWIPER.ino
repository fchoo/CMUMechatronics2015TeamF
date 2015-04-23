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
#define rightDriveMotor1   1
#define rightDriveMotor2   2
#define leftDriveMotor1    3
#define leftDriveMotor2    4
#define rightLinAct1       5
#define rightLinAct2       6
#define leftLinAct1        7
#define leftLinAct2        8
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
  
  // Read & Update sensor values
  updateSensors();
    
  // Check (or wait) if start switch to trigger, then begin the operating sequence
  if ( startSwitchVal == 1 )
  {
    // Check if the cleaning pad is in contact with the window --> extend linear Actuators if not
    checkWindowContact(rightWindowSwitchVal, leftWindowSwitchVal); 
    
    // Drive up the window and stop when it reaches the top
    Drive(rightWindowSwitchVal, leftWindowSwitchVal);
  }

}


int updateSensors()
{
  // re-initialize sensor variables
  rightWindowSwitchVal = 0; leftWindowSwitchVal = 0; frameSwitchVal = 0; startSwitchVal = 0; 
  
  // update values
  rightWindowSwitchVal = digitalRead(rightWindowSwitch);
  leftWindowSwitchVal = digitalRead(leftWindowSwitch);
  frameSwitchVal = digitalRead(frameSwitch);
  startSwitchVal = digitalRead(startSwitch);
  
  return rightWindowSwitchVal, leftWindowSwitchVal, frameSwitchVal, startSwitchVal; 
}

void checkWindowContact( int rightWindowSwitchVal, int leftWindowSwitchVal)
{
    // ----------------------------------------------------------------------------
    // ----------- CHECK IF PAD IS TOUCHING WINDOW --------------------------------
    // ----------------------------------------------------------------------------
    if ( rightWindowSwitchVal == 0 ) // check right switch
    {
      // extend linear actuator arm until switch is triggered (until pad is touching window)
      analogWrite(rightLinAct1, linActPWM);
      analogWrite(rightLinAct2, 0);
    }  
    else
    {
      // hold linear actuator position
      analogWrite(rightLinAct1, 0);
      analogWrite(rightLinAct2, 0); 
    }
    
    if ( leftWindowSwitchVal == 0 ) // check left switch
    {
      // extend linear actuator arm until switch is triggered (until pad is touching window)
      analogWrite(leftLinAct1, linActPWM);
      analogWrite(leftLinAct2, 0);
    }  
    else
    {
      // hold linear actuator position
      analogWrite(leftLinAct1, 0);
      analogWrite(leftLinAct2, 0); 
    }
  
}

void Drive( int rightWindowSwitchVal, int leftWindowSwitchVal )
{
  // check if cleaning pad is making contact on both sides of the window
  if ( rightWindowSwitchVal == 1 && leftWindowSwitchVal == 1 && frameSwitch == 0 )
  {
    // add IMU stuff in to account for roll (one wheel higher than the other) by adjusting
    // motor speed of each side to level out the bar
    int rightMotorPWM = 255;
    int leftMotorPWM = 255;
    
    // right side
    analogWrite(rightDriveMotor1, rightMotorPWM);
    analogWrite(rightDriveMotor2, 0);  
    // left side
    analogWrite(rightDriveMotor1, rightMotorPWM);
    analogWrite(rightDriveMotor2, 0);
  }
  else
  {
    // right side
    analogWrite(rightDriveMotor1, 0);
    analogWrite(rightDriveMotor2, 0);  
    // left side
    analogWrite(rightDriveMotor1, 0);
    analogWrite(rightDriveMotor2, 0); 
  } 
  
  
}
