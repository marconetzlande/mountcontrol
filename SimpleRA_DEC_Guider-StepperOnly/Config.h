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
#define RA_MAX_FREQUENCY            16896UL
#define RA_ACCELERATION              6144UL

/* currently not used
#define DEC_MAX_FREQUENCY            6400UL
#define DEC_ACCELERATION              256UL
#define DEC_MICROSTEPS                 32UL
*/

#define RA_STEPS                  3308800UL
#define DEC_STEPS                 2117440UL

/**
 * DEC Achse nachmessung 
 *  2.120.987 /1 = 2.120.987
 *  4.242.040 /2 = 2.121.020
 *  6.361.153 /3 = 2.120.384,3333333333333333333333333
 *  8.486.365 /4 = 2.121.591,25
 * 10.593.300 /5 = 2.118.660
 * 12.714.207 /6 = 2.119.034,5
 * 14.829.756 /7 = 2.118.536,5714285714285714285714286
 * 16.941.668 /8 = 2.117.708,5
 * 19.057.116 /9 = 2.117.457,3333333333333333333333
 * 
 **/

//messung:
//RA: 360° = 3.308.800 steps / 6400 = 517 fullsteps
//1 step = 0,00010879866927138740063054873213284 ° = 0,39167520937699464226997543567823″
//1 step = 0,02611168062513297615133169571188s = 38,297037037037037037037037037037Hz
//858,44761223187172395118082822383 oszillation per step
//32KHz ~~ 858,44761223187172395118082822383 counts for one step on RTC SQW with 32.876KHz
#define RA_DIVISOR   858.44761223187172395118082822383
