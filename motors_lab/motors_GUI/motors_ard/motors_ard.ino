
    // include libraries
    #include <Stepper.h>
    #include <Servo.h>
    #include <math.h>

    #define INPUT_FSR 5 // FSR connected to analog input A5
    #define INPUT_POT 3 // POT connected to analog input A3
    #define INPUT_IR 0 // IR connected to analog input A0
    #define INPUT_ENC 4 // ENC connected to analog input A4
    #define INPUT_MS 1 //  MicroSwitch connected to analog input A1
    #define PIN_SERVO 2 // servo signal pin
    #define DCMOTOR_PLUS 3 // Brushed DC motor +
    #define DCMOTOR_MINUS 4 // Brushed DC motor -

    #define MOTORSTEPPER 1
    #define MOTORSERVO 2
    #define MOTORDC 3

    // Stepper input pins
    #define PIN_STEP_IN_1 12
    #define PIN_STEP_IN_2 11
    #define PIN_STEP_IN_3 10
    #define PIN_STEP_IN_4 9

    #define PIN_LED 13 // arduino LED

    #define LOOP_CT 14
    #define ENC_THRESH 150
    #define PAUSE 10

    // initialize variables
    int potVal, fsrVal, irVal, irLinear, encVal, switchVal;
    unsigned int senStartByteFSR = 0xDEAD;
    unsigned int senStartBytePOT = 0xCAFE;
    unsigned int senStartByteIR = 0xBABE;
    byte handShake[2], data[2];

    // Initialize Motors
    Stepper motor(512, PIN_STEP_IN_1, PIN_STEP_IN_2, PIN_STEP_IN_3, PIN_STEP_IN_4);
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
      pinMode(PIN_STEP_IN_1, OUTPUT);
      pinMode(PIN_STEP_IN_2, OUTPUT);
      pinMode(PIN_STEP_IN_3, OUTPUT);
      pinMode(PIN_STEP_IN_4, OUTPUT);

      // setup servo
      servo.attach(PIN_SERVO);
      servo.write(10);
      delay(100);

      // Brushed DC motor
      pinMode(DCMOTOR_PLUS, OUTPUT);
      pinMode(DCMOTOR_MINUS, OUTPUT);

      // Sensor pins
      pinMode(INPUT_FSR, INPUT_PULLUP);
      pinMode(INPUT_POT, INPUT_PULLUP);
      pinMode(INPUT_IR, INPUT_PULLUP);
      pinMode(INPUT_ENC, INPUT_PULLUP);
      pinMode(INPUT_MS, INPUT_PULLUP);
      pinMode(PIN_LED, OUTPUT);
      Serial.begin(9600);

    }

    void loop()
    {
      getData((byte *)&handShake); // Blocking until receive correct handshake
      // for sensors reading
      if (isValidHS("sensors")) 
      {
        readSensors();
        sendSensorsData();
      }
      // for manual-input motors
      else if (isValidHS("man-motors"))
      {
      }
      // for sensor-controlled motors
      else if (isValidHS("FSR-STEPPER"))
        robotController("FSR-STEPPER");
      else if (isValidHS("POT-SERVO"))
        robotController("POT-SERVO");
      else if (isValidHS("IR-DC"))
        robotController("IR-DC");
    }


    //*********************** Serial Command ****************************************

    // Checking for valid handshake
    boolean isValidHS(String type) {
      if (type == "sensors")
        return (handShake[0] == 0xAA &&
                handShake[1] == 0xAA);
      else if (type == "man-motors")
        return (handShake[0] == 0xBB);
      else if (type == "FSR-STEPPER")
        return (handShake[0] == 0xCC &&
                handShake[1] == 0xAA);
      else if (type == "POT-SERVO")
        return (handShake[0] == 0xCC &&
                handShake[1] == 0xBB);
      else if (type == "IR-DC")
        return (handShake[0] == 0xCC &&
                handShake[1] == 0xCC);
      else
        return false;
    }

    void getData(byte* buf)
    {
      while (Serial.available()<2); // wait for 2 bytes
      Serial.readBytes((char *)buf, 2);
     }

    //*********************** Read Sensors ****************************************

    void readSensors()
    {
      potVal = 0; fsrVal = 0; irVal = 0; encVal = 0; switchVal = 0;// Resetting saved values
      // read and averages readings over a (100us*LOOP_CT) time period
      for (int i = 0; i < LOOP_CT; i++)
      {
        fsrVal += analogRead(INPUT_FSR);
        potVal += analogRead(INPUT_POT);
        irVal += analogRead(INPUT_IR);
        encVal += analogRead(INPUT_ENC);
        delay(1);
      }
      fsrVal /= LOOP_CT;
      potVal /= LOOP_CT;
      irVal /= LOOP_CT;
      encVal /= LOOP_CT;
      irLinear = round(12343.85 * pow(irVal, -1.15)); // Linearizing eqn, accuracy +- 5%

      // switch
      switchVal = digitalRead(INPUT_MS);
      
     // Interlace the startBytes (0xDEAD, 0xCAFE, 0xBABE) with data bytes
    }

    void sendSensorsData()
    {
      Serial.write((unsigned byte*)&senStartByteFSR, 2);
      Serial.write((unsigned byte*)&fsrVal, 2);
      Serial.write((unsigned byte*)&senStartBytePOT, 2);
      Serial.write((unsigned byte*)&potVal, 2);
      Serial.write((unsigned byte*)&senStartByteIR, 2);
      Serial.write((unsigned byte*)&irLinear, 2);

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

      else if (motorNum == 2)      // servo motor
      {
        servo.write(motorInput);
        delay(1);
      }

      else if (motorNum == 3)      // Brushed DC motor
      {
        if (motorSpeed >= 0 )
        {
          analogWrite(DCMOTOR_PLUS, motorSpeed);
          analogWrite(DCMOTOR_MINUS, LOW);
        }
        else
        {
          analogWrite(DCMOTOR_PLUS, LOW);
          analogWrite(DCMOTOR_MINUS, -motorSpeed);
        }
          encoder(INPUT_ENC, motorInput);
          analogWrite(DCMOTOR_PLUS, 0);
          analogWrite(DCMOTOR_MINUS, 0);
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

      while (n_tick <= ticks)
      {
        // Interrupt handler for different handshake received
        getData((byte *)&handShake);
        if (!isValidHS("IR-DC"))
          break;

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
        else
        {
          tick_val=1;
          if (old_tick_val==0)
          {
            n_tick++;
            old_tick_val=1;
          }
        }
      }
    }


    //********************** ROBOT Controller **********************************
    void robotController(String type)
    {
      long rPOTVal, rFSRVal, rIRVal;
      readSensors();

      // FSR - STEPPER
      if (type == "FSR-STEPPER")
      {
        rFSRVal = (fsrVal - 300.0)*(360.0/(800.0-300.0)); // 50-550
        if (rFSRVal > 100)
          motorController(MOTORSTEPPER, 10, 255);
      } 
      // POT - SERVO
      else if (type == "POT-SERVO")
      {
        rPOTVal = (potVal - 511.0)*(360.0/(1022.0-511.0)); // 511-1022 convert pot val from 0 to 360
        motorController(MOTORSERVO, rPOTVal/2, 255);
      }
      // IR - DC MOTOR
      else if (type == "IR-DC")
      {
        rIRVal = (irLinear - 10)*(360/(60-10)); // 10-60
        long gearRatio = 6 * 298;
        long rotations = gearRatio * 1/360;
        float speedVal = (rIRVal * (195.0/360.0))+60.0;
        motorController(MOTORDC, rotations , speedVal);
      }
    }

    int sign(long input)
    {
      if (input < 0)
        return -1;
      else
        return 1;
    }
