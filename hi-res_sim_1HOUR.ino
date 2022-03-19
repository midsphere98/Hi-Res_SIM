//  _  _ _                  ___ ___ __  __ 
// | || (_)___ _ _ ___ ___ / __|_ _|  \/  |
// | __ | |___| '_/ -_|_-< \__ \| || |\/| |
// |_||_|_|   |_| \___/__/ |___/___|_|  |_|
// 
// # # # # # 1 HOUR Version. 1 # # # # #




#include <IRremote.hpp>
#include <Wire.h>
#include "RTClib.h" //rtc library

IRrecv irrecv(10);//IR 리시버 핀은 10번임.
decode_results results;


int LEVEL = 0; //설정단계선언플래그. 0은 시작, 1은 깊이설정, 2는 모드설정, 3은 시간설정, 4는 작동
//stepper input
int PUL=7; //define Pulse pin
int DIR=8; //define Direction pin
int ENA=9; //define Enable Pin

bool isRemote = true; //리모컨으로 조종하고 있는가?
bool isDown = false; //모드 1 선택시 편도 작동을 위한 선언문. false 이면 내려가고 true 이면 올라옴. [기본값 = true]
bool isWaiting = true; //다음 시간까지 기다려야하는가? [기본값 : true]
bool isTime = false;

int relay = 6;

// = = = = = = = = = = = = = = = = = = = = = = = = = 이 박스 안에 있는 값들은 실제 움직임에 영향을 주는 변수들임.
long setnum = 4000;         //이거 조절하면 회전수 조절됨
long mdil = 1000;           //이거 조절하면 속도가 조절됨
int t_correction = 7;       //초 단위 보정을 한번에~~~~
int updown = 500;           //이거 조절하면 위아래 내리는거 가능, 500=10cm
// = = = = = = = = = = = = = = = = = = = = = = = = = 

int N = 0; //1분

RTC_DS3231 rtc;//rtc 선언

void setup() 
{
  pinMode (relay, OUTPUT);
  pinMode (PUL, OUTPUT);
  pinMode (DIR, OUTPUT);
  pinMode (ENA, OUTPUT);

  Serial.begin(9600);
  

  rtc.begin();
  Serial.print("\n\n\n# # # # # # # # # # # #\n");
  Serial.print("HI-res SIM\n");
  Serial.print("# # # # # # # # # # # #\n");
  Serial.print("# # # # # 1 HOUR Version. 1 # # # # #\n \n \n");


  Serial.print("= = = = = = = = = = = = = = = = = = = = = = = \n");
  Serial.print("Initializing Hi-res SIM \n");
  Serial.print("getting time data from connected pc");

  // rtc.adjust로 정해진 시간은 "컴파일 시간입니다"
  // 따라서 컴파일 땡 하는 순간의 시간을 잡게 되므로, 컴파일 이후 시간 연산이 안되는 동안에는 시간이 밀릴 수 밖에 없으니
  // 딜레이를 넣어 약간의 오차보정을 잡아줍니다.
  
  delay(300); //3초간 대기 // 시작시간 오차 잡는데 사용
  
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); //컴퓨터 시간 끌어다 씀. 시간 재설정할때 주석 풀것.
  rtc.adjust(DateTime(2022,3,13,19,59,48)); //수동(yyyy, mm , dd, hh, mm, ss)
  
  irrecv.enableIRIn(); //IR수신시작
}



// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = 모터 움직이는 코드
void goingDown() {
  digitalWrite(ENA,HIGH);
  digitalWrite(relay, HIGH);
  

  Serial.println("모터가 내려갈거임");

  
  delay(200);  
  for (long i=0; i<=setnum; i++)   //i 값이 클 수록 많이 내려가고 작을 수록 조금 내려감 
  {

    digitalWrite(DIR,HIGH); //LOW면 올라가고 HIGH면 내려감
    digitalWrite(PUL,HIGH);
    delayMicroseconds(mdil); //속도조절 -> 값이 낮을 수록 빠름
    digitalWrite(PUL,LOW);
    delayMicroseconds(mdil); //속도조절 -> 값이 낮을 수록 빠름
  }
  delay(200);  
  digitalWrite(relay, LOW);
  digitalWrite(ENA,LOW);
}



void goingUp() {
  digitalWrite(ENA,HIGH);
  digitalWrite(relay, HIGH);

  
  Serial.println("모터가 올라갈거임");

  
  delay(200);  
  for (long i=0; i<=setnum; i++)   //i 값이 클 수록 많이 내려가고 작을 수록 조금 내려감 
  {

    digitalWrite(DIR,LOW); //LOW면 올라가고 HIGH면 내려감
    digitalWrite(PUL,HIGH);
    delayMicroseconds(mdil); //속도조절 -> 값이 낮을 수록 빠름
    digitalWrite(PUL,LOW);
    delayMicroseconds(mdil); //속도조절 -> 값이 낮을 수록 빠름
  }
  delay(200);
  
  for (long II=0; II<=100; II++)  //브레이크 위치보
  {

    digitalWrite(DIR,HIGH); //LOW면 올라가고 HIGH면 내려감
    digitalWrite(PUL,HIGH);
    delayMicroseconds(500); //속도조절 -> 값이 낮을 수록 빠름
    digitalWrite(PUL,LOW);
    delayMicroseconds(500); //속도조절 -> 값이 낮을 수록 빠름
  }
    for (long I_=0; I_<=100; I_++)   //i 값이 클 수록 많이 내려가고 작을 수록 조금 내려감 
  {

    digitalWrite(DIR,LOW); //LOW면 올라가고 HIGH면 내려감
    digitalWrite(PUL,HIGH);
    delayMicroseconds(500); //속도조절 -> 값이 낮을 수록 빠름
    digitalWrite(PUL,LOW);
    delayMicroseconds(500); //속도조절 -> 값이 낮을 수록 빠름
  }

  
  delay(200);  
  digitalWrite(relay, LOW);
  digitalWrite(ENA,LOW);
}
// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = 모터 움직이는 코드 끗




// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = 모터 스케줄러(건들면망함)
void loop()
{
  DateTime now = rtc.now();
  DateTime correct (now + TimeSpan(0,0,0,t_correction)); //시간오차 보정; 일,시간,분,초
  
  switch(isRemote)
  {
    case true :
    Serial.print("Remote Adjust mode\n");
    Serial.print("Push 'up' arrow to  crank up, 'down' arrow to crank down, 'OK' for confirm.");
      while (isRemote == true) //리모트 선언이 false가 될 때 까지. *은 ff6897이고 #은 FFB04F임 위는 FF18E7이고 아래는 FF4AB5 OK는 FF38C7
      {
        digitalWrite(ENA,HIGH); 
        digitalWrite(relay, HIGH); //스텝모터 잠금해제
        
        if(irrecv.decode(&results)) //리모컨 시그널 받기
        {
          Serial.println(results.value, HEX);
          irrecv.resume();
          
          if (results.value == 0xFF4AB5)
          {
            Serial.print("weeeeee");
            for(int crankup = 0; crankup <= updown; crankup++)
            {
              digitalWrite(DIR,HIGH); //LOW면 올라가고 HIGH면 내려감
              digitalWrite(PUL,HIGH);
              delayMicroseconds(300); //속도조절 -> 값이 낮을 수록 빠름
              digitalWrite(PUL,LOW);
              delayMicroseconds(300); //속도조절 -> 값이 낮을 수록 빠름
            }
          }
  
          if (results.value == 0xFF18E7)
          {
            Serial.print("weeeeee");
            for(int crankdown = 0; crankdown <= updown; crankdown++)
            {
              digitalWrite(DIR,LOW); //LOW면 올라가고 HIGH면 내려감
              digitalWrite(PUL,HIGH);
              delayMicroseconds(300); //속도조절 -> 값이 낮을 수록 빠름
              digitalWrite(PUL,LOW);
              delayMicroseconds(300); //속도조절 -> 값이 낮을 수록 빠름
            }
          }

        
        } //리모컨 시그널 받기 끝
        
        if(results.value == 0xFF38C7) //OK 버튼 신호가 들어오면
        {
          digitalWrite(ENA,LOW); 
          digitalWrite(relay, LOW); //스텝모터 꺼버려 
        
          isRemote = false; //리모컨 조종을 꺼버림. 이제 본 작동으로 넘어갈거임
          break;
        }
      }

      default:
        break;
  }

  if(correct.minute() == 0 and correct.second() == 0)
  {
    isTime = true;
  }else{
    isTime = false;
  }

  
  // = = = = = = = = = = = = = = = = = 본 작동 코드
  
  switch(isWaiting)
  {
    case true :
            DateTime now = rtc.now();
            DateTime correct (now + TimeSpan(0,0,0,t_correction));
        while (isTime != true) //정시가 될 때 까지 while문 반
        
        {

            DateTime now = rtc.now();
            DateTime correct (now + TimeSpan(0,0,0,t_correction)); //시간오차 보정; 일,시간,분,초

            
            Serial.print(correct.hour());
            Serial.print(":");
            Serial.print(correct.minute());
            Serial.print(":");
            Serial.print(correct.second());
            Serial.print("/");
            Serial.print(N);
            Serial.print("\n");
            //시간이나 띄울거임
            
              if(correct.minute() == 0 and correct.second() == 0)
                {
                  isTime = true;
                }else{
                  isTime = false;
                }

            if(irrecv.decode(&results)) //리모컨 시그널 받기
            {
              if(results.value == 0xFF6897) //* 을 누르게되면
              {
                isRemote = true; //이제 리모컨 조종으로 넘어갈거임
                break;
              }
            }

            
            if(isTime == true) //정시가되면
            {
              isWaiting = false; //시간이 되었으니 움직여야지
              break; //탈출~~~~~~~~~~~~~~~~~~~~~
            }
        }
        
        //N++; //1분증가
        

        
        if (N == 60)
        {
          N = 0; //60분이 되면 N은 다시 0으로 돌아갈거임.
        }
        
        break;            //이제 꺼져
    
    case false :
        break; //움직여야 하니까 꺼져 
            
  }
  // = = = = = = = = = = = = = = = = = 기다려야하는지 판단(종료)

  
  // = = = = = = = = = = = = = = = = = 움직여야하는지 판단
  
  switch(isDown)
  {
    case false :          //내려가야하면
        goingDown();  //내려
        isWaiting = true;  //기다려야 함을 선언
        isDown = true; //내려왔음을 선언
  
        break;  //실행했으니 꺼져
        
    case true :           //올라가야하면
        goingUp();  //올려
        isWaiting = true;
        isDown = false; //올라왔음을 선언

        break; //실행했으니 꺼져
  }
  // = = = = = = = = = = = = = = = = = 움직여야하는지 판단(종료)
  
}
// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = 모터 스케줄러(끝)
