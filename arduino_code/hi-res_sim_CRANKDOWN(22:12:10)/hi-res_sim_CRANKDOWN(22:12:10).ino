//  _  _ _                  ___ ___ __  __ 
// | || (_)___ _ _ ___ ___ / __|_ _|  \/  |
// | __ | |___| '_/ -_|_-< \__ \| || |\/| |
// |_||_|_|   |_| \___/__/ |___/___|_|  |_|
// 
// # # # # # CRANK DOWN CODE. 3 # # # # #

//!!!UPDATE!!!
// 2022.03.31
// 1. 리미트스위치를 장착했음; 5번 핀에 장착되었으며 스위치가 눌릴 경우 정지함
// 2. 올라오는 양을 구분해놨음.

// 2022.04.05
// 1. 내려왔다 올라오는 걸로 수정완료했음.
// 2. DELAY_DIVER 변수 추가했음(수정하면 내려가서 얼마나 기다리는가 정할 수 있음)
//    1000이 1초임.

// 2022.04.13
// 1. i값 자료형을 unsigned long으로 바꾸어줫음. 왜냐면 부호가 필요 없기 때문 그만큼의 자료형 범위를 확보하기 위함임. 그러니까 음수는 사용할수 없음.
// 2. setnum을 DIVER_DOWN으로 교체하였음. 함수명만 고쳣고 기능은 동일함.

// 2022.04.21
// 1. 시작 시 다이버를 끌어올려서 영점을 잡도록 순서 바꿧음

// 2022.12.10
// 1. 형상 업데이트.
// 2. 리모콘 죽였음.(삼고빔)
// 3. RGB LED 장착 (pin 12,11,10)



// ==========사용법===========
// 1. 파일을 업로드한다
// 2. 시리얼 모니터에서 엔터를 누른다
// 3. 모터가 내려간다
// 4. 모터를 멈추고 싶다면 엔터를 누른다
// 여러번 누르면 꼬여서 안멈추니까 참고




#include <Wire.h>
#include "RTClib.h" //rtc library


// ------- RGB LED
int RED = 12;
int GREEN = 11;
int BLUE = 10;

//stepper input
int PUL=7; //define Pulse pin
int DIR=8; //define Direction pin
int ENA=9; //define Enable Pin

//endstop
const int Endstop = 5;

//깊이 측정용 값
long DEPTH = 0;

bool isRemote = false; //리모컨으로 조종하고 있는가?
bool isDown = false; //모드 1 선택시 편도 작동을 위한 선언문. false 이면 내려가고 true 이면 올라옴. [기본값 = true]
bool isWaiting = true; //다음 시간까지 기다려야하는가? [기본값 : true]
bool isTime = false;

int relay = 6;

// = = = = = = = = = = = = = = = = = = = = = = = = = 이 박스 안에 있는 값들은 실제 움직임에 영향을 주는 변수들임.

long mdil = 1000;           //이거 조절하면 속도가 조절됨. 작을 수록 빨라짐
int t_correction = 9;       //초 단위 보정을 한번에~~~~

long       DIVER_DOWN = 5000000;         //이거 조절하면 회전수 조절됨
const long DIVER_UP = 100000; // 끌어올리는 양
const long DIVER_ZERO = 1000; //스탑 한 이후 조금 내려갈 양

