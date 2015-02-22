// Include libraries
  /* None for now */

// Analog Pins
#define irPin 0            // analog pin  for IR sensor readings

// Digital Pins
#define forwardLED 2
#define backLED 3
#define leftLED 4
#define rightLED 5
#define stopLED 6
#define startLED 7         // Pin to control start LED
#define startButton 8      // digital pin for start button, high (1) == off, low (0) == on

  
// Fixed constants  
#define loopCount 12       // loop count for averaging IR readings
#define startDebounceDelay 500
#define moveDelay 0000
#define LEDduration 1000
#define turnForwardDuration 500
#define turnDist 15
#define stopDist 15

// Initialize global variables
boolean startReading = false;
boolean start = false;
boolean lastLap = false;
int irVal;
unsigned long beforeTime, afterTime, lastStartTime, curStartTime;
float dist;

int leftRightTurn = 0; // next turn: left == 1, next turn: right == 0;
                       // starting from bottom left corner, turn right first
boolean initialStartString = false;

void setup() {
  
  // Set and define pins
  pinMode(irPin, INPUT);
  pinMode(startButton, INPUT);
  pinMode(startLED, OUTPUT);
  pinMode(forwardLED, OUTPUT);
  pinMode(backLED, OUTPUT);
  pinMode(leftLED, OUTPUT);
  pinMode(rightLED, OUTPUT);
  pinMode(stopLED, OUTPUT);
  
  // Open serial port
  Serial.begin(9600); 
}

void loop() {
  
  startReading = digitalRead(startButton);
  
  if (startReading == true) {
    curStartTime = millis();
    if (curStartTime > (lastStartTime + startDebounceDelay)) {
      lastStartTime = curStartTime;
      start ^= true;
    }
  }
  
  // Print string to signal start of robot
  if ((!initialStartString) && (start)) {
    Serial.println("START");
    initialStartString = true;
  }
  
  if (start) digitalWrite(startLED, HIGH);
  else digitalWrite(startLED, LOW);
  
//  while (true) {
//    readIRsensor();
//    Serial.println(dist);
//  }
  
  if (start) {
    readIRsensor();
    
    if (dist < turnDist) {
      if (lastLap) moveStop();
      else if (leftRightTurn == 0) moveRight();
      else if (leftRightTurn == 1) moveLeft();
      leftRightTurn ^= 1;
    }
    else moveForward();
  }
}

/************************************************
 *** Helper Functions ***************************
 ************************************************/

void moveForward() {
//  Serial.println("FORWARD");
  digitalWrite(forwardLED, HIGH);
//  delay(LEDduration);
//  digitalWrite(forwardLED, LOW);
}

void moveBackward() {
  Serial.println("BACK");
  digitalWrite(forwardLED, LOW);
  digitalWrite(backLED, HIGH);
  delay(LEDduration);
  digitalWrite(backLED, LOW);
}

void moveLeft() {
  Serial.println("LEFT");
  digitalWrite(forwardLED, LOW);
  digitalWrite(leftLED, HIGH);
  delay(LEDduration);
  digitalWrite(leftLED, LOW);
  readIRsensor();
  if (dist < stopDist) lastLap = true;
  delay(moveDelay);

//  Serial.println("FORWARD");
  digitalWrite(forwardLED, HIGH);
  delay(turnForwardDuration);
  digitalWrite(forwardLED, LOW);
  
  Serial.println("LEFT");
  digitalWrite(leftLED, HIGH);
  delay(LEDduration);
  digitalWrite(leftLED, LOW);
}

void moveRight() {
  Serial.println("RIGHT");
  digitalWrite(forwardLED, LOW);
  digitalWrite(rightLED, HIGH);
  delay(LEDduration);
  digitalWrite(rightLED, LOW);
  readIRsensor();
  if (dist < stopDist) lastLap = true;
  delay(moveDelay);
  
//  Serial.println("FORWARD");
  digitalWrite(forwardLED, HIGH);
  delay(turnForwardDuration);
  digitalWrite(forwardLED, LOW);
  
  Serial.println("RIGHT");
  digitalWrite(rightLED, HIGH);
  delay(LEDduration);
  digitalWrite(rightLED, LOW);
}

void moveStop() {
  Serial.println("STOP");
  digitalWrite(forwardLED, LOW);
  digitalWrite(stopLED, HIGH);
  while (true);
}

void readIRsensor() {
  irVal = 0;
  for (int i = 0; i < loopCount; i++) {
    irVal += analogRead(irPin);
  }
  irVal /= loopCount;

  dist = 12343.85 * pow(irVal, -1.15);  // linearize the value
}

// code ends here
