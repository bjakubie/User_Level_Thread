#include <stdio.h>
#include <unistd.h>
#include "bjthread.h"

bjmutex_t mutex;

void wypisz(void)
{
  int i = 0;
  int j;
  printf("TUTAJ JESTEM 1-1\n");
  while(i < 5)
  {
    printf("TUTAJ JESTEM 1-2\n");
    bjthread_mutex_lock(&mutex);
    for (j = 0; j < 2000; j++)
    {
      printf("1111 - %d ", i);
      usleep(100);
    }
    i++;
    bjthread_mutex_unlock(&mutex);
    usleep(500000); //0.5s
  }
  bjthread_exit();
}

void wypisz2(void)
{
  printf("TUTAJ JESTEM 2-1\n");
  int i = 0;
  int j;
  while(i < 5)
  {
    printf("TUTAJ JESTEM 2-2\n");
    bjthread_mutex_lock(&mutex);
    for (j = 0; j < 2000; j++)
    {
      printf("2222 - %d ", i);
      usleep(100);
    }
    i++;
    bjthread_mutex_unlock(&mutex);
    usleep(500000); //0.5s
  }
  bjthread_exit();
}


int main()
{
  int i = 0;
  bjthread_mutex_init(&mutex);
  printf("MUTEX_DATA: state = %d, tid = %d\n", mutex.state, mutex.blocked_thread_tid);
  bjthread_start(1, wypisz, NULL);
  bjthread_start(1, wypisz2, NULL);
  while(i < 22000){
    printf("Main - ");
    usleep(100);
    i++;
  }
  //printf("ENDING\n");
  //free((void *) &mutex);
  //bjthread_destroy((void *)&mutex);
  //printf("after_destroy\n");
  //printf("%d\n", mutex.blocked_thread_tid);
  return 0;
}
