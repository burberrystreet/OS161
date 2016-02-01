#include <types.h>
#include <lib.h>
#include <thread.h>
#include <synch.h>
#include <test.h>

static int counter;

static struct semaphore *tsem = NULL;
static struct semaphore *psem = NULL;

static void init_sem(void) {
	if (tsem == NULL) {
		tsem = sem_create("tsem", 0);
		if (tsem == NULL) {
			panic("threadtest: sem_create failed\n");
		}
	}
	if (psem == NULL) {
		psem = sem_create("psem", 1);
		if (tsem == NULL) {
			panic("threadtest: sem_create failed\n");
		}
	}
}

static void unsafethreadcounter(void *junk, unsigned long num) {
	int i;
   int loopBound = (int) num;

	(void)junk;

	for (i=0; i<loopBound; i++) {
		counter++;
	}

	V(tsem);
}

static void safethreadcounter(void *junk, unsigned long num) {
	int i;
	int loopBound = (int) num;

	(void)junk;

	P(psem);
	for (i=0; i<loopBound; i++) {
		counter++;
	}
	V(psem);

	V(tsem);
}

static void threadfun2(int nThreads, int loopBound) {
	char name[16];
	int i, result;

	for (i=0; i<nThreads; i++) {
		snprintf(name, sizeof(name), "threadtest%d", i);
		result = thread_fork(name, NULL, unsafethreadcounter, NULL, loopBound);
		if (result) {
			panic("threadtest: thread_fork failed %s)\n", strerror(result));
		}
	}	

	for (i=0; i<nThreads; i++) {
		P(tsem);
	}

}

static void threadfun3(int nThreads, int loopBound) {
	char name[16];
	int i, result;

	for (i=0; i<nThreads; i++) {
		snprintf(name, sizeof(name), "threadtest%d", i);
		result = thread_fork(name, NULL, safethreadcounter, NULL, loopBound);
		if (result) {
			panic("threadtest: thread_fork failed %s)\n", strerror(result));
		}
	}

	for (i=0; i<nThreads; i++) {
		P(tsem);
	}
}

int threadtest5(int nargs, char **args) {
	counter = 0;
	int loopBound;

	if (nargs != 3 && nargs != 2) {
		kprintf("Usage: tt5 [int] [optional int]");
		return 0;
	}

	if (nargs == 3) {
		loopBound = atoi(args[2]);
	} else {
		loopBound = 10000;
	}

	int nThreads = atoi(args[1]);

	init_sem();
	kprintf("Starting thread test...\n");
	threadfun2(nThreads, loopBound);
	kprintf("\nThread test done.\n");
   kprintf("\nThe value of the counter should be %d. \n", loopBound*nThreads);
	kprintf("\nThe value of the counter is %d. \n", counter);
	return 0;
}

int threadtest6(int nargs, char **args) {
	counter = 0;
	int loopBound;

	if (nargs != 3 && nargs != 2) {
		kprintf("Usage: tt5 [int] [optional int]");
		return 0;
	}

	if (nargs == 3) {
		loopBound = atoi(args[2]);
	} else {
		loopBound = 10000;
	}

	int nThreads = atoi(args[1]);

	init_sem();
	kprintf("Starting thread test...\n");
	threadfun3(nThreads, loopBound);
	kprintf("\nThread test done.\n");
	kprintf("\nThe value of the counter should be %d. \n", loopBound*nThreads);
	kprintf("\nThe value of the counter is %d. \n", counter);
	return 0;
}
