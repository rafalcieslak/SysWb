#include <SPI.h>
#include <TimerOne.h>
const int DAC_SS_pin = 10;
#define SAMPLE_RATE 20000

const int quant = 1000/6;
const int leds[8] = {
  5,3,2,4,6,8,7,9};

volatile float debug;
volatile float debug2;
const unsigned char env[127] = {
  256,252,249,246,244,241,238,235,232,229,226,223,220,217,214,212,209,206,203,200,197,195,192,189,186,184,181,178,176,173,170,168,165,163,160,157,155,152,150,147,145,142,140,137,135,132,130,127,125,123,120,118,116,113,111,109,107,104,102,100,98,95,93,91,89,87,85,83,81,79,76,74,72,70,69,67,65,63,61,59,57,55,53,52,50,48,46,45,43,41,40,38,37,35,33,32,30,29,27,26,25,23,22,21,19,18,17,15,14,13,12,11,10,9,8,7,6,5,4,4,3,2,1,1,0,0,0,};
inline unsigned char getenv(unsigned int t){
  return env[t >> 8];
}
inline unsigned int time2inc(float ms){
  return 1.2*0x8000*1000.0/((float)ms*SAMPLE_RATE);
}
volatile unsigned int incs[3];
volatile unsigned int phases[3];
volatile unsigned int envphase[3] = {
  0x8000, 0x8000, 0x8000};
volatile unsigned int envinc[3] = {
  0x0020, 0x0020, 0x0020};

inline signed char process_synth(int s);
void start_note(int s, int m, float length);

inline float midi2freq(int m){
  return 440.0 * pow(2, (m-69)/12.0);
}

void setDAC(byte b){
  digitalWrite(DAC_SS_pin, LOW);
  SPI.transfer((b&0xf0) >> 4);
  SPI.transfer((b&0x0f) << 4);
  digitalWrite(DAC_SS_pin, HIGH);
}

void setup(){
  for(int i = 0; i < 8; i++) pinMode(leds[i], OUTPUT);
  pinMode(DAC_SS_pin, OUTPUT);
  Serial.begin(115200);
  SPI.begin(); 
  SPI.setDataMode(SPI_MODE2);
  SPI.setClockDivider(SPI_CLOCK_DIV2); 
  TCCR1A = 0x00;
  TCCR1B = 0x09;
  TCCR1C = 0x00;
  OCR1A=16000000.0 / SAMPLE_RATE; 
  TIMSK1|=  (1<<OCIE1A); 
  sei();
}

void schedule_note(int m, float length){
  int n = 0;
  Serial.print("scheduled ");
  Serial.print(m);
  Serial.print(" of l =");
  Serial.print(length);
  cli();
  while(!(envphase[n]&0x8000)) n++;
  Serial.print(" at ");
  Serial.println(n);
  start_note(n, m, length);
  sei();
}

