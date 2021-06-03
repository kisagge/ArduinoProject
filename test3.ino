#include <SoftwareSerial.h>
#include <LedControl.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
// Din 13, CIK 12, CS 11, 매트릭스 8개 사용 선언
//di cs cl
LiquidCrystal_I2C lcd(0x27, 16, 2);
LedControl lc = LedControl(0,2,1,8);
byte m1 = B00011000;
byte m2 = B00111100;
byte m3 = B01111110;
byte m4 = B11111111;

int btn0 = 14;
int btn1 = 5;

int p13 = 13;
int p12 = 12;
int p11 = 11;
int p10 = 10;
int p9 = 9;
int p8 = 8;
int p7 = 7;
int p6 = 6;
long v13, v12, v11, v10, v9, v8, v7, v6;

SoftwareSerial mySerial(6, 3);
byte resetMIDI = 4;

void setup() {
  lcd.init();
  lcd.backlight();
  mySerial.begin(31250);

  pinMode(resetMIDI, OUTPUT);
  digitalWrite(resetMIDI, LOW);
  delay(100);
  digitalWrite(resetMIDI, HIGH);
  delay(100);
  talkMIDI(0xB0, 0x07, 120); //0xB0 is channel message, set channel volume to near max (127)

  
  pinMode(btn0, INPUT_PULLUP);
  pinMode(btn1, INPUT_PULLUP);
  
  pinMode(p13, INPUT_PULLUP);
  pinMode(p12, INPUT_PULLUP);
  pinMode(p11, INPUT_PULLUP);
  pinMode(p10, INPUT_PULLUP);
  pinMode(p9, INPUT_PULLUP);
  pinMode(p8, INPUT_PULLUP);
  pinMode(p7, INPUT_PULLUP);
  pinMode(p6, INPUT_PULLUP);

  for(int num=0; num<8; num++){
    lc.shutdown(num, false);
    lc.setIntensity(num, 8);
    lc.clearDisplay(num);  
  }
}


int mode = 0; // mode 0: menu, 1: instrument, 2: game
int menu = 0; // 0: instument, 1: game
int inst_box[8] = {1, 117, 25, 41, 43, 72, 74, 80};
//             = 피아노, 드럼, 기타, 바이올린, 첼로, 클라리넷, 플룻, 오카리나
int inst = 0;
int music = 0;
int score;
bool gameOn = false;



