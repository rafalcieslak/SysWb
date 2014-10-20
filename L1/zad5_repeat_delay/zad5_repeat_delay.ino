int pin_in = 2;
int pin_out = 3;

int buffer[100];
int c = 0;

void setup() {
  pinMode(pin_in, INPUT);
  pinMode(pin_out, OUTPUT);
}

void loop() {
  buffer[(c-1)%100] = digitalRead(pin_in);
  digitalWrite(pin_out,buffer[c%100]);
  c++;
  if(c>100) c-=100;
  delay(10);
}
