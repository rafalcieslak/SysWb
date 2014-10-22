int brzeczyk_pin = 5;
int czujnik_pin = 1;
int przycisk_pin = 4;
int dioda_pin = 3 ;
int rnd;
bool wygrano=false;

void setup(){
  pinMode(brzeczyk_pin,OUTPUT);
  pinMode(przycisk_pin,INPUT);
  pinMode(dioda_pin,OUTPUT);
  randomSeed(analogRead(0));
  rnd = random(500,700);
  
}

void loop(){
  if(digitalRead(przycisk_pin) == HIGH) 
  {
    wygrano=false;
    rnd = random(500,700);
  }
  int val = analogRead(czujnik_pin);
  val = constrain(val,500,700);
  int dist = abs(val - rnd);
  if(dist<20) wygrano=true;
  if(!wygrano)
  {
    digitalWrite(brzeczyk_pin,HIGH);
    digitalWrite(dioda_pin,LOW);
    delay( map(dist, 0, 200, 100, 30 ));
    digitalWrite(brzeczyk_pin,LOW);
    delay( map(dist, 0, 200, 500, 50 ));
  }
  else
  {
    digitalWrite(dioda_pin,HIGH);
    analogWrite(brzeczyk_pin,128);
  }
  
}
