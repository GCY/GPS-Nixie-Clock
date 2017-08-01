#define __SERIAL__ 1
#define DOT 1

#ifdef DOT
bool dot = true;
#endif

#include <Wire.h>
#include <EEPROM.h>

#ifdef __SERIAL__
#include <SoftwareSerial.h>
#endif

#include "RTClib.h"
#include "ClosedCube_HDC1080.h"

#ifdef __SERIAL__
SoftwareSerial serial(13, 12); // RX,TX
#endif

const bool sync_time = false;

//RTC_DS1307 rtc;
RTC_DS3231 rtc;

ClosedCube_HDC1080 hdc1080;

unsigned long snt = 0;

int tubes[] = {2,3,4,5,6,7};
int K1551[] = {8,9,10,11};

float temperature = 0;
float humidity = 0;

int h = 0;
int m = 0;
int s = 0;  

unsigned long dot_last_time = 0;
unsigned long sn = 0;

const unsigned long s1 = 10000;
const unsigned long s2 = 14000;
const unsigned long s3 = 18000;
unsigned long sr = 0;

static char mode = '6';

void UpdateHDC1080Temperature()
{
  static int c = 0;
  static float temperature_t = 0;
  static const float ma = 150.0f;
  if(c < ma){
    temperature_t += hdc1080.readTemperature();
    ++c;
  }
  else if(c >= ma){
    temperature = temperature_t / ma;
    temperature_t = 0;
    c = 0;
  }
}

void UpdateHDC1080Humidity()
{
  static int c = 0;
  static float humidity_t = 0;
  static const float ma = 150.0f;
  if(c < ma){
    humidity_t += hdc1080.readHumidity();
    ++c;
  }
  else if(c >= ma){
    humidity = humidity_t / ma;
    humidity_t = 0;
    c = 0;
  }
}

void BCD(int val)
{  
  if(val & 0x01){
    digitalWrite(K1551[3],HIGH);
  }
  else{
    digitalWrite(K1551[3],LOW);
  }
  if(val & 0x02){
    digitalWrite(K1551[2],HIGH);
  }
  else{
    digitalWrite(K1551[2],LOW);
  }  
  if(val & 0x04){
    digitalWrite(K1551[1],HIGH);
  }
  else{
    digitalWrite(K1551[1],LOW);
  }    
  if(val & 0x04){
    digitalWrite(K1551[1],HIGH);
  }
  else{
    digitalWrite(K1551[1],LOW);
  }    
  if(val & 0x08){
    digitalWrite(K1551[0],HIGH);
  }
  else{
    digitalWrite(K1551[0],LOW);
  }        
}

void DisplayTemperature()
{
  static const int dt1 = 200;   
  static const int dt2 = 800;   
   
  digitalWrite(tubes[5],LOW);
  delayMicroseconds(dt1);
  BCD(((int)temperature)/10);
  digitalWrite(tubes[2],HIGH);
  delayMicroseconds(dt2);
  digitalWrite(tubes[2],LOW);
  delayMicroseconds(dt1);
  BCD(((int)temperature)%10);
  digitalWrite(tubes[3],HIGH);
  delayMicroseconds(dt2);
  digitalWrite(tubes[3],LOW);
  delayMicroseconds(dt1);
  BCD( (int)( (temperature - ((int)temperature)) * 100) / 10);
  #ifdef DOT
  digitalWrite(A0,HIGH);
  #endif
  digitalWrite(tubes[4],HIGH);
  delayMicroseconds(dt2);
  #ifdef DOT
  digitalWrite(A0,LOW);
  #endif
  digitalWrite(tubes[4],LOW);
  delayMicroseconds(dt1);
  BCD( (int)( (temperature - ((int)temperature)) * 100) % 10);
  digitalWrite(tubes[5],HIGH);
  delayMicroseconds(dt2);   
  digitalWrite(tubes[5],LOW);  
}

