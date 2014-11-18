int pin1=2;
int pin2=3;
int dioda1=5;
int dioda2=9;

bool tab[5][6] = {
  {1, 1, 1, 1, 1, 1},
  {1, 1, 0, 0, 1, 1},
  {1, 1, 0, 0, 1, 1},
  {1, 1, 0, 0, 1, 1},
  {1, 1, 1, 1, 1, 1}
};

int ktora=0;
unsigned long time;

void funa()
{
  if(ktora==1)
  {
    Serial.println(millis()-time);
    time=millis();
    digitalWrite(dioda2,LOW);
    Serial.println("port a");
    ktora=0;
    digitalWrite(dioda1,HIGH);
  }
}

void funb()
{
  if(ktora==0)
  {
    Serial.println(millis()-time);
    time=millis();
    digitalWrite(dioda1,LOW);
    Serial.println("port b");
    ktora=1;
    digitalWrite(dioda2,HIGH);
  }
}

void setup()
{
  pinMode(pin1, INPUT);
  pinMode(pin2, INPUT);
  pinMode(dioda1, OUTPUT);
  pinMode(dioda2, OUTPUT);
  attachInterrupt(0, funa, RISING);
  attachInterrupt(1, funb, RISING);
  Serial.begin(9600);
}



void loop()
{
  
  
 // digitalWrite(dioda1,digitalRead(pin1));
  //digitalWrite(dioda2,digitalRead(pin2));
}
