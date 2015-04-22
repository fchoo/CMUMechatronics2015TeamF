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
 * IO pins assignment
 *
 *****************************************************************************/

/*
  Assigned motor/edf numbers for RACER
               90
            (front)
              EDF1
               |
               |
    0  M1 |---------| M2  0
               |
               |
              EDF2
             (back)
              -90
*/

// Assigned pin numbers
// For Motors
#define PIN_MOTOR_1_1 10
#define PIN_MOTOR_1_2 5
#define PIN_MOTOR_2_1 2
#define PIN_MOTOR_2_2 3
// EDFs
#define PIN_EDF_1 11
#define PIN_EDF_2 12
// Pump
#define PIN_PUMP 9
// Switches
#define PIN_RSTIMU 40    // Recalibrate IMU
#define PIN_PATHFIND 38  // Activate pathfind
#define PIN_EDFSW 36      // Kill switch
// LED
#define PIN_LED 13
// Sensors
#define PIN_IR A7
// LEDs
#define PIN_RED2 53
#define PIN_RED1 51
#define PIN_BLUE 47
#define PIN_GREEN2 49
#define PIN_GREEN1 45

// Joystick
#define PIN_JOYX A2 // joystick X axis
#define PIN_JOYY A3 // joystick Y axis
#define PIN_POT A4  // Pot to control edf

// Assigned pin numbers for motor encoders
// PIN 2 --> WHEEL 1
// PIN 3 --> WHEEL 2
