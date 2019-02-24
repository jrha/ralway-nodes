#ifndef bw_yard_h
#define bw_yard_h


#define BIT 0
#define PIN 1
#define CLOSED 2
#define THROWN 3


#define SERVO_COUNT 5

#define SERVO_STEP_DELAY 100

// Mapping of CMRI bit to arduino pin number for servos
// Limits of servo travel for CLOSED ahead and THROWN positions
// Normally PWM pins are used here
static uint8_t SERVOS[SERVO_COUNT][4] = {
    // BIT, PIN, CLOSED, THROWN
    {1,  5,  75, 115}, // First 3-way point, right-hand route
    {2,  9,  90,  70}, // First 3-way point, left-hand route
    {3,  6,  89,  91}, // Second 3-way point, right-hand route
    {4, 10,  91,  81}, // Second 3-way point, left-hand route
    {5, 11,  80, 110}, // Yard Stop Signal
};


#define OUTPUT_COUNT 4
// Mapping of CMRI bit to arduino pin number for regular outputs
// Normally digital pins are used here
static uint8_t OUTPUTS[OUTPUT_COUNT][2] = {
    // BIT, PIN
    {6,  4},
    {7,  7},
    {8,  8},
    {9, 12},
};


#define INPUT_COUNT 8
// Mapping of CMRI bit to arduino pin number for inputs
// Normally digital and analog pins are used here
static uint8_t INPUTS[INPUT_COUNT][2] = {
    // BIT, PIN
    {A0,  1},
    {A1,  2},
    {A2,  3},
    {A3,  4},
    {A4,  5},
    {A5,  6},
    //{A6,  7}, //Analog only, needs manual conversion
    //{A7,  8}, //Analog only, needs manual conversion
    { 1,  9},
    { 2, 10},
};


// State table for servo outputs
// Used to prevent unnecessary servo activation
bool servo_states[SERVO_COUNT];

#endif
