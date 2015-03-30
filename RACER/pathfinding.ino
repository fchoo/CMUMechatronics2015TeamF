
/*=======================================*
 *           Pathfinding                 *
 *=======================================*/

boolean hasTravelledUFOR()
{
  targetTime--;
  if (targetTime <= 0) return true;
  else return false;
}

void pathfindingFSM()
{
  switch (state)
  {
    case LEFTU_NEXT: // move forward, left u-turn next
      moveForward();
      if (irDist < THR_IR)
      {
        state = LEFTU_1;
        stop();
      }
      break;
    case LEFTU_1: // left turn
      moveLeft();
      if (curDir == NORTH)
      {
        state = LEFTU_2;
        targetTime = TIME_UFOR; // Set target time for UFOR
        stop();
      }
      break;
    case LEFTU_2: // straight
      moveForward();
      if (hasTravelledUFOR())
      {
        state = LEFTU_3;
        stop();
      }
      break;
    case LEFTU_3: // left turn
      moveLeft();
      if (curDir == WEST)
      {
        state = RIGHTU_NEXT;
        stop();
      }
      break;
    case RIGHTU_NEXT: // move forward, right u-turn next
      moveForward();
      if (irDist < THR_IR)
      {
        state = RIGHTU_1;
        stop();
      }
      break;
    case RIGHTU_1: // right turn
      moveRight();
      if (curDir == NORTH)
      {
        state = RIGHTU_2;
        targetTime = TIME_UFOR; // Set target time for UFOR
        stop();
      }
      break;
    case RIGHTU_2: // straight
      moveForward();
      if (hasTravelledUFOR())
      {
        state = RIGHTU_3;
        stop();
      }
      break;
    case RIGHTU_3: // right turn
      moveRight();
      if (curDir == EAST)
      {
        state = LEFTU_NEXT;
        stop();
      }
      break;
  }
}

void setTargetDist(float dist)
{
  targetDist = dist;
}
