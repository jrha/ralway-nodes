#include <Auto485.h>
#include <CMRI.h>
#include <Servo.h>

#include "bw_yard.h"

//#define DEBUG

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

// Analog Input States
#define ANALOG_READING_COUNT 8
int analog_reading_index = 0;
int analog_readings[INPUT_ANALOG_COUNT][ANALOG_READING_COUNT];

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
        moveServo(i, SERVOS[i][CLOSED]);
        #ifdef DEBUG
            bus.print("Reset servo ");
            bus.print(i);
            bus.print(" to ");
            bus.println(SERVOS[i][CLOSED]);
        #endif
    }

    // Configure output pins
    for (int i = 0; i < OUTPUT_COUNT; i++) {
        pinMode(OUTPUTS[i][PIN], OUTPUT);
        #ifdef DEBUG
            bus.print("Set output mode on pin ");
            bus.println(OUTPUTS[i][PIN]);
        #endif
    }

    // Configure input pins
    for (int i = 0; i < INPUT_DIGITAL_COUNT; i++){
        pinMode(INPUTS_DIGITAL[i][PIN], INPUT);
        #ifdef DEBUG
            bus.print("Set input mode on pin ");
            bus.println(INPUTS_DIGITAL[i][PIN]);
        #endif
    }
    for (int i = 0; i < INPUT_ANALOG_COUNT; i++){
       pinMode(INPUTS_ANALOG[i][PIN], INPUT);
       #ifdef DEBUG
            bus.print("Set input mode on pin ");
            bus.println(INPUTS_ANALOG[i][PIN]);
        #endif
    }

    // Use a timer interrupt to regularly sample analog inputs
    OCR0A = 0xAF;
    TIMSK0 |= _BV(OCIE0A);
}


SIGNAL(TIMER0_COMPA_vect) {
    // Sample analog inputs
    unsigned long timestamp = millis();
    if (timestamp % ANALOG_SAMPLE_INTERVAL == 0) {
        refresh_analog_inputs();
    }
}


void refresh_servos() {
    // Update servos with feedback
    for (int i = 0; i < SERVO_COUNT; i++) {
        updateServo(i, cmri.get_bit(SERVOS[i][BIT]));
        cmri.set_bit(SERVOS[i][BIT], servo_states[i]);
        #ifdef DEBUG
            bus.print("Updated servo ");
            bus.print(i);
            bus.print(" from bit ");
            bus.println(SERVOS[i][BIT]);
        #endif
    }
}


void refresh_outputs() {
    // Update outputs with feedback
    for (int i = 0; i < OUTPUT_COUNT; i++) {
        bool state = cmri.get_bit(OUTPUTS[i][BIT]);
        digitalWrite(OUTPUTS[i][PIN], state);
        cmri.set_bit(OUTPUTS[i][BIT], state);
        #ifdef DEBUG
            bus.print("Updated output ");
            bus.print(i);
            bus.print(" on pin ");
            bus.print(OUTPUTS[i][PIN]);
            bus.print(" from bit ");
            bus.println(OUTPUTS[i][BIT]);
        #endif
    }
}


void refresh_analog_inputs() {
    // Update analog inputs
    for (int i = 0; i < INPUT_ANALOG_COUNT; i++) {
        analog_readings[i][analog_reading_index] = analogRead(INPUTS_ANALOG[i][PIN]);

        analog_reading_index++;
        if (analog_reading_index >= ANALOG_READING_COUNT) {
            analog_reading_index = 0;
        }

        int maxima = -1;
        int minima = 2048;

        for (int j = 0; j < ANALOG_READING_COUNT; j++) {
            maxima = max(maxima, analog_readings[i][j]);
            minima = min(minima, analog_readings[i][j]);
        }

        int range = maxima - minima;
        cmri.set_bit(INPUTS_ANALOG[i][BIT], range > INPUTS_ANALOG[i][THRESHOLD]);

        #ifdef DEBUG
            bus.print("Updated analog input ");
            bus.print(i);
            bus.print(" on pin ");
            bus.print(INPUTS_ANALOG[i][PIN]);
            bus.print(" to bit ");
            bus.print(INPUTS_ANALOG[i][BIT]);
            bus.print(" with range ");
            bus.print(range);
            bus.print(" -> ");
            bus.print(range > INPUTS_ANALOG[i][THRESHOLD]);
            bus.print(" using threshold ");
            bus.println(INPUTS_ANALOG[i][THRESHOLD]);
        #endif
    }
}


void refresh_digital_inputs () {
    // Update digital inputs
    for (int i=0; i < INPUT_DIGITAL_COUNT; i++) {
        bool value = 0;
        // Use analog read with a default threshold for analog pins
        if (INPUTS_DIGITAL[i][PIN] < A0) {
            value = !digitalRead(INPUTS_DIGITAL[i][PIN]);
        } else {
            value = analogRead(INPUTS_DIGITAL[i][PIN]) < 512;
        }

        cmri.set_bit(INPUTS_DIGITAL[i][BIT], value);
        #ifdef DEBUG
            bus.print("Updated digital input ");
            bus.print(i);
            bus.print(" on pin ");
            bus.print(INPUTS_DIGITAL[i][PIN]);
            bus.print(" to bit ");
            bus.print(INPUTS_DIGITAL[i][BIT]);
            bus.print(" with value ");
            bus.println(value);
        #endif
    }
}


void loop() {
    cmri.process();
    refresh_servos();
    refresh_outputs();
    refresh_digital_inputs();
}
