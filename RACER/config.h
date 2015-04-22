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
 * Configuration data for RACER.ino
 *
 *****************************************************************************/

// EDF Control
#define EDF_MIN 124 //133 for actual serial
#define EDF_MAX 254
#define EDF_DEFAULT 220
#define EDF_DELAY 50 // .05s

// define number of readings sensors take in
#define FILTER_SAMPLE 13

// define IR sensor distance threshold (in cm) before turning
#define THR_IR_TURN 20
#define THR_IR_LAST 15

// IMU vertical checking threshold
#define THR_ROLL_VERT 10
#define THR_PITCH_VERT 70
#define THR_HORZ_DUR 10
// IMU heading checking parameters
#define PITCH_NS_BASE 90
#define PITCH_EW_BASE 0
#define ROLL_ERR 10
#define PITCH_NS_ERR 10
#define PITCH_EW_ERR 10

#define PITCH_N (PITCH_NS_BASE-PITCH_NS_ERR)
#define PITCH_S -(PITCH_NS_BASE-PITCH_NS_ERR)
#define PITCH_EW (PITCH_EW_BASE+PITCH_EW_ERR)
#define ROLL_E (PITCH_NS_BASE-ROLL_ERR)
#define ROLL_W -(PITCH_NS_BASE-ROLL_ERR)
#define ROLL_N 0

// Motor
#define TORQ_MIN 80
#define TORQ_MAX 200
#define TORQ_DEFAULT 120
#define TORQ_UP 255
#define TORQ_TURN_LE 50
#define TORQ_TURN_GE 50
#define TORQ_FB 10          // For compensation
#define TIME_UFOR 600       // Time for travel straight
// Debounce delay
#define DEBOUNCE_DELAY 25

// FSM
typedef enum State {
    LEFTU_NEXT,
    LEFTU_1,
    LEFTU_2,
    LEFTU_3,
    RIGHTU_NEXT,
    RIGHTU_1,
    RIGHTU_2,
    RIGHTU_3,
    LAST_LAP,
    STOP
};

typedef enum Heading {
    NORTH,
    SOUTH,
    EAST,
    WEST,
    TURNING
};