const int DELAY_DIVER = 3000; //다이버가 내려갔다가 올라오기까지 기다릴 시간

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
  pinMode (Endstop, INPUT_PULLUP);

  pinMode (RED, OUTPUT);
  pinMode (GREEN, OUTPUT);
  pinMode (BLUE, OUTPUT);

  Serial.begin(9600);
  

  rtc.begin();
  Serial.print("\n\n\n# # # # # # # # # # # #\n");
  Serial.print("HI-res SIM\n");
  Serial.print("# # # # # # # # # # # #\n");
  Serial.print("# # # # # CRANK UP CODE. 20221210 # # # # #\n \n \n");


  Serial.print("= = = = = = = = = = = = = = = = = = = = = = = \n");
  Serial.print("Initializing Hi-res SIM \n");
  Serial.print("1. getting time data from connected pc... \n");

  // rtc.adjust로 정해진 시간은 "컴파일 시간입니다"
  // 따라서 컴파일 땡 하는 순간의 시간을 잡게 되므로, 컴파일 이후 시간 연산이 안되는 동안에는 시간이 밀릴 수 밖에 없으니
  // 딜레이를 넣어 약간의 오차보정을 잡아줍니다.

  
  delay(300); //3초간 대기 // 시작시간 오차 잡는데 사용
  
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); //컴퓨터 시간 끌어다 씀. 시간 재설정할때 주석 풀것.
    //rtc.adjust(DateTime(2022,3,13,19,59,40)); //수동(yyyy, mm , dd, hh, mm, ss)

  // Serial.print("= = = = = = = = = = = = = = = = = = = = = = = \n");
  // Serial.print("2. Initializing position : CRANK UP in process\n");
  // goingUp(); //스탑센서 닿을 때 까지 끌어올리기
  Serial.print("MAIN MOVE : CRANK UP START...");
}



// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = 모터 움직이는 코드
void goingDown() {
  digitalWrite(ENA,HIGH);
  digitalWrite(relay, HIGH);
  

  Serial.println("CRANK DOWN INIT ....");

  
  delay(5000);  
  for (unsigned long i=0; i<=DIVER_DOWN; i++)   //i 값이 클 수록 많이 내려가고 작을 수록 조금 내려감 
  {

    digitalWrite(DIR,HIGH); //LOW면 올라가고 HIGH면 내려감
    digitalWrite(PUL,HIGH);
    delayMicroseconds(mdil); //속도조절 -> 값이 낮을 수록 빠름
    digitalWrite(PUL,LOW);
    delayMicroseconds(mdil); //속도조절 -> 값이 낮을 수록 빠름

    DEPTH++;

    if(Serial.available() == 1)
    {
      Serial.println("\n \n= = = = = = = = Depth Calc RESULT = = = = = = =");
      Serial.println("\ndepth value : ");
      Serial.println(DEPTH);
      break;
    }
  }
  delay(200);  
  digitalWrite(relay, LOW);
  digitalWrite(ENA,LOW);
}



void goingUp() {
  digitalWrite(ENA,HIGH);
  digitalWrite(relay, HIGH);

  
  Serial.println("Crank up init");

  
  delay(200);  
  for (unsigned long i=0; i<=DIVER_UP; i++)   //i 값이 클 수록 많이 내려가고 작을 수록 조금 내려감 
  {

    digitalWrite(DIR,HIGH); //LOW면 올라가고 HIGH면 내려감
    digitalWrite(PUL,HIGH);
    delayMicroseconds(mdil); //속도조절 -> 값이 낮을 수록 빠름
    digitalWrite(PUL,LOW);
    delayMicroseconds(mdil); //속도조절 -> 값이 낮을 수록 빠름
    DEPTH++;

    if (digitalRead(Endstop) == true)
    
    {
      Serial.print("\nDepth is ");
      Serial.print(DEPTH);
      Serial.print("\n");
      break;
    }
  }
  
  delay(200);
  //정지 후 내려갈 양
    for (unsigned long i=0; i<=DIVER_ZERO; i++)   
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
// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = 모터 움직이는 코드 끗




// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = 모터 스케줄러(건들면망함)
void loop()
{
  DateTime now = rtc.now();
  DateTime correct (now + TimeSpan(0,0,0,t_correction)); //시간오차 보정; 일,시간,분,초

  Serial.end();
  Serial.begin(9600);

  Serial.println("\nPress ENTER to CRANK DOWN \n");
  while(Serial.available() == 0){

  }

  Serial.end();
  Serial.begin(9600);

  goingDown();
  
}
// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = 모터 스케줄러(끝)
