////////////////////////////////////////////////////////////////////////////////////
// Filename:		my_thread.h
// Author:			Charles Nelson - s2884951
// Date Modified:	01/10/2014 Created
//					29/10/2014 Updated 
// Description:		This file contains the defines and includes for all files, and shared Memory variables
////////////////////////////////////////////////////////////////////////////////////
#define	SH_MEM_READY		-1
#define	SH_MEM_NOT_READY	-2

#define BUSY				1
#define NOT_BUSY			0

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
	char serverBusy;					//- Server is busy and can not take any more query's atm
	char clientShutDown;					//- Lets the server know the client has shut down
	char clientFlag;					//- flag to tell server or client when to read/write from the variable number
	char serverFlag[10];					//- flag to tell server or client when to read/write from the variable clientQueryResponse[10]
	double cqr_Progress[10];				//- informs the client how each Query is progressing e.g - clientQueryResponse
	
	unsigned long number;					//- stores the user's 32bit int that is to be factorised
    unsigned long clientQueryResponse[10];	//- stores the clients quieres
};

///////////////////////////////////////////////////////////////////////////////////
// END OF FILE
///////////////////////////////////////////////////////////////////////////////////