int prev = -1;
void loop() {
  int check1 = analogRead(btn0); // next
  int check2 = readLong(); // go or back

 if(check1 < 100 && mode == 0){
    menu++;
    if(menu == 2) menu = 0;
  }
  if(check1 < 100 && mode == 1){
    inst++;
    if(inst == 8) inst = 0;
    talkMIDI(0xC0, inst_box[inst], 0);
  }
  if(check1 < 100 && mode == 2){
    music++;
    if(music == 3) music = 0;
  }
  if(check2 == 1 && mode == 2){
    gameOn = true;
  }
  if(check2 == 1 && mode == 0 && menu == 0){
    mode = 1;
    talkMIDI(0xC0, inst_box[inst], 0);
  }
  if(check2 == 1 && mode == 0 && menu == 1){
    mode = 2;
  }
  if(check2 == 2){
    re_set();
  }

  if(mode == 0){
      if(menu == 0){
        lcd.setCursor(0,0);
        lcd.print("Select Mode     ");
        lcd.setCursor(0,1);
        lcd.print(": Instrument    ");
      }
      else if(menu == 1){
        lcd.setCursor(0,0);
        lcd.print("Select Mode     ");
        lcd.setCursor(0,1);
        lcd.print(": Game          ");
      }
    }

    else if(mode == 1){
      if(inst == 0){
        lcd.setCursor(0,0);
        lcd.print("Instrument      ");
        lcd.setCursor(0,1);
        lcd.print(": Piano         ");
      }
      else if(inst == 1){
        lcd.setCursor(0,0);
        lcd.print("Instrument      ");
        lcd.setCursor(0,1);
        lcd.print(": Drum          ");
      }
      else if(inst == 2){
        lcd.setCursor(0,0);
        lcd.print("Instrument      ");
        lcd.setCursor(0,1);
        lcd.print(": Guitar        ");
      }
      else if(inst == 3){
        lcd.setCursor(0,0);
        lcd.print("Instrument      ");
        lcd.setCursor(0,1);
        lcd.print(": Violin        ");
      }
      else if(inst == 4){
        lcd.setCursor(0,0);
        lcd.print("Instrument      ");
        lcd.setCursor(0,1);
        lcd.print(": Cello         ");
      }
      else if(inst == 5){
        lcd.setCursor(0,0);
        lcd.print("Instrument      ");
        lcd.setCursor(0,1);
        lcd.print(": Clarinet      ");
      }
      else if(inst == 6){
        lcd.setCursor(0,0);
        lcd.print("Instrument      ");
        lcd.setCursor(0,1);
        lcd.print(": Flute         ");
      }
      else if(inst == 7){
        lcd.setCursor(0,0);
        lcd.print("Instrument      ");
        lcd.setCursor(0,1);
        lcd.print(": Ocarina       ");
      }
    }

    else if(mode == 2){
      if(gameOn == false){
        if(music == 0){
          lcd.setCursor(0,0);
          lcd.print("Select Music    ");
          lcd.setCursor(0,1);
          lcd.print(": 3 Bears       ");
        }
        else if(music == 1){
          lcd.setCursor(0,0);
          lcd.print("Select Music    ");
          lcd.setCursor(0,1);
          lcd.print(": Pororo        ");
        }
        else if(music == 2){
          lcd.setCursor(0,0);
          lcd.print("Select Music    ");
          lcd.setCursor(0,1);
          lcd.print(": Baby Shark    ");
        }
      }
      else{
        int prev = -1;
        int i = 13;
        unsigned long music_length;
        int music_speed;
        if(music == 0){
          music_length = 79000;
          music_speed = 500;
        }
        else if(music == 1){
          music_length = 74000;
          music_speed = 700;
        }
        else if(music == 2){
          music_length = 106000;
          music_speed = 500;
        }
        
        music_start();
        unsigned long t = millis();
        while(millis() - t < music_length){
          if(readLong() == 2){
            break;
          }
          lcd.setCursor(0,0);
          lcd.print("Playing Game    ");
          lcd.setCursor(0,1);
          lcd.print("Score:          ");
          if(score >= 1000) i = 12;
          if(score >= 10000) i = 11;
          lcd.setCursor(i,1);
          lcd.print(score);
          int where = random(0,8);
          if(where != prev) set(where, music_speed);
          prev = where;
        }
        show_score(score);
        score=0;
        re_set();
        gameOn=false;
      }
      
    }

    v13 = readP13();
    v12 = readP12();
    v11 = readP11();
    v10 = readP10();
    v9 = readP9();
    v8 = readP8();
    v7 = readP7();
    v6 = readP6();
    if(mode == 1){
      if(v13 == 1){
        noteOn(0, 60, 100);
        noteOff(0, 60, 100);
      }
      if(v12 == 1){
        noteOn(0, 62, 100);
        noteOff(0, 62, 100);
      }
      if(v11 == 1){
        noteOn(0, 64, 100);
        noteOff(0, 64, 100);
      }
      if(v10 == 1){
        noteOn(0, 65, 100);
        noteOff(0, 65, 100);
      }
      if(v9 == 1){
        noteOn(0, 67, 100);
        noteOff(0, 67, 100);
      } 
      if(v8 == 1){
        noteOn(0, 69, 100);
        noteOff(0, 69, 100);
      }
      if(v7 == 1){
        noteOn(0, 71, 100);
        noteOff(0, 71, 100);
      }
      if(v6 == 1){
        noteOn(0, 72, 100);
        noteOff(0, 72, 100);
      }
    }
  
  if(check1  < 100 || check2 != 0) delay(300);
}

