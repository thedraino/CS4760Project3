/* Name: Andrew Audrain */
/* File: oss.c  |  Executable (after make): oss */
/* Date created: 10/06/18 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <signal.h>
#include <time.h>
#include <fcntl.h>

#define SEM_NAME "/semaphore"

/* Signal handler for termination after z seconds have elapsed */
void sigAlarm_handler ( int signum ) {
	int clockBuffer[2];
	key_t key1 = 1993;
	int shmClockID;

	int msgBuffer[3];
	key_t key2 = 1994;
	int shmMsgID;

	shmClockID = shmget ( key1, sizeof ( clockBuffer ), 0666 );
	shmMsgID = shmget ( key2, sizeof ( msgBuffer ), 0666 );

	shmctl ( shmClockID, IPC_RMID, NULL );
	shmctl ( shmMsgID, IPC_RMID, NULL );

	sem_unlink ( SEM_NAME );

	exit ( 0 ); 
}

int main ( int argc, char *argv[] ) {
	signal ( SIGALRM, sigAlarm_handler ); 
	int i, j;	/* Control variable for loops */
	int opt;	/* Controls the getopt loop */
	int total = 0;	/* Serves as a control counter during child process creation */
	int totalProcesses = 100;	/* Maximum number of processes allowed to be created in total */
	int maxCurrentProcesses = 5;	/* Default number of user processes to spawn. Can be changed if specified with the -s option.*/
	int killTime = 2;	/* Default number of the seconds after which the oss should terminate
				itself and any children alive at the time. Can be changed if specified
				with the -t option.
				*/
	char *logName = "";	/* Character array to be specified with -l option or default option */

	sem_unlink ( SEM_NAME );
	sem_t *sem = sem_open ( SEM_NAME, O_CREAT, 0777, 1 ); /* Creation of named semaphore to be used in oss and user */

	/* Loop to implement getopt to get any command-line options and/or arguments */
	/* Options -s, -l, and -t all require arguments */
	while ( ( opt = getopt ( argc, argv, "hs:l:t:" ) ) != -1 ) {
		switch ( opt ) {
			/* Display the help message */
			case 'h':
				printf ( "Program: ./oss\n" );
				printf ( "Options:\n" );
				printf ( "\t-h : display help message (currently viewing)\n" );
				printf ( "\t-s : specify the maximum number of user processes spawned\n" );
				printf ( "\t-l : specify the name of the log file to be used\n" );
				printf ( "\t-t : specify the time in seconds when the master will terminate itself and all children\n" );
				printf ( "\tNote: -s, -l, and -t all require arguments to be passed with them\n" );
				printf ( "\tNote: oss does not require any options. Default values are provided if not specified.\n" );
				printf ( "Example usage:\n" );
				printf ( "\t./oss -s 3 -l filename.txt -t 3\n" );
				printf ( "\toss will create a maximum of 3 child processes, log the terminated processes in filename.txt,\n" );
				printf ( "\tand terminate all processes after 3 seconds if program is still running.\n" );
				exit ( 0 );
				break;
			
			/* Specify the maximum number of user processes spawned */
			case 's':
				maxCurrentProcesses = atoi ( optarg++ );
				break;

			/* Specify the name of the log file to be used */
			case 'l':
				logName = optarg++;
				break;

			/* Specify the time in seconds when the master will terminate itself and all children */
			case 't':
				killTime = atoi ( optarg++ );
				break;

			default:
				break;
		}
	}

	/* Creates the default file name for the log file if no name is provided with -l option.*/
	if ( logName[0] == '\0' ) 
		logName = "log.txt";
	
	/* Now that the logfile's name is decided, the file  can be opened for writing */
	FILE *fp;
	
	fp = fopen ( logName, "w+" );

	/* Set alarm based on value of killTime */
	/* All processes will be terminated if anything is still running after killTime seconds */
	alarm ( killTime );

	/* Prints the stored values to check */
	printf ( "Total processes: %d\tMax processes: %d\tKill time: %d\tFile name: %s\n", totalProcesses, maxCurrentProcesses, killTime, logName );

	/* Variables for shared memory */
	int clockBuffer[2];	
	key_t key1 = 1993;
	int *shmClock;
	int shmClockID;
	
	int msgBuffer[3];
	key_t key2 = 1994;
	int *shmMsg;
	int shmMsgID;

	/* Allocation and initialization of shared memory for clock and message */
	/* Clock shared memory */
	shmClockID = shmget ( key1, sizeof ( clockBuffer ), IPC_CREAT | 0666 );
	shmClock = ( int *) shmat ( shmClockID, NULL, 0 );
	shmClock[0] = 0;
	shmClock[1] = 0;	

	/* Message shared memory */
	shmMsgID = shmget ( key2, sizeof ( msgBuffer ), IPC_CREAT | 0666 );
	shmMsg = ( int *) shmat ( shmMsgID, NULL, 0 );
	shmMsg[0] = 0;
	shmMsg[1] = 0;
	shmMsg[2] = 0;

	/* Setup for child process creation */
	char *args[] = { "./user", NULL };	/* Array with information for exec */
	pid_t pid;
	int status;

	/* Fork off the initial children as specified by maxCurrentProcesses */
	j = 0;
	for ( i = 0; i < totalProcesses; ++i ) {
		while ( ( j < maxCurrentProcesses ) && ( total < totalProcesses ) ) {
			pid = fork();
			/* In the child process */
			if ( pid == 0 ) {
				execv ( args[0], args );
				exit ( 127 );
			}
			
			/* The fork failed */
			else if ( pid < 0 ) {
				perror ( "Unable to fork" );
				exit ( -1 );
			}
			j++;
			total++;
		}
		
		shmClock[1] = shmClock[1] + 1000;
		while ( shmClock[1] > 1000000 ) {
			shmClock[0]++;
			shmClock[1] = shmClock[1] - 1000000;
		}		
		
		wait( 0 );
		j--; 

		if ( shmMsg[0] != 0 ) {
			fprintf ( fp, "OSS: Child %d is terminating at %d.%d because it reached %d.%d in the user.\n\n", shmMsg[0], shmClock[0], shmClock[1], shmMsg[1], shmMsg[2] );
			shmMsg[0] = 0;
		}
	}
	
	printf ( "Parent %d is exiting...\n", getpid() );

	/* Close the logfile */
	fclose ( fp );

	/* Unlink the semaphore */
	sem_unlink ( SEM_NAME );

	/* Deallocation of shared memory segments */
	shmdt ( shmClock);
	shmdt ( shmMsg );
	shmctl ( shmClockID, IPC_RMID, NULL );
	shmctl ( shmMsgID, IPC_RMID, NULL );
	
	return 0;
}
