#define __SERIAL__ 1
#define DOT 1
#define __GPS__ 1

#ifdef DOT
bool dot = true;
#endif

#include <avr/wdt.h>
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

#ifdef __GPS__
#include <TinyGPS.h>

TinyGPS gps;
SoftwareSerial ss(16,15);

static const char GPGGA[7] = {'$','G','P','G','G','A',','};
int gps_sync_gmt_time_index = 0;
char gps_sync_gmt_time[6];

unsigned long gps_time = 0;
unsigned long timeout = 0;
boolean timeout_flag = false;
const unsigned long gps_update = 300000; /*86400000;*/
#endif

enum{
  IN_12B = 100,
  IN_14,
  IN_2
};

const int type = IN_14;

const bool sync_time = false;
const bool temperature_calibration = false;

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

unsigned short state = 0; 

const unsigned long s1 = 10000;
const unsigned long s2 = 12000;
const unsigned long s3 = 14000;
unsigned long sr = 0;

static char mode = '6';

void UpdateHDC1080Temperature()
{
  static int c = 0;
  static float temperature_t = 0;
  static const float ma = 1.0f;
  if(c < ma){
    temperature_t += hdc1080.readTemperature();
    ++c;
  }
  else if(c >= ma){
    temperature = temperature_t / ma;
    
    if(temperature_calibration){
      temperature -= 4.5f;
    }
    
    temperature_t = 0;
    c = 0; 
  }
}

