  #include <Stepper.h>
  #include <Servo.h>
  #include <math.h>
  
  // Assign I/O pins to sensors and actuators
  // 2 |------| 1
  //       |
  //       |
  // 4 |------| 3
  
  #define driveMotor1_1 33
  #define driveMotor1_2 32
  #define driveMotor2_1 34
  #define driveMotor2_2 35
  #define driveMotor3_1 31
  #define driveMotor3_2 30
  #define driveMotor4_1 36
  #define driveMotor4_2 37
  #define edfMotor 38
  
  #define inputFSR1 12
  #define inputFSR2 13
  #define inputFSR3 14
  #define inputFSR4 15
  #define inputENC1 16
  #define inputENC2 17
  #define inputENC3 18
  #define inputENC4 19
  #define inputIR 0
  #define inputIMU 21
  
  
  
  // Initiliaze variables
  int FSR[4];
  int IMU[3];
  int irVal;
  int turnDir = 0;
  int irThresh, fsrThresh;
  boolean turning;
  int value;
  
  
  
  void setup()
  {
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
        pinMode(edfMotor, OUTPUT);
        Serial.println("\r\n*Calibration begin*"); delay(200);
        // Initialize!
        analogWrite(edfMotor,0);
        Serial.println("Initializing to Zero..."); delay(500);
        // Engage!
        analogWrite(edfMotor,191);
        Serial.println("Engaging..."); delay(4000);
        Serial.println("*Calibration done*");
        
        
        // Sensor pins
        pinMode(inputFSR1, INPUT_PULLUP);
        pinMode(inputFSR2, INPUT_PULLUP);
        pinMode(inputFSR3, INPUT_PULLUP);
        pinMode(inputFSR4, INPUT_PULLUP);
        pinMode(inputENC1, INPUT_PULLUP);
        pinMode(inputENC2, INPUT_PULLUP);
        pinMode(inputENC3, INPUT_PULLUP);
        pinMode(inputENC4, INPUT_PULLUP);
        pinMode(inputIMU, INPUT);
        pinMode(inputIR, INPUT);
        
        // Initialize IMU
        // AHRS_Init();
        
        Serial.begin(115200);
        
        
        // Print Command Line Instructions
        help();
  }
  
  
  // Main control loop
  void loop() 
  {
    
    // Get feedback from FSR array and IMU
//    long FSR = readSensors(1);
//    IMU = readSensors(2);  
    
    // Use complimentary or averaging filter on FSR feeback
    
    
    
    // If FSR reading is too low OR if IMU pitch > 90 de, speed up EDF
    analogWrite(edfMotor,191);
    
    // read IR sensor
    float dist = readSensors(2);
    Serial.println(dist);
    
    if ( irVal < irThresh ) {
      // call turn function
      turning = true;
      turn(255, turnDir);
      // turn counter is input into the turn controller to tell it which direction to turn
      turnDir++; 
    }
    else {
      // call drive straight function
      //drive(255);
      turn(255, 0);
    }
    
    
      // Convert value from pot to a range of 180-255 to output to
  // the EDF. EDF operates between 190-255.
  value = map(value, 0, 1023, 180, 255); 
    
    
  
  }
  
  
  
    //*************************** READ SENSORS *****************************************
      long readSensors(int sensor)
      {
        int loopCount = 5;
        int fsrVal[4] = {0,0,0,0};  // reset sensor values
        int IMU[3] = {0,0,0};
        
        if ( sensor == 1) {// read FSR values
          for (int i = 0; i < loopCount; i++) {
            fsrVal[0] += analogRead(inputFSR1);
            fsrVal[1] += analogRead(inputFSR2);
            fsrVal[2] += analogRead(inputFSR3);
            fsrVal[3] += analogRead(inputFSR4);
          }
    
          fsrVal[0] /= loopCount;
          fsrVal[1] /= loopCount;
          fsrVal[2] /= loopCount;
          fsrVal[3] /= loopCount;
          
          long fsrAvg = (fsrVal[0]+fsrVal[1]+fsrVal[2]+fsrVal[3])/4;
          
          return fsrAvg;
        }
        
        if ( sensor == 2) {// read IR value
          irVal = 0;
          for (int i = 0; i < loopCount; i++) {
            irVal += analogRead(inputIR);
          }
    
          irVal /= loopCount;
          float irDist = 12343.85 * pow(irVal, -1.15); // Linearizing eqn, accuracy +- 5%
          return irDist;
        }
        
        if ( sensor == 3) {// read IMU value
//          Read_AHRS();
//          roll = Get_Roll();
//          pitch = Get_Pitch();
//          yaw = Get_Yaw();
//          IMU[3] = {roll, pitch, yaw];
//          return IMU;
        }
        
        
      }
  
    //*************************** TURN *****************************************
      void turn(int power, int turnDir)
      {
        // turn left or turn right 
        if ( turnDir % 2 == 0)  {  // turn right
          analogWrite(driveMotor1_1, power);
          analogWrite(driveMotor1_2, LOW);
          analogWrite(driveMotor2_1, LOW);
          analogWrite(driveMotor2_2, power);
          analogWrite(driveMotor3_1, power);
          analogWrite(driveMotor3_2, LOW);
          analogWrite(driveMotor4_1, LOW);
          analogWrite(driveMotor4_2, power); 
        } 
        
        else if ( turnDir % 2 == 1) { // turn left
          analogWrite(driveMotor1_1, LOW);
          analogWrite(driveMotor1_2, power);
          analogWrite(driveMotor2_1, power);
          analogWrite(driveMotor2_2, LOW);
          analogWrite(driveMotor3_1, LOW);
          analogWrite(driveMotor3_2, power);
          analogWrite(driveMotor4_1, power);
          analogWrite(driveMotor4_2, LOW); 
        }
      }
  
  //*************************** DRIVE *****************************************
      void drive(int power)
      {
        // power all drive motors equally to drive straight forward
        analogWrite(driveMotor1_1, power);
        analogWrite(driveMotor1_2, LOW);
        analogWrite(driveMotor2_1, power);
        analogWrite(driveMotor2_2, LOW);
        analogWrite(driveMotor3_1, power);
        analogWrite(driveMotor3_2, LOW);
        analogWrite(driveMotor4_1, power);
        analogWrite(driveMotor4_2, LOW); 
      }
  
      //*********************** Serial Command ****************************************

    long getSerial()
    {
      boolean negative = false;
      long serialData = 0;
      int aChar = 0;
      while (aChar != '/')
      {
        aChar = Serial.read();
        if (aChar == '-') // Current read is negative
          negative = true;
        else if (aChar >= '0' && aChar <= '9')
          serialData = serialData * 10 + aChar - '0';
        else if (aChar >= 'a' && aChar <= 'z')
          serialData = aChar - '0';
      }
      if (negative)
        serialData *= -1;
      return serialData;
    }
  
  //*************************** HELP *****************************************
      void help()
      {
        // Print Command Line Instructions
        Serial.print("Update Command line instructions\n");  
      }
      
      
  //*************************** Sign *****************************************
      int sign(long input)
      {
        int output = 0;
        
        if (input < 0 ) 
          output = -1; 
        else
          output = 1;
        
        return output;   
      }