void music_start(){
  byte b3[] = {B00111111,B00000001,B00000001,B00111111,B00111111,B00000001,B00000001,B00111111};
  byte b2[] = {B00111111,B00000001,B00000001,B00111111,B00111111,B00100000,B00100000,B00111111};
  byte b1[] = {B00011000,B00011000,B00011000,B00011000,B00011000,B00011000,B00011000,B00011000};
  for(int i=0;i<8;i++) lc.setRow(3,i,b3[i]); delay(1000);
  for(int i=0;i<8;i++) lc.setRow(3,i,b2[i]); delay(1000);
  for(int i=0;i<8;i++) lc.setRow(3,i,b1[i]); delay(2000);
  lc.clearDisplay(3);
}

void show_score(int score){
  byte b0[]={B00111111,B00100001,B00100001,B00100001,B00100001,B00100001,B00100001,B00111111};
  byte b1[]={B00011000,B00011000,B00011000,B00011000,B00011000,B00011000,B00011000,B00011000};
  byte b2[]={B00111111,B00000001,B00000001,B00111111,B00111111,B00100000,B00100000,B00111111};
  byte b3[]={B00111111,B00000001,B00000001,B00111111,B00111111,B00000001,B00000001,B00111111};
  byte b4[]={B00000100,B00001100,B00010100,B00100100,B00111111,B00000100,B00000100,B00000100};
  byte b5[]={B00111111,B00100000,B00100000,B00111111,B00111111,B00000001,B00000001,B00111111};
  byte b6[]={B00111111,B00100000,B00100000,B00111111,B00111111,B00100001,B00100001,B00111111};
  byte b7[]={B00111111,B00100001,B00100001,B00100001,B00000001,B00000001,B00000001,B00000001};
  byte b8[]={B00111111,B00100001,B00100001,B00111111,B00111111,B00100001,B00100001,B00111111};
  byte b9[]={B00111111,B00100001,B00100001,B00111111,B00111111,B00000001,B00000001,B00111111};
  int a[8];
  int k = 0;
  if(score == 0) for(int j=0;j<8;j++) lc.setRow(0,j,b0[j]);
  while(score!=0){
    a[k++] = score%10;
    score /= 10;
  }
  for(int i = k-1;i>=0;i--){
    if(a[i] == 0) for(int j=0;j<8;j++) lc.setRow(i,j,b0[j]);
    else if(a[i] == 1) for(int j=0;j<8;j++) lc.setRow(i,j,b1[j]);
    else if(a[i] == 2) for(int j=0;j<8;j++) lc.setRow(i,j,b2[j]);
    else if(a[i] == 3) for(int j=0;j<8;j++) lc.setRow(i,j,b3[j]);
    else if(a[i] == 4) for(int j=0;j<8;j++) lc.setRow(i,j,b4[j]);
    else if(a[i] == 5) for(int j=0;j<8;j++) lc.setRow(i,j,b5[j]);
    else if(a[i] == 6) for(int j=0;j<8;j++) lc.setRow(i,j,b6[j]);
    else if(a[i] == 7) for(int j=0;j<8;j++) lc.setRow(i,j,b7[j]);
    else if(a[i] == 8) for(int j=0;j<8;j++) lc.setRow(i,j,b8[j]);
    else if(a[i] == 9) for(int j=0;j<8;j++) lc.setRow(i,j,b9[j]);
  }
  delay(3000);
  for(int i=0;i<8;i++) lc.clearDisplay(i);
}

