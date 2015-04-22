////////////////////////////////////////////////////////////////////////////////////
// Filename:		my_semaphore.c
// Author:			Charles Nelson - s2884951
// Date Modified:	01/10/2014 Created
//					29/10/2014 Updated 
// Description:		This file contains the Semaphore functions for my Semaphore struct/class
////////////////////////////////////////////////////////////////////////////////////
#include  "my_semaphore.h"

#if WIN32

#else
	void s_wait ( Semaphore* semaphore )				//- Makes the semaphore wait
	{
		m_lock( &(semaphore->s_mutex) );			//- Locking the mutex
		
		while ( semaphore->value <= 0 )				//- We wait whilst there are no jobs
		{
			m_wait( &( semaphore->s_mutex ) );		//- Calling the wait function
		}
		(semaphore->value)--;	
		m_unlock( &( semaphore->s_mutex ) );			//- Unlocking the mutex
	}

	void s_signal (Semaphore* semaphore)				//- SIgnals the semaphore
	{
	    m_lock( &(semaphore->s_mutex) );				//- Locking the mutex	
		semaphore->value++;					//- Iterating the value by 1
		m_signal( &( semaphore->s_mutex ) );			//- Sending a signal
		m_unlock( &( semaphore->s_mutex ) );			//- Unlocking the mutex
	}

	void createSemaphore( Semaphore* semaphore, int inputValue )	//- Creates the Semaphore
	{
		semaphore->value = 0;					//- Initialsing the value
		semaphore->wakeups = 0;					//- Initialsing the value
		createMutex( &(semaphore->s_mutex) );			//- Initialsing the value
	}

	void destroySemaphore ( Semaphore* semaphore )			//- Destroys the Semaphore
	{
		destroyMutex( &( semaphore->s_mutex ) ); 
	}

#endif									//- END OF SEMAPHORE CLASS SECTION
	
///////////////////////////////////////////////////////////////////////////////////
// END OF FILE
///////////////////////////////////////////////////////////////////////////////////
