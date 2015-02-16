
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

    #define DC_SPEED_MAX 245
    #define TIMEOUT_MAX 200

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

    long gearRatio = 6 * 298;
    long rotations = gearRatio * 1/360;
    int angle, speed;

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
      readSensors();
      // for sensors reading
      if (isValidHS("sensors"))
        sendSensorsData();
      // Controlling Stepper Angle Clockwise
      else if (isValidHS("STEPPER-CW"))
      {
        getData((byte *)&data);
        angle = int(data[0])*256 + int(data[1]);
        motorController(MOTORSTEPPER, angle, 20);
      }
      // Controlling Stepper Angle Anti-Clockwise
      else if (isValidHS("STEPPER-ACW"))
      {
        getData((byte *)&data);
        angle = int(data[0])*256 + int(data[1]);
        motorController(MOTORSTEPPER, -angle, 20);
      }
      // Controlling Servo
      else if (isValidHS("SERVO"))
      {
        getData((byte *)&data);
        angle = int(data[0])*256 + int(data[1]);
        motorController(MOTORSERVO, angle, 0);
      }
      // Controlling DC Angle Clockwise
      else if (isValidHS("DC-ANGLE-CW"))
      {
        getData((byte *)&data);
        angle = int(data[0])*256 + int(data[1]);
        motorController(MOTORDC, angle, DC_SPEED_MAX);

      }
      // Controlling DC Angle Anti-Clockwise
      else if (isValidHS("DC-ANGLE-ACW"))
      {
        getData((byte *)&data);
        angle = int(data[0])*256 + int(data[1]);
        motorController(MOTORDC, angle, -DC_SPEED_MAX);
      }
      // Controlling DC Speed Clockwise
      else if (isValidHS("DC-SPEED-CW"))
      {
        getData((byte *)&data);
        speed = int(data[0])*256 + int(data[1]);
        motorController(MOTORDC, 360, speed);
        motorController(MOTORDC, 360, speed);
      }
      // Controlling DC Speed Anti-Clockwise
      else if (isValidHS("DC-SPEED-ACW"))
      {
        getData((byte *)&data);
        speed = int(data[0])*256 + int(data[1]);
        motorController(MOTORDC, 360, -speed);
        motorController(MOTORDC, 360, -speed);
      }
      // FSR control stepper
      else if (isValidHS("FSR-STEPPER")) {
        robotController("FSR-STEPPER");
        sendSensorsData();
      }
      // POT control servo
      else if (isValidHS("POT-SERVO")){
        sendSensorsData();
        robotController("POT-SERVO");
      }
      // POT control servo
      else if (isValidHS("IR-DC")){
        sendSensorsData();
        robotController("IR-DC");
      }
      else if (isValidHS("SW-STEPPER"))
      {
        sendSensorsData();
        debounce();
        robotController("SW-STEPPER");
      }
    }


    //*********************** Serial Command ****************************************

    // Checking for valid handshake
    boolean isValidHS(String type) {
      if (type == "sensors")
        return (handShake[0] == 0x11 &&
                handShake[1] == 0x11);
      else if (type == "STEPPER-CW")
        return (handShake[0] == 0x22 &&
                handShake[1] == 0x22);
      else if (type == "STEPPER-ACW")
        return (handShake[0] == 0x33 &&
                handShake[1] == 0x33);
      else if (type == "SERVO")
        return (handShake[0] == 0x44 &&
                handShake[1] == 0x44);
      else if (type == "DC-ANGLE-CW")
        return (handShake[0] == 0x55 &&
                handShake[1] == 0x55);
      else if (type == "DC-ANGLE-ACW")
        return (handShake[0] == 0x66 &&
                handShake[1] == 0x66);
      else if (type == "DC-SPEED-CW")
        return (handShake[0] == 0x77 &&
                handShake[1] == 0x77);
      else if (type == "DC-SPEED-ACW")
        return (handShake[0] == 0x88 &&
                handShake[1] == 0x88);
      else if (type == "FSR-STEPPER")
        return (handShake[0] == 0x99 &&
                handShake[1] == 0x99);
      else if (type == "POT-SERVO")
        return (handShake[0] == 0xAA &&
                handShake[1] == 0xAA);
      else if (type == "IR-DC")
        return (handShake[0] == 0xBB &&
                handShake[1] == 0xBB);
      else if (type == "SW-STEPPER")
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

    }

    void sendSensorsData()
    {
      // Interlace the startBytes (0xDEAD, 0xCAFE, 0xBABE) with data bytes
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
        motor.setSpeed(motorSpeed);
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
        if (motorSpeed >= 0)
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

    //********************** ROBOT Controller **********************************

    void robotController(String type)
    {
      long rPOTVal, rFSRVal, rIRVal;
      // FSR - STEPPER
      if (type == "FSR-STEPPER")
      {
        rFSRVal = (fsrVal - 300.0)*(360.0/(800.0-300.0)); // 50-550
        if (rFSRVal > 100)
          motorController(MOTORSTEPPER, 10, DC_SPEED_MAX);
      }
      // POT - SERVO
      else if (type == "POT-SERVO")
      {
        rPOTVal = (potVal - 511.0)*(360.0/(1022.0-511.0)); // 511-1022 convert pot val from 0 to 360
        motorController(MOTORSERVO, rPOTVal/2, DC_SPEED_MAX);
      }
      // IR - DC MOTOR
      else if (type == "IR-DC")
      {
        rIRVal = (irLinear - 10)*(360/(60-10)); // 10-60
        float speedVal = (rIRVal * (195.0/360.0))+60.0;
        motorController(MOTORDC, rotations, speedVal);
      }
      // SW - STEPPER
      else if (type == "SW-STEPPER")
      {
        if (ledState == HIGH)
          motorController(MOTORSTEPPER, -10, DC_SPEED_MAX);
        else
          motorController(MOTORSTEPPER, 10, DC_SPEED_MAX);

      }
    }

    //*********************** Debouncing Switch Function ***********************************

    void debounce()
    {
      // read the state of the switch into a local variable:
      int reading = digitalRead(INPUT_MS);

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
          }
        }
      }

      // set the LED:
      digitalWrite(PIN_LED, ledState);


      // save the reading.  Next time through the loop,
      // it'll be the lastButtonState:
      lastButtonState = reading;
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
      int timeout = TIMEOUT_MAX;
      // convert input degrees into encoder ticks
      // every rotation of the 3-tooth encoder wheel should generate 6 ticks
      // if statement differentiates between motors with different gearbox ratios
      long gearRatio = 298;
      ticks = 6*gearRatio*deg/360;

      while (n_tick <= ticks)
      {
        if (isValidHS("IR-DC")) {
            timeout--;
          if (timeout == 0) {
            timeout = TIMEOUT_MAX;
            break;
          }
        }
        // if (Serial.available() >= 2 &&
        //     (isValidHS("DC-SPEED-CW") ||
        //      isValidHS("DC-SPEED-ACW"))) {
        //     getData((byte *)&handShake);
        //     if (!(isValidHS("DC-SPEED-CW") || isValidHS("DC-SPEED-ACW")))
        //         break;
        // }
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


    int sign(long input)
    {
      if (input < 0)
        return -1;
      else
        return 1;
    }
