int butt_pins[4] = {2,4,6,8};
int led_pins [4] = {3,5,7,9};

#define ROUNDS 10

typedef enum GameStates{
  GAME_STATE_WAITING,
  GAME_STATE_LEDON
}GameStates;

GameStates state;

void WaitForInput(){
  char c = 0;
  while(c != '\n') if(Serial.available()) c = Serial.read();
}
void WaitForNoPress(){
  while(getButton() != -1);
}

int LED_id;
unsigned long LED_time;
unsigned int points, rounds;

void PrepareNextLED(){
  Serial.print("Round ");
  Serial.print(rounds);
  Serial.print("/");
  Serial.println(ROUNDS);
  LED_id = random(4);
  LED_time = millis() + 1500 + random(8000);
  state = GAME_STATE_WAITING;
}
void NewGame(){
  setNthLED(-1);
  points = 0;
  rounds = 0;
  Serial.println("Type anything to start a new game.");
  WaitForInput();
  NextRound();
}

void setup(){
  Serial.begin(9600);
  randomSeed(analogRead(0));
  pinMode(led_pins[0],OUTPUT);
  pinMode(led_pins[1],OUTPUT);
  pinMode(led_pins[2],OUTPUT);
  pinMode(led_pins[3],OUTPUT);
  pinMode(butt_pins[0],INPUT);
  pinMode(butt_pins[1],INPUT);
  pinMode(butt_pins[2],INPUT);
  pinMode(butt_pins[3],INPUT);
  NewGame();
}

byte getButtonStates(){
  byte res  = 1 * (digitalRead(butt_pins[0]) == HIGH);
       res |= 2 * (digitalRead(butt_pins[1]) == HIGH);
       res |= 4 * (digitalRead(butt_pins[2]) == HIGH);
       res |= 8 * (digitalRead(butt_pins[3]) == HIGH);
  return res;
}

void setLEDs(byte states){
  digitalWrite(led_pins[0],~states&0x01);
  digitalWrite(led_pins[1],~states&0x02);
  digitalWrite(led_pins[2],~states&0x04);
  digitalWrite(led_pins[3],~states&0x08);
}

void setNthLED(char n){
  if(n == 255) setLEDs(0);
  setLEDs(1<<n);
}

// -1 if there are no buttons pressed
// -2 of there are more than 1 buttons pressed
int getButton(){
  byte v = getButtonStates();
  if(v == 1) return 0;
  if(v == 2) return 1;
  if(v == 4) return 2;
  if(v == 8) return 3;
  if(v == 0) return -1;
  return -2;
}

int TimeToPts(unsigned long t){
  if(t<150) return 38;
  if(t<200) return 25;
  if(t<250) return 12;
  if(t<300) return 8;
  if(t<400) return 5;
  if(t<500) return 3;
  if(t<750) return 2;
  if(t<1000)return 1;
  return 0;
}

void EndGame(){
  Serial.print("Your total score: ");
  Serial.println(points);
  NewGame();
}
void NextRound(){
  rounds++;
  if(rounds > ROUNDS){
    EndGame();
  }else{  
    PrepareNextLED();
  }  
}
void LooseRound(int v){
  Serial.println("Wrong!");
  if(v == -2){
    Serial.println("You pressed multiple buttons.");  
  }else{
    Serial.print("You pressed button ");
    Serial.println(v);
  }
  WaitForNoPress();
  NextRound();
  delay(200);
}
void WinRound(){
  setNthLED(255);
  unsigned long t = millis() - LED_time;
  int pts = TimeToPts(t);
  if     (t < 150) Serial.println("IMPOSSIBRU!!!");
  else if(t < 250) Serial.println("Awesome!");
  else if(t < 350) Serial.println("Great!");
  else if(t < 500) Serial.println("Nice.");
  else if(t < 1000)Serial.println("Good.");
  else             Serial.println("Correct button, but no points.");
  WaitForNoPress();
  Serial.print("Points won this round: ");
  Serial.println(pts);
  points += pts;
  Serial.print("Total points: ");
  Serial.println(points);
  NextRound();
  delay(200);
}

void loop(){
  int v = getButton();
  if(state == GAME_STATE_WAITING){
    setNthLED(255);
    if(v != -1) LooseRound(v);
    if(millis() > LED_time) state = GAME_STATE_LEDON;
  }
  else if(state == GAME_STATE_LEDON){
    setNthLED(LED_id);
    if(v == LED_id) WinRound();
    else if(v != -1) LooseRound(v);
  }
  
}
