char serialdata[30];

void handleSerial() {
  static byte i = 0;
  static unsigned long last_packet;
  static boolean recvd = false;
  last_packet = millis();

  if ((i==0 && (Serial.peek() == '#'||Serial.peek() == ':')) || i>0) {
    serialdata[i++] = Serial.read();

    //#:Q#
    if (serialdata[0]=='#' && serialdata[1]==':' && serialdata[2]=='Q' && serialdata[3]=='#') {
      recvd = true;
      serialdata[i]='\0';
      stopGuiding();
    }
    
    //#:GC#
    if (serialdata[0]=='#' && serialdata[1]==':' && serialdata[2]=='G' && serialdata[3]=='C' && serialdata[4]=='#') {
      recvd = true;
      serialdata[i]='\0';
      getRTC_Time();
      while (rtcnow.hour() == 165) rtcnow = rtc.now();
      Serial.print(rtcnow.year(), DEC);
      Serial.print('/');
      Serial.print(rtcnow.month(), DEC);
      Serial.print('/');
      Serial.print(rtcnow.day(), DEC);
      Serial.print('#');
      //Returns: MM/DD/YY
    }

    //#:GS#
    if (serialdata[0]=='#' && serialdata[1]==':' && serialdata[2]=='G' && serialdata[3]=='S' && serialdata[4]=='#') {
      recvd = true;
      serialdata[i]='\0';
      getRTC_Time();
      CalculateLST();
      Serial.print(lstnow.hour(), DEC);
      Serial.print(':');
      Serial.print(lstnow.minute(), DEC);
      Serial.print(':');
      Serial.print(lstnow.second(), DEC);
      Serial.print('#');
      //Returns: HH:MM:SS#
    }
    
    //#:GL#
    if (serialdata[0]=='#' && serialdata[1]==':' && serialdata[2]=='G' && serialdata[3]=='L' && serialdata[4]=='#') {
      recvd = true;
      serialdata[i]='\0';
      getRTC_Time();
      Serial.print(rtcnow.hour(), DEC);
      Serial.print(':');
      Serial.print(rtcnow.minute(), DEC);
      Serial.print(':');
      Serial.print(rtcnow.second(), DEC);
      Serial.print('#');
      //Returns: HH:MM:SS#
    }
    
    //:SCMM/DD/YY#
    if (serialdata[0]==':' && serialdata[1]=='S' && serialdata[2]=='C' && serialdata[5]=='/' && serialdata[8]=='/' && serialdata[11]=='#') {
      recvd = true;
      serialdata[i]='\0';
      byte MM = atoi(serialdata+3);
      byte DD = atoi(serialdata+6);
      byte YY = atoi(serialdata+9);
      getRTC_Time();
      DateTime newTime = DateTime(YY,MM,DD,rtcnow.hour(),rtcnow.minute(),rtcnow.second());
      rtc.adjust(newTime);
      Serial.print('1');
    }
    
    //:SLHH:MM:SS# //:SSHH:MM:SS#
    if (serialdata[0]==':' && serialdata[1]=='S' && serialdata[2]=='L' && serialdata[5]==':' && serialdata[8]==':' && serialdata[11]=='#') {
      recvd = true;
      serialdata[i]='\0';
      byte hh = atoi(serialdata+3);
      byte mm = atoi(serialdata+6);
      byte ss = atoi(serialdata+9);
      getRTC_Time();
      DateTime newTime = DateTime(rtcnow.year(),rtcnow.month(),rtcnow.day(),hh,mm,ss);
      rtc.adjust(newTime);
      Serial.print('1');
    }

    //#:GR#
    if (serialdata[0]=='#' && serialdata[1]==':' && serialdata[2]=='G' && serialdata[3]=='R' && serialdata[4]=='#') {
      recvd = true;
      serialdata[i]='\0';
      char s[9];
      
      getRTC_Time();
      CalculateLST();
      DateTime LST = lstnow;
      float lst = 0;
      lst  = (float)RA_STEPS / 24.0 * LST.hour();
      lst += (float)RA_STEPS / 1440.0 * LST.minute();
      lst += (float)RA_STEPS / 86400.0 * LST.second();

      long steps = RA_Stepper.getSteps();
      
      if (DEC_Stepper.getSteps() > 0){
        steps = RA_STEPS/4 + steps;
      } else {
        steps = RA_STEPS/4 - steps;
        steps = - steps;
      }

      //RA = LST - HA
      steps = lst - steps;
      
      if (steps<0) steps += RA_STEPS;
      if (steps>RA_STEPS) steps -= RA_STEPS;

      float ra = steps;            
      ra = ra * 24 / (float)RA_STEPS;
      byte hh =   ra;
      byte mm = ( ra - hh) * 60.0;
      byte ss = ((ra - hh) * 60.0 - mm ) * 60.0;
     
      sprintf(s, "%02d:%02d:%02d#", int(hh), int(mm), int(ss));
      Serial.print(s);
    } 

    //#:GD#
    if (serialdata[0]=='#' && serialdata[1]==':' && serialdata[2]=='G' && serialdata[3]=='D' && serialdata[4]=='#') {
      recvd = true;
      serialdata[i]='\0';
      char s[10];
      long steps = DEC_Stepper.getSteps();
      if (steps < 0 ) {
        steps = DEC_STEPS/4 + steps;
      } else {
        steps = DEC_STEPS/4 - steps;
      }
      char sign;
      if (steps < 0) {
        sign = '-';
      } else {
        sign = '+';
      }
      float dec = abs(steps);
      dec = dec * 360 / (float)DEC_STEPS;
      byte hh =    dec;
      byte mm = (  dec - hh ) * 60.0;
      byte ss = (( dec - hh ) * 60.0 - mm ) * 60.0;
      sprintf(s, "%c%02d*%02d:%02d#", sign, int(hh), int(mm), int(ss));
      Serial.print(s);
    }

    //:Sr01:00:00#
    if (serialdata[0]==':' && serialdata[1]=='S' && serialdata[2]=='r' && serialdata[5]==':' && serialdata[8]==':' && serialdata[11]=='#') {
      recvd = true;
      serialdata[i]='\0';

      getRTC_Time();
      CalculateLST();
      DateTime LST = lstnow;
      
      byte hh = atoi(serialdata+3);
      byte mm = atoi(serialdata+6);
      byte ss = atoi(serialdata+9);
      volatile float fsteps;
      fsteps  = (float)RA_STEPS / 24.0 * hh;
      fsteps += (float)RA_STEPS / 1440.0 * mm;
      fsteps += (float)RA_STEPS / 86400.0 * ss;
      //fsteps += (float)RA_STEPS / 8640000.0 * mss;
      long ra_steps = RA_STEPS/4 + fsteps; //TODO fix this
      
      fsteps  = (float)RA_STEPS / 24.0 * LST.hour();
      fsteps += (float)RA_STEPS / 1440.0 * LST.minute();
      fsteps += (float)RA_STEPS / 86400.0 * LST.second();

      //HA = LST - RA
      ra_steps = fsteps - ra_steps;

      ra_steps -= ra_steps % 32;
      if (RA_Stepper.getSteps()<0) {
        ra_steps = ra_steps + RA_Stepper.getSteps();
      } else {
        ra_steps = ra_steps - RA_Stepper.getSteps();
      }

      if (ra_steps<0) {
        if (ra_steps*2 < -RA_STEPS) ra_steps += RA_STEPS/2;
      } else {
        if (ra_steps*2 >  RA_STEPS) ra_steps -= RA_STEPS/2;
      }
      
      if (abs(ra_steps)*2 > RA_STEPS) {
        Serial.print('0');
      } else {
        Serial.print('1');
        stopGuiding();
        RA_Stepper.move(ra_steps);
      }
    }

    //:Sd+89*00:00#
    if (serialdata[0]==':' && serialdata[1]=='S' && serialdata[2]=='d' && serialdata[6]=='*' && serialdata[9]==':' && serialdata[12]=='#') {
      recvd = true;
      serialdata[i]='\0';
      boolean negative = (serialdata[2] == '-');
      byte hh = atoi(serialdata+4);
      byte mm = atoi(serialdata+7);
      byte ss = atoi(serialdata+10);
      volatile float fsteps;
      fsteps  = (float)DEC_STEPS / 360 * hh;
      fsteps += (float)DEC_STEPS / 360 / 60 * mm;
      fsteps += (float)DEC_STEPS / 360 / 60 / 60 * ss;
      //fsteps += (float)DEC_STEPS / 360 / 60 / 60 / 10 * mss;
      long dec_steps = fsteps;
      
      dec_steps += dec_steps % 32;
      if (negative) {
        dec_steps = DEC_STEPS/4 + dec_steps;
      } else {
        dec_steps = DEC_STEPS/4 - dec_steps;
      }
      
      // Das muss von der RA-Richtugn abhängen und umgekehrt.
      if (RA_Stepper.getSteps()>0) dec_steps = -dec_steps;

      // Delta berechnen.
      if (DEC_Stepper.getSteps()<0) {
        dec_steps = dec_steps - DEC_Stepper.getSteps();
      } else {
        dec_steps = dec_steps + DEC_Stepper.getSteps();
      }
      
      if (abs(dec_steps)*3>DEC_STEPS) {
        Serial.print('0');
      } else {
        Serial.print('1');
        DEC_Stepper.move(dec_steps);
      }
    }
    
    if (serialdata[0]==':' && serialdata[1]=='M' && serialdata[2]=='S' && serialdata[3]=='#') {
      recvd = true;
      //:MS# Slew to Target Object 
      //Returns: 0 Slew is Possible 
      //         1<string># Object Below Horizon w/string message 
      //         2<string># Object Below Higher w/string message 
      //startGuiding();
      Serial.print('0');
    }

    if (serialdata[0]=='#' && serialdata[1]==':' && serialdata[2]=='B' && serialdata[3]=='V' && serialdata[4]=='#') {
      recvd = true;
      // 4.8v = 12v 
      // 5v = 1023
      // V = 5/1023*analogValue/4.8*12
      // Spannungsteiler R1 = 220kΩ, R2 = 330kΩ
      float analogValue = analogRead(VOLTAGE_PIN)/81.84;
      //int analogValue = analogRead(VOLTAGE_PIN);
      Serial.println(analogValue);
    }
    
    if (recvd || serialdata[i] == '#' || (i>13)) {
      for(byte n = 0; n< 30-i; n++ ) {
        serialdata[n] = serialdata[i+n];
      }
      recvd = false;
      i=0;
    }
  } else {
    Serial.read();
  }
}
