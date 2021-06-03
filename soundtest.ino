#include <SoftwareSerial.h>
#include <DFPlayer_Mini_Mp3.h>
#include <Wire.h>

int btn0 = 14;
int btn1 = 5;

void setup () {
    Serial.begin (9600);
    pinMode(btn0, INPUT_PULLUP);
    pinMode(btn1, INPUT_PULLUP);  

    mp3_set_serial (Serial);      // DFPlayer-mini mp3 module 시리얼 세팅
    delay(1);                     // 볼륨값 적용을 위한 delay
    mp3_set_volume (30);          // 볼륨조절 값 0~30
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
 
void loop () {   
     
    int check1 = analogRead(btn0); // next
    int check2 = readLong(); // go or back

    if(check1 < 100 && mode == 0){
      menu++;
      if(menu == 2) menu = 0;
    }
    if(check1 < 100 && mode == 1){
      inst++;
      if(inst == 8) inst = 0;
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
    }
    if(check2 == 1 && mode == 0 && menu == 1){
      mode = 2;
    }
    if(check2 == 2){
      re_set();
    }
  
    
   Serial.print("mode: ");
   Serial.println(mode);
   Serial.print("music: ");
   Serial.println(music);
      if(mode == 2){
        if(gameOn==true){
          delay(3500);
          unsigned long t = millis();
          if(music==0){
            mp3_play(1);
            breaking(t, 79000);
          }
          else if(music==1){
            mp3_play(2);
            breaking(t, 74000);
          }
          else if(music==2){
            mp3_play(3);
            breaking(t, 106000);
          }

          re_set();
        }
        
      }
    
    if(check1  < 100 || check2 != 0) delay(300);
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

void breaking(unsigned long t, unsigned long l){
  while(millis() - t < l){
    if(readLong() == 2){
      mp3_stop();
      re_set();
      break;
    }
  }
}


void re_set(){
  mode = 0;
  menu = 0;
  inst = 0;
  music = 0;
  gameOn = false;
}
