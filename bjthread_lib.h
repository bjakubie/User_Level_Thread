#ifndef __BJTHREAD_LIB_H__
#define __BJTHREAD_LIB_H__

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/time.h>
#include <ucontext.h>
#include <malloc.h>
#include <stdbool.h>
#include <unistd.h>


#define PRIORITY_MAX 1
#define PRIORITY_MIN 10
#define CONT_SIZE 8192

typedef struct __bjthread_t bjthread_t;

enum state
{
	STATE_RUNNING,
	STATE_READY,
	STATE_WAITING,
	STATE_START,
	STATE_DONE
};

struct __bjthread_t
{
	int tid;
	int state;
	int priority;
	ucontext_t *context;
	bjthread_t *prev_node;
	bjthread_t *next_node;
};






#endif

