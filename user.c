/* Name: Andrew Audrain */
/* File: user.c  |  Executable (after make): user */
/* Date created: 10/06/18 */

#include <stdio.h>

int main ( int argc, char *argv[] ) {
	
	printf ( "Child -- pid %d from parent pid %d\n", getpid(), getppid() );

	return 0;
}
