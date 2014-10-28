int PIN_IN = 2;
int PIN_OUT = 3;

int buffer[256];
byte c = 0;

void setup() {
  pinMode(PIN_IN, INPUT);
  pinMode(PIN_OUT, OUTPUT);
}

void loop() {
  buffer[c-1] = digitalRead(PIN_IN);
  digitalWrite(PIN_OUT,buffer[c++]);
  delay(1000/256);
}
