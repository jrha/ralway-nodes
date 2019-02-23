#include <Servo.h>
#include <CMRI.h>

CMRI cmri; // defaults to a SMINI with address 0. SMINI = 24 inputs, 48 outputs

Servo servo_point_1a;
Servo servo_point_1b;
Servo servo_point_2a;
Servo servo_point_2b;

bool point_1a = false;
bool point_1b = false;
bool point_2a = false;
bool point_2b = false;

void setup() {
  servo_point_1a.attach(6);
  servo_point_1b.attach(9);
  servo_point_2a.attach(10);
  servo_point_2b.attach(11);

  Serial.begin(9600, SERIAL_8N2); // make sure this matches your speed set in JMRI
  for (int i=2; i<=5; i++) { pinMode(i, OUTPUT); }
  for (int i=7; i<=8; i++) { pinMode(i, INPUT); digitalWrite(i, HIGH); }
}

void loop() {
  // 1: build up a packet
  cmri.process();

  // 2: update outputs
  digitalWrite(2, cmri.get_bit(0));
  digitalWrite(3, cmri.get_bit(1));
  digitalWrite(4, cmri.get_bit(2));
  digitalWrite(5, cmri.get_bit(3));

  // 2.1: update servo outputs
  point_1a = cmri.get_bit(4);
  point_1b = cmri.get_bit(5);
  point_2a = cmri.get_bit(6);
  point_2b = cmri.get_bit(7);

  servo_point_1a.write(30 * point_1a);
  servo_point_1b.write(30 * point_1b);
  servo_point_2a.write(30 * point_2a);
  servo_point_2b.write(30 * point_2b);

  // 3: update inputs (invert digitalRead due to the pullups)
  cmri.set_bit(0, !digitalRead(7));
  cmri.set_bit(1, !digitalRead(8));
}