void DisplayHumidity()
{
  static const int dt1 = 200;    
  static const int dt2 = 800;
  digitalWrite(tubes[5],LOW);
  delayMicroseconds(dt1);
  BCD(((int)humidity)/10);
  digitalWrite(tubes[2],HIGH);
  delayMicroseconds(dt2);
  digitalWrite(tubes[2],LOW);
  delayMicroseconds(dt1);
  BCD(((int)humidity)%10);
  digitalWrite(tubes[3],HIGH);
  delayMicroseconds(dt2);
  digitalWrite(tubes[3],LOW);
  delayMicroseconds(dt1);
  BCD( (int)( (humidity - ((int)humidity)) * 100) / 10);
  #ifdef DOT
  digitalWrite(A0,HIGH);
  #endif
  digitalWrite(tubes[4],HIGH);
  delayMicroseconds(dt2);
  #ifdef DOT
  digitalWrite(A0,LOW);
  #endif
  digitalWrite(tubes[4],LOW);
  delayMicroseconds(dt1);
  BCD( (int)( (humidity - ((int)humidity)) * 100) % 10);
  digitalWrite(tubes[5],HIGH);
  delayMicroseconds(dt2);     
  digitalWrite(tubes[5],LOW);
}

void DisplayTime()
{
  
  DateTime now = rtc.now();
  h = now.hour();
  m = now.minute();
  s = now.second();  
  
  static const int dt1 = 200;  
  static const int dt2 = 800;
  
  digitalWrite(tubes[5],LOW);
  delayMicroseconds(dt1);
  BCD(h/10);
  digitalWrite(tubes[0],HIGH);
  delayMicroseconds(dt2);
  digitalWrite(tubes[0],LOW);
  delayMicroseconds(dt1);
  BCD(h%10);  
  digitalWrite(tubes[1],HIGH);
  delayMicroseconds(dt2);
  digitalWrite(tubes[1],LOW);
  delayMicroseconds(dt1);
  BCD(m/10);
  #ifdef DOT
  if(dot){
    digitalWrite(A0,HIGH);
  }  
  #endif
  digitalWrite(tubes[2],HIGH);
  delayMicroseconds(dt2);
  #ifdef DOT
  if(dot){
    digitalWrite(A0,LOW);
  }  
  #endif
  digitalWrite(tubes[2],LOW);
  delayMicroseconds(dt1);
  BCD(m%10);
  digitalWrite(tubes[3],HIGH);
  delayMicroseconds(dt2);
  digitalWrite(tubes[3],LOW);
  delayMicroseconds(dt1);
  BCD(s/10);
  #ifdef DOT
  if(dot){
    digitalWrite(A0,HIGH);
  }  
  #endif
  digitalWrite(tubes[4],HIGH);
  delayMicroseconds(dt2);
  #ifdef DOT
  if(dot){
    digitalWrite(A0,LOW);
  }  
  #endif
  digitalWrite(tubes[4],LOW);
  delayMicroseconds(dt1);
  BCD(s%10);
  digitalWrite(tubes[5],HIGH);
  delayMicroseconds(dt2);   
  digitalWrite(tubes[5],LOW);
}

void DisplayRandom()
{
  static const int dt1 = 500;  
  static const int dt2 = 10000;
  
  digitalWrite(tubes[5],LOW);
  delayMicroseconds(dt1);
  BCD(random(0,9));
  digitalWrite(tubes[0],HIGH);
  delayMicroseconds(dt2);
  digitalWrite(tubes[0],LOW);
  delayMicroseconds(dt1);
  BCD(random(0,9));
  digitalWrite(tubes[1],HIGH);
  delayMicroseconds(dt2);
  digitalWrite(tubes[1],LOW);
  delayMicroseconds(dt1);
  BCD(random(0,9));
  digitalWrite(tubes[2],HIGH);
  delayMicroseconds(dt2);
  digitalWrite(tubes[2],LOW);
  delayMicroseconds(dt1);
  BCD(random(0,9));
  digitalWrite(tubes[3],HIGH);
  delayMicroseconds(dt2);
  digitalWrite(tubes[3],LOW);
  delayMicroseconds(dt1);
  BCD(random(0,9));
  digitalWrite(tubes[4],HIGH);
  delayMicroseconds(dt2);
  digitalWrite(tubes[4],LOW);
  delayMicroseconds(dt1);
  BCD(random(0,9));
  digitalWrite(tubes[5],HIGH);
  delayMicroseconds(dt2);   
  digitalWrite(tubes[5],LOW);  
}

