////////////////////////////////////////////////////////////////////////////////////
// Filename:		my_thread.h
// Author:			Charles Nelson - s2884951
// Date Modified:	01/10/2014 Created
//					29/10/2014 Updated 
// Description:		This file contains the prototypes for the Thread struct/class
////////////////////////////////////////////////////////////////////////////////////
#ifndef my_thread
#define my_thread
	#include  "my_semaphore.h"

	//-Globals /////////////////
	Semaphore thread_semaphore;										//- Semaphore for syncronisation between threads
	struct threadProgress* thread_Progress;							//- Array of structs to be malloced
	int threadProgressChecker[10];									//- Array to check the threads progress
	unsigned long testHarnessResults[300];							//- This stores the results of our testHarnessResults

	struct threadProgress		
	{
		double percentComplete;										//- The number the client supplied.
		int allocatedSlot;											//- The slot that it is writing the response too
		unsigned int threadID;										//- The threads id
	};

	void initTreadProgress		( struct threadProgress*, int );	//- Initialised the struct that Holds info for the threads to report their individual progress to the main thread
	void destroythreadProgress	( struct threadProgress* );			//- Holds a struct for the threads to report their individual progress to the main thread
	void* thread_worker			( void * data );					//- Initialising function for creating the threads

#if WIN32
		//- TO DO - CREATE THREAD FUNCTION FOR WINDOWS
#else
	pthread_t createThreads(void * ( *function ) ( void * ), void * data);
#endif //- END OF UNIX FUNCTIONS

#endif //- #ifndef my_thread.h

///////////////////////////////////////////////////////////////////////////////////
// END OF FILE
///////////////////////////////////////////////////////////////////////////////////
