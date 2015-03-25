
/*==================================
=            Locomotion            =
==================================*/

/**
 * Use encoders/IMU to coordinate the motors. This will attempt to straighten
 * the movement of the robot.
 */
void motorFeedback()
{
  // only in straight path states
  if ((state == LEFTU_NEXT) || (state == RIGHTU_NEXT))
  {
    // right wheel spinning more than left wheel
    if (rightWheelTicks > leftWheelTicks)
    // if ((rightWheelTicks > leftWheelTicks) || isVeeringLeft())
    {
      // Serial.println("Reduce Right");
      changeTorq(&torq_straight_1, 2);
      changeTorq(&torq_straight_2, -2);
    }
    // left wheel spinning more than right wheel
    else if (rightWheelTicks < leftWheelTicks)
    // else if ((rightWheelTicks < leftWheelTicks) || isVeeringRight())
    {
      // Serial.println("Reduce Left");
      changeTorq(&torq_straight_1, -2);
      changeTorq(&torq_straight_2, 2);
    }
  }
}

/**
 * Change PWM within the boundaries of 0-254
 */
void changeTorq(int *torque, int dir)
{
  if (dir<0 && (*torque)>TORQ_MIN) (*torque)--;
  if (dir>0 && (*torque)<254) (*torque)++;
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
  if (DEBUG) Serial.println("[INFO] Moving left.");
  analogWrite(PIN_MOTOR_1_1,0);
  analogWrite(PIN_MOTOR_1_2,TORQ_TURN);
  analogWrite(PIN_MOTOR_2_1,0);
  analogWrite(PIN_MOTOR_2_2,TORQ_TURN);
}

void moveRight()
{
  if (DEBUG) Serial.println("[INFO] Moving right.");
  analogWrite(PIN_MOTOR_1_1,TORQ_TURN);
  analogWrite(PIN_MOTOR_1_2,0);
  analogWrite(PIN_MOTOR_2_1,TORQ_TURN);
  analogWrite(PIN_MOTOR_2_2,0);
}

void moveForward()
{
  if (DEBUG) Serial.println("[INFO] Moving forward.");
  analogWrite(PIN_MOTOR_1_1,torq_straight_1);
  analogWrite(PIN_MOTOR_1_2,0);
  analogWrite(PIN_MOTOR_2_1,0);
  analogWrite(PIN_MOTOR_2_2,torq_straight_2);
}

void moveBack()
{
  if (DEBUG) Serial.println("[INFO] Moving back.");
  analogWrite(PIN_MOTOR_1_1,0);
  analogWrite(PIN_MOTOR_1_2,torq_straight_1);
  analogWrite(PIN_MOTOR_2_1,torq_straight_2);
  analogWrite(PIN_MOTOR_2_2,0);
}

void stop()
{
  if (DEBUG) Serial.println("[INFO] Stopping.");
  analogWrite(PIN_MOTOR_1_1,0);
  analogWrite(PIN_MOTOR_1_2,0);
  analogWrite(PIN_MOTOR_2_1,0);
  analogWrite(PIN_MOTOR_2_2,0);
}
