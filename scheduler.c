#include "scheduler.h"

struct itimerval it;
struct sigaction act, oact;
bjthread_t *current_thread;
sigset_t sig_mask;


bjthread_t *findPlaceInQueue(int priority)
{
	bjthread_t *current = scheduler_queue.first_node->next_node;

	if (scheduler_queue.first_node->priority > priority)
	{
		return scheduler_queue.first_node;
	}
	else
	{
		while (current != NULL && priority >= current->priority) // teraz trafia jako ew. ostatni o tym samym priorytecie wiec git
		{
			current = current->next_node;
		}
		return current;
	}
}

void setAsPrev(bjthread_t *bjthread, bjthread_t *wsk_thread)
{
	if (wsk_thread->prev_node != NULL) wsk_thread->prev_node->next_node = bjthread;
	else scheduler_queue.first_node = bjthread;

	printf("setAsPrev\n");

	bjthread->next_node 	= wsk_thread;
	bjthread->prev_node	= wsk_thread->prev_node;
	wsk_thread->prev_node	= bjthread;
	printf("head tid = %d\n", scheduler_queue.first_node->tid);
}

void setAsLastInQueue(bjthread_t *bjthread, bjthread_t *last_node)
{
	printf("setAsLastInQueue\n");
	bjthread->next_node		= NULL;
	bjthread->prev_node		= last_node;
	last_node->next_node		= bjthread;
	scheduler_queue.last_node	= bjthread;
	printf("TAIL tid = %d\n", scheduler_queue.last_node->tid);
	printf("TAIL->prev->tid = %d\n", scheduler_queue.last_node->prev_node->tid);

	//scheduler
}

void removeFromQueue(bjthread_t *bjthread) //czy tutaj sa potrzebne zabezpieczenia? 
{
	printf("removeFromQueue\n");
	if (bjthread->prev_node != NULL)
	{
		bjthread->next_node->prev_node = bjthread->prev_node;
		bjthread->prev_node->next_node = bjthread->next_node;
	}
	else
	{
		scheduler_queue.first_node = bjthread->next_node;
		bjthread->next_node->prev_node = NULL;
	}
	bjthread->state = STATE_DONE;
	scheduler_queue.count_nodes--;

}

void addThreadToQueue(bjthread_t *bjthread)
{
	printf("addThreadToQueue - start\n");
	if (scheduler_queue.first_node == NULL)
	{
		printf("addThreadToQueue - init head\n");
		bjthread->next_node = NULL;
		bjthread->prev_node = NULL;
		scheduler_queue.first_node  = bjthread;
		scheduler_queue.last_node   = bjthread;
		scheduler_queue.count_nodes = 1;
	}
	else
	{
		printf("addThreadToQueue - not head\n");
		bjthread_t *wsk_thread = findPlaceInQueue(bjthread->priority);
		if (wsk_thread != NULL)
		{
			setAsPrev(bjthread, wsk_thread);
		}
		else
		{
			setAsLastInQueue(bjthread, scheduler_queue.last_node);
		}
		scheduler_queue.count_nodes++;
	}
}

void shuffleSamePriorityThreads(bjthread_t *bjthread)
{
	if (bjthread->next_node != NULL)
	{
		printf("1 if \n");
		bjthread->next_node->prev_node = bjthread->prev_node;
	}
	if (bjthread->prev_node != NULL)
	{ 
		printf("2 if \n");
		bjthread->prev_node->next_node = bjthread->next_node;
	}
	else
	{
		scheduler_queue.first_node = bjthread;
	}
	if (bjthread->state == STATE_READY)
	{
		printf("3 if \n");
		addThreadToQueue(bjthread);
	}
}

bjthread_t *getNodeByStateWithMaxPriority(enum state st)
{
	bjthread_t *current = scheduler_queue.first_node;
	printf("current thread tid = %d\n", current->tid);
	//printf("tid_prev = %d, tid_next = %d\n", current->prev_node->tid, current->next_node->tid);

	while (current != NULL)
	{
		printf("wewnatrz while'a, current tid = %d\n", current->tid);
		if (current->state == st)
		{
			return current;
		}
		current = current->next_node;
	}
	printf("RETURNING NULL\n");
	return NULL;
}

bjthread_t *getRunningNode()
{
	//bjthread_t *current_thread;
	//current_thread = getNodeByStateWithMaxPriority(STATE_RUNNING);
	return current_thread;
}

void schedule()
{
	printf("schedule\n");
	bjthread_t *curr_thread = getRunningNode();
	printf("after curr_thread->tid = %d\t state=%d\n", curr_thread->tid, curr_thread->state);
	bjthread_t *next_node = getNodeByStateWithMaxPriority(STATE_READY);
	printf("after_next_node --> next_node_id = %d\n", next_node->tid);
	printf("after_next_node --> state = %d\n", curr_thread->state);

	if (curr_thread->state == STATE_RUNNING && next_node->state == STATE_READY)
	{
		printf("inside if\n");
		next_node->state = STATE_RUNNING;
		curr_thread->state = STATE_READY;
		shuffleSamePriorityThreads(curr_thread);
		current_thread = next_node;
		scheduler_queue.first_node = next_node;
		printf("current_thread->tid, before swapcontext = %d\n", next_node->tid);
		swapcontext(curr_thread->context, next_node->context);
	}
	else
	{
		printf("inside else\n");
		next_node->state = STATE_RUNNING;
		current_thread = next_node;
		scheduler_queue.first_node = next_node;
		swapcontext(curr_thread->context, next_node->context);
	}
}

void runTimer()
{
	printf("runTimer\n");
	it.it_interval.tv_sec = 1;
	it.it_interval.tv_usec = 0;
	it.it_value.tv_sec = 1;
	it.it_value.tv_usec = 0;
	setitimer(ITIMER_REAL, &it, NULL);
}

void stopTimer()
{
	printf("stopTimer\n");
	it.it_interval.tv_sec = 0;
	it.it_interval.tv_usec = 0;
	it.it_value.tv_sec = 0;
	it.it_value.tv_usec = 0;
	setitimer(ITIMER_REAL, &it, NULL);
}


void initSignals()
{
	printf("initSignals\n");
	act.sa_handler = schedule;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	sigaction(SIGALRM, &act, &oact);
}


void blockSignals()
{
	printf("blockSignals\n");
	//sigset_t sig_mask;
	sigemptyset(&sig_mask);
	sigaddset(&sig_mask, SIGALRM);
	sigprocmask(SIG_BLOCK, &sig_mask, NULL);
}

void unblockSignals()
{
	printf("unblockSignals\n");
	//sigset_t sig_mask;
	sigemptyset(&sig_mask);
	sigaddset(&sig_mask, SIGALRM);
	sigprocmask(SIG_UNBLOCK, &sig_mask, NULL);
	runTimer();
}


