/*
 * An example fork program.
 */

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <err.h>


int
main()
{
	printf("Program Start");
	
	static volatile int pid1, pid2;

	pid1 = getpid();
	printf("pid: %d \n",pid1);
	int status;
	pid2 = 5;
	printf("waitpid: %d \n",waitpid(pid2,&status,0));	
	
	pid_t pid;
	pid = fork();
	if(pid > 0) {
		printf("Parent\n");
	} else {
		printf("child\n");
	}

	_exit(0);
	return 0; /* avoid compiler warnings */
}
