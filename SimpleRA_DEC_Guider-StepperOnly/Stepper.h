/*
I don't know how to split this into .h and .cpp.
*/
#define MIN_YIELD_MICROS 100

class StepperDriver {

  public:
      static inline void delayMicros(unsigned long delay_us, unsigned long start_us = 0){
        if (delay_us){
            if (!start_us){
                start_us = micros();
            }
            if (delay_us > MIN_YIELD_MICROS){
                yield();
            }
            // See https://www.gammon.com.au/millis
            while (micros() - start_us < delay_us);
        }
    }

  private:
      unsigned long last_action_end = 0;
      unsigned long next_action_interval = 0;
      unsigned long stepper_freqency = 0;
      long steps = 0;
      boolean enabled = false;
      boolean dir = false;
      unsigned long stepper_interval = 0;
      short mode;
      short current_mode;
      boolean slowdown = false;
      boolean accellerate = false;
    
  protected:
      short step_pin;
      short dir_pin;
      short enable_pin;
      short m0_pin;
      short m1_pin;
      short m2_pin;
      
  public:
    StepperDriver::StepperDriver(short step_pin, short dir_pin, short enable_pin, short m0_pin, short m1_pin, short m2_pin)
    :step_pin(step_pin), dir_pin(dir_pin), enable_pin(enable_pin), m0_pin(m0_pin), m1_pin(m1_pin), m2_pin(m2_pin) {
    }

    void setup() {
      pinMode(step_pin,OUTPUT);
      pinMode(dir_pin,OUTPUT);
      pinMode(enable_pin,OUTPUT);
      pinMode(m0_pin,OUTPUT);
      pinMode(m1_pin,OUTPUT);
      pinMode(m2_pin,OUTPUT);
      digitalWrite(step_pin, LOW);
      digitalWrite(dir_pin, LOW);
      digitalWrite(enable_pin, HIGH);
      digitalWrite(m0_pin, HIGH);
      digitalWrite(m1_pin, HIGH);
      digitalWrite(m2_pin, HIGH);
    }

    boolean actionImminent() {
      return ((accellerate || slowdown) && (delayRemaining()<25));
    }

    unsigned long StepperDriver::delayRemaining() {
      long m = 0;
      if (last_action_end>0) {
         m = ((stepper_interval + last_action_end) - micros());
      }
      if (m<0) m=0;
      return m;
    }

    boolean StepperDriver::getEnabled() {
      return enabled;
    }

    long StepperDriver::getSteps() {
      return steps;
    }
  
    void StepperDriver::setDirection(boolean d) {
      dir = d;
    }

    void StepperDriver::speedup() {
      accellerate = true;
      slowdown = false;
    }
    
    boolean StepperDriver::breaks() {
      if (stepper_freqency > RA_MICROSTEPS) {
        accellerate = false;
        slowdown = true;
      }
      return (stepper_freqency == 0);
    }

    short StepperDriver::calcuateMode(unsigned long f) {
      if (f <= RA_DRIVER_MAX_FREQUENCY) {
        mode=32;
      } else if ((f > (RA_DRIVER_MAX_FREQUENCY       )) && ((f <= RA_DRIVER_MAX_FREQUENCY *  2UL))) {
        mode=16;
      } else if ((f > (RA_DRIVER_MAX_FREQUENCY *  2UL)) && ((f <= RA_DRIVER_MAX_FREQUENCY *  4UL))) {
        mode=8;
      } else if ((f > (RA_DRIVER_MAX_FREQUENCY *  4UL)) && ((f <= RA_DRIVER_MAX_FREQUENCY *  8UL))) {
        mode=4;
      } else if ((f > (RA_DRIVER_MAX_FREQUENCY *  8UL)) && ((f <= RA_DRIVER_MAX_FREQUENCY * 16UL))) {
        mode=2;
      } else if ((f > (RA_DRIVER_MAX_FREQUENCY * 16UL)) && ((f <= RA_DRIVER_MAX_FREQUENCY * 32UL))) {
        mode=1;
      }
      return mode;
    }

