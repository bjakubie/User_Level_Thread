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
	printf("initMainThread (inside Initialization)\n");
	current_thread = main_thread;

	addThreadToQueue(main_thread);
}



void bjthread_start(int priority, void (*func)(void), void *args)
{
	printf("bjthread_start...\n");

	if (init_bjthread_flag == false)
	{
		printf("bjthread_start - initializations\n");
		init_bjthread_flag = true;
		initMainThread();
		initSignals();
		runTimer();
	}
    
	//blockSignals();
	ucontext_t *ucontext = prepareContext(func, args);
	bjthread_t *bjthread = prepareBJThread(ucontext, priority);
	addThreadToQueue(bjthread);
	//runTimer();
	//unblockSignals();
	//schedule();
}


void bjthread_exit(void) //moze tutaj po prostu stan na exit i tyle...?
{
	blockSignals();
	removeFromQueue(current_thread);

	if (scheduler_queue.count_nodes <= 1) //idle + one, zatrzymany timer przeniesie do ostatniego, ktory nie jest idlem... (ale po co idle w takim razie...)
	{
		stopTimer();
	}
	unblockSignals();
	schedule();
}
