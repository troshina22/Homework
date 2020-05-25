#ifndef __PWM_H
#define __PWM_H

#include <stdlib.h>
#include <stdio.h>
#include <wiringPi.h>
#include <pthread.h>

#define MAX_PINS        40

extern int  PwmCreate(int pin, int init_value, int range) ;
extern void PwmWriteValue(int pin, int value) ;
extern void PwmStop(int pin) ;

#endif

