void moveServo(int id, int pos);

void setup();

SIGNAL(TIMER0_COMPA_vect);

void refresh_servos();

void refresh_outputs();

void refresh_analog_inputs();

void refresh_digital_inputs ();

void loop();
