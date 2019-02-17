#include <stdio.h>
#include <unistd.h>
#include "bjthread.h"

void wypisz(void)
{
    while(1)
    {
        printf("1111 ");
        usleep(100);
    }
    bjthread_exit();
}

void wypisz2(void)
{
    while(1)
    {
        printf("2222 ");
        usleep(100);
    }
    bjthread_exit();
}

void wypisz3(void)
{
    while(1)
    {
        printf("3333 ");
        usleep(100);
    }
    bjthread_exit();
}

int main()
{
    bjthread_start(1, wypisz, NULL);
    bjthread_start(1, wypisz2, NULL);
    bjthread_start(1, wypisz3, NULL);
    while(1);
    return 0;
}
