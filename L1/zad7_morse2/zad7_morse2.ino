int dioda = 5;
int buczek = 6;

char  alfabet[][6]={
    "kd",
    "dkkk",
    "dkdk",
    "dkk",
    "k",
    "kkdk",
    "ddk",
    "kkkk",
    "kk",
    "kddd",
    "dkd",
    "kdkk",
    "dd",
    "dk",
    "ddd",
    "kddk",
    "ddkd",
    "kdk",
    "kkk",
    "d",
    "kkd",
    "kkkd",
    "kdd",
    "dkkd",
    "dkdd",
    "ddkk"};
    
void setup()
{
  pinMode(dioda, OUTPUT);
  pinMode(buczek, OUTPUT);
  Serial.begin(9600);
}

void loop()
{
  char x;
  if (Serial.available() > 0) {
  x=Serial.read();
  Serial.print("dostalam: ");
  int y=x-97;
  
  int i=0;
  while(alfabet[y][i]=='d' || alfabet[y][i]=='k')
  {
    digitalWrite(buczek,HIGH);
    digitalWrite(dioda,HIGH);
    delay(100);
    if(alfabet[y][i]=='d') delay(300);
    digitalWrite(buczek,LOW);
    digitalWrite(dioda,LOW);
    delay(150);
    i++;
  }
  delay(50);
  Serial.println(x);}
}
