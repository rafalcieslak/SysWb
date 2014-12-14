#include <SPI.h>
#include <TimerOne.h>
const int DAC_SS_pin = 8;
#define SAMPLE_RATE 8000

volatile float freq = 430.0;
volatile float vol = 255;

typedef enum state_t{
  STATE_IDLE,
  STATE_PLAY,
  STATE_PLAYING
} state_t;
volatile state_t state = STATE_IDLE;

void setDAC(byte b){
  byte a1 = (b&0xf0) >> 4;
  byte a2 = (b&0x0f) << 4;
  digitalWrite(DAC_SS_pin, LOW);
  SPI.transfer(a1);
  SPI.transfer(a2);
  digitalWrite(DAC_SS_pin, HIGH);
}

volatile float debug;
void setup(){
  pinMode(DAC_SS_pin, OUTPUT);
  Serial.begin(115200);
  SPI.begin(); 
  SPI.setDataMode(SPI_MODE1);
  SPI.setClockDivider(SPI_CLOCK_DIV2); 
  Timer1.initialize(1000000/SAMPLE_RATE);
  Timer1.attachInterrupt(timer_int);
  Serial.println("adsadsad");
}

void loop(){
  //Serial.println(vol);
  vol = pow(10000,analogRead(3)/1024.0)/3600.0;
  if(vol < 0.01) vol = 0.0;
  freq = 80.0*pow(16,analogRead(4)/1024.0);
  delay(40);
  
}

int getsample(float p){
  float s = 128.0*p;//sin(p*2.0*3.141529);
  if(s <= 128.0) s = 0.0; else s = 1.0;
  float q = (s*2.0)-1.0;
  return q*min(vol,1.0);
}
void timer_int(){
  static float phase = 0;
  phase += freq/SAMPLE_RATE;
  if(phase >= 1.0) phase -= 1.0;
  int sample = getsample(phase);
  debug = sample;
  setDAC(sample);
}
