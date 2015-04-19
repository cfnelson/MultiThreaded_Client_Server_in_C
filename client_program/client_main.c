////////////////////////////////////////////////////////////////////////////////////
// Filename:		client_main.c
// Author:			Charles Nelson - s2884951
// Date Modified:	01/10/2014 Created
//					29/10/2014 Updated 
// Description:		This file contains the main function for the client application
////////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include  "shared_memory.h"
	struct sharedMemory* SHM_PTR;		//- pointer to sharedmemory
	#define MAX_KEYBOARD_BUFFER 1000	//- setting the max size for keyboard input
#if WIN32
	#include <conio.h>					//- khbit(), ...etc
	#include <Windows.h>				//- ZeroMemory(), ...etc
	#include <direct.h>					//- _getcwdf (used to get current directory), 
	#define bzero ZeroMemory			//- Defining bzero for compatablitiy with windows
	#define GetCurrentDir _getcwd		//- For retrieving the current directoy of the .exe
#else
	#include <unistd.h>					//- Essential POSIX functions & Constants
	#include <termios.h>				//- Enables teriminal I/O interfaces
	#include <sys/time.h>				//- Time & Date functions
	#include <dirent.h>					//- opening & listing Directories
	#include <sys/types.h>				//- Various different DataTypes
	#include <sys/ipc.h>				//- POSIX - for IPC
	#include <sys/shm.h>				//- POSIX - for sharedmemory IPC
	#include <sys/msg.h>				//- POSIX message queues
	#include <sys/msg.h>				//- POSIX semaphores
	
	/////////////////////////////- Below used for disabling io blocking e.g stdin/stdout
	#define STDIN_FILENO 0
	#define NB_ENABLE 1
	#define NB_DISABLE 0		
	/////////////////////////////- END IO Blocking defines
	
	#define GetCurrentDir getcwd				//- For retrieving the current directoy of the .exe

	int SHM_ID;									//- UNIX - Shared memory id
	//----------------------------------
	//--- check for keypress     -------
	//----------------------------------
	int kbhit()
	{
		struct timeval tv;
		fd_set fds;
		tv.tv_sec = 0;
		tv.tv_usec = 0;
		FD_ZERO(&fds);
		FD_SET(STDIN_FILENO, &fds); 
		select(STDIN_FILENO+1, &fds, NULL, NULL, &tv);
		return FD_ISSET(STDIN_FILENO, &fds);
	}

	//----------------------------------
	//--- allow nonblocking reading ----
	//----------------------------------
	void nonblock(int state)
	{
		struct termios ttystate;
		tcgetattr(STDIN_FILENO, &ttystate);													//- Get the terminal state

		if (state == NB_ENABLE)
		{
			ttystate.c_lflag &= ~ICANON;													//- Turn off canonical mode
			ttystate.c_cc[VMIN] = 1;														//- Minimum input chars read
		}
		else if (state==NB_DISABLE)
		{
		   ttystate.c_lflag |= ICANON;														//- Turn on canonical mode
		}
		
		tcsetattr(STDIN_FILENO, TCSANOW, &ttystate);										//- Set the terminal attributes.
	}

	void destroySharedMemory( void )
	{
		SHM_PTR->clientFlag  = SHUTDOWN;													//- Telling the server to shutdown as we have exited
		printf("Client: Destroying Shared Memory.\n");
		shmdt( (void *) SHM_PTR );
		printf("Client: Successfully detached its shared memory.\n");
		shmctl(SHM_ID, IPC_RMID, NULL);
		printf("Client: Successfully removed its shared memory.\n");
		printf("Client: Exiting...\n");
	}

	int createSharedMemory (  )
	{
		int i;
		key_t          shmKEY;
		//struct Memory  *SHM_PTR;

		shmKEY = ftok(".", 'x');
		printf("CLIENT hello herekey_t = %d \n", (int) shmKEY);
		SHM_ID = shmget(shmKEY, sizeof(struct sharedMemory), IPC_CREAT | 0666);
		printf("CLIENT after shmget\n");
		if (SHM_ID < 0) 
		{
			printf("*** shmget error (client) ***\n");
			exit(1);
		}
		printf("Client has allocated space for sharedMemory struct.\n");

		SHM_PTR = (struct sharedMemory *) shmat(SHM_ID, NULL, 0);							//- Opening and attaching to the shared memory
		if ( (int) SHM_PTR == -1 ) 
		{
			printf("*** shmat error (client) ***\n");
			exit(1);
		}
		printf("Client has attached to the shared memory.\n");

		/** Initialising values for the shared memory here **/
		SHM_PTR->number			= NUM_EMPTY;												//- Setting all to the value of 0
		SHM_PTR->serverBusy		= NUM_EMPTY;												//- Setting all to the value of 0
		SHM_PTR->clientFlag		= NUM_EMPTY;												//- Setting all to the value of 0
		SHM_PTR->clientShutDown = RUNNING;													//- Setting all to the value of 0
		
		for ( i = 0; i < 10; i++ )
		{
			SHM_PTR->serverFlag[i]			= NUM_EMPTY;									//- Setting all to the value of 0
			SHM_PTR->cqr_Progress[i]		= NUM_EMPTY;									//- Setting all to the value of 0
			SHM_PTR->clientQueryResponse[i] = NUM_EMPTY;									//- Setting all to the value of 0
		}
		printf("Client has initialised shared memory.\n");
		return 1;																			//- Returns 1 on success
	}

