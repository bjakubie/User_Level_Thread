test: bjthread_lib.o scheduler.o
	gcc thread_test.c bjthread_lib.o scheduler.o -o test

bjthread_lib.o:
	gcc -c bjthread_lib.c -o bjthread_lib.o

scheduler.o:
	gcc -c scheduler.c -o scheduler.o

clean:
	rm -f *.o test
