#define motorPlus 2     // (IN1) on motor driver
#define motorMinus 3    // (IN2) on motor driver
#define motorPWM 4      // (D2/PWM) on motor driver
#define motorLOW 5      // (D1/PWM) on motor driver

#define PAUSE 2000
#define PWMval 255

void setup()
{
  pinMode(motorPlus, OUTPUT);
  pinMode(motorMinus, OUTPUT);
  Serial.begin(9600);
}

void loop()
{
 analogWrite(motorPlus, PWMval);
 analogWrite(motorMinus, 0);
 analogWrite(motorPWM, PWMval);
 analogWrite(motorLOW, 0);
 delay(PAUSE);
 digitalWrite(motorPlus, 0);
 digitalWrite(motorMinus, 0);
 digitalWrite(motorPWM, 0);
 digitalWrite(motorLOW, 0);
 delay(PAUSE);
  
}
