#include <Auto485.h>
#include <CMRI.h>
#include <Servo.h>

#include "bw_yard.h"

#define DE_PIN 2
#define RE_PIN 2

Auto485 bus(DE_PIN, RE_PIN);

#define CMRI_ADDR 10

/*
 * SUSIC Node with 2 input cards and 1 output card (48 inputs, 24 outputs)
 * First 24 inputs are reserved for feedback of outputs
 */
#define SUSIC_CARDS_INPUT 2
#define SUSIC_CARDS_OUTPUT 1
#define SUSIC_CARD_BITS 24
CMRI cmri(CMRI_ADDR, SUSIC_CARDS_INPUT * SUSIC_CARD_BITS, SUSIC_CARDS_OUTPUT * SUSIC_CARD_BITS, bus);

// As we are attaching and detaching servos as required, we only need a single Servo object
Servo servo;

// State table for servo outputs
// Used to prevent unnecessary servo activation
bool servo_states[SERVO_COUNT];

// Directly move a servo without limits or speed control
void moveServo(int id, int pos) {
    // Connect servo, move, wait for move to complete and then disconnect to prevent jittering
    if (id < SERVO_COUNT) {
        servo.attach(SERVOS[id][PIN]);
        servo.write(pos);
        delay(1500);
        servo.detach();
    }
}

// Flip the state of a servo and slowly change its physical position
void toggleServo(int id) {
    if (id < SERVO_COUNT) {
        int start_pos = SERVOS[id][servo_states[id] + 2];
        int end_pos = SERVOS[id][(!servo_states[id]) + 2];

        servo.attach(SERVOS[id][PIN]);
        digitalWrite(LED_BUILTIN, HIGH);

        if (start_pos < end_pos) {
            for (int pos = start_pos; pos <= end_pos; pos++) {
                servo.write(pos);
                digitalWrite(LED_BUILTIN, pos % 2);
                delay(SERVO_STEP_DELAY);
            }
        } else {
            for (int pos = start_pos; pos >= end_pos; pos--) {
                servo.write(pos);
                digitalWrite(LED_BUILTIN, pos % 2);
                delay(SERVO_STEP_DELAY);
            }
        }

        digitalWrite(LED_BUILTIN, LOW);
        servo.detach();
        servo_states[id] = !servo_states[id];
    }
}


// Call toggleServo if the state really has changed
void updateServo(int id, bool state) {
    if (state != servo_states[id]) {
        toggleServo(id);
    }
}


void setup() {
    bus.begin(115200);

    // Configure onboard LED for output
    pinMode(LED_BUILTIN, OUTPUT);

    // Configure servo pins and reset servos
    for (int i = 0; i < SERVO_COUNT; i++) {
        moveServo(i, 90);
    }

    // Configure output pins
    for (int i = 0; i < OUTPUT_COUNT; i++) {
        pinMode(OUTPUTS[i][PIN], OUTPUT);
    }

    // Configure input pins
    for(int i = 0; i < INPUT_DIGITAL_COUNT; i++){
        pinMode(INPUTS_DIGITAL[i][PIN], INPUT);
    }
    for(int i = 0; i < INPUT_ANALOG_COUNT; i++){
       pinMode(INPUTS_ANALOG[i][PIN], INPUT);
    }
}


void loop() {
    cmri.process();

    // Update servos with feedback
    for (int i = 0; i < SERVO_COUNT; i++) {
        updateServo(i, cmri.get_bit(SERVOS[i][BIT]));
        cmri.set_bit(SERVOS[i][BIT], servo_states[i]);
    }

    // Update outputs with feedback
    for (int i = 0; i < OUTPUT_COUNT; i++) {
        bool state = cmri.get_bit(OUTPUTS[i][BIT]);
        digitalWrite(OUTPUTS[i][PIN], state);
        cmri.set_bit(OUTPUTS[i][BIT], state);
    }

    // Update analog inputs
    for (int i = 0; i < INPUT_ANALOG_COUNT; i++) {
        cmri.set_bit(INPUTS_ANALOG[i][BIT], analogRead(INPUTS_ANALOG[i][PIN]) > INPUTS_ANALOG[i][THRESHOLD]);
    }

    // Update digital inputs
    for (int i=0; i < INPUT_DIGITAL_COUNT; i++) {
        cmri.set_bit(INPUTS_DIGITAL[i][BIT], !digitalRead(INPUTS_DIGITAL[i][PIN]));
    }
}
