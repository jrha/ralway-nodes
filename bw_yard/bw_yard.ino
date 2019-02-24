#include <CMRI.h>
#include <Servo.h>

#include "bw_yard.h"

/*
 * SMINI (24 inputs, 48 outputs)
 * address = 10
 */
CMRI cmri(10);

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

        /*
        Serial.print("# moving servo ");
        Serial.print(id);
        Serial.print(" from ");
        Serial.print(start_pos);
        Serial.print(" to ");
        Serial.print(end_pos);
        Serial.println(" #");
        */

        servo.attach(SERVOS[id][PIN]);
        digitalWrite(LED_BUILTIN, HIGH);

        if (start_pos < end_pos) {
            for (int pos = start_pos; pos <= end_pos; pos++) {
                servo.write(pos);
                digitalWrite(LED_BUILTIN, pos % 2);
                Serial.print("# ");
                Serial.println(pos);
                delay(SERVO_STEP_DELAY);
            }
        } else {
            for (int pos = start_pos; pos >= end_pos; pos--) {
                servo.write(pos);
                digitalWrite(LED_BUILTIN, pos % 2);
                Serial.print("# ");
                Serial.println(pos);
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
        Serial.print("# state changed ");
        toggleServo(id);
    }
}


void setup() {
    Serial.begin(9600, SERIAL_8N2);

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
    for (int i = 0; i < INPUT_COUNT; i++) {
        pinMode(INPUTS[i][PIN], INPUT);
    }
}


void loop() {
    cmri.process();

    // Update servos
    for (int i = 0; i < SERVO_COUNT; i++) {
        updateServo(i, cmri.get_bit(SERVOS[i][BIT]));
    }

    // Update outputs
    for (int i = 0; i < OUTPUT_COUNT; i++) {
        digitalWrite(OUTPUTS[i][PIN], cmri.get_bit(OUTPUTS[i][BIT]));
    }

    // Update inputs
    for (int i = 0; i < INPUT_COUNT; i++) {
        cmri.set_bit(INPUTS[i][BIT], !digitalRead(INPUTS[i][PIN]));
    }
}
