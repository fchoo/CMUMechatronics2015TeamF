
    // include libraries
    #include <Stepper.h>
    #include <Servo.h>
    #include <math.h>

    #define inputFSR 5 // FSR connected to analog input A5
    #define inputPOT 3 // POT connected to analog input A3
    #define inputIR 0 // IR connected to analog input A0
    #define inputENC 4 // ENC connected to analog input A4
    #define inputMS 1 //  MicroSwitch connected to analog input A1
    #define servoPin 2 // servo signal pin
    #define dcMotorPlus 3 // Brushed DC motor +
    #define dcMotorMinus 4 // Brushed DC motor -

    // Stepper input pins
    #define in1Pin 12
    #define in2Pin 11
    #define in3Pin 10
    #define in4Pin 9

    #define ledPin 13 // arduino LED

    #define loopCount 14
    #define ENC_THRESH 150
    #define PAUSE 10

    // initialize variables
    int potVal, fsrVal, irVal, irLinear, encVal, switchVal;
    unsigned int senStartByteFSR = 0xDEAD;
    unsigned int senStartBytePOT = 0xCAFE;
    unsigned int senStartByteIR = 0xBABE;
    byte handShakeSen[2] = {0xAA,0xAA};
    byte recHandShake[2];
    byte recData[2];

    // Initialize Motors
    Stepper motor(512, in1Pin, in2Pin, in3Pin, in4Pin);
    Servo servo;

    // Switch Debouncing
    int lastButtonState = LOW;
    int ledState = HIGH;
    long lastDebounceTime = 0;  // the last time the output pin was toggled
    long debounceDelay = 50;    // the debounce time; increase if the output flickers
    int buttonState;

    void setup()
    {
      // Stepper Motor
      pinMode(in1Pin, OUTPUT);
      pinMode(in2Pin, OUTPUT);
      pinMode(in3Pin, OUTPUT);
      pinMode(in4Pin, OUTPUT);

      // setup servo
      servo.attach(servoPin);
      servo.write(10);
      delay(100);

      // Brushed DC motor
      pinMode(dcMotorPlus, OUTPUT);
      pinMode(dcMotorMinus, OUTPUT);

      // Sensor pins
      pinMode(inputFSR, INPUT_PULLUP);
      pinMode(inputPOT, INPUT_PULLUP);
      pinMode(inputIR, INPUT_PULLUP);
      pinMode(inputENC, INPUT_PULLUP);
      pinMode(inputMS, INPUT_PULLUP);
      pinMode(ledPin, OUTPUT);
      Serial.begin(9600);

    }

    void loop()
    {
      if (Serial.available() > 0) // check for input from serial
      {
        getHandShake(); // Blocking until receive correct handshake

        if (recHandShake[0] == handShakeSen[0] &&
            recHandShake[1] == handShakeSen[1])
          readSensors();
      }
    }


    //*********************** Serial Command ****************************************

    void getHandShake()
    {
      while (1) 
      {
        while (Serial.available()<2); // wait for 2 bytes
        Serial.readBytes((char *)recHandShake, 2);
        // Received a correct starting handshake byte
        if (recHandShake[0] == 0xAA ||
            recHandShake[0] == 0xBB ||
            recHandShake[0] == 0xCC)
          break; // Terminate if received handshake
      }
    }

    long getData()
    {
      while (Serial.available()<2); // wait for 2 bytes
      Serial.readBytes((char *)recData, 2);
     }

    //*********************** Read Sensors ****************************************

    void readSensors()
    {
      potVal = 0; fsrVal = 0; irVal = 0; encVal = 0; switchVal = 0;// Resetting saved values
      // read and averages readings over a (100us*loopCount) time period
      for (int i = 0; i < loopCount; i++)
      {
        fsrVal += analogRead(inputFSR);
        potVal += analogRead(inputPOT);
        irVal += analogRead(inputIR);
        encVal += analogRead(inputENC);
        delay(1);
      }
      fsrVal /= loopCount;
      potVal /= loopCount;
      irVal /= loopCount;
      encVal /= loopCount;
      irLinear = round(12343.85 * pow(irVal, -1.15)); // Linearizing eqn, accuracy +- 5%

      // switch
      switchVal = digitalRead(inputMS);

     // Interlace the startBytes (0xDEAD, 0xCAFE, 0xBABE) with data bytes
      Serial.write((unsigned byte*)&senStartByteFSR, 2);
      Serial.write((unsigned byte*)&fsrVal, 2);
      Serial.write((unsigned byte*)&senStartBytePOT, 2);
      Serial.write((unsigned byte*)&potVal, 2);
      Serial.write((unsigned byte*)&senStartByteIR, 2);
      Serial.write((unsigned byte*)&irLinear, 2);
      // return fsrVal, potVal, irLinear, encVal, switchVal;
    }


    //*********************** Motor Controller ***********************************
    void motorController(int motorNum, long motorInput, long motorSpeed)
    {
      if (motorNum == 1)      // stepper motor
      {
        int inputSign = sign(motorSpeed);
        motor.setSpeed(inputSign*20);
        long steps = 200*motorInput/360;
        motor.step(steps);
      }

      if (motorNum == 2)      // servo motor
      {
        servo.write(motorInput);
        delay(1);
      }

      if (motorNum == 3)      // Brushed DC motor
      {
        if (motorSpeed >= 0 )
        {
          analogWrite(dcMotorPlus, motorSpeed);
          analogWrite(dcMotorMinus, LOW);
          encoder(inputENC, motorInput);
          analogWrite(dcMotorPlus, 0);
          analogWrite(dcMotorMinus, 0);
        }
        else if (motorSpeed < 0)
        {
          analogWrite(dcMotorPlus, LOW);
          analogWrite(dcMotorMinus, -motorSpeed);
          encoder(inputENC, motorInput);
          analogWrite(dcMotorPlus, 0);
          analogWrite(dcMotorMinus, 0);
        }
      }
    }


    //*********************** Encoder Function ***********************************

    void encoder(int pin_num, long deg)
    {
      // define variables
      long ticks;

      int n_tick=0;
      int old_tick_val=0;
      int tick_val=0;
      int sensor;

      // convert input degrees into encoder ticks
      // every rotation of the 3-tooth encoder wheel should generate 6 ticks
      // if statement differentiates between motors with different gearbox ratios
      long gearRatio = 298;
      ticks = 6*gearRatio*deg/360;

      while (n_tick<= ticks)
      {
        sensor=analogRead(pin_num); // 0 = IO_C0
        if (sensor>= ENC_THRESH)
        {
          tick_val=0;
          if (old_tick_val==1)
          {
            n_tick++;
            old_tick_val=0;
          }
        }

        if (sensor< ENC_THRESH)
        {
          tick_val=1;
          if (old_tick_val==0)
          {
            n_tick++;
            old_tick_val=1;
          }
        }
        //Serial.println(sensor);
      }
    }


    //********************** ROBOT Controller **********************************
    void robotController(int sensor, int motor)
    {
      long sensorVal;

      readSensors();

      // use designated sensor feedback as motor control input
      if (sensor == 1)  {
        sensorVal = potVal;      // 511 - 1022
        sensorVal = (sensorVal - 511.0)*(360.0/(1022.0-511.0)); } // convert pot val from 0 to 360
      else if (sensor == 2)  {
        sensorVal = fsrVal;      // 50 - 550
        sensorVal = (sensorVal - 300.0)*(360.0/(800.0-300.0)); }
      else if (sensor == 3) {
        sensorVal = irLinear;    // 10 - 60
        sensorVal = (sensorVal - 10)*(360/(60-10)); }
      else if (sensor == 4)
        sensorVal = encVal;      //

       // run motor controller with specified parameters
      if (motor == 1)
      {
        if (sensor == 2)
        {
          if (sensorVal > 100)
            motorController(motor, 10, 255);
        }
        else if (sensor == 1)
        {
          if (sensorVal > 90)
            motorController(motor, 10, 20);
          else if (sensorVal < 90)
            motorController(motor, -10, 20);
        }
      }
      else if (motor == 2)
      {
        motorController(motor, sensorVal/2, 255);
      }
      else if (motor == 3)
      {
        long gearRatio = 6 * 298;
        long rotations = gearRatio * 1/360;
        Serial.println(sensorVal);
        float speedVal = (sensorVal * (195.0/360.0))+60.0;
        Serial.println(speedVal);
        motorController(motor, rotations , speedVal);
      }
    }

    //*************************** HELP *****************************************
    // Print Command Line Instructions
    void help()
    {
      Serial.print("Select a mode then type '/': \n\t's' - Sensor Package \n\t'm' - Motor Control\n\t'r' - Robot Control (sensors actuate motors) \n");

      Serial.print("('s') Select a Sensor: \n\t'1' - Potentiometer \n\t'2' - FSR \n\t'3' - IR \n\t'4' - Encoder\n\t'5' - Switch \n\t");
      Serial.print("Type '/'\n\t");
      Serial.print("Type the sensor feedback run time [seconds] followed by '/'\n\tHit Enter key\n");

      Serial.print("('m') Select a Motor: \n\t'1' - Stepper motor \n\t'2' - Servo \n\t'3' - Brushed DC Motor \n\t");
      Serial.print("Type '/'\n\t");
      Serial.print("For Stepper and DC control type '1' for CW and '0' for CCW rotation followed by '/'\n\t");
      Serial.print("Type the motor control input parameter [degrees] followed by '/'\n\tHit Enter key\n\n");

      Serial.print("('r') Follow the steps below to select a Robot: \n\t");
      Serial.print("1) Select a sensor ('1','2','3', or '4') followed by '/' \n\t");
      Serial.print("2) Select a motor ('1','2',or '3') followed by '/' \n\t");
      Serial.print("3) Choose run time for operation [s] followed by '/'\n\t");
      Serial.print("Hit Enter key\n\n");

      Serial.print("Example: 's/2/10/' -> the program will return the FSR sensor feedback over serial for 10 seconds\n\n");
      Serial.print("Enter 'h/' for help menu\n");
      //Serial.print("Enjoy mechatronics because it's fun!\n");
    }

    int sign(long input)
    {
      if (input < 0)
        return -1;
      else
        return 1;
    }