#endif

	void startServer( char* numThreads )													//- Starts the server, takes the number of threads to start the server with 
	{
		char argbuf[256];																	//- Used to pass argv to WIN32 CreateProcess
		char cCurrentPath[FILENAME_MAX];													//- Current dir folder
#if WIN32
		STARTUPINFO startupInfo;															//- Startup info
		PROCESS_INFORMATION processInfo;													//- Process info child process
		
		ZeroMemory(&startupInfo, sizeof(startupInfo));										//- Clearing the memory
		startupInfo.cb = sizeof(startupInfo);												//- Setting the startup info for the child to inherit ?
#endif
		printf("Inside the Start Server function\n");
		
		if ( !GetCurrentDir( cCurrentPath, sizeof(cCurrentPath) ) )							//- Retrieving the current programs file path. e.g currentDir
		{
			printf("Failed to retrieve currentDir Path");
			exit(EXIT_FAILURE);																//- Has failed
		}
		cCurrentPath[sizeof(cCurrentPath) - 1] = '\0';										//- Adding to the end of the string
#if WIN32
		strncat( cCurrentPath, "\\server.exe", 12 );										//- Appending the server's details

		/** TO DO ------- NOTE DOES NOT WORK WITHIN VISUAL STUDIO, RETURNS ERROR CODE, PROGRAM FUNCTIONS NORMALLY OUTSIDE OF VISUAL STUDIO. **/
		sprintf(argbuf,"%s %s", cCurrentPath, numThreads);
		if ( !CreateProcess(	NULL, argbuf ,												//- Argv commands to be passed to the process
			NULL, NULL,																		//- Processes and thread handles not inheritable
			TRUE,																			//- Set inherit handles to true
			0, NULL,																		//- No creation flags, use the parents environment			
			NULL,																			//- Use parent's starting directory
			&startupInfo, &processInfo) )													//- Pass the startupInfo and processInfo
		{
			fprintf(stderr,"createprocess failed %d\n",GetLastError());
			system("PAUSE");
			exit(EXIT_FAILURE);																//- Creation process has failed 
		}
#else
		strncat( cCurrentPath, "/server", 7 );												//- Appending the server's details

		int pid = fork();																	//- Fork program so we can start the server and have client running at same time
		if ( pid < 0 )																		//- Error out as it is not in the accepted range of 0 or Greater
		{
			perror( "ERROR on fork\n" );													//- Error Message to the console screen
			exit( EXIT_FAILURE );															//- Exit the process
		}
		else if ( pid == 0 )																//- Child Process is here - Is the Server
		{
			int err = execvp( cCurrentPath, ( char *[] ) 
							{ cCurrentPath, numThreads, NULL }  );							//- Starting the server program with the sepcified number of threads
			printf("Unrecoverable Error: unable to start server, execvp has failed.\n");
			printf("err = %d\n", err);
			exit( EXIT_FAILURE );															//- Exiting the client program
		}
		else																				//- Parent process - Client process
		{
			printf("Server Started\n");														//- We do nothing and client program continues on
		}
#endif
	}	