    void StepperDriver::configureMode() {
      if (current_mode != mode) { 
        switch (mode) {
          case 32: // 1/32 step
          digitalWrite(m0_pin, HIGH);
          digitalWrite(m1_pin, LOW);
          digitalWrite(m2_pin, HIGH);
          break;
          case 16: // 1/16 step
          digitalWrite(m0_pin, LOW);
          digitalWrite(m1_pin, LOW);
          digitalWrite(m2_pin, HIGH);
          break;
          case 8: // 1/8 step
          digitalWrite(m0_pin, HIGH);
          digitalWrite(m1_pin, HIGH);
          digitalWrite(m2_pin, LOW);
          break;
          case 4: // 1/4 step
          digitalWrite(m0_pin, LOW);
          digitalWrite(m1_pin, HIGH);
          digitalWrite(m2_pin, LOW);
          break;
          case 2: // 1/2 step
          digitalWrite(m0_pin, HIGH);
          digitalWrite(m1_pin, LOW);
          digitalWrite(m2_pin, LOW);
          break;
          case 1: // full step
          digitalWrite(m0_pin, LOW);
          digitalWrite(m1_pin, LOW);
          digitalWrite(m2_pin, LOW);
          break;
        }
        current_mode = mode;
      }
    }
  
    long StepperDriver::nextAction() {
      if (accellerate || slowdown) {
        configureMode();
    
        if (!enabled) {
          digitalWrite(enable_pin, LOW);
          digitalWrite(dir_pin, dir);
          enabled = true;
        }
        
        if (dir) {steps = steps + RA_MICROSTEPS/mode;} else {steps = steps - RA_MICROSTEPS/mode;};
        
        delayMicros(next_action_interval, last_action_end);
        digitalWrite(step_pin, HIGH);
        unsigned m = micros();
        
        unsigned long pulse = stepper_interval;
        // TODO: Die Geschwindigkeit muss abhängig von der Zeit verändert werdcen. Nicht abhängig
        // von den Steps. Oder es muss beücksichtigt werden, dass die Zeit zwischen den Steps sich verändert.
        unsigned long deltaf = (RA_ACCELERATION * stepper_freqency / (1000000 * (RA_MICROSTEPS/mode)));
        if (deltaf <1 ) deltaf = 1;
        if (!slowdown) {
          stepper_freqency = stepper_freqency + deltaf;
        } else {
          stepper_freqency = stepper_freqency - deltaf;
        }
  
        if (stepper_freqency > RA_MAX_FREQUENCY) stepper_freqency = RA_MAX_FREQUENCY;
        if (stepper_freqency < RA_MICROSTEPS) stepper_freqency = RA_MICROSTEPS;
        if (slowdown &&  stepper_freqency == RA_MICROSTEPS && (steps % RA_MICROSTEPS == 0)) {
          slowdown = false;
          stepper_freqency = 0;
        }
  
        // TODO: Minimal akzeptables stepper_interval befor Mode-Switch berechnen! Das hängt von dem Modus ab, in dem wir uns bereits befinden. 
        // Theoretisch müsste der Strom am Motor abhängig von der Geschwindigkeit immer weiter erhöhrt werden, um höhere Geschwindigkeiten zu erreichen.
        stepper_interval = 1000000 * (RA_MICROSTEPS/mode) / stepper_freqency;
        calcuateMode(stepper_freqency);
        
        delayMicros(2);
        digitalWrite(step_pin, LOW);
        
        last_action_end = micros();
        m = last_action_end - m;
        next_action_interval = (pulse > m) ? pulse - m : 1;
        //return next_action_interval;
        return stepper_interval;
      }
    }
  
    long StepperDriver::disable(void) {
      if (enabled) {
        digitalWrite(enable_pin, HIGH);
        digitalWrite(LED1, LOW);
        enabled = false;
      }
      mode = RA_MICROSTEPS;
      stepper_freqency = 0;
      stepper_interval = 0;
      next_action_interval = 0;
      last_action_end = 0;
    }
};
