#include <stdio.h>
#include <unistd.h>
#include "bjthread.h"
#define MAX 100

bjmutex_t mutex;
bjcond_t cond_p, cond_c;
int buffer = 0;

void producer(void)
{
  int i;
  for (i = 1; i <= MAX; i++)
  {
    bjthread_mutex_lock(&mutex);
    while (buffer != 0)
    {
      bjthread_cond_wait(&cond_p, &mutex);
    }
    buffer = i;
    printf("pr1 -> buf = %d\n", buffer);
    bjthread_cond_signal(&cond_c);
    bjthread_mutex_unlock(&mutex);
  }
  bjthread_exit();
}

void consumer(void)
{
  int i;
  for (i = 1; i <= MAX; i++)
  {
    bjthread_mutex_lock(&mutex);
    while (buffer == 0)
    {
      bjthread_cond_wait(&cond_c, &mutex);
    }
    buffer = 0;
    printf("cons1 -> buf = %d\n", buffer);
    bjthread_cond_signal(&cond_p);
    bjthread_mutex_unlock(&mutex);
  }
  bjthread_exit();
}


int main()
{
  int i = 0;
  bjthread_mutex_init(&mutex);
  bjthread_cond_init(&cond_p);
  bjthread_cond_init(&cond_c);
  bjthread_start(1, producer, NULL);
  bjthread_start(1, consumer, NULL);

  while(i < 20000){
    printf("Main - ");
    usleep(100000);
    i++;
  }
  /*bjthread_destroy(&mutex);
  bjthread_destroy(&cond_p);
  bjthread_destroy(&cond_m);*/
  return 0;
}
