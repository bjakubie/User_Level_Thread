all: mutex_test thread_test cond_test

cond_test: bjthread_lib.o scheduler.o
	gcc cond_test.c bjthread_lib.o scheduler.o -o test_cond

mutex_test: bjthread_lib.o scheduler.o
	gcc mutex_test.c bjthread_lib.o scheduler.o -o test_mutex

thread_test: bjthread_lib.o scheduler.o
	gcc thread_test.c bjthread_lib.o scheduler.o -o test_thread

bjthread_lib.o:
	gcc -c bjthread_lib.c -o bjthread_lib.o

scheduler.o:
	gcc -c scheduler.c -o scheduler.o

clean:
	rm -f *.o test*