void start_note(int s, int m, float length){
  incs[s] = midi2freq(m)*65536/SAMPLE_RATE;
  phases[s] = 0;
  envphase[s] = 0;
  envinc[s] = time2inc(length);
}
const int chords[2][11]{
  {63,64,65,45,69,70,76, 81, 82, 79},
  {61,62,63,43,69,70,76, 81, 82, 79} 
};
int current_chord = 0;
#define LEDON 200
#define LEDOFF 201
#define END 255
#define SYNC 242
#define OPTDROP 241
const byte seq[]={
  2, 1, 4, 1, 0,
        4, 2,  0,
  3, 1,        0,
  2, 1,        0,
  2, 1,        0,
               0,               
  2, 1,        0,
  2, 1,        0,
  3, 1, 4, 1,  0,
  2, 1, 4, 2,  0,
  2, 1,        0,
  2, 1,        0,
  1, 1,        0,
  2, 1,        0,
  2, 1,        0,
  2, 1,        0,
  2, 1, 4, 1,  0,
        4, 1, 0,
  3, 1,        0,
  2, 1,        0,
  2, 1, 4, 1,  0,
        4, 1,  0,
  3, 1,        0,
  2, 1,        0,
  3, 1, 4, 1,  0,
  2, 1, 4, 2,  0,
  2, 1,        0,
  2, 1,        0,
  1, 1,        0,
  2, 1,        0,
  2, 1,        0,
  2, 1,        SYNC, END,
};
const byte ledseq[]{
  LEDON, 0, LEDOFF, 1, 0,
  LEDON, 1, 0,
  LEDON, 2, LEDOFF, 0, 0,
  LEDON, 3, LEDOFF, 1, 0,
  LEDON, 4, LEDOFF, 2, 0,
  LEDON, 5, LEDOFF, 3, 0,
  LEDON, 6, LEDOFF, 4, 0,
  LEDON, 7, LEDOFF, 5, 0,
  LEDON, 7, LEDOFF, 6, 0,
  LEDON, 6, 0,
  LEDON, 5, LEDOFF, 7, 0,
  LEDON, 4, LEDOFF, 6, 0,
  LEDON, 3, LEDOFF, 5, 0,
  LEDON, 2, LEDOFF, 4, 0,
  LEDON, 1, LEDOFF, 3, 0,
  LEDON, 0, LEDOFF, 2, END, 0,
};
const byte seq2[]={
  5, 1, 0,
  0,
  6, 1, 0,
  5, 1, 0,
  7, 4, 0, 0, 0, 0,
  8, 1, 0,
  0,
  9, 1, 0,
  8, 1, 0,
  7, 4, 0, 0, 0, 0,
  5, 1, 0,
  0,
  6, 1, 0,
  5, 1, 0,
  7, 1, 0, 0,
  8, 1, 0, 0,
  10, 8, 0,
  END,
};

int sync = 0, optdrop = 0;
int process_notes(const byte* table, int n){
next:
  if(table[n] == END){
    Serial.println("END");
    return 0;
  }
  int note = table[n];
  if(note > 0 && note < 127){
    int l = table[++n];
    schedule_note(chords[current_chord][note-1], quant*l);
  }
  else if(note == LEDON){
    digitalWrite(leds[table[++n]], HIGH);
  }
  else if(note == LEDOFF){
    digitalWrite(leds[table[++n]], LOW);
  }
  else if(note == 0){
    // nothing!
    n--;
  }
  else if(note == SYNC){
    sync = 1;
    if(current_chord == 0) current_chord = 1;
    else current_chord = 0;
  }
  else if(note == OPTDROP){
    optdrop = 1;
  }
  if(table[++n] != 0) goto next;
  n++;
  return n;
}

void loop(){
  static int n = 0, n2 = -1, c = 0, nl = 0;
  n = process_notes(seq, n);
  if(n2 != -1) n2 = process_notes(seq2,n2);
  if(n2 == 0) n2 = -1;
  nl = process_notes(ledseq,nl);
  if(sync){
    Serial.println("SYNC");
    sync = 0; 
    c++;
    if(c%4 == 2){
      n2 = 0;
    }
  }

  Serial.println(debug2);
  delay(quant);
}

inline signed char getsample(word p){
  //return (p<=32250)?-100:100;
  return p/256-127;
  //return abs(p/512-255)-127;
  //return sin(3.141529*2.0*p);
}

SIGNAL(TIMER1_COMPA_vect){
  //void timer_int(){
  signed int sum = 0;
  unsigned char mult = 0;
  if(!(envphase[0]&0x8000)) mult = getenv(envphase[0] += envinc[0]);
  sum += ((signed int)getsample(phases[0] += incs[0]) * mult / 256) ;
  mult = 0;
  if(!(envphase[1]&0x8000)) mult = getenv(envphase[1] += envinc[1]);
  sum += ((signed int)getsample(phases[1] += incs[1]) * mult / 256) ;
  mult = 0;
  if(!(envphase[2]&0x8000)) mult = getenv(envphase[2] += envinc[2]);
  sum += ((signed int)getsample(phases[2] += incs[2]) * mult / 256) ;
  sum >>= 2;
  int out = sum+127;
  //debug2 = out;
  //out = max(0,min(255,out));
  static unsigned char n = 0;
  setDAC(out);
}

