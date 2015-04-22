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
 * Pathfinding. Includes a finite state machine for pathfinding.
 *
 *****************************************************************************/

void pathfindingFSM()
{
  switch (state)
  {
    // TRAVELLING EAST
    case LEFTU_NEXT: // move forward, left u-turn next
      moveForward();
      if (irDist < THR_IR_TURN)
      {
        state = LEFTU_1;
        stop();
      }
      break;
    case LEFTU_1: // left turn
      moveLeft();
      if (pastDir == NORTH)
      {
        state = LEFTU_2;
        targetTime = TIME_UFOR; // Set target time for UFOR
        stop();
      }
      break;
    case LEFTU_2: // straight
      moveUp();
      if (hasTravelledUFOR())
      {
        state = LEFTU_3;
        stop();
      }
      if (irDist < THR_IR_LAST)
      {
        isLastLap = true;
        state = LEFTU_3;
        stop();
      }
      break;
    case LEFTU_3: // left turn
      moveLeft();
      if (pastDir == WEST)
      {
        state = (isLastLap) ? LAST_LAP: RIGHTU_NEXT;
        stop();
      }
      break;
    // TRAVELLING WEST
    case RIGHTU_NEXT: // move forward, right u-turn next
      moveForward();
      if (irDist < THR_IR_TURN)
      {
        state = RIGHTU_1;
        stop();
      }
      break;
    case RIGHTU_1: // right turn
      moveRight();
      if (pastDir == NORTH)
      {
        state = RIGHTU_2;
        targetTime = TIME_UFOR; // Set target time for UFOR
        stop();
      }
      break;
    case RIGHTU_2: // straight
      moveUp();
      if (hasTravelledUFOR())
      {
        state = RIGHTU_3;
        stop();
      }
      if (irDist < THR_IR_LAST)
      {
        isLastLap = true;
        state = RIGHTU_3;
        stop();
      }
      break;
    case RIGHTU_3: // right turn
      moveRight();
      if (pastDir == EAST)
      {
        state = (isLastLap) ? LAST_LAP : LEFTU_NEXT;
        stop();
      }
      break;
    case LAST_LAP: // last lap forward, stop after
      moveForward();
      if (irDist < THR_IR_TURN)
      {
        state = STOP;
        stop();
      }
      break;
    case STOP:
      stop();
      break;
  }
}

boolean hasTravelledUFOR()
{
  targetTime--;
  if (targetTime <= 0) return true;
  else return false;
}
