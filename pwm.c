#include "pwm.h"

// #define PIN             28
// #define RANGE           1000
// #define VALUE           600


static volatile int OutputOn;
static volatile int PwmRange;
static volatile pthread_t globalThread;

static void *PwmThread (void *arg)
{
  int pin, On, space ;
  struct sched_param param ;

  param.sched_priority = sched_get_priority_max(SCHED_RR) ;
  pthread_setschedparam(pthread_self (), SCHED_RR, &param) ;

  pin = *((int *)arg) ;

  piHiPri(90);

  while(1)
  {
    On = OutputOn;
    space = PwmRange - On ;

    if (On != 0)
      digitalWrite (pin, HIGH) ;
    delayMicroseconds (On * 100) ;

    if (space != 0)
      digitalWrite (pin, LOW) ;
    delayMicroseconds (space * 100) ;
  }

  return NULL ;
}
int PwmCreate (int pin, int init_value, int range)
{
  int res ;
  pthread_t pwmThread ;
  int *passPin ;

  if (pin >= MAX_PINS)
    return -1 ;

  if (range <= 0)
    return -1 ;
  //Init pin for output
  digitalWrite (pin, LOW) ;
  pinMode      (pin, OUTPUT) ;
  OutputOn = init_value ;
  PwmRange = range ;
  *passPin = pin ;
  res = pthread_create(&pwmThread, NULL, PwmThread, (void *)passPin) ;
  globalThread = pwmThread;
  return res ;
}


void PwmWriteValue(int pin, int value)
{
  if (pin < MAX_PINS)
  {
    if (value < 0)
      value = 0 ;
    else if (value > PwmRange)
      value = PwmRange;

    OutputOn = value ;
  }
}

void PwmStop(int pin)
{
  if (pin < MAX_PINS)
  {
    if (PwmRange != 0)
    {
      pthread_cancel(globalThread) ;
      pthread_join(globalThread, NULL) ;
      PwmRange = 0 ;
      digitalWrite (pin, LOW) ;
    }
  }
}