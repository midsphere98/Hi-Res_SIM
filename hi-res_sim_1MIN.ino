
#include <Wire.h>
#include "RTClib.h" //rtc library


int LEVEL = 0; //설정단계선언플래그. 0은 시작, 1은 깊이설정, 2는 모드설정, 3은 시간설정, 4는 작동
//stepper input
int PUL=7; //define Pulse pin
int DIR=8; //define Direction pin
int ENA=9; //define Enable Pin

bool isDown = false; //모드 1 선택시 편도 작동을 위한 선언문. false 이면 내려가고 true 이면 올라옴. [기본값 = true]
bool isWaiting = true; //다음 시간까지 기다려야하는가? [기본값 : true]


int relay = 6;

// = = = = = = = = = = = = = = = = = = = = = = = = = 이 박스 안에 있는 값들은 실제 움직임에 영향을 주는 변수들임.
long setnum = 4000;         //이거 조절하면 회전수 조절됨
long mdil = 1000;           //이거 조절하면 속도가 조절됨
int t_correction = 5;       //초 단위 보정을 한번에~~~~
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

  //rtc setup
  Serial.print("= = = = = = = = = = = = = = = = = = = = = = = \n");
  Serial.print("Initializing Hi-res SIM \n");
  Serial.print("getting time data from connected pc");

  // rtc.adjust로 정해진 시간은 "컴파일 시간입니다"
  // 따라서 컴파일 땡 하는 순간의 시간을 잡게 되므로, 컴파일 이후 시간 연산이 안되는 동안에는 시간이 밀릴 수 밖에 없으니
  // 딜레이를 넣어 약간의 오차보정을 잡아줍니다.
  
  delay(300); //3초간 대기 // 시작시간 오차 잡는데 사용
  
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); //컴퓨터 시간 끌어다 씀. 시간 재설정할때 주석 풀것.
  // rtc.adjust(DateTime(2022,3,13,19,59,48)); //수동(yyyy, mm , dd, hh, mm, ss)
 
}



//##############
//여기서부터는 함수임.
//##############
//void timeshow() {
//  //DateTime now = rtc.now();
//  //DateTime future (now + TimeSpan(0,0,0,t_correction)); //시간오차 보정; 일,시간,분,초
//  
//
//  Serial.print(hournow);
//  Serial.print(":");
//  Serial.print(minutenow);
//  Serial.print(":");
//  Serial.print(secondnow);
//  Serial.print("\n");지
//}


//tb6600 기준 pulse 1600, 0.5A
//i=4000일 때 딜레이 4000일 시 약 40cm고 1cm/sec으로 움직인다.

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
  digitalWrite(relay, LOW);
  digitalWrite(ENA,LOW);
}
// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = 모터 움직이는 코드 끗


//void mode2(){
//  DateTime now = rtc.now();
//  DateTime correct (now + TimeSpan(0,0,0,t_correction));
//  
//  if (isDown == false){
//    if (correct.minute() == 0 && correct.second() == 0){ //0분이 되면
//      goingDown(); //정회전
//      MODEFLAG = 1; //다 돌았으므로 편도 완료했다고 플래그 선언
//      N++;
//      
//      Serial.println("MOTOR DOWN");
//
//      while(future.minute() == 1){ // 특정 시간까지 대기
//        delay(100);
//        Serial.println("1분되기 전");
//
//        }
//        Serial.println("1분됨");
//      }
//    
//  if (isDown == true){ //만약 편도 완료된 상태라면(이제 올라와야 한다면)
//    if (correct.minute() == 1 && correct.second() == 0){ //특정 시간이 되면
//      goingUp(); //역회전
//      MODEFLAG = 0; //다시 올라왔다고 플래그 선언
//      N++;
//      
//      Serial.println("MOTOR UP");
//
//      while(correct.minute() == 2){ // 특정 시간까지 대기
//      delay(100);
//      Serial.println("1분되기 전");
//
//      }
//      Serial.println("1분됨");
//        }
//      }
//    }
//  }


// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = 모터 스케줄러(건들면망함)
void loop()
{
  DateTime now = rtc.now();
  DateTime correct (now + TimeSpan(0,0,0,t_correction)); //시간오차 보정; 일,시간,분,초

  // = = = = = = = = = = = = = = = = = 기다려야하는지 판단
  switch(isWaiting)
  {
    case true :
        while (correct.minute() != N && correct.second() != 0) //정시가 될 때 까지 while문 반
        
        {

            DateTime now = rtc.now();
            DateTime correct (now + TimeSpan(0,0,0,t_correction)); //시간오차 보정; 일,시간,분,초

            
            Serial.print(correct.hour());
            Serial.print(":");
            Serial.print(correct.minute());
            Serial.print(":");
            Serial.print(correct.second());
            Serial.print("\n");
            //시간이나 띄울거임
        }
        isWaiting = false; //시간이 되었으니 움직여야지
        N++; //1분증가
        
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
