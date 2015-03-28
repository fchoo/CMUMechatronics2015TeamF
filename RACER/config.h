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

// EDF Control
#define PWM_MIN 132 //133 for actual serial
#define PWM_MAX 150
#define PWM_DEFAULT 220
#define PWM_DELAY 50 // .05s

// define number of readings sensors take in
#define FILTER_AVG 12

// define IR sensor distance threshold (in cm) before turning
#define THR_IR 10

// IMU vertical checking threshold
#define THR_ROLL_VERT 20
#define THR_PITCH_VERT 70
#define THR_HORZ_DUR 10
// IMU heading checking parameters
#define ROLL_ERR 5
#define PITCH_ERR 5
#define PITCH_N (90-PITCH_ERR)
#define PITCH_S -(90-PITCH_ERR)
#define PITCH_EW (0+PITCH_ERR)
#define ROLL_E (90-ROLL_ERR)
#define ROLL_W -(90-ROLL_ERR)
// Use pitch for steering

// Up: -10, 86
// Left: -88, 5
// Right: 88, 5
// Down: 155, -87

// Motor
#define TORQ_MIN 100
#define TORQ_MAX 250
#define TORQ_DEFAULT 160
#define TORQ_TURN 50   // 100 for wall, 50 for floor Analog value for turning torq
#define DIST_TURN90_1 2.8 // Dist for encoder to check 90
#define DIST_TURN90_2 2.9 // Dist for encoder to check 90
#define DIST_TURN90_3 3.0 // Dist for encoder to check 90
#define DIST_TURN90_4 3.0 // Dist for encoder to check 90
#define DIST_UFOR 2.0 // Dist for encoder to move forward during uturn
#define TORQ_FB 10

// Joystick parameters
#define JOY_RANGE 12 // output range of X or Y movement
#define JOY_DELAY 5 // response delay of the mouse, in ms
#define JOY_THRESHOLD JOY_RANGE/4      // resting threshold
#define JOY_CENTER JOY_RANGE/2         // resting position value

// FSM
typedef enum State {
    LEFTU_NEXT,
    LEFTU_1,
    LEFTU_2,
    LEFTU_3,
    RIGHTU_NEXT,
    RIGHTU_1,
    RIGHTU_2,
    RIGHTU_3
};

typedef enum Heading {
    NORTH,
    SOUTH,
    EAST,
    WEST
};

/**
 * 1 - Move forward; left U-turn next
 * 2 - Left turn
 * 3 - Straight ahead
 * 4 - Left turn
 * 5 - Move forward; right U-turn next
 * 6 - Right turn
 * 7 - Straight ahead
 * 8 - Right turn
 * 9 - Move forward; last lap
 * 10 - STOP
 */
