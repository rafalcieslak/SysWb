#include <Wire.h>

// A4 -> p5
// A5 -> p6
// VCC -> p8
// GND -> pozostale

// :00000001ff

void eeprom_write(byte addr, byte data){
    Wire.beginTransmission(0x50);
    Wire.write(addr);
    Wire.write(data);
    Wire.endTransmission();
    delay(2);
}

byte eeprom_read(byte addr){
    Wire.beginTransmission(0x50);
    Wire.write(addr);
    Wire.endTransmission();
    Wire.requestFrom(0x50,1);
    delay(2);
    if(Wire.available()){
      return Wire.read();
    }else{
      Serial.println("NO DATA!");
      return 0xff;
    }
}

byte eeprom_readn(byte addr, byte* buf, byte n){
    Wire.beginTransmission(0x50);
    Wire.write(addr);
    Wire.endTransmission();
    Wire.requestFrom(0x50,n);
    delay(2);
    int count = 0;
    if(Wire.available()){
      while(Wire.available()) buf[count++] = Wire.read();
      return count;
    }else{
      Serial.println("NO DATA!");
      return 0xff;
    }
}


byte i8hex_pos(byte p){
  return 1+2*p; 
}

unsigned char h2c[] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
void i8hex_place(char* i, byte pos, byte val){
  pos = i8hex_pos(pos);
  i[pos]   = h2c[val>>4];
  i[pos+1] = h2c[val&0x0f];
}
byte c2h[255];
byte i8hex_get(const char* i, byte pos){
  pos = i8hex_pos(pos);
  byte mshb = c2h[i[pos]];
  byte lshb = c2h[i[pos+1]];
  return (mshb<<4)|lshb;
}

char i8hex_chksum(char* h){
  int n = i8hex_get(h,0);
  unsigned char sum = 0;
  for(int i = 0; i < n+4; i++) sum += i8hex_get(h,i);
  return (signed char)0xff-sum+0x01;
}

void printhex(byte b){
  char tmp[4];
  tmp[3] = 0;
  i8hex_place(tmp,0,b);
  Serial.println(tmp+1);
}


void read_i8hex(){
    Serial.println("I8HEX plz.");
    while(1){
      char buffer[200];
      byte n = 0;
      while(n==0){
        delay(50);
        n = Serial.readBytesUntil('\n',buffer,199);
      }
      buffer[n] = 0;
      if(n < 10){
        Serial.println("Error: message too short");
        continue;
      }
      byte len    = i8hex_get(buffer, 0);
      byte addrhb = i8hex_get(buffer, 1);
      byte addrlb = i8hex_get(buffer, 2);
      byte msgtype= i8hex_get(buffer, 3);
      
      byte correct = i8hex_chksum(buffer);
      byte used = i8hex_get(buffer,len+4);
      if(correct != used){
        Serial.println("Invalid checksum.");
        Serial.print("Is: "); printhex(used);
        Serial.print("Should be: "); printhex(correct);
        continue; 
      }
        
      if(msgtype == 1) return;
      else if(msgtype == 0){
        if(addrhb != 0){
           Serial.println("Address too high.");
           continue;
        }
        byte addr = addrlb;
        for(int i = 0; i < len; i++){
          byte data = i8hex_get(buffer,i+4);
          eeprom_write(addr+i,data);
        }
        Serial.println("Done.");
      }else{
        Serial.println("Unsupported message type");  
      }
      
    }
}
void print_i8hex(byte addr, unsigned int len){
    byte buf[len+1];
    int n = eeprom_readn(addr,buf,len);
    buf[n] = 0;
    char i8hex[2*n+12];
    i8hex[2*n+11] = 0;
    i8hex[0] = ':';
    i8hex_place(i8hex,0,n);
    i8hex_place(i8hex,1,0);
    i8hex_place(i8hex,2,addr);
    i8hex_place(i8hex,3,0);
    
    for(int i = 0; i < n; i++)
      i8hex_place(i8hex,i+4,buf[i]);
    
    i8hex_place(i8hex,n+4, i8hex_chksum(i8hex));
    
    Serial.println((char*)i8hex);
}

char* strtok_r_noempty(char* b, const char* ch, char** ptr){
  char* q = strtok_r(b,ch,ptr);
  while(q != NULL && strlen(q) <= 0) q = strtok_r(NULL,ch,ptr);
  return q;
}

void setup(){
    Wire.begin(); // initialise the connection
    Serial.begin(9600);
    c2h['0'] = 0; c2h['1'] = 1; c2h['2'] = 2; c2h['3'] = 3; c2h['4'] = 4; c2h['5'] = 5; c2h['6'] = 6;
    c2h['7'] = 7; c2h['8'] = 8; c2h['9'] = 9; c2h['A'] = 0xA; c2h['a'] = 0xa; c2h['B'] = 0xB;
    c2h['b'] = 0xb; c2h['C'] = 0xc; c2h['c'] = 0xc; c2h['D'] = 0xD; c2h['d'] = 0xd; c2h['E'] = 0xE;
    c2h['e'] = 0xe; c2h['F'] = 0xF; c2h['f'] = 0xf;
    
}

void loop(){
    Serial.println("---");
    
    char buffer[200];
    byte n = 0;
    while(n==0){
      delay(50);
      n = Serial.readBytesUntil('\n',buffer,199);
    }
    buffer[n] = 0;
    
    byte addr, val;
    char* tokenptr;
    char* t = strtok_r_noempty(buffer," \n", &tokenptr);
    if(t != NULL && 0 == strcmp(t,"write")){
      t = strtok_r_noempty(NULL," \n", &tokenptr);
      if(t == NULL){
        read_i8hex();
      }else{
        addr = atoi(t);
        t = strtok_r_noempty(NULL," \n", &tokenptr);
        if(t == NULL){
          Serial.println("Missing value");
        }else{
           val = atoi(t);
           Serial.print("Writing "); Serial.print(val); Serial.print(" to addr "); Serial.print(addr); Serial.println(".");
           eeprom_write(addr,val);
        }
      }
    }else if(t != NULL && 0 == strcmp(t,"read")){
      t = strtok_r_noempty(NULL," \n", &tokenptr);
      if(t == NULL){
        Serial.println("Missing addr"); 
      }else{
        addr = atoi(t);
        t = strtok_r_noempty(NULL," \n", &tokenptr);
        if(t == NULL){
          val = eeprom_read(addr);
          Serial.print("The value at "); Serial.print(addr); Serial.print(" is "); Serial.print(val); Serial.println(".");
        }else{
          unsigned int len = atoi(t);
          print_i8hex(addr,len);
        }
      }
    }else{
         Serial.println("Invalid command.");
    }
    delay(50);
}
