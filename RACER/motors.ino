/******************************************************************************
 * (18-578 / 16-778 / 24-778) MECHATRONIC DESIGN
 * TEAM F: [RACER]
 * MEMBERS:
 *          - CHOO, FOO LAI
 *          - EREBOR, TELSON
 *          - FLAREAU, JOSHUA
 *          - KALOUCHE, SIMON
 *          - TAN, NICHOLAS
 *
 * LAST REVISION: 04/03/2015
 *
 * Motor related functions. Includes moving, turning, feedback and compensation.
 *
 *****************************************************************************/

#define round(x) ((x>=0)?(int)(x+0.5):(int)(x-0.5))

/**
 * Use IMU to coordinate the motors. This will attempt to straighten
 * the movement of the robot.
 */
void motorFeedback()
{
  if ((state == LEFTU_NEXT) ||
      ((state == LAST_LAP) && (pastDir == EAST)))
  {
    if (round(pitch)>PITCH_EW_BASE) // veering left
      compensateToRight();
    else if (round(pitch)<PITCH_EW_BASE) // veering right
      compensateToLeft();
  }
  else if ((state == RIGHTU_NEXT) ||
      ((state == LAST_LAP) && (pastDir == WEST)))
  {
    if (round(pitch)>PITCH_EW_BASE) // veering right
      compensateToLeft();
    else if (round(pitch)<PITCH_EW_BASE) // veering left
      compensateToRight();
  }
  else if ((state == LEFTU_2) ||
           (state == RIGHTU_2))
  {
    if (round((roll-ROLL_N)/SCALE_FACTOR)<ROLL_N)
      compensateToRight();
    else if (round((roll-ROLL_N)/SCALE_FACTOR)>ROLL_N)
      compensateToLeft();
  }
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
  analogWrite(PIN_MOTOR_1_2,TORQ_TURN_LE);
  analogWrite(PIN_MOTOR_2_1,0);
  analogWrite(PIN_MOTOR_2_2,TORQ_TURN_GE);
}

void moveRight()
{
  analogWrite(PIN_MOTOR_1_1,TORQ_TURN_GE);
  analogWrite(PIN_MOTOR_1_2,0);
  analogWrite(PIN_MOTOR_2_1,TORQ_TURN_LE);
  analogWrite(PIN_MOTOR_2_2,0);
}

void moveForward()
{
  analogWrite(PIN_MOTOR_1_1,torq_straight_1);
  analogWrite(PIN_MOTOR_1_2,0);
  analogWrite(PIN_MOTOR_2_1,0);
  analogWrite(PIN_MOTOR_2_2,torq_straight_2);
}

void moveUp()
{
  analogWrite(PIN_MOTOR_1_1,TORQ_UP);
  analogWrite(PIN_MOTOR_1_2,0);
  analogWrite(PIN_MOTOR_2_1,0);
  analogWrite(PIN_MOTOR_2_2,TORQ_UP);
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
}
