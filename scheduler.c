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
		while (current != NULL && priority >= current->priority)
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

	bjthread->next_node 	= wsk_thread;
	bjthread->prev_node	= wsk_thread->prev_node;
	wsk_thread->prev_node	= bjthread;

}

void setAsLastInQueue(bjthread_t *bjthread, bjthread_t *last_node)
{
	bjthread->next_node		= NULL;
	bjthread->prev_node		= last_node;
	last_node->next_node		= bjthread;
	scheduler_queue.last_node	= bjthread;
}

void removeFromQueue(bjthread_t *bjthread)
{
	if (bjthread->prev_node != NULL)
	{
		bjthread->prev_node->next_node = bjthread->next_node;
		if (bjthread->next_node != NULL)
		{
			bjthread->next_node->prev_node = bjthread->prev_node;
		}
	}
	else
	{
		scheduler_queue.first_node = bjthread->next_node;
		bjthread->next_node->prev_node = NULL;
	}
	bjthread->state = STATE_DONE;
	bjthread->next_node = NULL;
	bjthread->prev_node = NULL;
	scheduler_queue.count_nodes--;

}

void addThreadToQueue(bjthread_t *bjthread)
{
	if (scheduler_queue.first_node == NULL)
	{
		bjthread->next_node = NULL;
		bjthread->prev_node = NULL;
		scheduler_queue.first_node  = bjthread;
		scheduler_queue.last_node   = bjthread;
		scheduler_queue.count_nodes = 1;
	}
	else
	{
		bjthread_t *wsk_thread = findPlaceInQueue(bjthread->priority);
		if (wsk_thread != NULL)
		{
			setAsPrev(bjthread, wsk_thread);
		}
		else
		{
			setAsLastInQueue(bjthread, scheduler_queue.last_node);
		}
	}
}

void shuffleSamePriorityThreads(bjthread_t *bjthread)
{
	if (bjthread->next_node != NULL)
	{
		bjthread->next_node->prev_node = bjthread->prev_node;
	}
	if (bjthread->prev_node != NULL)
	{
		bjthread->prev_node->next_node = bjthread->next_node;
	}
	else
	{
		scheduler_queue.first_node = bjthread;
	}
	if (bjthread->state == STATE_READY)
	{
		addThreadToQueue(bjthread);
	}
}

bjthread_t *getNodeByStateWithMaxPriority(enum state st)
{
	bjthread_t *current = scheduler_queue.first_node;

	while (current != NULL)
	{
		if (current->state == st)
		{
			return current;
		}
		current = current->next_node;
	}
	return NULL;
}

bjthread_t *getRunningNode()
{
	return current_thread;
}

void schedule()
{
	bjthread_t *curr_thread = getRunningNode();
	bjthread_t *next_node = getNodeByStateWithMaxPriority(STATE_READY);

	if (curr_thread->state == STATE_RUNNING && next_node->state == STATE_READY)
	{
		next_node->state = STATE_RUNNING;
		curr_thread->state = STATE_READY;
		if (next_node->priority == curr_thread->priority)
		{
			shuffleSamePriorityThreads(curr_thread);
			scheduler_queue.first_node = next_node;
		}
		current_thread = next_node;
		swapcontext(curr_thread->context, next_node->context);
	}
	else
	{
		next_node->state = STATE_RUNNING;
		current_thread = next_node;
		scheduler_queue.first_node = next_node;
		swapcontext(curr_thread->context, next_node->context);
	}
}

void runTimer()
{
	it.it_interval.tv_sec = 1;
	it.it_interval.tv_usec = 0;
	it.it_value.tv_sec = 1;
	it.it_value.tv_usec = 0;
	setitimer(ITIMER_REAL, &it, NULL);
}

void stopTimer()
{
	it.it_interval.tv_sec = 0;
	it.it_interval.tv_usec = 0;
	it.it_value.tv_sec = 0;
	it.it_value.tv_usec = 0;
	setitimer(ITIMER_REAL, &it, NULL);
}


void initSignals()
{
	//printf("initSignals\n");
	act.sa_handler = schedule;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	sigaction(SIGALRM, &act, &oact);
}


void blockSignals()
{
	//printf("blockSignals\n");
	//sigset_t sig_mask;
	sigemptyset(&sig_mask);
	sigaddset(&sig_mask, SIGALRM);
	sigprocmask(SIG_BLOCK, &sig_mask, NULL);
}

void unblockSignals()
{
	//printf("unblockSignals\n");
	//sigset_t sig_mask;
	sigemptyset(&sig_mask);
	sigaddset(&sig_mask, SIGALRM);
	sigprocmask(SIG_UNBLOCK, &sig_mask, NULL);
	runTimer();
}
