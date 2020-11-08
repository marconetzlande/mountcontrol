#include "RTClib.h"
#include "Wire.h"
RTC_DS1307 rtc;

void setupRTC() {  
  delay(2000);
  Wire.begin();
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    abort();
  } 
  /* else {
    rtc.adjust(DateTime(__DATE__, __TIME__));
    DateTime now = rtc.now();
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(' ');
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();

  } */
  
  if (! rtc.isrunning()) {
     Serial.println("RTC is NOT running!");
     rtc.adjust(DateTime(__DATE__, __TIME__));
  }
  
  rtc.writeSqwPinMode(DS1307_SquareWave32kHz);
  pinMode(RTC_SQW, INPUT_PULLUP);
}

DateTime rtcnow;
DateTime lstnow;

//filter wrong rtc responses
void getRTC_Time() {
  rtcnow = rtc.now();
  byte n = 0;
  while (
    (rtcnow.year()  == 2165) ||
    (rtcnow.month()  > 12)   || (rtcnow.month()  == 0)   || 
    (rtcnow.day()    > 31)   || (rtcnow.day()    == 0)   || 
    (rtcnow.hour()   > 60)   || 
    (rtcnow.minute() > 60)   ||
    (rtcnow.second() > 60)   && 
    ( n < 2 ) // Do not retry more than 2 times.
  ) {
    n++;
    delayMicroseconds(100);
    rtcnow = rtc.now();
  }

}

/*
 * https://logout.hu/bejegyzes/tavcsoves/local_sidereal_time_on_atmega328_arduino.html
 * Based on http://astro.neutral.org/arduino/local-sidereal-time-arduino-microcontroller.shtml
 * and https://forum.arduino.cc/index.php?topic=359573.0 V-drive user's example.
 * 
 * távcsőves 2017.06.24.
 * 
 * 
**/

#define TIMEZONE 1 // Your timezone, my location timezone is 1 (Central Europe)
#define DST 0 // Value = 1 if summer time, in winter value = 0
#define LONGITUDE 8.098551 // Your position to the Earth (West value - sign. My current position in the example, if you not have GPS receiver, Google Maps tell your cordinate's.)

void CalculateLST() { // Calculate Local Sidereal Time
  double M, Y, D, MN, H, S;
  double A, B, C, E, F;

  M =  (int)rtcnow.month();
  Y =  (int)rtcnow.year();
  D =  (int)rtcnow.day(); 
  MN = (int)rtcnow.minute();
  H =  (int)rtcnow.hour() - (TIMEZONE + DST); // Hours must be GMT/UT.
  S =  (int)rtcnow.second();

  if (M < 3) {
    M += 12;
    Y -= 1;
  }

/*
A = (long)Y/100;
B = (long)A/4;
C = (long)2-A+B;
E = (long)(365.25*(Y+4716));
F = (long)(30.6001*(M+1));
CurrentJDN = C+D+E+F-1524.5;


CurrentJDN = Year/100 + Year/100/4 + 365,25 * (Year+4716) + 30,6001*(Month+1) + Day  2 - 1524,5;
CurrentJDN = Year/100 + Year/100/4 + 365,25 * (Year+4716) + 30,6001*(Month+1) + Day - 1522,5;
CurrentJDN = Year*(1/100 + 1/100/4) + 365,25 * (Year+4716) + 30,6001*(Month+1) + Day - 1522,5;
CurrentJDN = Year*(1/100 + 1/25) + 365,25 * (Year+4716) + 30,6001*(Month+1) + Day - 1522,5;
CurrentJDN = Year/20 + 365,25 * (Year+4716) + 30,6001*(Month+1) + Day - 1522,5;
CurrentJDN = Year/20 + 365,25 * (Year+4716) + 30,6001*Month + 30,6001 + Day - 1522,5;
CurrentJDN = Year/20 + 365,25 * (Year+4716) + 30,6001*Month + Day - 1491,8999;
CurrentJDN = Year/20 + 365,25 * Year + 1722519 + 30,6001*Month + Day - 1491,8999;
CurrentJDN = Year/20 + 365,25 * Year + 30,6001*Month + Day + 1721027,1001;
CurrentJDN = Y*365,3 + 30,6001*M + D + 1721027,1001;
TESTEN!! --> Es gibt eine Abweichung von 117. Die formel ist warscheinlich nicht richtig umgestellt.

NewJDN = LastJDN + H / 24;

Current_d = Year*365,3 + 30,6001*Month + Day  -730.512,999;
*/


  A = (long)Y / 100;
  B = (long)A / 4;
  C = (long)2 - A + B;
  E = (long)(365.25 * (Y + 4716));
  F = (long)(30.6001 * (M + 1));

  double LastJDN = C + D + E + F - 1524.5; // Julian day last midnight UT
  double Current_d = LastJDN - 2451545.0; //Days since Year 2000
  long Current_T = Current_d / 36525; // Julian centuries since Year 2000
  long NewJDN = LastJDN + H / 24; // Julian day today

/*
  Serial.println();
  Serial.print("Julian Day Number..: ");
  Serial.print(NewJDN); // Julian day today

  Serial.println(); 
  double x = Y*365.3 + 30.6001*M + D + 1721027.1001 - 117 + H/24;
  Serial.println(NewJDN - x); 

  long NewJDN = Y*365.3 + 30.6001*M + D + 1721027.1001 - 117.0 + H/24;
*/

  double Term1 = 6.697374558; // this line must be a double!
  double Term2 = 0.06570982441908 * Current_d;

  H = H + ((double)MN / 60) + ((double)S / 3600);

  float GMST;
  float Term3;

  Term3 = 0.00273790935 * H;
  Term3 += H;
  GMST = Term1 + Term2 + Term3; // Terms are found at http://aa.usno.navy.mil/faq/docs/GAST.php

  //add on longitude divided by 15 to get LST

  double LST = GMST + (LONGITUDE / 15); // longitude as hour angle.

  //reduce it to 24 format

  int LSTint;
  LSTint = (int)LST;
  LSTint /= 24;
  LST = LST - (double)LSTint * 24;
  
  int LST_H = (int)LST;
  int LST_M = ((LST - LST_H) * 60);
  int LST_S = (((LST - LST_H) * 60) - LST_M) * 60;

  lstnow = DateTime(rtcnow.year(),rtcnow.month(),rtcnow.day(),LST_H,LST_M,LST_S);
}

boolean raGuiding = false;

boolean isGuiding() {
  return raGuiding;
}

void startGuiding() {
  raGuiding = true;
  RA_Stepper.setDirection(1);
  RA_Stepper.enable();
  attachInterrupt(digitalPinToInterrupt(RTC_SQW), RTCGuidingStep, FALLING);
}

void stopGuiding() {
  raGuiding = false;
  detachInterrupt(digitalPinToInterrupt(RTC_SQW));
}

void RTCGuidingStep() {
  if (raGuiding) {
    static double ra_precise = RA_DIVISOR;
    static long ra_divisor = RA_DIVISOR;
    static long fc = 0;
    fc++;
    
    if (fc>ra_divisor) {
      fc=0;
      
      RA_Stepper.step();
  
      //moreprecise ra guiding
      ra_precise = (ra_precise + ra_divisor)/2;
      boolean ra_diff = (fabs(ra_precise - RA_DIVISOR)>=0.1);
      if (ra_diff) {
        if (ra_precise > RA_DIVISOR) {
          ra_divisor = RA_DIVISOR-1;
        } else {
          ra_divisor = RA_DIVISOR+1;
        }
      } else {
        ra_divisor = RA_DIVISOR;
      }
    }
  }
}
