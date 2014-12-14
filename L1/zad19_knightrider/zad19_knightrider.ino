#include <SPI.h>
#include <TimerOne.h>
const int DAC_SS_pin = 10;
#define SAMPLE_RATE 8000

const int quant = 1000/6;
const int leds[8] = {5,3,2,4,6,8,7,9};

typedef enum synth_state{
  STATE_IDLE,
  STATE_PLAYING
} state_t;

typedef struct s{
  synth_state state;
  float freq;
  float phase;
  unsigned long endtime;
} synth;

inline int process_synth(synth* s);
void start_note(synth* s, int m, float length);

synth synthesizers[2];

inline float midi2freq(int m){
  return 440.0 * pow(2, (m-69)/12.0);
}

void setDAC(byte b){
  byte a1 = (b&0xf0) >> 4;
  byte a2 = (b&0x0f) << 4;
  digitalWrite(DAC_SS_pin, LOW);
  SPI.transfer(a1);
  SPI.transfer(a2);
  digitalWrite(DAC_SS_pin, HIGH);
}

volatile float debug;
volatile float debug2;
void setup(){
  for(int i = 0; i < 8; i++) pinMode(leds[i], OUTPUT);
  pinMode(DAC_SS_pin, OUTPUT);
  Serial.begin(115200);
  SPI.begin(); 
  SPI.setDataMode(SPI_MODE0);
  SPI.setClockDivider(SPI_CLOCK_DIV2); 
  Timer1.initialize(1000000/SAMPLE_RATE);
  Timer1.attachInterrupt(timer_int);
}

void schedule_note(int m, float length){
  int n = 0;
  while(synthesizers[n].state == STATE_PLAYING) n++;
  Serial.print("schedulied ");
  Serial.print(m);
  Serial.print(" at ");
  Serial.println(n);
  start_note(synthesizers + n, m, length);
}

void start_note(synth* s, int m, float length){
  s->freq = midi2freq(m);
  s->phase = 0.0;
  s->endtime = micros() + 1000.0*length;
  s->state = STATE_PLAYING;
}
const int chords[2][4]{
  {63,64,65,41},
  {61,62,63,39} 
};
int current_chord = 0;
#define LEDON 200
#define LEDOFF 201
#define END 255
#define SWCHORD 242
const byte seq[]={
  2, 1, 4, 2,  LEDON, 0, LEDOFF, 1, 0,
               LEDON, 1, 0,
  3, 1,        LEDON, 2, LEDOFF, 0, 0,
  2, 1,        LEDON, 3, LEDOFF, 1, 0,
  2, 1,        LEDON, 4, LEDOFF, 2, 0,
               LEDON, 5, LEDOFF, 3, 0,
  2, 1,        LEDON, 6, LEDOFF, 4, 0,
  2, 1,        LEDON, 7, LEDOFF, 5, 0,
  3, 1, 4, 2,  LEDON, 7, LEDOFF, 6, 0,
  2, 1,        LEDON, 6, 0,
  2, 1,        LEDON, 5, LEDOFF, 7, 0,
  2, 1,        LEDON, 4, LEDOFF, 6, 0,
  1, 1,        LEDON, 3, LEDOFF, 5, 0,
  2, 1,        LEDON, 2, LEDOFF, 4, 0,
  2, 1,        LEDON, 1, LEDOFF, 3, 0,
  2, 1,        LEDON, 0, LEDOFF, 2, 0,
  2, 1, 4, 2,  LEDON, 0, LEDOFF, 1, 0,
               LEDON, 1, 0,
  3, 1,        LEDON, 2, LEDOFF, 0, 0,
  2, 1,        LEDON, 3, LEDOFF, 1, 0,
  2, 1, 4, 2,  LEDON, 4, LEDOFF, 2, 0,
               LEDON, 5, LEDOFF, 3, 0,
  3, 1,        LEDON, 6, LEDOFF, 4, 0,
  2, 1,        LEDON, 7, LEDOFF, 5, 0,
  3, 1, 4, 2,  LEDON, 7, LEDOFF, 6, 0,
  2, 1,        LEDON, 6, 0,
  2, 1,        LEDON, 5, LEDOFF, 7, 0,
  2, 1,        LEDON, 4, LEDOFF, 6, 0,
  1, 1,        LEDON, 3, LEDOFF, 5, 0,
  2, 1,        LEDON, 2, LEDOFF, 4, 0,
  2, 1,        LEDON, 1, LEDOFF, 3, 0,
  2, 1,        LEDON, 0, LEDOFF, 2, SWCHORD, 0,
  END,
};

void loop(){
  static int n = 0;
  if(seq[n] == END) n = 0;
next:
  int note = seq[n];
  if(note > 0 && note < 127){
    int l = seq[++n];
    schedule_note(chords[current_chord][note-1], quant*l - quant/4);
  }else if(note == LEDON){
    digitalWrite(leds[seq[++n]], HIGH);
  }else if(note == LEDOFF){
    digitalWrite(leds[seq[++n]], LOW);
  }else if(note == 0){
    // nothing!
    n--;
  }else if(note == SWCHORD){
    if(current_chord == 0) current_chord = 1;
    else current_chord = 0;
  }
  if(seq[++n] != 0) goto next;
  n++;
  Serial.println(debug2);
  delay(quant);
}

inline int getsample(float p){
  return (p<=0.5)?-127:127;
  //return sin(3.141529*2.0*p);
}

inline int process_synth(synth* s){
  if(s->state == STATE_IDLE) return 0.0;
  float q = s->phase + s->freq/SAMPLE_RATE;
  if(q >= 1.0) q -= 1.0;
  s->phase = q;
  //debug = q;
  int sample = getsample(q);
  if(micros() >= s->endtime) s->state = STATE_IDLE;
  return sample;
}

void timer_int(){
  int sum = 0;
  int n = 0;
  sum += process_synth(synthesizers + 0);
  //sum += process_synth(synthesizers + 1);
  sum *= 0.8;
  int out = sum+127;
  debug2 = out;
  out = max(0.0,min(255.0,out));
  setDAC(out);
}
