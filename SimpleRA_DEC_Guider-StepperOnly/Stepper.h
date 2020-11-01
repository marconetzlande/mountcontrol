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
      unsigned long stepper_interval = 0;
      
      long steps = 0;
      long steps_to_go = 0;
      long steps_remaining = 0;
      long steps_done = 0;
      
      boolean enabled = false;
      boolean dir = false;
      short mode;
      short current_mode;
      unsigned long steps_accelerate;
      unsigned long steps_decelerate;
      boolean slowdown = false;
      boolean accelerate = false;
      boolean gotomode = false;
    
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

    void StepperDriver::setup() {
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

    boolean StepperDriver::actionImminent() {
      return ((accelerate || slowdown || gotomode) && (delayRemaining()<25));
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
      accelerate = true;
      slowdown = false;
    }
    
    boolean StepperDriver::breaks() {
      if (stepper_freqency > RA_MICROSTEPS) {
        accelerate = false;
        slowdown = true;
      }
      return (stepper_freqency == 0);
    }

    byte StepperDriver::calculateMode(float p) {
      #define MIN_P 250
      byte modes[] = {32,16,8,4,2,1}; //depending on microstepping, we choose 32
      byte n=0;
      while ((p * mode / modes[n]) < MIN_P) n++;
      mode = modes[n];
      return modes[n];
    }

    short StepperDriver::calculateMode(unsigned long f) {
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

    void StepperDriver::move(long s) {
      if (s!=0)
      if (steps_to_go == 0) {
        dir = (s>=0);
        //s = 0,5 * a * t*t
        //s = 0,5 * (v²/a)
        //t = sqrt(2 * s /a)
        steps_accelerate = RA_MAX_FREQUENCY * RA_MAX_FREQUENCY / (2 * RA_ACCELERATION);
        if (steps_accelerate*2 > s) steps_accelerate = s/2;
        steps_decelerate = RA_MAX_FREQUENCY * RA_MAX_FREQUENCY / (2 * RA_ACCELERATION);
        if (steps_decelerate*2 > s) steps_decelerate = s/2;
        steps_to_go = s;
        steps_remaining = abs(s);
        steps_done = 0;
        gotomode = true;
      }
    }
  
    long StepperDriver::nextAction() {
      if (steps_remaining > 0) {
        if (gotomode) {
          //nach der hälfte der steps abbremsen
          if (steps_done < steps_remaining) {
            accelerate = true;
            slowdown = false;
          } else if (steps_done > steps_remaining) {
            accelerate = false;
            slowdown = true;
          } else {
            accelerate = false;
            slowdown = false;
          }
        }
      } else {
        if (steps_to_go != 0) {
          steps_remaining = 0;
          steps_to_go = 0;
          gotomode = false;
          slowdown = false;
          accelerate = false;
        }
      }

      if (accelerate || slowdown || gotomode) {
        configureMode();
    
        if (!enabled) {
          digitalWrite(enable_pin, LOW);
          digitalWrite(dir_pin, dir);
          enabled = true;
        }
       
        delayMicros(next_action_interval, last_action_end);
        digitalWrite(step_pin, HIGH);
        unsigned m = micros();

        if (dir) {steps += RA_MICROSTEPS/mode;} else {steps -= RA_MICROSTEPS/mode;};
        steps_remaining -= RA_MICROSTEPS/mode;
        steps_done += RA_MICROSTEPS/mode;

        unsigned long last_stepper_interval = stepper_interval;
        // TODO: Die Geschwindigkeit muss abhängig von der Zeit verändert werdcen. Nicht abhängig
        // von den Steps. Oder es muss beücksichtigt werden, dass die Zeit zwischen den Steps sich verändert.
        if (accelerate xor slowdown) {
          //V = A * T; V = deltaS/deltaT; T=sqrt(S*2/A)
          //deltaT = deltaS / (A * T);
          //sqrt(RA_ACCELERATION * (abs(steps_to_go) - steps_remaining)*(abs(steps_to_go) - steps_remaining)) = (RA_MICROSTEPS/mode)/stepper_interval

          unsigned long deltaf = (RA_ACCELERATION * stepper_freqency / (1000000UL * (RA_MICROSTEPS/mode)));
          if (deltaf <1 ) deltaf = 1;
          if (accelerate) {
            if (!gotomode) {
              stepper_freqency = stepper_freqency + deltaf;
              stepper_interval = 1000000UL * (RA_MICROSTEPS/mode) / stepper_freqency;
            } else {
              stepper_freqency = sqrt(2 * (steps_done+1) * RA_ACCELERATION * (RA_MICROSTEPS/mode) * (RA_MICROSTEPS/mode));
              stepper_interval = 1000000UL / stepper_freqency;
            }            
          } else if (slowdown) {
            if (!gotomode) {
              stepper_freqency = stepper_freqency - deltaf;
              stepper_interval = 1000000UL * (RA_MICROSTEPS/mode) / stepper_freqency;
            } else{
              stepper_freqency = sqrt(2 * (steps_remaining-1) * RA_ACCELERATION * (RA_MICROSTEPS/mode) * (RA_MICROSTEPS/mode));
              stepper_interval = 1000000UL / stepper_freqency;
            }
          }
        }
        
        if (!gotomode) {
          if (stepper_freqency > RA_MAX_FREQUENCY) stepper_freqency = RA_MAX_FREQUENCY;
          if (stepper_freqency < RA_MICROSTEPS) stepper_freqency = RA_MICROSTEPS;
          if (slowdown &&  stepper_freqency == RA_MICROSTEPS && (steps % RA_MICROSTEPS == 0)) {
            slowdown = false;
            stepper_freqency = 0;
          }
        }
        if (gotomode && steps_remaining<=0) {
          slowdown = false;
          stepper_freqency = 0;
          stepper_interval = 0;
        }
        calculateMode((float)stepper_interval);
        delayMicros(2,m);
        digitalWrite(step_pin, LOW);
        
        last_action_end = micros();
        m = last_action_end - m;
        next_action_interval = (last_stepper_interval > m) ? last_stepper_interval - m : 1;
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
