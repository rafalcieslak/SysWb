int pin1 = 3;
int pin2 = 5;
int pin3 = 6;

int lastn;

int colors[][3]{
  {255,255,255},
  {255,0,0},
  {0,255,0},
  {0,0,255},
  {255,255,0},
  {255,0,255},
  {0,255,255}
};

void setup(){
  randomSeed(analogRead(0));
  pinMode(pin1, OUTPUT);
  pinMode(pin2, OUTPUT);
  pinMode(pin3, OUTPUT);
}

void set(int a, int b, int c, int alpha){
  analogWrite(pin1,(255-a*alpha/255));
  analogWrite(pin2,(255-b*alpha/255));
  analogWrite(pin3,(255-c*alpha/255));
}

void loop(){
  int n, mult, x,y,z;
  while (lastn==( n = random(0, sizeof(colors)/sizeof(colors[0]))));
  lastn = n;
  x = colors[n][0];
  y = colors[n][1];
  z = colors[n][2];
  // fade in
  for(mult = 0; mult < 255; mult++){
    set(x,y,z,mult);
    delay(6);
  }
  // fade out
  for(mult = 255; mult > 0; mult--){
    set(x,y,z,mult);
    delay(6);
  }
  set(0,0,0,0);
  delay(1000);
}
