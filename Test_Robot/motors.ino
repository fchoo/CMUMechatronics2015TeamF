
/*==================================
=            Locomotion            =
==================================*/

void motorFeedback()
{
  if ((state == LEFTU_NEXT) || (state == RIGHTU_NEXT))
  {
    torq_straight_1 = torq_straight_1;
    torq_straight_2 = torq_straight_2;
  }
}

void motor_init()
{
  //for motors
  pinMode(PIN_MOTOR_1_1,OUTPUT); analogWrite(PIN_MOTOR_1_1,LOW);
  pinMode(PIN_MOTOR_1_2,OUTPUT); analogWrite(PIN_MOTOR_1_2,LOW);
  pinMode(PIN_MOTOR_2_1,OUTPUT); analogWrite(PIN_MOTOR_2_1,LOW);
  pinMode(PIN_MOTOR_2_2,OUTPUT); analogWrite(PIN_MOTOR_2_2,LOW);
}

void moveLeft()
{
  if (DEBUG) Serial.println("[INFO] Moving left.");
  analogWrite(PIN_MOTOR_1_1,TORQ_TURN);
  analogWrite(PIN_MOTOR_1_2,0);
  analogWrite(PIN_MOTOR_2_1,TORQ_TURN);
  analogWrite(PIN_MOTOR_2_2,0);
}

void moveRight()
{
  if (DEBUG) Serial.println("[INFO] Moving right.");
  analogWrite(PIN_MOTOR_1_1,0);
  analogWrite(PIN_MOTOR_1_2,TORQ_TURN);
  analogWrite(PIN_MOTOR_2_1,0);
  analogWrite(PIN_MOTOR_2_2,TORQ_TURN);
}

void moveForward()
{
  if (DEBUG) Serial.println("[INFO] Moving forward.");
  analogWrite(PIN_MOTOR_1_1,0);
  analogWrite(PIN_MOTOR_1_2,torq_straight_1);
  analogWrite(PIN_MOTOR_2_1,torq_straight_2);
  analogWrite(PIN_MOTOR_2_2,0);
}

void moveBack()
{
  if (DEBUG) Serial.println("[INFO] Moving back.");
  analogWrite(PIN_MOTOR_1_1,torq_straight_1);
  analogWrite(PIN_MOTOR_1_2,0);
  analogWrite(PIN_MOTOR_2_1,0);
  analogWrite(PIN_MOTOR_2_2,torq_straight_2);
}

void stop()
{
  if (DEBUG) Serial.println("[INFO] Stopping.");
  analogWrite(PIN_MOTOR_1_1,0);
  analogWrite(PIN_MOTOR_1_2,0);
  analogWrite(PIN_MOTOR_2_1,0);
  analogWrite(PIN_MOTOR_2_2,0);
}
