/*
 * DO NOT TOUCH THIS FILE!
 */

#ifndef A2_QUEUE_H
#define A2_QUEUE_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/queue.h>

#include "resources.h"

typedef struct
{
  Patient* patient;
} Waiting;

struct entry
{
  Waiting waiting;
  TAILQ_ENTRY(entry) entries;
};

void queue_init();
void queue_push_back(Waiting waiting);
bool queue_pop_front(Waiting* waiting);

#endif
