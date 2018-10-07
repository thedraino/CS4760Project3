/* Name: Andrew Audrain */
/* File: oss.c  |  Executable (after make): oss */
/* Date created: 10/06/18 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main ( int argc, char *argv[] ) {
	printf ( "Hello, from oss.\n" );
	int c;	/* Controls the getopt loop */
	int maxProcesses = 5;	/* Default number of user processes to spawn. Can be changed if 
				specified with the -s option. 
				*/
	int killTime = 2;	/* Default number of the seconds after which the oss should terminate
				itself and any children alive at the time. Can be changed if specified
				with the -t option.
				*/
	char *logName = "";	/* Character array to be specified with -l option or default option */

	/* Loop to implement getopt to get any command-line options and/or arguments */
	/* Options -s, -l, and -t all require arguments */
	while ( ( c = getopt ( argc, argv, "hs:l:t:" ) ) != -1 ) {
		switch ( c ) {
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
				maxProcesses = atoi ( optarg++ );
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

	printf ( "Max Processes: %d\n", maxProcesses );
	printf ( "Terminate Time: %d\n", killTime );
	printf ( "Log file: %s\n", logName );

	return 0;
}