void DisplaySN()
{
  static const int dt1 = 200;  
  static const int dt2 = 800;
  
  digitalWrite(tubes[5],LOW);
  delayMicroseconds(dt1);
  BCD(sn);
  digitalWrite(tubes[0],HIGH);
  delayMicroseconds(dt2);
  digitalWrite(tubes[0],LOW);
  delayMicroseconds(dt1);
  BCD(sn);
  digitalWrite(tubes[1],HIGH);
  delayMicroseconds(dt2);
  digitalWrite(tubes[1],LOW);
  delayMicroseconds(dt1);
  BCD(sn);
  digitalWrite(tubes[2],HIGH);
  delayMicroseconds(dt2);
  digitalWrite(tubes[2],LOW);
  delayMicroseconds(dt1);
  BCD(sn);
  digitalWrite(tubes[3],HIGH);
  delayMicroseconds(dt2);
  digitalWrite(tubes[3],LOW);
  delayMicroseconds(dt1);
  BCD(sn);
  digitalWrite(tubes[4],HIGH);
  delayMicroseconds(dt2);
  digitalWrite(tubes[4],LOW);
  delayMicroseconds(dt1);
  BCD(sn);
  digitalWrite(tubes[5],HIGH);
  delayMicroseconds(dt2);   
  digitalWrite(tubes[5],LOW);
}

void setup() {

  snt = millis();
  sr = millis();

  #ifdef __SERIAL__
  Serial.begin(9600);
  serial.begin(9600);
  #endif  
  
  Wire.begin();
  Wire.setClock(3400000);
  
  rtc.begin();
  if(sync_time){
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  
  hdc1080.begin(0x40);

  randomSeed(analogRead(A3));

  pinMode(A0,OUTPUT);
  
  for(int i = 0;i < (sizeof(tubes)/sizeof(tubes[0]));++i){
    pinMode(tubes[i],OUTPUT);
    digitalWrite(tubes[i],LOW);
  }
  for(int i = 0;i < (sizeof(K1551)/sizeof(K1551[0]));++i){
    pinMode(K1551[i],OUTPUT);
  } 

}

void loop() {
  #ifdef __SERIAL__
  if (serial.available()) {
    char temp = serial.read();
    if(temp >= '0' && temp <= '6'){
      mode = temp;
    }
    Serial.write(mode);           
  }  

  if (Serial.available()) {
    char temp = Serial.read();
    if(temp >= '0' && temp <= '6'){
      mode = temp;
    }
    Serial.write(mode);           
  }    
  #endif

  //UpdateHDC1080Temperature();
  //UpdateHDC1080Humidity();
  if(mode == '1'){
    DisplayTime();
  }
  else if(mode == '2'){
    UpdateHDC1080Temperature();
    DisplayTemperature();
  }  
  else if(mode == '3'){
    UpdateHDC1080Humidity();
    DisplayHumidity();
  }  
  else if(mode == '4'){
    DisplayRandom();
  }  
  else if(mode == '5'){
    if((millis() - snt) > 1000){
      snt = millis();
      Serial.println("T");
      ++sn;
      if(sn > 9){
        sn = 0;
      }
    }
    DisplaySN();
  }    
  else if(mode == '6'){
    unsigned long srt = millis() - sr;
    if(srt < s1){
      DisplayTime();
    }
    else if(srt > s1 && srt < s2){
      UpdateHDC1080Temperature();
      DisplayTemperature();     
    }
    else if(srt > s2 && srt < s3){
      UpdateHDC1080Humidity();
      DisplayHumidity();      
    }

    if(srt > s3){
      sr = millis();
    }
  }

  #ifdef DOT
  if((millis() - dot_last_time) > 1000){
    dot ^= true;
    dot_last_time = millis();
  }
  #endif

  /*
  if(dot){
    digitalWrite(A0,HIGH);
  }
  else{
    digitalWrite(A0,LOW);
  }*/


}
