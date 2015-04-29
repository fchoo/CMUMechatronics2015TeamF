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
#define D_TORQ_MIN 160
#define D_TORQ_MAX 255
#define D_TORQ_DEFAULT 230

// FSM
typedef enum State {
    WAIT,
    CLEAN,
    STOP
};
