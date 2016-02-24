#include <types.h>
#include <lib.h>
#include <thread.h>
#include <synch.h>
#include <spinlock.h>
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
			panic("threadtest: spinlock_init failed\n");
		}
	}
}

static void fastspinlockcounter(void *junk, unsigned long num) {
	int i;
	int loopBound = (int) num;

	(void)junk;

	spinlock_acquire(&lk->lk_spinlock);
	for (i=0; i<loopBound; i++) {
		counter++;
	}
	spinlock_release(&lk->lk_spinlock);

	V(tsem);
}

static void slowspinlockcounter(void *junk, unsigned long num) {
	int i;
	int loopBound = (int) num;

	(void)junk;

	for (i=0; i<loopBound; i++) {
		spinlock_acquire(&lk->lk_spinlock);
		counter++;
		spinlock_release(&lk->lk_spinlock);
	}

	V(tsem);
}

static void threadfun10(int nThreads, int loopBound) {
	char name[16];
	int i, result;

	for (i=0; i<nThreads; i++) {
		snprintf(name, sizeof(name), "threadtest%d", i);
		result = thread_fork(name, NULL, fastspinlockcounter, NULL, loopBound);
		if (result) {
			panic("threadtest: thread_fork failed %s)\n", strerror(result));
		}
	}	

	for (i=0; i<nThreads; i++) {
		P(tsem);
	}

}

static void threadfun11(int nThreads, int loopBound) {
	char name[16];
	int i, result;

	for (i=0; i<nThreads; i++) {
		snprintf(name, sizeof(name), "threadtest%d", i);
		result = thread_fork(name, NULL, slowspinlockcounter, NULL, loopBound);
		if (result) {
			panic("threadtest: thread_fork failed %s)\n", strerror(result));
		}
	}

	for (i=0; i<nThreads; i++) {
		P(tsem);
	}
}

int threadtest10(int nargs, char **args) {
	counter = 0;
	int loopBound;

	if (nargs != 3 && nargs != 2) {
		kprintf("Usage: tt10 [int] [optional int]");
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
	threadfun10(nThreads, loopBound);
	kprintf("\nThread test done.\n");
   kprintf("\nThe value of the counter should be %d. \n", loopBound*nThreads);
	kprintf("\nThe value of the counter is %d. \n", counter);
	return 0;
}

int threadtest11(int nargs, char **args) {
	counter = 0;
	int loopBound;

	if (nargs != 3 && nargs != 2) {
		kprintf("Usage: tt11 [int] [optional int]");
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
	threadfun11(nThreads, loopBound);
	kprintf("\nThread test done.\n");
	kprintf("\nThe value of the counter should be %d. \n", loopBound*nThreads);
	kprintf("\nThe value of the counter is %d. \n", counter);
	return 0;
}
