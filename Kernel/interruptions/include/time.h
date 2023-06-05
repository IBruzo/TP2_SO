#ifndef _TIME_H_
#define _TIME_H_

#include <interrupts.h>

void timer_handler();
int ticks_elapsed();
int seconds_elapsed();

#endif