void set(int num, int Speed){
  unsigned long t = millis();
  bool correct = false;
  for(int k = 3; k <= 4; k++) lc.setRow(num, k, m1);
  while(millis() - t < Speed*1/5){
    int v = 0;
    if(num == 0) v = readP13();
    else if(num == 1) v = readP12();
    else if(num == 2) v = readP11();
    else if(num == 3) v = readP10();
    else if(num == 4) v = readP9();
    else if(num == 5) v = readP8();
    else if(num == 6) v = readP7();
    else if(num == 7) v = readP6();
    
    if(v == 1 && correct == false){
      correct = true;
      score += 100;
    }
  }
  for(int k = 2; k <= 5; k++) lc.setRow(num, k, m2);
  while(millis() - t < Speed*2/5){
    int v = 0;
    if(num == 0) v = readP13();
    else if(num == 1) v = readP12();
    else if(num == 2) v = readP11();
    else if(num == 3) v = readP10();
    else if(num == 4) v = readP9();
    else if(num == 5) v = readP8();
    else if(num == 6) v = readP7();
    else if(num == 7) v = readP6();
    
    if(v == 1 && correct == false){
      correct = true;
      score += 200;
    }
  }
  for(int k = 1; k <= 6; k++) lc.setRow(num, k, m3);
  while(millis() - t < Speed*3/5){
    int v = 0;
    if(num == 0) v = readP13();
    else if(num == 1) v = readP12();
    else if(num == 2) v = readP11();
    else if(num == 3) v = readP10();
    else if(num == 4) v = readP9();
    else if(num == 5) v = readP8();
    else if(num == 6) v = readP7();
    else if(num == 7) v = readP6();
    
    if(v == 1 && correct == false){
      correct = true;
      score += 300;
    }
  }
  for(int k = 0; k <= 7; k++) lc.setRow(num, k, m4);
  while(millis() - t < Speed*5/5){
    int v = 0;
    if(num == 0) v = readP13();
    else if(num == 1) v = readP12();
    else if(num == 2) v = readP11();
    else if(num == 3) v = readP10();
    else if(num == 4) v = readP9();
    else if(num == 5) v = readP8();
    else if(num == 6) v = readP7();
    else if(num == 7) v = readP6();
    
    if(v == 1 && correct == false){
      correct = true;
      score += 500;
    }
  }
  lc.clearDisplay(num);
}

int readLong(){
  static boolean prev = 100; // temp value
  static unsigned long last, pressed;
  boolean curr;

  if((millis()-last) >= 10){
    curr = digitalRead(btn1);
    if(prev != 0 && curr == 0){
      pressed = millis();
      prev = 0;
    }
    else if(prev == 0 && curr != 0){
      prev = 100;
      if(millis() - pressed > 1000){
        return 2;
      }
      else return 1;
    }
    if(prev == 0 && curr == 0){
      if(millis() - pressed > 1000){
        return 2;
      }
    }
      
    last = millis();
  }
  return 0;
}

//Send a MIDI note-on message.  Like pressing a piano key
//channel ranges from 0-15
void noteOn(byte channel, byte note, byte attack_velocity) {
  talkMIDI( (0x90 | channel), note, attack_velocity);
  delay(50);
}

//Send a MIDI note-off message.  Like releasing a piano key
void noteOff(byte channel, byte note, byte release_velocity) {
  talkMIDI( (0x80 | channel), note, release_velocity);
}

//Plays a MIDI note. Doesn't check to see that cmd is greater than 127, or that data values are less than 127
void talkMIDI(byte cmd, byte data1, byte data2) {
  mySerial.write(cmd);
  mySerial.write(data1);

  //Some commands only have one data byte. All cmds less than 0xBn have 2 data bytes 
  //(sort of: http://253.ccarh.org/handout/midiprotocol/)
  if( (cmd & 0xF0) <= 0xB0)
    mySerial.write(data2);

//  digitalWrite(ledPin, LOW);
}

int readP13(){
  static boolean prev = HIGH;
  static unsigned long last, pressed;
  boolean curr;

  if((millis()-last) >= 10){
    curr = digitalRead(p13);
    if(prev == HIGH && curr == LOW){
      pressed = millis();
      prev = LOW;
    }
    if(prev == LOW && curr == HIGH){
      prev = HIGH;
      if(millis() - pressed > 200){
        return 0;
      }
      else return 1;
    }
    if(prev==LOW&&curr==LOW){
      if(millis() - pressed > 200){
        return 0;
      }
    }
    last = millis();
  }
  return 0;
}

