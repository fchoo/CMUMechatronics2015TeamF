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
              90

           (front)
              |
              |
    0  1 |---------| 2  0
              |
              |
            (back)

             -90
*/

// Assigned pin numbers
// For Motors
#define PIN_MOTOR_1_1 10
#define PIN_MOTOR_1_2 11
#define PIN_MOTOR_2_1 8
#define PIN_MOTOR_2_2 9
#define PIN_EDF 4  // Digital PWM pin for EDF.
#define PIN_KILL 7
#define PIN_LED 13
#define PIN_IR A1
#define PIN_JOYX A2 // joystick X axis
#define PIN_JOYY A3 // joystick Y axis
#define PIN_POT A4  // Pot to control edf

// Assigned pin numbers for motor encoders
// PIN 2 --> WHEEL 1
// PIN 3 --> WHEEL 2
