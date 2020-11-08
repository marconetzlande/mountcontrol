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
  boolean go_home = (dec_plus && dec_minus && ra_plus && ra_minus);

  if (!go_home) {
    if ((ra_plus && ra_minus) && (!dec_plus || dec_minus)) {
      startGuiding();
      delay(1000);
    } else if (ra_control_pressed) {
      stopGuiding();
      RA_Stepper.setDirection(ra_plus);
      RA_Stepper.speedup();
    } else {
      if (RA_Stepper.breaks()) {
        if (!isGuiding()) RA_Stepper.disable();
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
  }
  
  static boolean last_go_home = false;
  if (go_home && !last_go_home) {
    last_go_home = go_home;
    while (!RA_Stepper.breaks()||!DEC_Stepper.breaks()) {
      RA_Stepper.nextAction();
      DEC_Stepper.nextAction();
    }
    RA_Stepper.disable();
    DEC_Stepper.disable();
    delay(1000);
    RA_Stepper.move(-RA_Stepper.getSteps());
    DEC_Stepper.move(-DEC_Stepper.getSteps());
  } else {
    last_go_home = LOW;
  }
  
  if (RA_Stepper.actionImminent()) RA_Stepper.nextAction();
  if (DEC_Stepper.actionImminent()) DEC_Stepper.nextAction();
  if (Serial.available() > 0) handleSerial();
}
