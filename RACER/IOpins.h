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
 * IO pins assignment
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
#define PIN_MOTOR_1_1 7
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

// Assigned pin numbers for motor encoders
// PIN 3 --> WHEEL 1
// PIN 2 --> WHEEL 2
