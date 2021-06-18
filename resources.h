#ifndef A2_RESOURCES_H
#define A2_RESOURCES_H

#include <pthread.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct
{
  pthread_t tid;
  size_t id;
  size_t was_treated;

  pthread_mutex_t patient_lock;
  pthread_cond_t patient_is_waiting;
} Patient;

typedef struct
{
  pthread_t tid;
  size_t id;
} Doctor;

#endif
