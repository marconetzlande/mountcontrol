//Pins
#define RA_STEPPER_DIR      12
#define RA_STEPPER_STEP     10
#define RA_STEPPER_ENABLE   11
#define RA_STEPPER_MODE0     4
#define RA_STEPPER_MODE1    A4
#define RA_STEPPER_MODE2    A5
#define RA_PLUS             A1
#define RA_MINUS            A3

#define DEC_STEPPER_DIR      6
#define DEC_STEPPER_STEP     8
#define DEC_STEPPER_ENABLE   9
#define DEC_STEPPER_MODE0    5
#define DEC_STEPPER_MODE1   14
#define DEC_STEPPER_MODE2   15
#define DEC_PLUS            A0
#define DEC_MINUS           A2

#define LED1                13
#define RTC_SQW              7
//#define Wire                 2
//#define Wire                 3

#define RA_DRIVER_MAX_FREQUENCY       640UL
#define RA_MAX_FREQUENCY            32000UL
#define RA_ACCELERATION               256UL
#define RA_MICROSTEPS                  32UL

#define DEC_DRIVER_MAX_FREQUENCY      400UL
#define DEC_MAX_FREQUENCY           32000UL
#define DEC_ACCELERATION              256UL
#define DEC_MICROSTEPS                 32UL

//messung:
//360° = 3.308.864 steps
//1 step = 0,00010879866927138740063054873213284 ° = 0,39167520937699464226997543567823″
//1 step = 0,02611168062513297615133169571188s = 38,297037037037037037037037037037Hz
//858,44761223187172395118082822383 oszillation per step
//32KHz ~~ 858,44761223187172395118082822383 counts for one step on RTC SQW with 32.876KHz
#define RA_DIVISOR   858.44761223187172395118082822383
