#include <Servo.h>

String inputId = "";
String inputPosition = "";
bool gotId = false;
bool gotPosition = false;


static int SERVO_PINS[5] = {
  3,
  5,
  6,
  9,
  10,
};

Servo servo;


void moveServo(int id, int pos) {
  // Connect servo, move, wait for move to complete and then disconnect to prevent jittering
  servo.attach(SERVO_PINS[id]);
  servo.write(pos);
  delay(1500);
  servo.detach();
}



void setup() {
  Serial.begin(9600);
  inputId.reserve(20);
  inputPosition.reserve(20);
  // Reset all servos to mid point
  for (int i = 0; i < 5; i++) {
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
