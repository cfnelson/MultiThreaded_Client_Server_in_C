////////////////////////////////////////////////////////////////////////////////////
// Filename:		my_thread.c
// Author:			Charles Nelson - s2884951
// Date Modified:	01/10/2014 Created
//					29/10/2014 Updated 
// Description:		This file contains the thread functions
////////////////////////////////////////////////////////////////////////////////////
#include  "my_thread.h"

#if WIN32

#else																								//- UNIX threads 

	pthread_t createThreads(void * ( *function )( void * ), void * data)							//- Initialising function for creating the threads
	{
		pthread_t pid;																				//- Threads Id
		pthread_create(&pid, NULL, function, data);													//- Creating the thread with the passed function f
		return pid;																					//- Returning the thread's id
	}

	void initTreadProgress(struct threadProgress* t_progress, int numThreads )						//- Initialised the struct that Holds info for the threads to report their individual progress to the main thread
	{
		int i;
		for ( i = 0; i < numThreads; i++ )															//- Create the same number of threads as the numThreads
		{
			t_progress[i].percentComplete = 0;														//- Setting to 0 as all progress starts at 0%
			t_progress[i].allocatedSlot = -1;														//- Setting to -1 as it is outside the value range
			t_progress[i].threadID = -1;															//- Initialising the thread
		}
	}

	void destroythreadProgress(struct threadProgress* t_progress )									//- Holds a struct for the threads to report their individual progress to the main thread
	{
		free( t_progress );
	}

	void * thread_worker ( void * data )															//- Worker thread that completes the trial division
	{
		Job my_job;
		int i = 0;
		int threadProgressSlot = -1;																//- Initialising to outside our range, stores the index of the thread
		
		while( 1 )																					//- Loops forever, thread worker will go to sleep and get woken when more jobs avaliable
		{
			s_wait(&thread_semaphore);																//- Waiting for a job, unitl woken by semaphore signal
			
			m_lock(&job_queue_access);																//- Locking the queue

			if ( threadProgressSlot == -1 )															//- CSheck to see if we have found a spot for ourselves yet
			{
				while( thread_Progress[i].threadID != -1 )
				{
					i++;
				}
				threadProgressSlot = i;																//- Storing the index of our slot id
				thread_Progress[threadProgressSlot].threadID = (unsigned int) pthread_self();		//- Assigning an id
			}

			if ( job_Queue.size > 0 )																//- Checks to see if there is a job
			{ 
				my_job = pop(&job_Queue);															//- Getting the job off the list so we can use it and removing it at same time
				m_unlock(&job_queue_access);
				thread_Progress[threadProgressSlot].allocatedSlot = my_job.querySlot;
				//printf("querySlot = %d\n", thread_Progress[threadProgressSlot].allocatedSlot);
				//printf("my_Job: numsToFactorise %d\n",my_job.numsToFactorise);
				/* printf(	"TID: %u - ThreadProgressSlot = %u \n",
					(unsigned int) pthread_self() , threadProgressSlot ); */
				if ( my_job.originalNumber == -6 )													//- Test Mode Operation
				{
					printf( "TEST MODE Of WORKER THREAD#: %lu - HAS STARTED\n", 
							(unsigned long) pthread_self() );
					unsigned long k;
					m_lock( &job_mailSlot_access[my_job.querySlot] );								//- Locking the completed jobs mailslots
					
					for ( k = 0; k < 10; k++ ) 
					{
						while ( SHM_PTR->serverFlag[my_job.querySlot] == NUM_FILLED  )
						{
							usleep(20);																//- Going to sleep for 20 microseconds
						}
						SHM_PTR->clientQueryResponse[my_job.querySlot] =  
							( ( (unsigned long) k ) + 
							( (unsigned long) pthread_self() * 10 ) );								//- Sending the value to the slot.
						SHM_PTR->serverFlag[my_job.querySlot] = NUM_FILLED;							//- Setting the value of the serverFlag to 1, so the client will read from it.
					}
					threadProgressChecker[ my_job.querySlot ] += 1;					
					m_unlock(&job_mailSlot_access[my_job.querySlot]);								//- Access the finished jobs list
				}
				else if (my_job.originalNumber == -5)												//- TEST HARNESS with TEST MODE
				{
					printf(	"TEST HARNESS/TEST MODE Of WORKER THREAD#: %lu - HAS STARTED\n",
							(unsigned long) pthread_self());
					unsigned long k;
					m_lock(&job_test_harness);														//- Locking the job_test_harness mutex so we can store the result					
					
					for ( k = 0; k < 10; k++ ) 
					{
						int p;
						for ( p = 0; p < 300; p++ )
						{
							if ( testHarnessResults[p] == 0 ) 
							{
								testHarnessResults[p] = ( ( ( unsigned long ) k ) + 
														(	( unsigned long ) pthread_self() * 
															( unsigned long ) 10 ) );				//- Sending the value to the slot.
								printf(	"TID = %u,  testHarnessResults[p] %lu \n", 
										(unsigned int) pthread_self(), testHarnessResults[p]);
								break;
							}
						}
					}
					threadProgressChecker[ my_job.querySlot ] += 10;	
					printf(	"THREAD PROGRESS CHECKER = %lu\n",
							threadProgressChecker[ my_job.querySlot ] ); 
					m_unlock(&job_test_harness);													//- Finished accessing the finished test harness job array
				}
				else																				//- Normal Operation 
				{
					if ( thread_trial_division(my_job, threadProgressSlot) )						//- Complete Trial Division and return the responses to the client
					{
						thread_Progress[threadProgressSlot].percentComplete = 100.00;
						threadProgressChecker[ my_job.querySlot ] += 1;								//- Iterating the value here up to 32, when 32 is reached its 100%
						printf(	"\nSUCCES: Finished thread_trial_division for %lu, Original Num = %d \n", 
								my_job.numsToFactorise, my_job.originalNumber );
					}
					else
					{
						printf("\nFAILED: Finished thread_trial_division\n");
					}
				}
			}
		}
	}
#endif																								//- End of Unix Code

///////////////////////////////////////////////////////////////////////////////////
// END OF FILE
///////////////////////////////////////////////////////////////////////////////////
