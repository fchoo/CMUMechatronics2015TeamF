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
 * Sensors related functions. Includes initialization, reading and calculation
 * functions for IR, IMU and encoders.
 *
 *****************************************************************************/

void updateAngles()
{
  roll = getRoll();
  pitch = getPitch();
}

void checkHeading()
{
  if (pitch >= PITCH_N) curDir = NORTH;
  else if (pitch <= PITCH_S) curDir = SOUTH;
  else if (abs(pitch) <= PITCH_EW)
  {
    if (roll >= ROLL_E) curDir = EAST;
    else if (roll <= ROLL_W) curDir = WEST;
  }
  else
    curDir = TURNING;
  if (curDir != TURNING)
    pastDir = curDir;
}

void checkVertical()
{
  if (abs(roll)<THR_ROLL_VERT && pitch < THR_PITCH_VERT)
    // imu is horizontal
    horzDur++;
  else
    // imu is vertical
    horzDur = 0; // reset duration
  // Update vertical flag
  isVert = (horzDur > THR_HORZ_DUR) ? false : true;
}

void readIR()
/* Function takes "loopCount" number of IR sensor readings and
 * stores the linearized value in float "irDist"
 */
{
  irRawData = analogRead(PIN_IR);                        // read sensor 1
  irSmoothData = digitalSmooth(irRawData, irArray);  // every sensor you use with digitalSmooth needs its own array
  irDist = 12343.85 * pow(irSmoothData, -1.15); // Linearizing eqn, accuracy +- 5%
}

/** A digital smoothing filter for smoothing sensor jitter
 This filter accepts one new piece of data each time through a loop, which the
 filter inputs into a rolling array, replacing the oldest data with the latest reading.
 The array is then transferred to another array, and that array is sorted from low to high.
 Then the highest and lowest %15 of samples are thrown out. The remaining data is averaged
 and the result is returned.
 */
float digitalSmooth(float rawIn, float *sensSmoothArray)
{
  // "int *sensSmoothArray" passes an array to the function - the asterisk indicates the array name is a pointer
  int j, k, temp, top, bottom;
  long total;
  static int i;
  static float sorted[FILTER_SAMPLE];
  boolean done;

  i = (i + 1) % FILTER_SAMPLE;    // increment counter and roll over if necc. -  % (modulo operator) rolls over variable
  sensSmoothArray[i] = rawIn;                 // input new data into the oldest slot

  // Serial.print("raw = ");

  for (j=0; j<FILTER_SAMPLE; j++){     // transfer data array into anther array for sorting and averaging
    sorted[j] = sensSmoothArray[j];
  }

  done = 0;                // flag to know when we're done sorting
  while(done != 1){        // simple swap sort, sorts numbers from lowest to highest
    done = 1;
    for (j = 0; j < (FILTER_SAMPLE - 1); j++){
      if (sorted[j] > sorted[j + 1]){     // numbers are out of order - swap
        temp = sorted[j + 1];
        sorted [j+1] =  sorted[j] ;
        sorted [j] = temp;
        done = 0;
      }
    }
  }

  // throw out top and bottom 15% of samples - limit to throw out at least one from top and bottom
  bottom = max(((FILTER_SAMPLE * 15)  / 100), 1);
  top = min((((FILTER_SAMPLE * 85) / 100) + 1  ), (FILTER_SAMPLE - 1));   // the + 1 is to make up for asymmetry caused by integer rounding
  k = 0;
  total = 0;
  for ( j = bottom; j< top; j++){
    total += sorted[j];  // total remaining indices
    k++;
  }

  return total / k;    // divide by number of samples
}

// void checkEncoder(int wheelNum)
// {
//   Serial.println("[INFO] CHECKING ENCODER");
//   if (wheelNum == 1)
//     curDist = rightWheelTicks/5000*360/360*21.5;
//   else if (wheelNum == 2)
//     curDist = leftWheelTicks/5000*360/360*21.5;
// }

// /**
//  * ISR routines for encoders
//  */

// void updateRightTick()
// {
//   rightWheelTicks++;
// }

// void updateLeftTick()
// {
//   leftWheelTicks++;
// }
