/*
 * DO NOT TOUCH THIS FILE!
 */

#ifndef A2_DEFINITIONS_H
#define A2_DEFINITIONS_H

#include <pthread.h>
#include <stdint.h>
#include <stdlib.h>

#define NUM_RECEPTIONIST_STAFF 2    // You can change this number as you will

void moveIntoHospital()
{ 
  nanosleep((const struct timespec[]) {{0, 1000L*1000L*10L}}, NULL);
};

void fillOutTheRegistrationForm()
{ 
  nanosleep((const struct timespec[]) {{0, 1000L*1000L*50L}}, NULL);
};

void takeAShortBreak() 
{ 
  nanosleep((const struct timespec[]) {{0, 1000L*1000L*100L}}, NULL);
};

void findIllnessAndTreadIt() 
{ 
  nanosleep((const struct timespec[]) {{0, 1000L*1000L*20L}}, NULL);
};

#endif //A2_DEFINITIONS_H
