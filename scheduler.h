#ifndef __SCHEDULER_H__
#define __SCHEDULER_H__

#include "bjthread_lib.h"

struct queue
{
	int count_nodes;
	bjthread_t *first_node;
	bjthread_t *last_node;
} scheduler_queue;


bjthread_t *findPlaceInQueue(int priority);

void setAsPrev(bjthread_t *bjthread, bjthread_t *wsk_thread);

void setAsLastInQueue(bjthread_t *bjthread, bjthread_t *last_node);

void removeFromQueue(bjthread_t *bjthread);

void addThreadToQueue(bjthread_t *bjthread);

void shuffleSamePriorityThreads(bjthread_t *bjthread);

bjthread_t *getNodeByStateWithMaxPriority(enum state st);

bjthread_t *getRunningNode();

void schedule();

void initSignals();

void blockSignals();

void unblockSignals();

void runTimer();

void stopTimer();

#endif
