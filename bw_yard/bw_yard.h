#ifndef bw_yard_h
#define bw_yard_h


#define BIT 0
#define PIN 1
#define THRESHOLD 2
#define CLOSED 2
#define THROWN 3


#define SERVO_COUNT 5

#define SERVO_STEP_DELAY 50

// Mapping of CMRI bit to arduino pin number for servos
// Limits of servo travel for CLOSED ahead and THROWN positions
// Normally PWM pins are used here
static uint8_t SERVOS[SERVO_COUNT][4] = {
    // BIT, PIN, CLOSED, THROWN
    {0,  9,  90,  70}, // First 3-way point, right-hand route
    {1,  5,  80, 100}, // First 3-way point, left-hand route
    {2,  6,  85,  95}, // Second 3-way point, right-hand route
    {3, 10,  95,  85}, // Second 3-way point, left-hand route
    {4, 11, 100, 135}, // Yard Stop Signal
};


#define OUTPUT_COUNT 3
// Mapping of CMRI bit to arduino pin number for regular outputs
// Normally digital pins are used here
static uint8_t OUTPUTS[OUTPUT_COUNT][2] = {
    // BIT, PIN
    {10, 7},
    {11, 8},
    {12, 12},
};


#define INPUT_ANALOG_COUNT 5
// Mapping of CMRI bit to arduino pin number for inputs
// These can only be analog inputs
static uint8_t INPUTS_ANALOG[INPUT_ANALOG_COUNT][3] = {
    // Current Sensors
    // BIT, PIN, THRESHOLD
    {0, A0, 30},
    {1, A1, 30},
    {2, A2, 30},
    {3, A3, 30},
    {4, A4, 30},
};

#define INPUT_DIGITAL_COUNT 5
// Mapping of CMRI bit to arduino pin number for inputs
// Physically these can be digital or analog inputs, but will all be treated as digital inputs
static uint8_t INPUTS_DIGITAL[INPUT_DIGITAL_COUNT][2] = {
    // Position Sensors
    // BIT, PIN
    {10, A7},
    {11, A6},
    {12, A5},
    {13,  4},
    {14,  3},
};

#endif
