////////////////////////////////////////////////////////////////////////////////////
// Filename:		my_mutex.c
// Author:			Charles Nelson - s2884951
// Date Modified:	01/10/2014 Created
//					29/10/2014 Updated 
// Description:		This file contains the mutex functions used for my Mutex Struct/Class
////////////////////////////////////////////////////////////////////////////////////
#include  "my_mutex.h"

#if WIN32

#else

	void m_lock( Mutex* m_mutex )											//- Locks the mutex
	{
		pthread_mutex_lock( &( m_mutex->mutex ) );
	}

	void m_unlock ( Mutex* m_mutex )										//- Unlocks the mutex
	{
		pthread_mutex_unlock( &( m_mutex->mutex ) );
	}

	void m_wait( Mutex* m_mutex )											//- Wait's on condition
	{
		int rCode = pthread_cond_wait( &( m_mutex->condition ), &( m_mutex->mutex ) );
	}

	void m_signal( Mutex* m_mutex )											//- Signals to the threads to wakeup
	{
		pthread_cond_signal( &( m_mutex->condition ) );
	}
		
	void m_broadcast( Mutex* m_mutex )										//- BroadCast's the signal to everyone
	{
		pthread_cond_signal( &( m_mutex->condition ) );
	}

	void createMutex( Mutex* m_mutex )										//- Creates and Initialising the Mutex 
	{
		int rCodeM = pthread_mutex_init( &( m_mutex->mutex ), NULL );						//- Initilaising the mutex 
		int rCodeC = pthread_cond_init(  &( m_mutex->condition ), NULL );					//- Initialising the condition variable
	}

	void destroyMutex( Mutex* m_mutex )										//- Destroys the struct Mutex's mutex and condition var
	{
		pthread_mutex_destroy( &( m_mutex->mutex ) );								//-Destroys the mutex
		pthread_cond_destroy( &( m_mutex->condition ) );							//-Destroys the condition variable
	}
#endif															//- END OF MUTEX CLASS SECTION
	
///////////////////////////////////////////////////////////////////////////////////
// END OF FILE
///////////////////////////////////////////////////////////////////////////////////
