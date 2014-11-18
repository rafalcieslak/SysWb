int przycisk = 6;
int buczek = 5;
char abecadlo[5][16];
unsigned long time;
void alfabet()
{
  abecadlo[1][0]='e';
  abecadlo[1][1]='t';
  
  abecadlo[2][0]='i';
  abecadlo[2][1]='a';
  abecadlo[2][2]='n';
  abecadlo[2][3]='m';
  
  abecadlo[3][0]='s';
  abecadlo[3][1]='u';
  abecadlo[3][2]='r';
  abecadlo[3][3]='w';
  abecadlo[3][4]='d';
  abecadlo[3][5]='k';
  abecadlo[3][6]='g';
  abecadlo[3][7]='o';
  
  abecadlo[4][0]='h';
  abecadlo[4][1]='v';
  abecadlo[4][2]='f';
  abecadlo[4][4]='l';
  abecadlo[4][6]='p';
  abecadlo[4][7]='j';
  abecadlo[4][8]='b';
  abecadlo[4][9]='x';
  abecadlo[4][10]='c';
  abecadlo[4][11]='y';
  abecadlo[4][12]='z';
  abecadlo[4][13]='q';
  
  
}
void setup()
{
  pinMode(przycisk, INPUT);
  pinMode(buczek, OUTPUT);
  Serial.begin(9600);
  alfabet();
}
int dlugosc=0;;
int znak=0;
void loop()
{
  if(dlugosc>0 && millis()-time>1000)
  {
    if(dlugosc>4 || znak>13) Serial.println("niepoprawny kod");
    else
    {
    Serial.print(abecadlo[dlugosc][znak]);
    dlugosc=0;
    znak=0;
    }
  }
  if(digitalRead(przycisk)==HIGH)
  {
    digitalWrite(buczek,HIGH);
    if(millis()-time>5000) Serial.print(" ");
    znak*=2;
    time=millis();
    delay(100);
 //   Serial.println("przed");
    while(digitalRead(przycisk)==HIGH){}
   // Serial.println("po");
    if(millis()-time>300) znak++;
    ///if(digitalRead(przycisk)==HIGH) znak++;
    //else digitalWrite(przycisk,LOW);
    dlugosc++;
    delay(10);
    digitalWrite(buczek,LOW);
  }
}
