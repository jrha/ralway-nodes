#include <Auto485.h>
#include <CMRI.h>
#include <Servo.h>

#include "bw_yard.h"

//#define DEBUG

Auto485 bus(RS485_PIN_DE, RS485_PIN_RE);

// Build virtual SUSIC node
#if CMRI_ADDR <= 0 or CMRI_ADDR >= 256
#error CMRI node address must be above 0 and below 256
#endif

#if SUSIC_CARDS_INPUT < SUSIC_CARDS_OUTPUT
#error To support feedback, the number of input cards must be equal or greater than the number of output cards
#endif

#if SUSIC_CARDS_INPUT + SUSIC_CARDS_OUTPUT > 64
#error Total number of SUSIC cards cannot be more than 64
#endif

#if SUSIC_CARD_BITS==24 or SUSIC_CARD_BITS==32
#else
#error SUSIC_CARD_BITS Must be either 24 or 32
#endif

CMRI cmri(CMRI_ADDR, SUSIC_CARDS_INPUT * SUSIC_CARD_BITS, SUSIC_CARDS_OUTPUT * SUSIC_CARD_BITS, bus);

// Analog Input States
#if ANALOG_READING_COUNT <= 0 and ANALOG_READING_COUNT >= 256
#error ANALOG_READING_COUNT must be between 1 and 255
#endif
byte analog_reading_index = 0;
int analog_readings[INPUT_ANALOG_COUNT][ANALOG_READING_COUNT];

// As we are attaching and detaching servos as required, we only need a single Servo object
Servo servo;

// Current position of servos
// Used to gradually move to desired position
unsigned int servo_positions[SERVO_COUNT];

// Directly move a servo without limits or speed control
void moveServo(int id, int pos) {
    // Connect servo, move, wait for move to complete and then disconnect to prevent jittering
    if (id < SERVO_COUNT) {
        servo.attach(SERVOS[id][PIN]);
        servo.write(pos);
        delay(1500);
        servo_positions[id] = pos;
        servo.detach();
    }
}


void setup() {
    bus.begin(RS485_RATE);

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

    // Use a timer interrupt to regularly sample analog inputs and update servo positions
    OCR0A = 0xAF;
    TIMSK0 |= _BV(OCIE0A);
}

/*
 * Handle interrupts generated by Timer0
 * Drives sampling of analog inputs and updates servo positions
 */
SIGNAL(TIMER0_COMPA_vect) {
    // Sample analog inputs
    unsigned long timestamp = millis();
    if (timestamp % ANALOG_SAMPLE_INTERVAL == 0) {
        refresh_analog_inputs();
    }

    // Update servo positions
    if (timestamp % SERVO_STEP_INTERVAL == 0) {
        refresh_servos();
    }
}


void refresh_servos() {
    // Update servos with feedback
    for (int i = 0; i < SERVO_COUNT; i++) {
        byte desired_position = SERVOS[i][cmri.get_bit(SERVOS[i][BIT]) + 2];
        if (servo_positions[i] != desired_position) {
            if (servo_positions[i] < desired_position) {
                servo_positions[i]++;
            } else {
                servo_positions[i]--;
            }
            servo.attach(SERVOS[i][PIN]);
            servo.write(servo_positions[i]);
            servo.detach();
        }
        cmri.set_bit(SERVOS[i][BIT], servo_positions[i] == desired_position);

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
    refresh_outputs();
    refresh_digital_inputs();
}
