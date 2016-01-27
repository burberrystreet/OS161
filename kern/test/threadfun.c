#include <types.h>
#include <lib.h>
#include <thread.h>
#include <synch.h>
#include <test.h>

static struct semaphore *tsem = NULL;

static void init_sem(void) {
	if (tsem == NULL) {
		tsem = sem_create("tsem", 0);
		if (tsem == NULL) {
			panic("threadtest: sem_create failed\n");
		}
	}
}

static void mythread(void *junk, unsigned long num) {
	int ch = '0' + num;
	int i;

	(void)junk;

	for (i=0; i<100; i++) {
		putch(ch);
	}

	V(tsem);
}

static void threadfun(int nThreads) {
	char name[16];
	int i, result;

	for (i=0; i<nThreads; i++) {
		snprintf(name, sizeof(name), "threadtest%d", i);
		result = thread_fork(name, NULL, mythread, NULL, i);
		if (result) {
			panic("threadtest: thread_fork failed %s)\n", strerror(result));
		}
	}	

	for (i=0; i<nThreads; i++) {
		P(tsem);
	}

}

int threadtest4(int nargs, char **args) {
	
	if (nargs != 2) {
		kprintf("Usage: tt4 [int]");
		return 0;
	}

	(void)nargs;
	
	int nThreads = atoi(args[1]);

	init_sem();
	kprintf("Starting thread test...\n");
	threadfun(nThreads);
	kprintf("\nThread test done.\n");

	return 0;
}
