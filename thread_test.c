#include <stdio.h>
#include <unistd.h>
#include "bjthread.h"

void wypisz(void)
{
    int i = 0;
    while(i < 8000)
    {
        printf("1111 - %d ", i);
        usleep(100);
        i++;
    }
    bjthread_exit();
}

void wypisz2(void)
{
    int i = 0;
    while(i < 10000)
    {
        printf("2222 - %d ", i);
        usleep(100);
        i++;
    }
    bjthread_exit();
}

void wypisz3(void)
{
  int i = 0;
  while(i < 7000)
  {
      printf("3333 - %d ", i);
      usleep(100);
      i++;
  }
  bjthread_exit();
}

void wypisz4(void)
{
    int i = 0;
    while(i < 5000)
    {
        printf("4444 - %d ", i);
        usleep(100);
        i++;
    }
    bjthread_exit();
}

int main()
{
    bjthread_start(1, wypisz, NULL);
    bjthread_start(1, wypisz2, NULL);
    bjthread_start(2, wypisz4, NULL);
    bjthread_start(3, wypisz3, NULL);
    while(1){
        printf("Main ");
        usleep(100);
    }
    return 0;
}