void UpdateHDC1080Humidity()
{
  static int c = 0;
  static float humidity_t = 0;
  static const float ma = 1.0f;
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
  if(!(type == IN_2)){
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
  if(!(type == IN_2)){
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
}

void DisplayTime()
{
  
  DateTime now = rtc.now();
  h = now.hour();
  m = now.minute();
  s = now.second();  

  static const int dt1 = 100;  
  static const int dt2 = 400;
  
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
  if(!(type == IN_2)){
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

  wdt_enable (WDTO_1S);

  snt = millis();
  sr = millis();

  #ifdef __SERIAL__
  Serial.begin(9600);
  if(type == IN_12B){
    serial.begin(9600);
  }
  else if(type == IN_14){
    serial.begin(115200);
    #ifdef __GPS__
    //ss.begin(9600);
    gps_time = millis();
    #endif
  }
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

  UpdateHDC1080Temperature();
  UpdateHDC1080Humidity();

}

const static int set_time_length = 15;
char set_time[set_time_length];
char set_time_index = 0;

void loop() {
  #ifdef __SERIAL__

  while(Serial.available()) {
    char temp = Serial.read();
    
    if(temp >= '0' && temp <= '9'){
      mode = temp;
    }

    if(set_time_index < set_time_length){
      set_time[set_time_index++] = temp;
    }
    
    if(set_time_index == set_time_length){
      set_time_index = 0;
      if(set_time[0] == 's' || set_time[0] == 'S'){
        
        boolean set_time_flag = true;
        
        for(int i = 1;i < set_time_length;++i){
          if(set_time[i] >= '0' && set_time[i] <= '9'){
          }
          else{
            set_time_flag = false;
          }
        }

        if(set_time_flag){
          char year_char[5];
          char month_char[3];
          char day_char[3];
          char hour_char[3];
          char minute_char[3];
          char second_char[3];

          year_char[0] = set_time[1];
          year_char[1] = set_time[2];
          year_char[2] = set_time[3];
          year_char[3] = set_time[4];

          month_char[0] = set_time[5];
          month_char[1] = set_time[6];

          day_char[0] = set_time[7];
          day_char[1] = set_time[8];

          hour_char[0] = set_time[9];
          hour_char[1] = set_time[10];          

          minute_char[0] = set_time[11];
          minute_char[1] = set_time[12];   

          second_char[0] = set_time[13];
          second_char[1] = set_time[14];   

          int set_year = atoi(year_char);
          int set_month = atoi(month_char);
          int set_day = atoi(day_char);
          int set_hour = atoi(hour_char);
          int set_minute = atoi(minute_char);
          int set_second = atoi(second_char);    

          if(set_year < 1970){
            set_time_flag = false;
          }
          if(set_month < 1 || set_month > 12){
            set_time_flag = false;
          }
          if(set_day < 1 || set_day > 31){
            set_time_flag = false;
          }
          if(set_hour < 0 || set_hour > 23){
            set_time_flag = false;
          }    
          if(set_minute < 0 || set_minute > 59){
            set_time_flag = false;
          }
          if(set_second < 0 || set_second > 59){
            set_time_flag = false;
          }        

          if(set_time_flag){
/*            
            Serial.println("Set Time!");
            Serial.println(set_year);
            Serial.println(set_month);
            Serial.println(set_day);
            Serial.println(set_hour);
            Serial.println(set_minute);
            Serial.println(set_second);
*/
            DateTime dt(set_year,set_month,set_day,set_hour,set_minute,set_second);
            rtc.adjust(dt);
/*
            DateTime now = rtc.now();
            Serial.println(now.year());
            Serial.println(now.month());
            Serial.println(now.day());
            Serial.println(now.hour());
            Serial.println(now.minute());
            Serial.println(now.second());            
*/
            mode = '6';
          }
          
        }
        
      }
    }
    
    Serial.write(mode);           
  }  

  while(serial.available()) {
    char temp = serial.read();
    
    if(temp >= '0' && temp <= '9'){
      mode = temp;
    }

    if(set_time_index < set_time_length){
      set_time[set_time_index++] = temp;
    }
    
    if(set_time_index == set_time_length){
      set_time_index = 0;
      if(set_time[0] == 's' || set_time[0] == 'S'){
        
        boolean set_time_flag = true;
        
        for(int i = 1;i < set_time_length;++i){
          if(set_time[i] >= '0' && set_time[i] <= '9'){
          }
          else{
            set_time_flag = false;
          }
        }

        if(set_time_flag){
          char year_char[5];
          char month_char[3];
          char day_char[3];
          char hour_char[3];
          char minute_char[3];
          char second_char[3];

          year_char[0] = set_time[1];
          year_char[1] = set_time[2];
          year_char[2] = set_time[3];
          year_char[3] = set_time[4];

          month_char[0] = set_time[5];
          month_char[1] = set_time[6];

          day_char[0] = set_time[7];
          day_char[1] = set_time[8];

          hour_char[0] = set_time[9];
          hour_char[1] = set_time[10];          

          minute_char[0] = set_time[11];
          minute_char[1] = set_time[12];   

          second_char[0] = set_time[13];
          second_char[1] = set_time[14];   

          int set_year = atoi(year_char);
          int set_month = atoi(month_char);
          int set_day = atoi(day_char);
          int set_hour = atoi(hour_char);
          int set_minute = atoi(minute_char);
          int set_second = atoi(second_char);    

          if(set_year < 1970){
            set_time_flag = false;
          }
          if(set_month < 1 || set_month > 12){
            set_time_flag = false;
          }
          if(set_day < 1 || set_day > 31){
            set_time_flag = false;
          }
          if(set_hour < 0 || set_hour > 23){
            set_time_flag = false;
          }    
          if(set_minute < 0 || set_minute > 59){
            set_time_flag = false;
          }
          if(set_second < 0 || set_second > 59){
            set_time_flag = false;
          }        

          if(set_time_flag){
/*            
            Serial.println("Set Time!");
            Serial.println(set_year);
            Serial.println(set_month);
            Serial.println(set_day);
            Serial.println(set_hour);
            Serial.println(set_minute);
            Serial.println(set_second);
*/
            DateTime dt(set_year,set_month,set_day,set_hour,set_minute,set_second);
            rtc.adjust(dt);
/*
            DateTime now = rtc.now();
            Serial.println(now.year());
            Serial.println(now.month());
            Serial.println(now.day());
            Serial.println(now.hour());
            Serial.println(now.minute());
            Serial.println(now.second());            
*/
            mode = '6';
          }
          
        }
        
      }
    }
    
    serial.write(mode);           
  }  
  #endif

  #ifdef __GPS__
  
  if((millis() - gps_time) > gps_update){
    if(!timeout_flag){
      timeout = millis();
      timeout_flag = true;
    }

    ss.begin(9600);
    while(ss.available()){
      char c = ss.read();

      if(c == GPGGA[gps_sync_gmt_time_index]){
        ++gps_sync_gmt_time_index;
      }
      else if(gps_sync_gmt_time_index < 6){
        gps_sync_gmt_time_index = 0;
      }

      if(gps_sync_gmt_time_index > 6 && gps_sync_gmt_time_index < 14){
        if((gps_sync_gmt_time_index - 8) > -1){
          gps_sync_gmt_time[gps_sync_gmt_time_index - 8] = c;
        }
        ++gps_sync_gmt_time_index;
      }

      if(gps_sync_gmt_time_index >= 14){
        /*
        for(int i = 0;i < 6;++i){
          Serial.print(gps_sync_gmt_time[i]);
        }
        Serial.println();
        */
        
        char hour_char[3];
        char minute_char[3];
        char second_char[3];

        hour_char[0] = gps_sync_gmt_time[0];
        hour_char[1] = gps_sync_gmt_time[1];          

        minute_char[0] = gps_sync_gmt_time[2];
        minute_char[1] = gps_sync_gmt_time[3];   

        second_char[0] = gps_sync_gmt_time[4];
        second_char[1] = gps_sync_gmt_time[5];      

        int set_hour = atoi(hour_char);
        int set_minute = atoi(minute_char);
        int set_second = atoi(second_char);    

        boolean set_time_flag = true;

        for(int i = 0;i < 6;++i){
          if((gps_sync_gmt_time[i] < '0') || (gps_sync_gmt_time[i] > '9')){
            set_time_flag = false;
          }
        }
        
        if(set_hour < 0 || set_hour > 23){
          set_time_flag = false;
        }    
        if(set_minute < 0 || set_minute > 59){
          set_time_flag = false;
        }
        if(set_second < 0 || set_second > 59){
          set_time_flag = false;
        }     

        if(set_time_flag){
          DateTime now = rtc.now();
          DateTime dt(now.year(),now.month(),now.day(),set_hour,set_minute,set_second);
          rtc.adjust(dt + TimeSpan(0,8,0,0));          
        }    
        /*
        DateTime now = rtc.now();
        
        Serial.print(now.year(), DEC);
        Serial.print('/');
        Serial.print(now.month(), DEC);
        Serial.print('/');
        Serial.print(now.day(), DEC);
        Serial.print(" ");
        Serial.print(now.hour(), DEC);
        Serial.print(':');
        Serial.print(now.minute(), DEC);
        Serial.print(':');
        Serial.print(now.second(), DEC);
        Serial.println();    
        */                     

        /*
        for(int i = 0;i < 6;++i){
          gps_sync_gmt_time[i] = 0;
        }        
        */
        
        gps_sync_gmt_time_index = 0;
        timeout_flag = false;
        for(int i = 0;i < sizeof(gps_sync_gmt_time)/sizeof(char);++i){
          gps_sync_gmt_time[i] = 0;
        }
      }
    }

    if((millis() - timeout) > 10000){
      gps_sync_gmt_time_index = 0;
      timeout_flag = false;
    }

    if(!timeout_flag){
      gps_time = millis();
      ss.end();
    }
  }
  #endif

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
      if(state != 0){
        state = 0;
      }
      DisplayTime();
    }
    else if(srt > s1 && srt < s2){
      if(state != 1){
        UpdateHDC1080Temperature();
        state = 1;
      }
      DisplayTemperature();     
    }
    else if(srt > s2 && srt < s3){
      if(state != 2){
        UpdateHDC1080Humidity();
        state = 2;
      }
      DisplayHumidity();      
    }

    if(srt > s3){
      sr = millis();
    }
  }
  else if(mode == '7'){
  }

  #ifdef DOT
  if((millis() - dot_last_time) > 1000){
    dot ^= true;
    dot_last_time = millis();
  }
  #endif

  wdt_reset();

}
