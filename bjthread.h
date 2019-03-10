#ifndef __BJTHREAD_H__
#define __BJTHREAD_H__
#include "bjthread_lib.h"

/*
 * Wystartowanie watku uzytkownika poprzez podanie adresu funkcji reprezentujacej watek i priorytetu jego wykonania.
 * Nizsza wartosc procesu oznacza pierwszenstwo przy dostepie do procesora.
 */
void bjthread_start(int priority, void (*func)(void), void *args);

/*
 * Zakonczenie watku uzytkownika poprzez wywolanie funkcji w ciele watku
 */
void bjthread_exit(void);

/*
 * Utworzenie mutexa
 */
int bjthread_mutex_init(bjmutex_t *bjmutex);

/*
 * Zajecie mutexa
 */
int bjthread_mutex_lock(bjmutex_t *bjmutex);

/*
 * Zwolnienie mutexa
 */
int bjthread_mutex_unlock(bjmutex_t *bjmutex);

/*
 * Utworzenie zmiennej warunkowej
 */
//int bjthread_cond(cond_t *cond);

/*
 * Oczekiwanie na zmienna warunkowa. Po zasygnalizowaniu zmiennej zajmowany jest mutex
 */
//int bjthread_wait(cond_t *cond, mutex_t *mutex);

/*
 * Zasygnalizowanie zmiennej warunkowej
 */
//int bjthread_signal(cond_t *cond);

/*
 * Usuniecie mutexa lub zmiennej warunkowej (obiekt podany przez wskaznik o)
 */
int bjthread_destroy(void *object);


#endif
