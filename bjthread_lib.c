#include "bjthread_lib.h"
#include "scheduler.h"

int tid_counter = 0;
bool init_bjthread_flag = false;
extern bjthread_t *current_thread;


static ucontext_t *prepareContext(void (*func)(void), void *args)
{
	ucontext_t *ucontext = (ucontext_t *) malloc(sizeof(ucontext_t));
	getcontext(ucontext);

	ucontext->uc_link 		= 0;
	ucontext->uc_stack.ss_sp 	= (char *) malloc(CONT_SIZE);
	ucontext->uc_stack.ss_size 	= CONT_SIZE;
	ucontext->uc_stack.ss_flags	= 0;

	if (!args) makecontext(ucontext, func, 0);
	else makecontext(ucontext, func, 1, args);

	return ucontext;
}


static bjthread_t *prepareBJThread(ucontext_t *ucontext, int priority)
{
	bjthread_t *bjthread	= (bjthread_t *) malloc(sizeof(bjthread_t));
	bjthread->tid 		= tid_counter++;
	bjthread->state 	= STATE_READY;
	bjthread->priority	= priority;
	bjthread->context	= ucontext;

	return bjthread;
}


static void initMainThread()
{
	bjthread_t *main_thread	= (bjthread_t *) malloc(sizeof(bjthread_t));
	main_thread->context	= (ucontext_t *) malloc(sizeof(ucontext_t));
	main_thread->state	= STATE_RUNNING;
	main_thread->priority 	= PRIORITY_MAX;
	main_thread->tid	= -1;
	//printf("initMainThread (inside Initialization)\n");
	current_thread = main_thread;

	addThreadToQueue(main_thread);
}



void bjthread_start(int priority, void (*func)(void), void *args)
{
	//printf("bjthread_start...\n");

	if (init_bjthread_flag == false)
	{
		//printf("bjthread_start - initializations\n");
		init_bjthread_flag = true;
		initMainThread();
		initSignals();
		runTimer();
	}

	//blockSignals();
	ucontext_t *ucontext = prepareContext(func, args);
	bjthread_t *bjthread = prepareBJThread(ucontext, priority);
  scheduler_queue.count_nodes++;
  addThreadToQueue(bjthread);
	//runTimer();
	//unblockSignals();
	//schedule();
}


void bjthread_exit(void) //moze tutaj po prostu stan na exit i tyle...?
{
	//blockSignals();
	removeFromQueue(current_thread);
    //printf("\n----------------------------------- %d -----------------------------------\n", scheduler_queue.count_nodes);
	if (scheduler_queue.count_nodes < 2)
	{
		stopTimer();
	}
	//unblockSignals();
	schedule();
}


int bjthread_mutex_init(bjmutex_t *bjmutex)
{
	bjmutex = (bjmutex_t *) malloc (sizeof(bjthread_t));
	bjmutex->state = MUTEX_UNLOCK;
	bjmutex->blocked_thread_tid = -2; //none of tid
}


int bjthread_mutex_lock(bjmutex_t *bjmutex)
{
	if (bjmutex == NULL || bjmutex->state == MUTEX_INDEFINITE)
	{
		perror("bjmutex is not initialized!");
		return RET_MUTEX_ERROR;
	}
	else if (bjmutex->state == MUTEX_UNLOCK)
	{
		bjmutex->state = MUTEX_LOCK;
		bjmutex->blocked_thread_tid = current_thread->tid;
	}
	else if (bjmutex->state == MUTEX_LOCK)
	{
		schedule();
		bjthread_mutex_lock(bjmutex);
	}
	else
	{
		perror("bjmutex_mutex_lock!");
		return RET_MUTEX_ERROR;
	}
}


int bjthread_mutex_unlock(bjmutex_t *bjmutex)
{
	if (bjmutex == NULL || bjmutex->state == MUTEX_INDEFINITE)
	{
		perror("bjthread_mutex_unlock -> bjmutex is not initialized!");
		return RET_MUTEX_ERROR;
	}
	else if (bjmutex->state == MUTEX_LOCK && bjmutex->blocked_thread_tid == current_thread->tid)
	{
		bjmutex->state = MUTEX_UNLOCK;
	}
	else if(bjmutex->state == MUTEX_LOCK && bjmutex->blocked_thread_tid != current_thread->tid)
	{
		perror("bjmutex_mutex_unlock -> tried to unlock from another thread!");
		return RET_MUTEX_ERROR;
	}
	else
	{
		perror("bjthread_mutex_unlock!");
		return RET_MUTEX_ERROR;
	}
}


int bjthread_destroy(void *object)
{
	free(object); //TODO: czy tutaj cos więcej?
}
