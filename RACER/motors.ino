#define round(x) ((x>=0)?(int)(x+0.5):(int)(x-0.5))

/*==================================
=            Locomotion            =
==================================*/

/**
 * Use encoders/IMU to coordinate the motors. This will attempt to straighten
 * the movement of the robot.
 */
// void motorFeedback()
// {
//   // only in straight path states
//   if ((state == LEFTU_NEXT) || (state == RIGHTU_NEXT))
//   {
//     // right wheel spinning more than left wheel
//     if (rightWheelTicks > leftWheelTicks)
//     // if ((rightWheelTicks > leftWheelTicks) || isVeeringLeft())
//     {
//       changeTorq(&torq_straight_1, TORQ_FB);
//       changeTorq(&torq_straight_2, -TORQ_FB);
//     }
//     // left wheel spinning more than right wheel
//     else if (rightWheelTicks < leftWheelTicks)
//     // else if ((rightWheelTicks < leftWheelTicks) || isVeeringRight())
//     {
//       changeTorq(&torq_straight_1, -TORQ_FB);
//       changeTorq(&torq_straight_2, TORQ_FB);
//     }
//   }
// }

/**
 * Use IMU to coordinate the motors. This will attempt to straighten
 * the movement of the robot.
 */
void motorFeedback()
{
  if (curDir==EAST)
  {
    if (round(pitch)>PITCH_EW_BASE) // veering left
      compensateToRight();
    else if (round(pitch)<PITCH_EW_BASE) // veering right
      compensateToLeft();
  }
  else if (curDir==WEST)
  {
    if (round(pitch)>PITCH_EW_BASE) // veering right
      compensateToLeft();
    else if (round(pitch)<PITCH_EW_BASE) // veering left
      compensateToRight();
  }
  else if (curDir==NORTH)
  {
    // if (roll<ROLL_N) // veering left
    //   compensateToRight();
    // else if (roll>ROLL_N) // veering right
    //   compensateToLeft();
  }
  // TODO: Compensation might be too high for north...
}

/**
 * Correct veering to right. Decrease left wheel speed and increase right wheel
 * speed.
 */
void compensateToLeft()
{
  changeTorq(&torq_straight_1, -TORQ_FB);
  changeTorq(&torq_straight_2, TORQ_FB);
}

/**
 * Correct veering to left. Decrease right wheel speed and increase left wheel
 * speed.
 */
void compensateToRight()
{
  changeTorq(&torq_straight_1, TORQ_FB);
  changeTorq(&torq_straight_2, -TORQ_FB);
}

/**
 * Change PWM within the boundaries of 0-254
 */
void changeTorq(int *torque, int dir)
{
  if (dir<0 && (*torque)>TORQ_MIN) (*torque)--;
  if (dir>0 && (*torque)<TORQ_MAX) (*torque)++;
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
  analogWrite(PIN_MOTOR_1_2,TORQ_TURN);
  analogWrite(PIN_MOTOR_2_1,0);
  analogWrite(PIN_MOTOR_2_2,TORQ_TURN);
}

void moveRight()
{
  analogWrite(PIN_MOTOR_1_1,TORQ_TURN);
  analogWrite(PIN_MOTOR_1_2,0);
  analogWrite(PIN_MOTOR_2_1,TORQ_TURN);
  analogWrite(PIN_MOTOR_2_2,0);
}

void moveForward()
{
  analogWrite(PIN_MOTOR_1_1,torq_straight_1);
  analogWrite(PIN_MOTOR_1_2,0);
  analogWrite(PIN_MOTOR_2_1,0);
  analogWrite(PIN_MOTOR_2_2,torq_straight_2);
}

void moveBack()
{
  analogWrite(PIN_MOTOR_1_1,0);
  analogWrite(PIN_MOTOR_1_2,torq_straight_1);
  analogWrite(PIN_MOTOR_2_1,torq_straight_2);
  analogWrite(PIN_MOTOR_2_2,0);
}

void stop()
{
  analogWrite(PIN_MOTOR_1_1,0);
  analogWrite(PIN_MOTOR_1_2,0);
  analogWrite(PIN_MOTOR_2_1,0);
  analogWrite(PIN_MOTOR_2_2,0);
  // Reset torq variables
  torq_straight_1 = TORQ_DEFAULT;
  torq_straight_2 = TORQ_DEFAULT;
  // Reset encoder variables
  leftWheelTicks = 0;
  rightWheelTicks = 0;
}