int readP12(){
  static boolean prev = HIGH;
  static unsigned long last, pressed;
  boolean curr;

  if((millis()-last) >= 10){
    curr = digitalRead(p12);
    if(prev == HIGH && curr == LOW){
      pressed = millis();
      prev = LOW;
    }
    if(prev == LOW && curr == HIGH){
      prev = HIGH;
      if(millis() - pressed > 200){
        return 0;
      }
      else return 1;
    }
    if(prev==LOW&&curr==LOW){
      if(millis() - pressed > 200){
        return 0;
      }
    }
    last = millis();
  }
  return 0;
}

int readP11(){
  static boolean prev = HIGH;
  static unsigned long last, pressed;
  boolean curr;

  if((millis()-last) >= 10){
    curr = digitalRead(p11);
    if(prev == HIGH && curr == LOW){
      pressed = millis();
      prev = LOW;
    }
    if(prev == LOW && curr == HIGH){
      prev = HIGH;
      if(millis() - pressed > 200){
        return 0;
      }
      else return 1;
    }
    if(prev==LOW&&curr==LOW){
      if(millis() - pressed > 200){
        return 0;
      }
    }
    last = millis();
  }
  return 0;
}

int readP10(){
  static boolean prev = HIGH;
  static unsigned long last, pressed;
  boolean curr;

  if((millis()-last) >= 10){
    curr = digitalRead(p10);
    if(prev == HIGH && curr == LOW){
      pressed = millis();
      prev = LOW;
    }
    if(prev == LOW && curr == HIGH){
      prev = HIGH;
      if(millis() - pressed > 200){
        return 0;
      }
      else return 1;
    }
    if(prev==LOW&&curr==LOW){
      if(millis() - pressed > 200){
        return 0;
      }
    }
    last = millis();
  }
  return 0;
}

int readP9(){
  static boolean prev = HIGH;
  static unsigned long last, pressed;
  boolean curr;

  if((millis()-last) >= 10){
    curr = digitalRead(p9);
    if(prev == HIGH && curr == LOW){
      pressed = millis();
      prev = LOW;
    }
    if(prev == LOW && curr == HIGH){
      prev = HIGH;
      if(millis() - pressed > 200){
        return 0;
      }
      else return 1;
    }
    if(prev==LOW&&curr==LOW){
      if(millis() - pressed > 200){
        return 0;
      }
    }
    last = millis();
  }
  return 0;
}

int readP8(){
  static boolean prev = HIGH;
  static unsigned long last, pressed;
  boolean curr;

  if((millis()-last) >= 10){
    curr = digitalRead(p8);
    if(prev == HIGH && curr == LOW){
      pressed = millis();
      prev = LOW;
    }
    if(prev == LOW && curr == HIGH){
      prev = HIGH;
      if(millis() - pressed > 200){
        return 0;
      }
      else return 1;
    }
    if(prev==LOW&&curr==LOW){
      if(millis() - pressed > 200){
        return 0;
      }
    }
    last = millis();
  }
  return 0;
}

int readP7(){
  static boolean prev = HIGH;
  static unsigned long last, pressed;
  boolean curr;

  if((millis()-last) >= 10){
    curr = digitalRead(p7);
    if(prev == HIGH && curr == LOW){
      pressed = millis();
      prev = LOW;
    }
    if(prev == LOW && curr == HIGH){
      prev = HIGH;
      if(millis() - pressed > 200){
        return 0;
      }
      else return 1;
    }
    if(prev==LOW&&curr==LOW){
      if(millis() - pressed > 200){
        return 0;
      }
    }
    last = millis();
  }
  return 0;
}

int readP6(){
  static boolean prev = HIGH;
  static unsigned long last, pressed;
  boolean curr;

  if((millis()-last) >= 10){
    curr = digitalRead(p6);
    if(prev == HIGH && curr == LOW){
      pressed = millis();
      prev = LOW;
    }
    if(prev == LOW && curr == HIGH){
      prev = HIGH;
      if(millis() - pressed > 200){
        return 0;
      }
      else return 1;
    }
    if(prev==LOW&&curr==LOW){
      if(millis() - pressed > 200){
        return 0;
      }
    }
    last = millis();
  }
  return 0;
}

void re_set(){
  mode = 0;
  menu = 0;
  inst = 0;
  music = 0;
  gameOn = false;
}
