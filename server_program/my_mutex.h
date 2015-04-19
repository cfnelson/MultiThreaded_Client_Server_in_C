////////////////////////////////////////////////////////////////////////////////////
// Filename:		my_mutex.h
// Author:			Charles Nelson - s2884951
// Date Modified:	01/10/2014 Created
//					29/10/2014 Updated 
// Description:		This file contains the prototypes for the my_mutex struct/class
////////////////////////////////////////////////////////////////////////////////////
#ifndef my_mutex
#define my_mutex

	#include  "job_queue.h"

	/*** GLOBALS ***/ 
	Queue job_Queue;

	typedef struct Mutex								//- Mutex Class (C struct), with a condition variable which is used for implementing the Semaphore Class (C struct)
	{
#if WIN32	
		//- TO DO
#else	
		pthread_mutex_t mutex;							//- pthread mutex
		pthread_cond_t condition;						//- pthread condition variable
 #endif	
		void (*m_lock)			(struct Mutex*);		//- function pointer, function locks the mutex using pthreads
		void (*m_unlock)		(struct Mutex*);		//- function pointer, function unlocks the mutex using pthreads
		void (*m_wait)			(struct Mutex*);		//- function pointer,  function calls pthread waits
		void (*m_signal)		(struct Mutex*);		//- function pointer, function signals mutex using pthreads
		void (*m_broadcast)		(struct Mutex*);		//- function pointer, function broadcasts mutex using pthreads

	} Mutex;

	void m_lock					( Mutex*  );			//- Locks the mutex
	void m_unlock				( Mutex*  );			//- Unlocks the mutex
	void m_wait					( Mutex*  );			//- Wait's on condition
	void m_signal				( Mutex*  );			//- Signals to the threads to wakeup
	void m_broadcast			( Mutex*  );			//- BroadCast's the signal to everyone
	void createMutex			( Mutex*  );			//- Creates and Initialising the Mutex 
	void destroyMutex			( Mutex*  );			//- Destroys the struct Mutex's mutex and condition var
#endif													//- #ifndef my_mutex.h

///////////////////////////////////////////////////////////////////////////////////
// END OF FILE
///////////////////////////////////////////////////////////////////////////////////
