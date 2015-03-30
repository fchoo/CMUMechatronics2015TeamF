
/*=======================================*
 *           Pathfinding                 *
 *=======================================*/

void updateFlags()
{
  // Serial.print("[INFO] State: ");
  // Serial.println(state);
  switch (state)
  {
    // Waiting for U-turn
    case LEFTU_NEXT:
    case RIGHTU_NEXT:
      if (irDist < THR_IR) irFlag = true;
      break;
    // In U-turn
    case LEFTU_1:
    case LEFTU_2:
    case LEFTU_3:
      checkEncoder(1);
      if (targetDist < curDist) encoderFlag = true;
      break;
    case RIGHTU_1:
    case RIGHTU_2:
    case RIGHTU_3:
      checkEncoder(2);
      if (targetDist < curDist) encoderFlag = true;
      break;
  }
}

void pathfindingFSM()
{
  switch (state)
  {
    case LEFTU_NEXT: // move forward, left u-turn next
      moveForward();
      if (irFlag)
      {
        state = LEFTU_1;
        irFlag = false;
        setTargetDist(DIST_TURN90_1);
        stop();
      }
      break;
    case LEFTU_1: // left turn
      moveLeft();
      if (encoderFlag)
      {
        state = LEFTU_2;
        encoderFlag = false;
        setTargetDist(DIST_UFOR);
        stop();
      }
      break;
    case LEFTU_2: // straight
      moveForward();
      if (encoderFlag)
      {
        state = LEFTU_3;
        encoderFlag = false;
        setTargetDist(DIST_TURN90_2);
        stop();
      }
      break;
    case LEFTU_3: // left turn
      moveLeft();
      if (encoderFlag)
      {
        state = RIGHTU_NEXT;
        encoderFlag = false;
        stop();
      }
      break;
    case RIGHTU_NEXT: // move forward, right u-turn next
      moveForward();
      if (irFlag)
      {
        state = RIGHTU_1;
        irFlag = false;
        setTargetDist(DIST_TURN90_3);
        stop();
      }
      break;
    case RIGHTU_1: // right turn
      moveRight();
      if (encoderFlag)
      {
        state = RIGHTU_2;
        encoderFlag = false;
        setTargetDist(DIST_UFOR);
        stop();
      }
      break;
    case RIGHTU_2: // straight
      moveForward();
      if (encoderFlag)
      {
        state = RIGHTU_3;
        encoderFlag = false;
        setTargetDist(DIST_TURN90_4);
        stop();
      }
      break;
    case RIGHTU_3: // right turn
      moveRight();
      if (encoderFlag)
      {
        state = LEFTU_NEXT;
        encoderFlag = false;
        stop();
      }
      break;
  }
}

void setTargetDist(float dist)
{
  targetDist = dist;
}
