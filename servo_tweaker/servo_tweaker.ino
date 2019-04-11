#include <Servo.h>

String inputId = "";
String inputPosition = "";
bool gotId = false;
bool gotPosition = false;

#define SERVO_COUNT 6

static int SERVO_PINS[SERVO_COUNT] = {
    9,
    5,
    6,
    10,
    11,
};


Servo servo;


void moveServo(int id, int pos) {
    // Connect servo, move, wait for move to complete and then disconnect to prevent jittering
    if (id < SERVO_COUNT) {
        servo.attach(SERVO_PINS[id]);
        servo.write(pos);
        delay(1500);
        servo.detach();
    }
}


void setup() {
    Serial.begin(9600);
    inputId.reserve(20);
    inputPosition.reserve(20);
    // Reset all servos to mid point
    for (int i = 0; i < SERVO_COUNT; i++) {
        moveServo(i, 90);
    }
}


void loop() {
    if (gotId && gotPosition) {
        int id = max(0, min(inputId.toInt(), 180));
        int pos = max(0, min(inputPosition.toInt(), 180));

        Serial.print("| moving servo ");
        Serial.print(id);
        Serial.print(" to position ");
        Serial.print(pos);
        Serial.println(" |");

        moveServo(id, pos);

        // clear the inputs and reset states
        inputId = "";
        inputPosition = "";
        gotId = false;
        gotPosition = false;
    }
}


void serialEvent() {
    while (Serial.available()) {
        char inChar = (char)Serial.read();

        if (inChar == ' ') {
            gotId = true;
        } else if (inChar == '\n') {
            gotId = true;
            gotPosition = true;
        } else if (!gotId) {
            inputId += inChar;
        } else if (!gotPosition) {
            inputPosition += inChar;
        }

    /*
        Serial.println("       inputId = " + inputId);
        Serial.println(" inputPosition = " + inputPosition);
        Serial.print("         gotId = "); Serial.println(gotId);
        Serial.print("   gotPosition = "); Serial.println(gotPosition);
        Serial.println("--------");
        */
    }
}
