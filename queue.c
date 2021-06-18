/*
 * DO NOT TOUCH THIS FILE!
 * 
 * Implementation of a single queue
 * Warning: Not thread-safe at all!
 */

#include "queue.h"

TAILQ_HEAD(tailhead, entry) head;

void queue_init()
{
  TAILQ_INIT(&head);
}

void queue_push_back(Waiting waiting)
{
  struct entry *elem;
  elem = malloc(sizeof(struct entry));
  if (elem)
    elem->waiting = waiting;

  TAILQ_INSERT_TAIL(&head, elem, entries);
}

bool queue_pop_front(Waiting* waiting)
{
  struct entry *elem = head.tqh_first;
  if(!elem)
    return false;
  TAILQ_REMOVE(&head, head.tqh_first, entries);
  *waiting = elem->waiting;
  free(elem);
  return true;
}
