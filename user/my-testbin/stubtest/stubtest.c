/*
 * An example program.
 */
#include <sys/types.h>
#include <sys/stat.h>
#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <err.h>
#include "tools.h"

int randint(void) {
	int x;
	fillrand(&x, sizeof(x));
	return x;
}

void * randptr(void) {
	void *x;
	fillrand(&x, sizeof(x));
	return x;
}

int
main()
{
	printf("Program Start\n\n");
	printf("Testing waitpid");

	int dofork = 1;

	int a0 = randint();
	void * a1 = randptr();
	int a2 = randint();
	int result, pid, status;
	char buf[128];

	snprintf(buf, sizeof(buf), "waitpid(%d, %p, %d)", (a0), (a1), (a2));
	printf("%-47s", buf);

	pid = dofork ? fork() : 0;
	if (pid < 0) {
		err(1, "fork");
	}
	if (pid > 0) {
		waitpid(pid, &status, 0);
		return;
	}

	result = waitpid(a0, a1, a2);
	printf(" result %d, errno %d\n", result, errno);
	if (dofork){
		exit(0);
	}
	return 0; /* avoid compiler warnings */
}
