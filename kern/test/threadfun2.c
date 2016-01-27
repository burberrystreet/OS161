#include <types.h>
#include <lib.h>
#include <thread.h>
#include <synch.h>
#include <test.h>

static int unsafethreadcounter;

static struct semaphore *tsem = NULL;

static void init_sem(void) {
	if (tsem == NULL) {
		tsem = sem_create("tsem", 0);
		if (tsem == NULL) {
			panic("threadtest: sem_create failed\n");
		}
	}
}

static void mythread2(void *junk, unsigned long num) {
	int i;
	int loopBound = (int) num;

	(void)junk;

	for (i=0; i<loopBound; i++) {
		unsafethreadcounter++;
	}

	V(tsem);
}

static void threadfun2(int nThreads, int loopBound) {
	char name[16];
	int i, result;

	for (i=0; i<nThreads; i++) {
		snprintf(name, sizeof(name), "threadtest%d", i);
		result = thread_fork(name, NULL, mythread2, NULL, loopBound);
		if (result) {
			panic("threadtest: thread_fork failed %s)\n", strerror(result));
		}
	}	

	for (i=0; i<nThreads; i++) {
		P(tsem);
	}

}

int threadtest5(int nargs, char **args) {

	(void)nargs;
	
	int nThreads = atoi(args[1]);
	int loopBound = atoi(args[2]);

	init_sem();
	kprintf("Starting thread test...\n");
	threadfun2(nThreads, loopBound);
	kprintf("\nThread test done.\n");
   kprintf("\nThe value of the counter is %d. \n", unsafethreadcounter);
	return 0;
}
