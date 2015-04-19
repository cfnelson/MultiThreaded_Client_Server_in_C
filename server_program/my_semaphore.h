////////////////////////////////////////////////////////////////////////////////////
// Filename:		my_semaphore.h
// Author:			Charles Nelson - s2884951
// Date Modified:	01/10/2014 Created
//					29/10/2014 Updated 
// Description:		This file contains the prototypes for the my_semaphore struct/class
////////////////////////////////////////////////////////////////////////////////////
#ifndef my_semaphore
#define my_semaphore

	#include  "my_mutex.h"

	/*** Globals ***/
	Mutex job_queue_access;							//- Mutex for accessing the pending queue
	Mutex job_mailSlot_access[10];					//- Mutex for accessing the mailslots
	Mutex job_test_harness;							//- Mutex for processing the test_harness

	typedef struct Semaphore						//- Semaphore class made using C struct
	{
		Mutex s_mutex;								//- Mutex for controlling access, contains condition variable
		int value;									//- Current value of the semaphore 
		int wakeups;								//- Current Count of pending signals to wake the threads up.

		void (*s_wait)		(struct Semaphore*);	//- Locks the mutex and waits  while value <= 0
		void (*s_signal)	(struct Semaphore*);	//- Signals all the threads to wake up
    
	} Semaphore;

	void s_wait				( Semaphore* semaphore );
	void s_signal			( Semaphore* semaphore );

	void createSemaphore	( Semaphore* semaphore, int inputValue );
	void destroySemaphore	( Semaphore* semaphore );

#endif												//- #ifndef my_semaphore.h

///////////////////////////////////////////////////////////////////////////////////
// END OF FILE
///////////////////////////////////////////////////////////////////////////////////
