#include "RTClib.h"
RTC_DS1307 rtc;

void setupRTC() {  
  delay(1000);
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    abort();
  } /*else {
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
  }*/

  if (! rtc.isrunning()) {
     //Serial.println("RTC is NOT running!");
     rtc.adjust(DateTime(__DATE__, __TIME__));
  }
  
  rtc.writeSqwPinMode(DS1307_SquareWave32kHz);
  pinMode(RTC_SQW, INPUT_PULLUP);
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
  static double ra_precise = RA_DIVISOR;
  static long ra_divisor = RA_DIVISOR;
  static long fc = 0;
  fc++;
  
  if (fc>ra_divisor) {
    fc=0;

    //Serial.println(ra_divisor);
    //Serial.println(ra_precise);
    
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
