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
  
  if (RA_Stepper.actionImminent()) RA_Stepper.nextAction();
  if (DEC_Stepper.actionImminent()) DEC_Stepper.nextAction();
}