int checkNumThreadsIsDigit (char* s_token)													//- Returns the numThreads user has specified, prints out warning if not digit
{
	int i = 0;																				//- Simple iterator
	int numThreads = 32;																	//- Set numThreads to default value
	printf("checkNumThreadIsDigit s_token = %s\n",s_token);
	if ( ( atoi(s_token) == -1 ) )
	{
		numThreads = -1;
	}
	else
	{
		while ( s_token[i] != '\0' && s_token[i] != '\n' )									//- Loop until you reach the end of the string
		{
			if (  isdigit( (int) s_token[i] ) )												//- Checking to see if the char is a digit
			{
				i++;																		//- Iterating to the next char
			}
			else
			{
				printf("checkNumThreadsDigit = %c \n", s_token[i]);
				printf("Warning you have not entered a integer value.\n");
				printf("Number of threads used will now be set to 32.\n");
				return numThreads;															//- Returning the defualt value
			}	
		}
		numThreads = atoi(s_token);															//- Setting the numThreads to the user specified thread, converts the string to an int.
	}
	return numThreads;																		//- Returning the user entered thread count.
}

int checkIfDigit (char* s_token)															//- Returns the numThreads user has specified, prints out warning if not digit
{
	int i = 0;																				//- Simple iterator
	int isDigit = 1;																		//- isDigit is True
	int notDigit = 0;																		//- Not a digit e.g false
	printf("checkIfDigit s_token = %s\n",s_token);

	while ( s_token[i] != '\0' && s_token[i] != '\n' )										//- Loop until you reach the end of the string
	{
		if ( isdigit( (int) s_token[i] ) )													//- Checking to see if the char is a digit
		{
			i++;																			//- Iterating to the next char
		}
		else
		{
			printf("checkIfDigit = %c \n", s_token[i]);
			printf("Warning you have not entered a integer value.\n");
			printf("Number of threads used will now be set to 32.\n");
			return notDigit;																//- Returns False
		}	
	}
	return isDigit;																			//- Returning the user entered thread count.
}

int getNumThreadsFromUser()
{
	int numThreads = 32;																	//- Set numThreads to default value
	
	char* s_token;																			//- Stores the token removed from the user line
	char keyboardBuffer[MAX_KEYBOARD_BUFFER];												//- Stores the users keyboard input

	fgets (keyboardBuffer, MAX_KEYBOARD_BUFFER, stdin);										//- Get the user's thread number
	
	s_token = strtok( keyboardBuffer, " " );												//- Splitting user input to check if valid input and only one input provided.	
	
	numThreads = checkNumThreadsIsDigit(s_token);											//- Checking to see if the user's entered numThreads value is a digit

	if ( numThreads < 1 && numThreads > 1000 )												//- Check to see if the numThreads entered by user is within the allowed range. If it is not then we warn them and reset to default of 32 threads.
	{
		printf("Max threads allowed is 1000.\n");
		printf("You have entered %d\n",numThreads);
		printf("Your calculation will now use 32 threads.\n");
		numThreads = 32;																	//- Resetting the default threads used to 32
	}

	if ( ( s_token = strtok(NULL, " ") ) != NULL )											//- Check to see the user has only entered 1 command, if hasnt then default of 32 is used.
	{
		printf("Warning you have entered more then one command.\n");
		printf("s_token = %s\n",s_token);
	}
	return numThreads;
}

struct clientQueryNumAndSlot
{
	unsigned long clientQueryNum;															//- The number the client supplied.
	int allocatedSlot;
	
	struct timeval start_time;
};

void createQueryNumAndSlot(struct clientQueryNumAndSlot* slot)
{
	slot->clientQueryNum = -2;																//- Setting to -2 as it is outside the value range
	slot->allocatedSlot = -2;																//- Setting to -2 as it is outside the value range
}

