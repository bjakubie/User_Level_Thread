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
	current_thread = main_thread;

	addThreadToQueue(main_thread);
}



void bjthread_start(int priority, void (*func)(void), void *args)
{
	if (init_bjthread_flag == false)
	{
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
	//unblockSignals();
}


void bjthread_exit(void)
{
	removeFromQueue(current_thread);
	if (scheduler_queue.count_nodes < 2)
	{
		stopTimer();
	}
	schedule();
}


int bjthread_mutex_init(bjmutex_t *bjmutex)
{
	//bjmutex = (bjmutex_t *) malloc (sizeof(bjthread_t));
	bjmutex->state = MUTEX_UNLOCK;
	bjmutex->blocked_thread_tid = -2; //none of tid
}


int bjthread_mutex_lock(bjmutex_t *bjmutex)
{

	if (bjmutex == NULL || bjmutex->state == MUTEX_UNDEF)
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
	if (bjmutex == NULL || bjmutex->state == MUTEX_UNDEF)
	{
		perror("bjthread_mutex_unlock -> bjmutex is not initialized!");
		return RET_MUTEX_ERROR;
	}
	else if (bjmutex->state == MUTEX_LOCK && bjmutex->blocked_thread_tid == current_thread->tid)
	{
		bjmutex->state = MUTEX_UNLOCK;
	}
	else if (bjmutex->state == MUTEX_LOCK && bjmutex->blocked_thread_tid != current_thread->tid)
	{
		perror("bjthread_mutex_unlock -> tried to unlock from another thread!");
		return RET_MUTEX_ERROR;
	}
	else if (bjmutex->state == MUTEX_UNLOCK)
	{
		//do nothing - it is possible, that condition variable unblocked mutex
	}
	else
	{
		perror("bjthread_mutex_unlock!");
		return RET_MUTEX_ERROR;
	}
}


int bjthread_cond_init(bjcond_t *bjcond)
{
	bjcond->state = COND_WAIT;
}


int bjthread_cond_wait(bjcond_t *bjcond, bjmutex_t *bjmutex)
{
	/*
	if (bjmutex->state == MUTEX_UNLOCK)
	{
		bjmutex->state = MUTEX_LOCK;
	}
	*/

	if (bjcond == NULL)
	{
		perror("bjthread_cond_wait -> do not initialized conditional variable!");
		return RET_COND_ERROR;
	}
	else if (bjcond->state == COND_WAIT || bjcond->state == COND_UNDEF) //COND_UNDEF not needed probably
	{
		bjthread_mutex_unlock(bjmutex);
		schedule();
		bjthread_cond_wait(bjcond, bjmutex);
	}
	else if (bjcond->state == COND_OPEN)
	{
		bjcond->state = COND_WAIT;
	}
	else
	{
		perror("bjthread_cond_wait -> another error!");
		return RET_COND_ERROR;
	}
}


int bjthread_cond_signal(bjcond_t *bjcond)
{
	bjcond->state = COND_OPEN;
}


int bjthread_destroy(void *object)
{
	object = NULL;
}
