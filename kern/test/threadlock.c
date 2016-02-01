#include <types.h>
#include <lib.h>
#include <thread.h>
#include <synch.h>
#include <test.h>

static int counter;

static struct semaphore *tsem = NULL;
static struct lock *lk = NULL;

static void init_sem(void) {
	if (tsem == NULL) {
		tsem = sem_create("tsem", 0);
		if (tsem == NULL) {
			panic("threadtest: sem_create failed\n");
		}
	}
}

static void init_lock(void) {
	if (lk == NULL) {
		lk = lock_create("lk");
		if (lk == NULL) {
			panic("threadtest: lock_create failed\n");
		}
	}
}

static void fastlockedcounter(void *junk, unsigned long num) {
	int i;
	int loopBound = (int) num;

	(void)junk;

	lock_acquire(lk);
	for (i=0; i<loopBound; i++) {
		counter++;
	}
	lock_release(lk);

	V(tsem);
}

static void slowlockedcounter(void *junk, unsigned long num) {
	int i;
	int loopBound = (int) num;

	(void)junk;

	for (i=0; i<loopBound; i++) {
		lock_acquire(lk);
		counter++;
		lock_release(lk);
	}

	V(tsem);
}

static void threadfun5(int nThreads, int loopBound) {
	char name[16];
	int i, result;

	for (i=0; i<nThreads; i++) {
		snprintf(name, sizeof(name), "threadtest%d", i);
		result = thread_fork(name, NULL, fastlockedcounter, NULL, loopBound);
		if (result) {
			panic("threadtest: thread_fork failed %s)\n", strerror(result));
		}
	}	

	for (i=0; i<nThreads; i++) {
		P(tsem);
	}

}

static void threadfun6(int nThreads, int loopBound) {
	char name[16];
	int i, result;

	for (i=0; i<nThreads; i++) {
		snprintf(name, sizeof(name), "threadtest%d", i);
		result = thread_fork(name, NULL, slowlockedcounter, NULL, loopBound);
		if (result) {
			panic("threadtest: thread_fork failed %s)\n", strerror(result));
		}
	}

	for (i=0; i<nThreads; i++) {
		P(tsem);
	}
}

int threadtest8(int nargs, char **args) {
	counter = 0;
	int loopBound;

	if (nargs != 3 && nargs != 2) {
		kprintf("Usage: tt8 [int] [optional int]");
		return 0;
	}

	if (nargs == 3) {
		loopBound = atoi(args[2]);
	} else {
		loopBound = 10000;
	}

	int nThreads = atoi(args[1]);

	init_sem();
	init_lock();
	kprintf("Starting thread test...\n");
	threadfun5(nThreads, loopBound);
	kprintf("\nThread test done.\n");
   kprintf("\nThe value of the counter should be %d. \n", loopBound*nThreads);
	kprintf("\nThe value of the counter is %d. \n", counter);
	return 0;
}

int threadtest9(int nargs, char **args) {
	counter = 0;
	int loopBound;

	if (nargs != 3 && nargs != 2) {
		kprintf("Usage: tt9 [int] [optional int]");
		return 0;
	}

	if (nargs == 3) {
		loopBound = atoi(args[2]);
	} else {
		loopBound = 10000;
	}

	int nThreads = atoi(args[1]);

	init_sem();
	init_lock();
	kprintf("Starting thread test...\n");
	threadfun6(nThreads, loopBound);
	kprintf("\nThread test done.\n");
	kprintf("\nThe value of the counter should be %d. \n", loopBound*nThreads);
	kprintf("\nThe value of the counter is %d. \n", counter);
	return 0;
}
