char serialdata[30];

void handleSerial() {
  static byte i = 0;
  static unsigned long last_packet;
  static boolean recvd = false;
  last_packet = millis();

  if ((i==0 && (Serial.peek() == '#'||Serial.peek() == ':')) || i>0) {
    serialdata[i++] = Serial.read();
    if (serialdata[0]=='#' && serialdata[1]==':' && serialdata[2]=='Q' && serialdata[3]=='#') {
      recvd = true;
      serialdata[i]='\0';
    }
    if (serialdata[0]=='#' && serialdata[1]==':' && serialdata[2]=='G' && serialdata[3]=='R' && serialdata[4]=='#') {
      recvd = true;
      serialdata[i]='\0';
      char s[9];
      float ra = abs(RA_Stepper.getSteps());
      ra = ra * 24 / (float)RA_STEPS;
      byte hh =   ra;
      byte mm = ( ra - hh) * 60.0;
      byte ss = ((ra - hh) * 60.0 - mm ) * 60.0;
      sprintf(s, "%02d:%02d:%02d#", int(hh), int(mm), int(ss));
      Serial.print(s);
    } 
    if (serialdata[0]=='#' && serialdata[1]==':' && serialdata[2]=='G' && serialdata[3]=='D' && serialdata[4]=='#') {
      recvd = true;
      serialdata[i]='\0';
      char s[10];
      float dec = abs(DEC_Stepper.getSteps() + DEC_STEPS/4);
      dec = dec * 360 / (float)DEC_STEPS;
      byte hh =    dec;
      byte mm = (  dec - hh ) * 60.0;
      byte ss = (( dec - hh ) * 60.0 - mm ) * 60.0;
      sprintf(s, "%+03d*%02d:%02d#", int(hh), int(mm), int(ss));
      Serial.print(s);
    }
    if (serialdata[0]==':' && serialdata[1]=='S' && serialdata[2]=='r' && serialdata[5]==':' && serialdata[8]==':' && serialdata[11]=='#') {
      recvd = true;
      serialdata[i]='\0';
      //:Sr01:00:00#
      byte hh = atoi(serialdata+3);
      byte mm = atoi(serialdata+6);
      byte ss = atoi(serialdata+9);
      volatile float fsteps;
      fsteps  = (float)RA_STEPS / 24.0 * hh;
      fsteps += (float)RA_STEPS / 1440.0 * mm;
      fsteps += (float)RA_STEPS / 86400.0 * ss;
      //fsteps += (float)RA_STEPS / 8640000.0 * mss;
      long ra_steps = fsteps;
      ra_steps -= ra_steps % 32;
      if (RA_Stepper.getSteps()<0) {
        ra_steps = ra_steps + RA_Stepper.getSteps();
      } else {
        ra_steps = ra_steps - RA_Stepper.getSteps();
      }
      if (abs(ra_steps)*3>RA_STEPS) {
        Serial.print('0');
      } else {
        Serial.print('1');
        RA_Stepper.move(ra_steps);
      }
    }
    if (serialdata[0]==':' && serialdata[1]=='S' && serialdata[2]=='d' && serialdata[6]=='*' && serialdata[9]==':' && serialdata[12]=='#') {
      recvd = true;
      serialdata[i]='\0';
      //:Sd+89*00:00#
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
      
      //if (dec_steps>0 and RA_Stepper.getSteps()>0)
      
      dec_steps += dec_steps % 32;
      if (negative) {
        dec_steps = DEC_STEPS/4 + dec_steps;
      } else {
        dec_steps = DEC_STEPS/4 - dec_steps;
      }
      dec_steps = -dec_steps;

      if (DEC_Stepper.getSteps()<0) {
        dec_steps = dec_steps - DEC_Stepper.getSteps();
      }else {
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
      Serial.print('0');
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
