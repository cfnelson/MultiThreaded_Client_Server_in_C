////////////////////////////////////////////////////////////////////////////////////
// Filename:		my_thread.h
// Author:			Charles Nelson - s2884951
// Date Modified:	01/10/2014 Created
//					29/10/2014 Updated 
// Description:		This file contains the defines and includes for all files, and shared Memory variables
////////////////////////////////////////////////////////////////////////////////////
#ifndef shared_memory
#define shared_memory

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
	
#define DIR_LEFT 0
#define DIR_RIGHT 1
#define BUF_SIZE 33
#define SLOT_IN_USE 1
#define SLOT_NOT_IN_USE 0
#define NO_FREE_SLOTS -1
#define MAX_QUEUE_SIZE 320					//- MAX QUEUE SIZE IS 320, e.g - 10 slots x 32 bit shift
#define BIT_SHIFT_ROTATE_32 32				//- Shift by 32 bits/e.g 32 times from 0 - 31

struct sharedMemory* SHM_PTR;				//- Pointer to sharedmemory

#if WIN32									//- WIN32 INCLUDES & DEFINES

	#include <WinDef.h>

	#define byte BYTE						//- WIN32 typedef unsigned char BYTE;

#else										//- UNIX INCLUDES & DEFINES

	#include <pthread.h>					//- POSIX - threads
	#include <unistd.h>						//- Essential POSIX functions & Constants
	#include <termios.h>					//- Enables teriminal I/O interfaces
	#include <sys/time.h>					//- Time & Date functions
	#include <dirent.h>						//- Opening & listing Directories
	#include <sys/types.h>					//- Various different DataTypes
	#include <sys/ipc.h>					//- POSIX - for IPC
	#include <sys/shm.h>					//- POSIX - for sharedmemory IPC
	#include <sys/msg.h>					//- POSIX message queues
	#include <sys/msg.h>					//- POSIX semaphores
	#include <stdint.h>						//- For the type byte

	#define byte  uint8_t 


#endif										//- END OF INCLUDES & DEFINES

#define	SH_MEM_READY		-1
#define	SH_MEM_NOT_READY	-2

#define BUSY				1
#define NOT_BUSY			0

#define TEST_HARNESS		-3
#define NUM_FILLED			1
#define NUM_EMPTY			0

#define SHUTDOWN			1
#define RUNNING				0


//Both are initially 0 meaning that there is no new data available
//A client can only write data to ‘number’ for the server while clientflag == 0; the client must set clientflag = 1 to indicate to the server that new data is available for it to read
//The server will only read data from ‘number’ from the client if there is a free slot and if clientflag ==1. It will then write the index of the slot that will be used for the request back to ‘number’ and set clientflag = 0 to indicate that the request has been accepted.
//A server can only write data to slot x for the client while serverflag[x] == 0; the server must set serverflag[x] = 1 to indicate to the client that new data is available for it to read
//The client will only read data from slot x if serverflag[x] ==1 and will set serverflag[x] = 0 to indicate that the data has been read from slot x

struct sharedMemory 
{
	//int status;
	char serverBusy;						//- Server is busy and can not take any more query's atm
	int clientShutDown;						//- Lets the server know the client has shut down
	char clientFlag;						//- Flag to tell server or client when to read/write from the variable number
	char serverFlag[10];					//- Flag to tell server or client when to read/write from the variable clientQueryResponse[10]
	double cqr_Progress[10];				//- Informs the client how each Query is progressing e.g - clientQueryResponse
	
	unsigned long number;					//- Stores the user's 32bit int that is to be factorised
    unsigned long clientQueryResponse[10];	//- Stores the clients quieres
} ;

#endif										//- #ifndef shared_memory.h

///////////////////////////////////////////////////////////////////////////////////
// END OF FILE
///////////////////////////////////////////////////////////////////////////////////
