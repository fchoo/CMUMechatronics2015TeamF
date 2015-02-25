import processing.serial.*; // import serial library

// Initialize Global Variables
Serial arduinoPort;

// Canvas Data
int maxWindowH = 153*5;   // 153cm == 5 feet, 5 pixels per cm
int maxWindowW = 183*5;   // 183cm == 6 feet, 5 pixels per cm
int border     = 50;      // 50 pixels border for canvas
int canvasH    = maxWindowH + 2*border;
int canvasW    = maxWindowW + 2*border;

// Global variables
int robotRadii = 3;
int curX = border + robotRadii;              // starting pos: bottom left corner
int curY = canvasH - border - robotRadii; 
String curDir = "up";

// Program Options
boolean pause = false;

// Program variables
String nextDir;
boolean started = false;
int beforeTime, afterTime;
int delayDuration = 1000;

/*************************************************************************
 ******* START OF MAIN FUNCTION ******************************************
 *************************************************************************/

void setup() {
  size(canvasW, canvasH, P2D);
  String portName = Serial.list()[0];
  arduinoPort = new Serial(this, portName, 115200);
}
  
void draw() {

  // Reset current canvas
//  background(#DEC9C9);
  
  // Draw maximum border
  rectMode(CORNER);
  strokeWeight(2);
  noFill();
  rect(border, border, maxWindowW, maxWindowH);
  
  // Draw "cleaned" section of window
  rectMode(CENTER);
  strokeWeight(0);
  fill(255, 0, 0);
  rect(curX, curY, 5, 5);
  
  // Update current orientation of robot
  updateOrientation();
  if (started) {
    
    // Update current location of robot
    updateLocation();
  }
}

/*************************************************************************
 ******* HELPER FUNCTIONS AND CLASS DECLARATIONS *************************
 *************************************************************************/

void updateOrientation() {
  
  if (arduinoPort.available() > 0) {
    nextDir = arduinoPort.readStringUntil('\n');
    println(nextDir);
  }
  else nextDir = null;
  if (nextDir != null) {
    println(nextDir);
    nextDir = trim(nextDir);  

    println(nextDir);
    // Case 0: Start the robot
    if (nextDir.equals("START")) {
      started = true;
    }
    
    if (nextDir.equals("PAUSE")) {
      pause = true;
    }
    
    // Case 1: Robot moving forward
    else if (nextDir.equals("FORWARD")) {
      if (pause = true) pause = false;
      // next direction is forward == continue
    }  
   
    // Case 2: Robot moving left    
    else if (nextDir.equals("LEFT")) {
      if (curDir.equals("up")) curDir = "left";
      else if (curDir.equals("down")) curDir = "right";
      else if (curDir.equals("left")) curDir = "down";
      else if (curDir.equals("right")) curDir = "up";
      beforeTime = millis();
      afterTime = 0;
      while (afterTime < (beforeTime + delayDuration)) {
        afterTime = millis();
      }
      if (pause = true) pause = false;
      
    }
    
    // Case 3: Robot moving right
    else if (nextDir.equals("RIGHT")) {
      if (curDir.equals("up")) curDir = "right";
      else if (curDir.equals("down")) curDir = "left";
      else if (curDir.equals("left")) curDir = "up";
      else if (curDir.equals("right")) curDir = "down";
      beforeTime = millis();
      afterTime = 0;
      while (afterTime < (beforeTime + delayDuration)) {
        afterTime = millis();
      }
      if (pause = true) pause = false;
      
    }
   
    // Case 4: Robot moving backward -- No need for feature yet
    else if (nextDir.equals("BACK")) {
      curDir = curDir;
    }
    // Case 5b: Robot stopped
    else if (nextDir.equals("STOP")) {
      curDir = "stop";
    }  
  }
}

void updateLocation() {
  if (pause == false) {
    if (curDir.equals("up")) curY -= 1;
    else if (curDir.equals("down")) curY += 1;
    else if (curDir.equals("left")) curX -= 10;
    else if (curDir.equals("right")) curX += 10;
    //else if (curDir.equals("pause")) curX = curX;
    else if (curDir.equals("stop")) noLoop();
  }
}


