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
 * Configuration data for SWIPER.ino
 *
 *****************************************************************************/

// IMU Constant
#define ROLL_FLAT 0
// Motor Constant
#define D_TORQ_MIN_1 100
#define D_TORQ_MAX_1 140
#define D_TORQ_DEFAULT_1 130
#define D_TORQ_MIN_2 100
#define D_TORQ_MAX_2 140
#define D_TORQ_DEFAULT_2 130

// EDF Control
#define EDF_MIN 124
#define EDF_MAX 254
#define EDF_DELAY 50 // .05s

// FSM
typedef enum State {
    WAIT,
    EDF,
    CLEAN,
    STOP
};
