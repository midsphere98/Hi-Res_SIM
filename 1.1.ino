#include <DS1302.h>
#include <stdio.h>
//stepper input
int PUL=7; //define Pulse pin
int DIR=6; //define Direction pin
int ENA=5; //define Enable Pin

//RCT input
const int RST = 2; 
const int IO = 1; //dat
const int CLK = 0;

//creating rct obj
DS1302 rtc(RST, IO, CLK);

String dayAsString(const Time::Day day) {
  switch (day) {
    case Time::kSunday: return "Sunday";
    case Time::kMonday: return "Monday";
    case Time::kTuesday: return "Tuesday";
    case Time::kWednesday: return "Wednesday";
    case Time::kThursday: return "Thursday";
    case Time::kFriday: return "Friday";
    case Time::kSaturday: return "Saturday";
  }
  return "(unknown day)";
}


void setup() 
{
  pinMode (PUL, OUTPUT);
  pinMode (DIR, OUTPUT);
  pinMode (ENA, OUTPUT);
  Serial.begin(9600);
  rtc.halt(false);
  rtc.writeProtect(false);
  Time t(2022, 1, 27, 12, 52, 50, Time::kSunday);
  rtc.time(t);
}

//아워미터
void printTime() {
  // Get the current time and date from the chip.
  Time t = rtc.time();

  // Name the day of the week.
  const String day = dayAsString(t.day);

  // Format the time and date and insert into the temporary buffer.
  char buf[50];
  snprintf(buf, sizeof(buf), "%s %04d-%02d-%02d %02d:%02d:%02d",
           day.c_str(),
           t.yr, t.mon, t.date,
           t.hr, t.min, t.sec);

  // Print the formatted string to serial so we can see the time.
  Serial.println(buf);
}


void loop() {
  for (int i=0; i<4000; i++)   //i 값이 클 수록 많이 내려가고 작을 수록 조금 내려감 
  {
    digitalWrite(DIR,HIGH); //LOW면 내려가고 HIGH면 올라감
    digitalWrite(ENA,HIGH);
    digitalWrite(PUL,HIGH);
    delayMicroseconds(800); //속도조절 -> 값이 낮을 수록 빠름
    digitalWrite(PUL,LOW);
    delayMicroseconds(800); //속도조절 -> 값이 낮을 수록 빠름
  }
  printTime();
  delay(0); //i만큼 내려간 이후 얼마나 기다리느냐 : 1000이 1초
}
