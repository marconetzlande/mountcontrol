#include "Config.h"
#include "Stepper.h"

StepperDriver RA_Stepper(RA_STEPPER_STEP, RA_STEPPER_DIR, RA_STEPPER_ENABLE, RA_STEPPER_MODE0, RA_STEPPER_MODE1, RA_STEPPER_MODE2);
StepperDriver DEC_Stepper(DEC_STEPPER_STEP, DEC_STEPPER_DIR, DEC_STEPPER_ENABLE, DEC_STEPPER_MODE0, DEC_STEPPER_MODE1, DEC_STEPPER_MODE2);
    
void loop () {
  boolean dec_plus  = !digitalRead(DEC_PLUS);
  boolean dec_minus = !digitalRead(DEC_MINUS);
  boolean ra_plus   = !digitalRead(RA_PLUS);
  boolean ra_minus  = !digitalRead(RA_MINUS);

  boolean dec_control_pressed = (dec_plus xor dec_minus);
  boolean ra_control_pressed = (ra_plus xor ra_minus);

  if (ra_control_pressed) {
    RA_Stepper.setDirection(ra_plus);
    RA_Stepper.speedup();
  } else {
    if (RA_Stepper.breaks()) {
      RA_Stepper.disable();
    }
  }

  if (dec_control_pressed) {
    DEC_Stepper.setDirection(dec_plus);
     DEC_Stepper.speedup();
  } else {
    if (DEC_Stepper.breaks()) {
       DEC_Stepper.disable();
    }
  }
  
  static boolean last_r = LOW;
  boolean r = !digitalRead(7);
  if (r && !last_r) {
    last_r = r;
    Serial.println(-RA_Stepper.getSteps());
    Serial.println(-DEC_Stepper.getSteps());
    RA_Stepper.move(-RA_Stepper.getSteps());
    DEC_Stepper.move(-DEC_Stepper.getSteps());
    delay(100);
  } else {
    last_r = LOW;
  }
  
  if (RA_Stepper.actionImminent()) RA_Stepper.nextAction();
  if (DEC_Stepper.actionImminent()) DEC_Stepper.nextAction();

  if (Serial.available() > 0) {
      char command = Serial.read();
      switch (command) {
        /*
        case 'G':
          Serial.println("Starting RA guider");
          stopGoto();
          startGuiding();
        break;
        case 'D': if (true) {
            volatile String s;
            s = Serial.readStringUntil('#');
            ra_divisor = s.toInt();
            delay(250);
          }
        break;          
        */
        case 'O': if (true) {
            volatile String s;
            s = Serial.readStringUntil(',');
            volatile long r = s.toInt();
            s = Serial.readStringUntil('#');
            volatile long d = s.toInt();
            Serial.print("Movin: dRA:");
            Serial.print(r);
            Serial.print(", dDEC:");
            Serial.print(d);
            Serial.println();
            delay(250);
            RA_Stepper.move(r);
            DEC_Stepper.move(d);
          }
        break;
        case 'F': if (true) {
            volatile String s;
            volatile float fsteps;
            volatile byte b;
            s = Serial.readStringUntil('h');
            b = s.toInt();
            fsteps = (float)RA_STEPS / 24.0 * b;
            s = Serial.readStringUntil('m');
            b = s.toInt();
            fsteps += (float)RA_STEPS / 1440.0 * b;
            s = Serial.readStringUntil('.');
            b = s.toInt();
            fsteps += (float)RA_STEPS / 86400.0 * b;
            s = Serial.readStringUntil('s');
            b = s.toInt();
            fsteps += (float)RA_STEPS / 8640000.0 * b;
            long ra_steps = fsteps;
            ra_steps -= ra_steps % 32;
            
            Serial.readStringUntil('/');
            b = Serial.peek();
            if (b == char('-') || b == char('+')) {
              Serial.read();
            }
            boolean negative = (b == char('-'));
            s = Serial.readStringUntil('g');
            b = s.toInt();
            fsteps = (float)DEC_STEPS / 360 * b;
            s = Serial.readStringUntil('\'');
            b = s.toInt();
            fsteps -= (float)DEC_STEPS / 360 / 60 * b;
            s = Serial.readStringUntil('.');
            b = s.toInt();
            fsteps -= (float)DEC_STEPS / 360 / 60 / 60 * b;
            s = Serial.readStringUntil('"');
            b = s.toInt();
            fsteps -= (float)DEC_STEPS / 360 / 60 / 60 / 10 * b;
            long dec_steps = fsteps;
            dec_steps -= dec_steps % 32;
            if (negative) dec_steps = -dec_steps;
            dec_steps = DEC_STEPS/4 - dec_steps;

            Serial.println("--------");
            
            //Serial.println(ra_steps);
            //Serial.println(dec_steps);

            if (ra_steps * 2 > RA_STEPS) if ((ra_steps - RA_STEPS/2)*4 < RA_STEPS) ra_steps = ra_steps - RA_STEPS/2;
            if (ra_steps * 4 > RA_STEPS) ra_steps = RA_STEPS - ra_steps;

            //if (dec_steps > DEC_STEPS) dec_steps -= DEC_STEPS;

            Serial.println(ra_steps-RA_Stepper.getSteps());
            Serial.println(dec_steps-DEC_Stepper.getSteps());

            if (abs(ra_steps)*4>RA_STEPS) {
              Serial.println("RA out of range");
            } else if (abs(dec_steps)>DEC_STEPS) {
              Serial.println("DEC out of range");
            } else {
              RA_Stepper.move(ra_steps-RA_Stepper.getSteps());
              DEC_Stepper.move(dec_steps-DEC_Stepper.getSteps());
            }
        }
        break;
        case 'Z':
          Serial.println("Going to home position");
          Serial.println(-RA_Stepper.getSteps());
          Serial.println(-DEC_Stepper.getSteps());
          delay(100);
          RA_Stepper.move(-RA_Stepper.getSteps());
          DEC_Stepper.move(-DEC_Stepper.getSteps());
          break;
        /*
        case 'S':
          Serial.println("Steps to do");
          Serial.println(goto_steps[stRA]);
          Serial.println(goto_steps[stDEC]);
          delay(250);
        break;
        */
        case 'P':
          Serial.println("Current position");
          Serial.println(RA_Stepper.getSteps());
          Serial.println(DEC_Stepper.getSteps());
          delay(250);
        break;
        case 'V':
          //+12v---330kΩ---+---100kΩ---GND
          //2.79V at 12v input.
          Serial.println((float)analogRead(VOLTAGE_PIN)/1023*21.5);
          delay(250);
        break;
        case 'X':
          Serial.println("Stopping everything");
          delay(250);
        break;
      }
      while (Serial.available()) Serial.read();
   }
}
