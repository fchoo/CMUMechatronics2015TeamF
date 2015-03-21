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
 * LAST REVISION: 03/20/2015
 *
 * Configuration data for RACER.ino
 *
 *****************************************************************************/

/*
  Assigned motor numbers for RACER
              0

           (front)
              |
              |
  -90  1 |---------| 2  90
              |
              |
            (back)

             180
*/

// Assigned pin numbers
// For Motors
#define PIN_MOTOR_1_1 3
#define PIN_MOTOR_1_2 5
#define PIN_MOTOR_2_1 6
#define PIN_MOTOR_2_2 9
#define PIN_EDF 10  // Digital PWM pin for EDF.
#define PIN_KILL 4
#define PIN_LED 13
#define PIN_IR A1
#define PIN_JOYX A3 // joystick X axis
#define PIN_JOYY A4 // joystick Y axis
#define PIN_POT A5  // Pot to control edf

// EDF Control
#define PWM_MIN 120 //133 for actual serial
#define PWM_MAX 240
#define PWM_DELAY 50 // .05s
#define PWM_STEPSIZE 1

// define number of readings sensors take in
#define FILTER_AVG 12

// define IR sensor distance threshold (in cm) before turning
#define THRESHOLD_IR 10

// Motor
#define TORQ_DEFAULT 200
#define TORQ_TURN 200   // Analog value for turning torq
#define DIST_TURN90 2.5 // Dist for encoder to check 90
#define DIST_UFOR 10 // Dist for encoder to move forward during uturn

// Joystick parameters
#define JOY_RANGE 12 // output range of X or Y movement
#define JOY_DELAY 5 // response delay of the mouse, in ms
#define JOY_THRESHOLD JOY_RANGE/4      // resting threshold
#define JOY_CENTER JOY_RANGE/2         // resting position value
