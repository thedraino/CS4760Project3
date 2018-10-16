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
#include <signal.h>
#include <semaphore.h>
#include <fcntl.h>
#include <time.h>
#include <stdbool.h>

#define SEM_NAME "/semaphore"

int main ( int argc, char *argv[] ) {

	/* Access the named semaphore created in oss */
	sem_t *sem;

	if ( ( sem  = sem_open ( SEM_NAME, O_CREAT, 0777, 1 ) ) == SEM_FAILED ) {
		perror ( "sem_open" );
		exit ( -1 );
	}

	/* Variables for shared memory */
	int clockBuffer[2];
	key_t key1 = 1993;
	int *shmClock;
	int shmClockID;

	int msgBuffer[3];
	key_t key2 = 1994;
	int *shmMsg;
	int shmMsgID;

	/* Accessing shared memory allocated in oss */
	if ( ( shmClockID = shmget ( key1, sizeof ( clockBuffer), 0666 ) ) < 0 ) {
		perror ( "Clock shmget" );
		exit ( 1 );
	} 
	
	if ( ( shmClock = ( int *) shmat ( shmClockID, NULL, 0 ) ) < 0 ) {
		perror ( "Clock shmat" );
		exit ( 1 );
	}

	if ( ( shmMsgID = shmget ( key2, sizeof ( msgBuffer ), 0666 ) ) < 0 ) {
		perror ( "Message shmget" );
		exit ( 1 );
	}

	if ( ( shmMsg = ( int *) shmat ( shmMsgID, NULL, 0 ) ) < 0 ) {
		perror ( "Message shmat" );
		exit ( 1 );
	}

	/* Starts by reading the simulated time clock */
	int seconds = shmClock[0];
	int nano = shmClock[1];

	srand ( time ( 0 ) );

	int num = ( rand() % ( 1000000 - 1 + 1 ) ) + 1;
	nano += num;
	while ( nano > 1000000 ) {
		seconds++;
		nano = nano - 1000000;
	}

	bool stillAlive = true;
	
	/* Critical Section */
	do {
		//printf ( "Child %d is attempting to enter the CS.\n", getpid() );
		sem_wait ( sem );
		//printf ( "Child %d has entered the CS.\n", getpid() );
		if ( ( seconds + nano ) >= ( shmClock[0] + shmClock[1] ) && shmMsg[0] == 0 ) {
			shmMsg[0] = getpid();
			shmMsg[1] = seconds;
			shmMsg[2] = nano;
			
			stillAlive = false;
		}
		/*else {
			printf ( "Child %d left the CS without being able to terminate.\n", getpid() ); 
		} */		
		sem_post ( sem );
	} while ( stillAlive );
		
	//printf ( "Child %d is able to terminate.\n", getpid() );	

	return 0;
}
