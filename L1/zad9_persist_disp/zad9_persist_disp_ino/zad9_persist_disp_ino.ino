int pin1=2;
int pin2=3;
int dioda[5]={5,6,7,8,9};

volatile int ktora=0;
volatile unsigned long time;
volatile unsigned long roznica;
volatile byte sync = 0;

const int off = 0;
bool tab1[5][5] = {
  {1, 0, 0, 0, 1},
  {0, 1, 0, 1, 0},
  {0, 0, 1, 0, 0},
  {0, 1, 0, 1, 0},
  {1, 0, 0, 0, 1}
};

bool tab[5][6] = {
  {1, 1, 1, 1, 1, 1},
  {1, 1, 0, 0, 1, 1},
  {1, 1, 0, 0, 1, 1},
  {1, 1, 0, 0, 1, 1},
  {1, 1, 1, 1, 1, 1}
};

bool tab2[5][9] = {
  {0, 1, 1, 1, 0, 1, 0, 0, 1},
  {0, 1, 0, 1, 0, 1, 1, 0, 1},
  {0, 1, 0, 1, 0, 1, 0, 1, 1},
  {0, 1, 1, 1, 0, 1, 0, 0, 1},
  {0, 1, 0, 1, 0, 1, 0, 0, 1}
};

const int T_SIZE = sizeof(tab[0])/sizeof(tab[0][0]);

void funa()
{
    //Serial.println("A");
  if(ktora==1)
  {
    //Serial.println(millis()-time);
    roznica=millis()-time;
    time=millis();
    //digitalWrite(dioda[0],LOW);
    //Serial.println("port a");
    ktora=0;
    sync = 1;
    //digitalWrite(dioda[1],HIGH);
  }
}

void funb()
{
    //Serial.println("B");
  if(ktora==0)
  {
    //Serial.println(millis()-time);
    roznica=millis()-time;
    time=millis();
    //digitalWrite(dioda[1],LOW);
    //Serial.println("port b");
    ktora=1;
    //digitalWrite(dioda[0],HIGH);
  }
}

void setup()
{
  pinMode(pin1, INPUT);
  pinMode(pin2, INPUT);
  pinMode(dioda[0], OUTPUT);
  pinMode(dioda[1], OUTPUT);
  pinMode(dioda[2], OUTPUT);
  pinMode(dioda[3], OUTPUT);
  pinMode(dioda[4], OUTPUT);
  attachInterrupt(0, funa, RISING);
  attachInterrupt(1, funb, RISING);
  Serial.begin(9600);
}

int set_leds(int n){
  for(int p = 0; p < 5; p++) digitalWrite(dioda[p], tab[p][(n+off)%T_SIZE]?HIGH:LOW);
  if(sync){
    sync = 0;
    return  1;
  }
  return 0;
}

void loop(){
  //Serial.println(ktora);
  //delay(300);
  //return;
   while(1){
     Serial.println("SYNC");
     Serial.println(T_SIZE);
     for(int t = 0; t < T_SIZE; t++){
      if(set_leds(t)) return;
      delay(roznica/T_SIZE);
     }
     for(int t = T_SIZE-1; t >=0; t--){
      if(set_leds(t)) return;
      delay(roznica/T_SIZE);
     }
     
   }
  
  
 // digitalWrite(dioda1,digitalRead(pin1));
  //digitalWrite(dioda2,digitalRead(pin2));
}
