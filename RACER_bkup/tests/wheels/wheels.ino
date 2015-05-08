#define PIN_MOTOR_1_1 10
#define PIN_MOTOR_1_2 5
#define PIN_MOTOR_2_1 2
#define PIN_MOTOR_2_2 3

int rightTick, leftTick;

void setup()
{
  Serial.begin(115200);
  motor_init();
  Serial.println("[INFO] Initialization Done.");
}

void loop()
{

  moveForward();
  delay(3000);
  moveStop();
  delay(300);

  moveBack();
  delay(3000);
  moveStop();
  delay(300);

  moveLeft();
  delay(3000);
  moveStop();
  delay(300);

  moveRight();
  delay(3000);
  moveStop();
  delay(300);

}

void motor_init()
{
  //for motors
  pinMode(PIN_MOTOR_1_1,OUTPUT); analogWrite(PIN_MOTOR_1_1,0);
  pinMode(PIN_MOTOR_1_2,OUTPUT); analogWrite(PIN_MOTOR_1_2,0);
  pinMode(PIN_MOTOR_2_1,OUTPUT); analogWrite(PIN_MOTOR_2_1,0);
  pinMode(PIN_MOTOR_2_2,OUTPUT); analogWrite(PIN_MOTOR_2_2,0);
}

void moveLeft()
{
  analogWrite(PIN_MOTOR_1_1,0);
  analogWrite(PIN_MOTOR_1_2,200);
  analogWrite(PIN_MOTOR_2_1,0);
  analogWrite(PIN_MOTOR_2_2,200);
}

void moveRight()
{
  analogWrite(PIN_MOTOR_1_1,200);
  analogWrite(PIN_MOTOR_1_2,0);
  analogWrite(PIN_MOTOR_2_1,200);
  analogWrite(PIN_MOTOR_2_2,0);
}

void moveForward()
{
  analogWrite(PIN_MOTOR_1_1,200);
  analogWrite(PIN_MOTOR_1_2,0);
  analogWrite(PIN_MOTOR_2_1,0);
  analogWrite(PIN_MOTOR_2_2,200);
}

void moveBack()
{
  analogWrite(PIN_MOTOR_1_1,0);
  analogWrite(PIN_MOTOR_1_2,200);
  analogWrite(PIN_MOTOR_2_1,200);
  analogWrite(PIN_MOTOR_2_2,0);
}

void moveStop()
{
  analogWrite(PIN_MOTOR_1_1,0);
  analogWrite(PIN_MOTOR_1_2,0);
  analogWrite(PIN_MOTOR_2_1,0);
  analogWrite(PIN_MOTOR_2_2,0);
}
