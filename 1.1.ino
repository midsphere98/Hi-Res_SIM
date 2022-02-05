#include <stdio.h>
#include <DS1302.h>


int Green = 5;
unsigned long past = 0;
int buzzerPin = 6;
int flag = 0; // 과거 기준 시간보다 1000ms 지났음을 판단하는 플래그. 시간 플로팅에 이용됨.

DS1302 rtc(4, 3, 2);

void setup() {
  //RTC를 시작하고 작성보호모드(Write-Protection)을 해제 
  rtc.halt(false);
  rtc.writeProtect(false);
  //시리얼 시작
  Serial.begin(9600);
  
  //시간,날짜 초기 설정. 설정 이후 주석 처리 하여 리셋되지 않도록 하여야
  //rtc.setDOW(SATURDAY);
  rtc.setTime(0, 43, 40); //시간 포맷은 24hr임
  //rtc.setDate(5, 2, 2022);
  pinMode(Green, OUTPUT);
}

void time()
{
  //요일 출력
  Serial.print(rtc.getDOWStr());
  Serial.print(" ");
  //날짜 출력
  Serial.print(rtc.getDateStr());
  Serial.print(" -- ");
  //시간 출력
  Serial.print(rtc.getTimeStr());
  Serial.print("\n");
}

void LED()
{
  unsigned long times = millis();
  //1초 대기 후 점
  if(times - past >= 1000)
  {
    past = times;
    flag = 1;
  }

  if(flag ==1)
  {
    digitalWrite(Green, !(digitalRead(Green)));
    time();
    flag = 0;
  }

}

void loop() {
  LED();

}
