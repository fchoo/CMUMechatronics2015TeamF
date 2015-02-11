
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

    #define MOTORSTEPPER 1
    #define MOTORSERVO 2
    #define MOTORDC 3

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
    unsigned int senStartByteFSR = 0xAAAA;
    unsigned int senStartBytePOT = 0xAABB;
    unsigned int senStartByteIR = 0xAACC;

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
      pinMode(inputIR, INPUT);
      pinMode(inputENC, INPUT_PULLUP);
      pinMode(inputMS, INPUT_PULLUP);
      pinMode(ledPin, OUTPUT);
      Serial.begin(9600);

      // Print Command Line Instructions
      help();
    }

    void loop()
    {
      if (Serial.available() > 0) // check for input from serial
      {
        int mode = getSerial();

        switch(mode) {
          case 67:        // 's' == 67 in ASCII, 's' is for sensors
          {
            int chooseSensor = getSerial();
            int runTime = getSerial()*1000; // convert to seconds
            int startTime = millis();    // start clock
            int timeNow = 0;

            if (chooseSensor == 1)    // potentiometer
            {
              while (timeNow < runTime)
              {
                readSensors();
                Serial.print("Potentiometer: ");
                Serial.println(potVal);
                delay(PAUSE);
                timeNow = millis() - startTime;  // calculate elapsed time
              }
            }


            if (chooseSensor == 2)    // FSR
            {
              while (timeNow < runTime)
              {
                readSensors();
                Serial.print("FSR: ");
                Serial.println(fsrVal);
                delay(PAUSE);
                timeNow = millis() - startTime;  // calculate elapsed time
              }
            }

            if (chooseSensor == 3)    // IR
            {
              while (timeNow < runTime)
              {
                readSensors();
                Serial.print("IR: ");
                Serial.println(irLinear);
                delay(PAUSE);
                timeNow = millis() - startTime;  // calculate elapsed time
              }
            }

            if (chooseSensor == 4) // encoder
            {
              while (timeNow < runTime)
              {
                readSensors();
                Serial.print("Enc: ");
                Serial.println(encVal);
                delay(PAUSE);
                timeNow = millis() - startTime;  // calculate elapsed time
              }
            }

            if (chooseSensor == 5) // micro switch with DEBOUNCING
            {
              while (timeNow < runTime)
              {
                readSensors();
                Serial.print("Switch: ");
                Serial.println(switchVal);

                // read the state of the switch into a local variable:
                int reading = digitalRead(inputMS);

                // check to see if you just pressed the button
                // (i.e. the input went from LOW to HIGH),  and you've waited
                // long enough since the last press to ignore any noise:

                // If the switch changed, due to noise or pressing:
                if (reading != lastButtonState)
                {
                  // reset the debouncing timer
                  lastDebounceTime = millis();
                }

                if ((millis() - lastDebounceTime) > debounceDelay)
                {
                  // whatever the reading is at, it's been there for longer
                  // than the debounce delay, so take it as the actual current state:

                  // if the button state has changed:
                  if (reading != buttonState)
                  {
                    buttonState = reading;

                    // only toggle the LED if the new button state is HIGH
                    if (buttonState == HIGH)
                    {
                      ledState = !ledState;
                      motor.setSpeed(20);
                      motor.step(10);
                    }
                  }
                }

                // set the LED:
                digitalWrite(ledPin, ledState);


                // save the reading.  Next time through the loop,
                // it'll be the lastButtonState:
                lastButtonState = reading;


               // Button without debouncing
                   if (switchVal == HIGH) {
                        // turn LED on:
                        digitalWrite(ledPin, HIGH);
                      }
                      else {
                        // turn LED off:
                        digitalWrite(ledPin, LOW);
                      }

                delay(PAUSE);
                timeNow = millis() - startTime;  // calculate elapsed time
              }
            }
            break;
          }

          case 61:         // motor controller
          {
            Serial.println("motor");
            int motorNum = getSerial();
            long motorInput = getSerial();
            int motorSpeed;
            // Stepper or servo
            if (motorNum == 1 || motorNum == 2)
            {
              motorSpeed = 255;
              // if (motorInput < 0 )
              //   motorInput = -(motorInput + 3000);
            }
            // Brushed Motor
            else
            {
              motorSpeed = sign(motorInput)*255;
              // if (motorInput < 0)
              //   motorInput = motorInput + 3000;
            }
            Serial.println(motorSpeed);
            Serial.println(motorInput);

            motorController(motorNum, motorInput, motorSpeed);
            break;
          }

          case 66:     // 'r' == 66 in ASCII, robot control
          {
            // int moduleSensor = getSerial();
            // int moduleMotor = getSerial();
            int runTime = getSerial()*1000;
            int startTime = millis();
            int timeNow = 0;

            while (timeNow < runTime) {
              robotController();
              timeNow = millis() - startTime;
            }

            break;
          }

          case 56:     // 'h' == 56 in ASCII, display help menu and command options
          {
            help();
            break;
          }
        }
      }
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

    //*********************** Read Sensors ****************************************

    long readSensors()
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

     // Interlace the startBytes with data bytes
     // Serial.write((unsigned byte*)&senStartByteFSR, 2);
     // Serial.write((unsigned byte*)&fsrVal, 2);
     // Serial.write((unsigned byte*)&senStartBytePOT, 2);
     // Serial.write((unsigned byte*)&potVal, 2);
     // Serial.write((unsigned byte*)&senStartByteIR, 2);
     // Serial.write((unsigned byte*)&irLinear, 2);
    //  Serial.print("volt = ");
    //  Serial.print(float(irVal)/1023*5);
    //  Serial.print("\n");
    //  Serial.print("linearized = ");
    //  Serial.print(irLinear);
    //  Serial.print("\n");
      return fsrVal, potVal, irLinear, encVal, switchVal;
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
    void robotController()
    {
      long rPOTVal, rFSRVal, rIRVal;

      readSensors();

      // use designated sensor feedback as motor control input
      rPOTVal = (potVal - 511.0)*(360.0/(1022.0-511.0)); // 511-1022 convert pot val from 0 to 360
      rFSRVal = (fsrVal - 300.0)*(360.0/(800.0-300.0)); // 50-550
      rIRVal = (irLinear - 10)*(360/(60-10)); // 10-60

      // FSR - STEPPER
      if (rFSRVal > 100)
        motorController(MOTORSTEPPER, 10, 255);
      // POT - SERVO
      motorController(MOTORSERVO, rPOTVal/2, 255);
      // IR - DC MOTOR
      long gearRatio = 6 * 298;
      long rotations = gearRatio * 1/360;
      float speedVal = (rIRVal * (195.0/360.0))+60.0;
      motorController(MOTORDC, rotations , speedVal);
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
