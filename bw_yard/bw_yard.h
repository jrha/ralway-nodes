#ifndef bw_yard_h
#define bw_yard_h


#define BIT 0
#define PIN 1
#define THRESHOLD 2
#define CLOSED 2
#define THROWN 3


#define SERVO_COUNT 5

#define SERVO_STEP_INTERVAL 50
#define ANALOG_SAMPLE_INTERVAL 200

// Mapping of CMRI bit to arduino pin number for servos
// Limits of servo travel for CLOSED ahead and THROWN positions
// Normally PWM pins are used here
// Bits should be in the range 0-9
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
// Bits should be in the range 10-19
static uint8_t OUTPUTS[OUTPUT_COUNT][2] = {
    // BIT, PIN
    {10, 7},
    {11, 8},
    {12, 12},
};


#define INPUT_ANALOG_COUNT 5
// Mapping of CMRI bit to arduino pin number for inputs
// These can only be analog inputs
// Bits should be in the range 20-29
static unsigned int INPUTS_ANALOG[INPUT_ANALOG_COUNT][3] = {
    // Current Sensors
    // BIT, PIN, THRESHOLD
    {20, A0, 600},
    {21, A1, 600},
    {22, A2, 600},
    {23, A3, 600},
    {24, A4, 600},
};

#define INPUT_DIGITAL_COUNT 5
// Mapping of CMRI bit to arduino pin number for inputs
// Physically these can be digital or analog inputs, but will all be treated as digital inputs
// Bits should be in the range 30-39
static uint8_t INPUTS_DIGITAL[INPUT_DIGITAL_COUNT][2] = {
    // Position Sensors
    // BIT, PIN
    {30, A7},
    {31, A6},
    {32, A5},
    {33,  4},
    {34,  3},
};

#endif
