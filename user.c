/* Name: Andrew Audrain */
/* File: user.c  |  Executable (after make): user */
/* Date created: 10/06/18 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>

int main ( int argc, char *argv[] ) {

	/* Variables for shared memory */
	int clockBuffer[2];
	key_t key1 = 1993;
	int *shmClock;
	int shmClockid;

	int msgBuffer[3];
	key_t key2 = 1994;
	int *shmMsg;
	int shmMsgid;

	/* Accessing shared memory allocated in oss */
	if ( ( shmClockid = shmget ( key1, sizeof ( clockBuffer), 0666 ) ) < 0 ) {
		perror ( "Clock shmget" );
		exit ( 1 );
	} 
	
	if ( ( shmClock = ( int *) shmat ( shmClockid, NULL, 0 ) ) < 0 ) {
		perror ( "Clock shmat" );
		exit ( 1 );
	}

	if ( ( shmMsgid = shmget ( key2, sizeof ( msgBuffer ), 0666 ) ) < 0 ) {
		perror ( "Message shmget" );
		exit ( 1 );
	}

	if ( ( shmMsg = ( int *) shmat ( shmMsgid, NULL, 0 ) ) < 0 ) {
		perror ( "Message shmat" );
		exit ( 1 );
	}

	//shmClock[0]++;
	//shmClock[1]++;
	//shmMsg[0] = getpid();
	//shmMsg[1] = shmClock[0];
	//shmMsg[2] = shmClock[1];
	
	printf ( "Child -- pid %d from parent pid %d\n", getpid(), getppid() );
	// printf ( "Child: %d terminated at %d.%d\n", shmMsg[0], shmMsg[1], shmMsg[2] );

	return 0;
}
