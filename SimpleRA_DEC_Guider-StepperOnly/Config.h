//Pins
//Stepper Driver DRV8825 for RA
#define RA_STEPPER_DIR      12
#define RA_STEPPER_STEP     10
#define RA_STEPPER_ENABLE   11
#define RA_STEPPER_MODE0     4
#define RA_STEPPER_MODE1    22
#define RA_STEPPER_MODE2    16

//Stepper Driver DRV8825 for DEC
#define DEC_STEPPER_DIR      6
#define DEC_STEPPER_STEP     8
#define DEC_STEPPER_ENABLE   9
#define DEC_STEPPER_MODE0    5
#define DEC_STEPPER_MODE1   14
#define DEC_STEPPER_MODE2   15

//Remote Control buttons
#define DEC_PLUS            18
#define RA_PLUS             19
#define DEC_MINUS           20
#define RA_MINUS            21

#define LED1                13
#define RTC_SQW              7
//#define GO_HOME_PIM         17
#define WireSDA              2
#define WireSLC              3
#define VOLTAGE_PIN         23 // A5

#define MIN_P                           500 //minimum time between pulses in μs
#define RA_MICROSTEPS                  32UL
#define RA_MAX_FREQUENCY            12800UL
#define RA_ACCELERATION              2048UL

/* currently not used
#define DEC_MAX_FREQUENCY            6400UL
#define DEC_ACCELERATION              256UL
#define DEC_MICROSTEPS                 32UL
*/

#define RA_STEPS                  3308800UL
#define DEC_STEPS                 2112000UL

//messung:
//RA: 360° = 3.308.800 steps / 6400 = 517 fullsteps
//DEC: 360° = 2.112.000 steps / 6400 = 330 fullsteps
//1 step = 0,00010879866927138740063054873213284 ° = 0,39167520937699464226997543567823″
//1 step = 0,02611168062513297615133169571188s = 38,297037037037037037037037037037Hz
//858,44761223187172395118082822383 oszillation per step
//32KHz ~~ 858,44761223187172395118082822383 counts for one step on RTC SQW with 32.876KHz
#define RA_DIVISOR   858.44761223187172395118082822383
