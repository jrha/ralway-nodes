#include <CMRI.h>
#include <Servo.h>

/*
 * SMINI (24 inputs, 48 outputs)
 * address = 10
 */
CMRI cmri(10);

#define SERVO_COUNT 4

static uint8_t SERVO_PINS[SERVO_COUNT] = {
    6,  9, 10, 11,
};
static uint8_t SERVO_BITS[SERVO_COUNT] = {
    1,  2,  3,  4,
};
static uint8_t SERVO_OFFSETS[2][SERVO_COUNT] = {
    {  0,  0,  0,  0 },
    { 10, 10, 10, 10 },
};

bool servo_values[SERVO_COUNT] = {
    false, false, false, false,
};

Servo servos[SERVO_COUNT];


void setup() {
    Serial.begin(9600, SERIAL_8N2);
    for (int i = 0; i < SERVO_COUNT; i++) {
        servos[i].attach(SERVO_PINS[i]);
    }
}


void loop() {
    cmri.process();

    for (int i = 0; i < SERVO_COUNT; i++) {
        bool v = cmri.get_bit(SERVO_BITS[i]);
        if (v != servo_values[i]) {
            servo_values[i] = v;
            servos[i].write(SERVO_OFFSETS[v][i]);
        }
    }
}
