#define PIN_MOTOR_1_1 8

void setup()
{
  Serial.begin(115200);
  motor_init();
  Serial.println("[INFO] Initialization Done.");
}

void loop()
{
  analogWrite(PIN_MOTOR_1_1, 200);
  delay(3000);
  analogWrite(PIN_MOTOR_1_1, 0);
  delay(3000);
}

void motor_init()
{
  //for motors
  pinMode(PIN_MOTOR_1_1,OUTPUT); analogWrite(PIN_MOTOR_1_1,0);

}


