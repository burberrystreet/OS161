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

static void testingprint(void *junk, unsigned long num) {
	(void)num;
	(void)junk;

	kprintf("This is a test of kprintf\n");

	V(tsem);
}

static void threadprinttest(int nThreads) {
	char name[16];
	int i, result;

	for (i=0; i<nThreads; i++) {
		snprintf(name, sizeof(name), "threadtest%d", i);
		result = thread_fork(name, NULL, testingprint, NULL, i);
		if (result) {
			panic("threadtest: thread_fork failed %s)\n", strerror(result));
		}
	}	

	for (i=0; i<nThreads; i++) {
		P(tsem);
	}

}

int threadtest0(int nargs, char **args) {
	
	if (nargs != 2) {
		kprintf("Usage: tt0 [int]");
		return 0;
	}

	(void)nargs;
	
	int nThreads = atoi(args[1]);

	init_sem();
	kprintf("Starting thread test...\n");
	threadprinttest(nThreads);
	kprintf("\nThread test done.\n");

	return 0;
}
