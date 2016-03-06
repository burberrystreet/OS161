/*
 * An example program.
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

	_exit(0);
	return 0; /* avoid compiler warnings */
}