int main ( int argc, char* argv[] )
{
	int i ;
	int flagK = 0;
	int byteCounter = 0;																	//- Counts number of chars read in from line
	int intToFactorise = 0;																	//- Integer to factorise
	int numThreads = 32;																	//- numThreads for server to calculate with
	struct clientQueryNumAndSlot querySlotAndNum[10];										//- Slots and the client Query Number

	char c;																					//- Char to store the char read from stdin stream
	char* s_token;																			//- Stores the token removed from the user line
	
	char keyboardBuffer			[MAX_KEYBOARD_BUFFER];										//- Stores the users keyboard input
	char originalkeyboardBuffer	[MAX_KEYBOARD_BUFFER];										//- Stores the original copy of the users keyboard input
	char str_numThreads			[MAX_KEYBOARD_BUFFER];										//- String of the numThreads
	
	
	
#if !WIN32
	nonblock(NB_ENABLE);																	//- Setting the state of the stdin/stdout to be non blocking
#endif
	for ( i = 0; i < 10; i++ )																//- Looping to initialise the ClientQueryNumANdSlot array
	{
		createQueryNumAndSlot( &(querySlotAndNum[i]) );										//- Creating the slot and giving value of -2 which is outside expected value range
	}
	printf("Please enter the number of threads that the server should use. (Max: 1000 threads)\n");

	numThreads = getNumThreadsFromUser();													//- Getting the numThreads from the user, handles error checking
	printf("numThreads Returned = %d \n",numThreads);
	sprintf(str_numThreads,"%d",numThreads);												//- Converting the numThreads int to a string
	printf("numThreads passing to startServer = %s \n",str_numThreads);

#if WIN32
	//- TO DO - spawn the server.
	startServer(str_numThreads);															//- Start the server, takes the string version of the numThreads
#else
	createSharedMemory();																	//- creating the shared memory
	atexit(destroySharedMemory);
	startServer(str_numThreads);															//- Start the server, takes the string version of the numThreads	
#endif
	//- Cleaning/Clearing the buffers 
	bzero(keyboardBuffer,MAX_KEYBOARD_BUFFER);
	bzero(originalkeyboardBuffer,MAX_KEYBOARD_BUFFER);

	while (1)																				//- Clients main logic flow
	{
		if ( kbhit() )																		//- User input
		{
#if WIN32
			c = getch();																	//- Retrieve the char from the stdin
#else
			c = getc(stdin);																//- Retrieve the char from the stdin
#endif
	
#if WIN32
			if ( c == '\r' && byteCounter > 0)	
#else
			if ( c == '\n' && byteCounter > 0)												//- The enter button has been pressed
#endif
			{
				intToFactorise = 0;															//- Resetting the value to 0
				keyboardBuffer[byteCounter] = '\0';											//- Setting the end of the string with the null char character
				printf("This is the keyboard Buffer: %s\n\n",keyboardBuffer);
				strncpy( originalkeyboardBuffer, keyboardBuffer, MAX_KEYBOARD_BUFFER );		//- Storing the original message so that we can use it later on.  
				s_token = strtok( originalkeyboardBuffer, " " );							//- Splits user input, returns the user's 32bit integer
				
				if ( checkIfDigit(s_token) )												//- Checking to see if the string is a digit
				{
					intToFactorise = atoi(s_token);											//- Converts the string to an int
					if ( ( s_token = strtok(NULL, " ") ) != NULL )							//- Check to see the user has only entered 1 command, if hasnt then default of 32 is used.
					{
						printf("Warning you have entered more then one command.\n");
						printf("We will only use the first integer entered.\n");
					}
					printf("CLIENT: SHM_PRT = %d\n",SHM_PTR);
					//- LOGIC FOR PASSING THE INPUTTED VALUE TO THE SERVER
					if ( SHM_PTR->serverBusy == BUSY )										//- Checking to see if the server is full & Busy
					{
						printf("CLIENT: The server is currently busy with 10 queries, please try again later.\n");
					}
					else if ( SHM_PTR->clientFlag == NUM_FILLED && SHM_PTR->serverBusy == NOT_BUSY )
					{
						printf("CLIENT: The server has not processed your last number, please try again later.\n");
					}
					else if ( SHM_PTR->clientFlag == NUM_EMPTY && SHM_PTR->serverBusy == NOT_BUSY	)
					{
						SHM_PTR->number = (unsigned long) intToFactorise;					//- Place the intToFactorise into the number var
						SHM_PTR->clientFlag = NUM_FILLED;									//- Telling server that the number is ready to read 
						printf("CLIENT: Successfully filled the NUMBER value in shm \n");
						while ( SHM_PTR->clientFlag == NUM_FILLED )							//- We wait until the server has allocated the free slot.
						{
							#if WIN32
								//- TODO WINDOWS //- sleep
							#else
								usleep(100);												//- Going to sleep for 500 microseconds
							#endif
						}
						
						querySlotAndNum[SHM_PTR->number].allocatedSlot = SHM_PTR->number; 
						querySlotAndNum[SHM_PTR->number].clientQueryNum = (unsigned long) intToFactorise;
					#if WIN32
						//- TODO WINDOWS - time stamp of when it was started
					#else
						gettimeofday(&(querySlotAndNum[SHM_PTR->number].start_time),NULL);
						//tv.tv_sec		// seconds
						//tv.tv_usec	// microseconds
					#endif
						SHM_PTR->number = NUM_EMPTY;										//- Clearing the input in the number
					}
				}
				else
				{
					printf("Warning you have not entered a postive integer value.\n");
					printf("Please enter a positive integer value\n");
				}
				//- Clearing the memory
				bzero(keyboardBuffer,MAX_KEYBOARD_BUFFER);
				bzero(originalkeyboardBuffer,MAX_KEYBOARD_BUFFER);
				byteCounter = 0;															//- Resetting the byteCounter, as we have sent off the user's input to the server
			}
			else																			//- We store the char in our buffer until enter has been hit.
			{
				if ( c == 'q' )																//- Checks to see if the user has entered 'q' to quit.
				{
					//- TO DO CODE 
					//- For quiting the threads gracefully
					printf("Client asking to quit\n");
					//SHM_PTR->clientShutDown = SHUTDOWN; 
					break;																	//- Break out of the loop so that we can quit etc.
				}
#if WIN32
				if ( c == '\r' )	
#else
				if ( c == '\n' )
#endif
				{
					bzero(keyboardBuffer,MAX_KEYBOARD_BUFFER);
					bzero(originalkeyboardBuffer,MAX_KEYBOARD_BUFFER);
					//printf("User trying to enter an empty argument = %c\n", c);
				}
				else																		//- User have entered a char and its not '\n'
				{
					keyboardBuffer[byteCounter] = c;										//- Adding the char just read to the the  c string str
					byteCounter++;															//- Counting number of bytes
					if ( byteCounter == ( ( MAX_KEYBOARD_BUFFER ) - 1) )					//- used to handle error of buffer overflow
					{
						keyboardBuffer[MAX_KEYBOARD_BUFFER] = '\0';							//- setting the null char for string
						printf("Your command is longer then the allowed max input.\n" );
						printf("Please re-enter a new shorter command.\n");
						bzero(keyboardBuffer,MAX_KEYBOARD_BUFFER);
						bzero(originalkeyboardBuffer,MAX_KEYBOARD_BUFFER);
						byteCounter = 0;													//- resetting the count to 0, as we want user to re-enter correct amount of numbers
					}
				}																			//- END ELSE
			}																				//- END ELSE
		}																					//- END of kbhit()
		
		for ( i = 0; i < 10; i++ )															//- Looping through all the slots, to check if they shoudl be read.
		{
			if ( SHM_PTR->serverFlag[i]	== NUM_FILLED )										//- Checking to see if one is filled
			{
				printf("Slot[%d], OrigNum: %lu, Factor: %lu ",i, querySlotAndNum[i].clientQueryNum, SHM_PTR->clientQueryResponse[i]);
				SHM_PTR->clientQueryResponse[i] = NUM_EMPTY;		
				SHM_PTR->serverFlag[i]			= NUM_EMPTY;
				//printf("Client: mailSlot[%d] After Reset: Value = %lu\n",i, SHM_PTR->clientQueryResponse[i]);
				flagK = -1;
			}
		}
		printf("\r");
		fflush(stdout);
		if (flagK == -1) 
		{
			printf("\n");
			flagK = 0;
		}
		for ( i = 0; i < 10; i++ )															//- Printing Percentage for the Overall query
		{
			if ( SHM_PTR->cqr_Progress[i] > 0.00  )											//- Only display active query's that have started
			{
				printf ( "Query #%d, Progress: %.4f%% ", i,  (double) SHM_PTR->cqr_Progress[i] );
			}
		}
		printf("\r");
		fflush(stdout); 

		for ( i = 0; i < 10; i++ )															//- Printing Percentage for the Overall query
		{
			if ( SHM_PTR->cqr_Progress[i] == 100.00 )										//- Checking to see if we are at 100%
				{
				
				#if WIN32
					//- TODO - TIME STRUCT for windows
				#else
					long seconds;
					long useconds;
					struct timeval end_time;
					gettimeofday(&end_time,NULL);
					seconds = end_time.tv_sec - querySlotAndNum[i].start_time.tv_sec;		//-  Seconds
					useconds = end_time.tv_usec - querySlotAndNum[i].start_time.tv_usec;	//-  Microseconds
					printf ("Query Completed! Orignal Num: %lu: Time Taken: %ld seconds and %ld microseconds\n",querySlotAndNum[i].clientQueryNum, seconds, useconds );
				#endif 
					SHM_PTR->cqr_Progress[i] = 0;											//- Resetting the value to 0 as we have alerted the client to its success.
				}
		}
#if WIN32

#else
		usleep(20);																			//- Going to sleep for 500 microseconds
#endif 
	}																						//- END WHILE
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////
// END OF FILE
///////////////////////////////////////////////////////////////////////////////////
