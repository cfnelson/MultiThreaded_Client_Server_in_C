////////////////////////////////////////////////////////////////////////////////////
// Filename:		server_main.c
// Author:			Charles Nelson - s2884951
// Date Modified:	01/10/2014 Created
//					29/10/2014 Updated 
// Description:		This file contains the main function for the server application
////////////////////////////////////////////////////////////////////////////////////

#include  "bitShift_rotate_and_factorisation.h" 

int createSharedMemory (  void )
{
	#if WIN32

#else 
	key_t          shmKEY;		//- Shared Memory Key
#endif
	
    int            SHM_ID;		//- Shared Memory ID 

	#if WIN32					//- WIN32 CODE FOR SHARED MEMORY
	//- TO DO CODE
#else 
	shmKEY = ftok(".", 'x');
    SHM_ID = shmget( shmKEY, sizeof(struct sharedMemory), 0666 );
#endif
	if (SHM_ID < 0) 
	{
        printf("*** shmget error (server) ***\n");
        exit(1);
    }
    printf("SERVER: Successfully received id\n");

#if WIN32
	//- TO DO CODE
#else 
    SHM_PTR = (struct sharedMemory *) shmat(SHM_ID, NULL, 0);
#endif
    if ( (int) SHM_PTR == -1) 
	{
        printf("*** shmat error (server) ***\n");
        exit(1);
    }
    printf("SERVER: Has attached to the shared memory...\n");
	return 1;
}

void destroySharedMemory( void )
{
	printf("Server: Freeing Queue.\n");
	printf("Server: Releasing Shared Memory.\n");
#if WIN32
	//- TO DO CODE
#else 
	shmdt( (void *) SHM_PTR );
#endif
	printf("Server: Successfully detached its shared memory.\n");
	printf("Server: Exiting...\n");
}

int findFreeSlot(int* slotsInUse)														//- Returns the slot to allocate to the job, returns -1 if no free slot
{
	int i;
	for ( i = 0; i < 10; i++ )
	{
		if ( slotsInUse[i] == SLOT_NOT_IN_USE )
		{
			slotsInUse[i] == SLOT_IN_USE;												//- Setting the slot to in use
			return i;
		}
	}
	return NO_FREE_SLOTS;																//- All slots are in use
}

int checkForOutStandingQueries( int* slotsInUse )										//- Checks to see if no queries are outstanding for the client, Returns the number of outstanding queries
{
	int i;
	int count = -1;
	for ( i = 0; i < 10; i++ )
	{
		if ( slotsInUse[i] == SLOT_IN_USE )
		{
			count++;																	//- Setting the slot to in use
		}
	}
	return count;																		//- All slots are in use
}

void normalModeOutput(int numThreads, int slotsInUse[] )								//- Reports the normal output and progress to the client
{
	int i,k;
	for ( i = 0; i < 10; i++ )															//- loop through the array and check the number of items in the job completed e.g completed items/ 32
	{
		SHM_PTR->cqr_Progress[ i ] = 0;
		if ( threadProgressChecker[ i ] >= 1 )
		{
			SHM_PTR->cqr_Progress[ i ] = ( (	(double) threadProgressChecker[ i ] /	
												(double) 32 ) * (double) 100 );
		}
	}																					//- END FOR LOOP 

	for ( i = 0; i < numThreads; i++ )													//- Percentage for the trial division
	{
		if ( thread_Progress[i].threadID != -1  )
		{
			for ( k = 0; k < 10; k++ )													//- We check our progress client array in shared memory. they share the same slot index
			{
				if ( thread_Progress[i].allocatedSlot == k )
				{
					if ( thread_Progress[i].percentComplete == ( double ) 100 )			//- If it is 100% we reset to 0, so we do not re-add the value to the progress
					{
        				thread_Progress[i].percentComplete = ( double ) 0;
					}
					double addPer = ( ( ( thread_Progress[i].percentComplete / 
									( double ) 100.00 ) / ( double ) 32.00 ) * 
									( double ) 100.00);
					SHM_PTR->cqr_Progress[ k ] += addPer;
					if ( SHM_PTR->cqr_Progress[ k ] == 100.00 )							//- Freeing the query slot, so that we can use it for other jobs
					{
						slotsInUse[k] = SLOT_NOT_IN_USE;								//- Setting the slot to not in use
						thread_Progress[i].allocatedSlot == -1;							//- Setting the slot to -1, so that it is outside the value range expected
					}
				}
			}																			//- END OF FOR LOOP K < 10 (NUM OF QUERY SLOTS)
		}	
	}																					//- END OF FOR LOOP I < NUMTHREADS
}

void testModeOutput( int* testHarness, int* testMode, int numThreads, int slotsInUse[] )//- Function containing logic for the TestHarness Output and Also the TestMode Output
{
	int i;
	for ( i = 0; i < 10; i++ )															//- Loop through the array and check the number of items in the job completed e.g completed items/ 32
	{
		if ( *testHarness == 1  )														//- Check if Test Harness is on
		{
			if ( threadProgressChecker[ i ] == 300 ) 
			{
				int n;
				int count = 0;
				for ( n = 0; n < 300; n++ )												//- SUCH ZEN CODE!!! EXTREMELY STYLISH AND EFFICIENT, Loop here and check for it all instead of recursion, to tired.
				{
					count = 1;
					int t;
					unsigned long checkThereIsThree = testHarnessResults[n];			//- Stores the result

					for (  t = 0; t < 300;  t++ )										//- 2nd level
					{
						int q;
						if ( checkThereIsThree == testHarnessResults[t] && t != n	)	//- checking to see if there is one that matches.
						{
							count++;													//- We have found one match so we increment the count
							for (  q = 0; q < 300 ;  q++ )								//- 3rd number
							{
								if (	checkThereIsThree == testHarnessResults[q] &&  
										q != t && q != n	)							//- Checking to see if there is one that matches.
								{
									count++;
									break;												//- Break out, to not loop unnecesarily 
								}
							}
							break;														//- Break out, to not loop unnecesarily 
						}
					}																	//- END OF FOR LOOP T < 300
							
					if ( count == 3 )													//- Check to see if the Numbers have been found 3 times
					{
						printf("FACTOR: FOUND 3 times.\n");
					}	
					else																//- Has not been found 3 times
					{
						printf(	"FACTOR %lu: Did not appear 3 times.\n", 
								testHarnessResults[n]);
						printf("Only found %d times.\n", count);
					}
					count = 0;													//- Reseting the count.
				}
				printf ("FINISHED TEST HARNESS & TEST MODE!!!\n");
				printf ("WE ARE NOW RETURNING TO NORMAL OPERATION.\n");
				*testMode = -1;													//- Reseting to normal mode
				slotsInUse[i] = SLOT_NOT_IN_USE;								//- Setting the slot to not in use
				threadProgressChecker[ i ] = 0;
				break;															//- Exit the for loop to save looping
			}
		}
		else 
		{
			if ( threadProgressChecker[ i ] == 30 )								//- If it has finished, e.g 30 jobs are complete
			{
				printf ("FINISHED TEST MODE!!!\n");
				*testMode = -1;													//- Reseting to normal mode
				slotsInUse[i] = SLOT_NOT_IN_USE;								//- Setting the slot to not in use
				threadProgressChecker[ i ] = 0;									//- Resetting to 0
				break;															//- Exit the for loop to save looping
			}																	//- END OF IF
		}																		//- END OF ELSE 
	}																			//- END OF MAIN FOR LOOP I < 300
}																				//- END OF FUNCTION testModeOutput

int main (int argc, char* argv[])												//- Main function starts here, client enters the number of threads to pass to the server
{
	int i,k;
	int no_TM_Q_Running = 1;													//- Setting testModeOkToRun to 1
	int numThreads = 32;
	int testMode = -1;															//- Bool check value for seeing if test mode is on, -1 is off and 1 is on.
	int testHarness = -1;														//- Bool check value for seeing if test harnes/ mode has been enabled on, -1 is off and 1 is on.
	char* str_numThreads;

	unsigned long numberToRotate = -1;
	unsigned long numsToFactorise[32] = {0};									//- Setting all too 0	

	int slotsInUse[10] = {0};													//- Setting all too 0	
	

	int checkQueueAlmostFull = MAX_QUEUE_SIZE - 32;								//- Queue is almost full, one space left

	if ( argc != 2  )
	{
		printf("Server: Error: Usage invalid,");
		printf(" expecting one commandline argument.\n");
		printf("Server: Correct Usage: number of threads\n");
		return -1;																//- Exit with error code -1
	}

	str_numThreads = argv[1];
	printf("Server: numThreads to start = %s\n",str_numThreads);
	numThreads = atoi(str_numThreads);

	if ( numThreads == -1 )														//- TEST HARNESS/TEST MODE HAS BEEN ENABLED
	{
		numThreads = 10;
		testMode = 1;															//- Setting test mode
		testHarness = 1;														//- Setting test harness to true 
		printf("SERVER: TEST HARNESS ENABLED\n");
		for (i = 0; i < 300; i++)												//- Clearning the array of values
		{
			testHarnessResults[i] = 0;
		}
	}

	thread_Progress = (	struct threadProgress* ) malloc ( 
						numThreads * sizeof( struct threadProgress ) );			//- Alocating the memory space required for our thread progress reporting array. 
	initTreadProgress( thread_Progress, numThreads );							//- Initialising the values

#if WIN32

	//-TO DO - WIN32 CREATE THREADS FUNCTION

#else
	
	createSharedMemory();														//- Function for creating shared memory, server attachs to the memory
	atexit(destroySharedMemory);												//- Setting the at exit function to destory/detach from shared memory
	job_Queue = createQueue();													//- Creating the queue
	
	createMutex( &job_queue_access  );											//- Creating queue mutex
	for ( i = 0; i < 10; i++ )													//- Creates array of 10 mutex's for synching clientSlot responses by the threads
	{
		createMutex( &job_mailSlot_access[i] );									//- Creating mailSlot mutex
	}

	createSemaphore( &thread_semaphore, 0);										//- Creating the semaphore 
	for ( i = 0; i < numThreads; i++ )											//- Create the specified number of threads provided by the client
    {
		createThreads(thread_worker, NULL);										//-Creating the thread workers
    }
	if ( testHarness == 1 )	createMutex(&job_test_harness);						//- Creating/initialising the test harness mutex

	printf("SERVER: Finished Creating Threads\n");
	printf("SERVER: Please enter a integer to factorise.\n");
	printf("SERVER: Or press q at any time to quit.\n");
	if ( testMode == 1)															//- NOW THAT THE SHARED MEMORY & THREADS ARE CREATED - We can setup for the TEST MODE 
	{
		testMode = -1;															//- Setting test mode to false, so that we can enter normal flow of program
		SHM_PTR->clientFlag = NUM_FILLED;										//- Setting FLAG to FIlled, so the server will read the input value of 0 for test mode
		SHM_PTR->number = 0;													//- Setting to Testmode
	}
	while (1)																	//- Main Loop - run forever
	{
		/*if ( SHM_PTR->clientShutDown == SHUTDOWN )							//- Check to see if the client has shut down.
		{
			//SHM_PTR->clientShutDown = RUNNING;
			//- TO DO - Gracefully exit
			printf("SERVER: Client has requested to quit.\n");
			printf("SERVER: Shutting down.\n");
			//destroySharedMemory();
			//exit(3);
			break;																//- Exit out of the main loop here so we can shut down the server
		}*/

		if ( job_Queue.size == 0 )												//- Check to see if the Queue is empty, if so we reset the flag to NOT_BUSY
		{
			SHM_PTR->serverBusy = NOT_BUSY;
		}

		if ( job_Queue.size > ( MAX_QUEUE_SIZE - 32 ) )							//- Check to see if the client is busy and queue is full
		{
			//printf("checkAlmostFull = %d\n",checkQueueAlmostFull);			//- Used for testing purposes 
			//printf("queueSize = %d\n",job_Queue.size );						//- Used for testing purposes 
			SHM_PTR->serverBusy = BUSY;
		}
		
		if (	SHM_PTR->clientFlag == NUM_FILLED && 
				SHM_PTR->serverBusy == NOT_BUSY && testMode != 1  )				//- CHeck to see if there is enough space to allocat the new numbersToFactorise
		{
			int i;																//- Iterator
			int freeQuerySlot = findFreeSlot(slotsInUse);						//- Finding a free slot to use
				
			if ( freeQuerySlot != NO_FREE_SLOTS )								//- Make sure there is a free slot to assosiate the job_items with, else loop around and wait
			{
				//- Prompting user for a new input
				printf("SERVER: Please enter a integer to factorise.\n");
				printf("SERVER: Or press q at any time to quit.\n");
				slotsInUse[freeQuerySlot] = SLOT_IN_USE;						//- Setting the slot to in use, tired and being lazy as to why its not in the function
				numberToRotate = SHM_PTR->number;								//- Setting number to rotate to the number provided
				//printf("SERVER: numberToRotate = %lu\n\n",numberToRotate);	//- Used for testing purposes
					
				SHM_PTR->number = NUM_EMPTY;									//- Reseting the value to 0, just for reducing any errors
				SHM_PTR->number = freeQuerySlot;								//- Setting the freeQuerySLot that was assigned to the job. So that the client can read it. 
				SHM_PTR->clientFlag = NUM_EMPTY;								//- Telling the client the server has read the number

				if (	numberToRotate == 0 && job_Queue.size == 0 && 
						checkForOutStandingQueries(slotsInUse) == -1 )			//- TEST MODE IS ENTERED HERE
				{
					int testHarnessAndTestMode = -6;							//- Setting the testharness mode

					if ( testHarness == 1)										//- If testHarness is true then we change the input
					{
						testHarnessAndTestMode = -5;
						printf("SERVER: TEST HARNESS ENABLED...");
					}
					printf("SERVER: TEST MODE ENABLED...\n\n");
					testMode = 1;												//- Test mode is set to true
					m_lock(&job_queue_access);									//- Locking the queue so we can add to it
		
					for( i = 0; i < 3; i++ )									//- Loop through and place all the TEST MODE NUMBERS into the queue
					{
						unsigned long k;
						for( k = 0; k < 10; k++)								//- Loop through and place all the TEST MODE NUMBERS into into the queue
						{
							push (	&job_Queue, freeQuerySlot, 
									k, testHarnessAndTestMode ) ;				//- Adding the numsToFactorise to the queue
							s_signal( &thread_semaphore);						//- Signal a thread to wake up and to start processing
						}
					}
					//display(&job_Queue);										//- Used for testing purposes, prints out the contents of the queue
					m_unlock(&job_queue_access);								//- Unlocking the queue as we have finished adding to it
					numberToRotate = -1;										//- Reseting the value
					SHM_PTR->serverBusy	= BUSY;									//- Server has reached its capactiy						
				}
				else if ( numberToRotate == 0 )									//- Server is busy sorry!! - only one test mode at a time
				{
					printf("SERVER: TEST MODE UNABLE TO START.\n"); 
					printf("Server still busy processing other jobs. \n\n");
					numberToRotate = -1;										//- Reseting the value
				}
				else															//- Normal operation
				{
					bitShiftClientNum(numsToFactorise,numberToRotate);			//- Rotating the numberToRotate and storing it in the numsToFactorise
					m_lock(&job_queue_access);									//- Locking the queue so we can add to it
					
					for ( i = 0; i < BIT_SHIFT_ROTATE_32; i++ )					//- Loop through and place all the numsToFactorise into the queue
					{
						push (	&job_Queue, freeQuerySlot, 
								numsToFactorise[i], numberToRotate ) ;			//- Adding the numsToFactorise to the queue
						s_signal( &thread_semaphore);							//- Signal a thread to wake up and to start processing
					}
					//display(&job_Queue);										//- Used for testing purposes, prints out the contents of the queue
					m_unlock(&job_queue_access);								//- Unlocking the queue as we have finished adding to it
					memset(numsToFactorise,0,32);								//- Sets all the values in the array to 0, max size is 32;
						
					if ( job_Queue.size > (MAX_QUEUE_SIZE - 32  ) )				//- Check to see if the Job_Queue contains enough queries to be considered full.
					{
						SHM_PTR->serverBusy	= BUSY;								//- Server has reached its capactiy
					}
				}
			} 
			else																//- We continue on and loop aroud and we wait until queue is not full
			{
				//printf("SERVER: OHH NOO!!! No free slots avalaible.\n");		//- Used for testing purposes
			}			
		}																		//- END OF IF - HM_PTR->clientFlag
		
		if ( testMode != 1 )													//- Check to see if test mode is on or not, if its not on then we enter here
		{
			normalModeOutput(numThreads, slotsInUse);							//- Nomral operation with progress counter
		}	
		else																	//- TestMode has been enabled																																												
		{
			testModeOutput( &testHarness, &testMode,  numThreads, slotsInUse );	//- TestMode & TestHarness Output logic is completed here
		}
		usleep(20);																//- Sleeping for 20 microseconds
	}																			//- END OF MAIN WHILE LOOP, ONLY BREAKS IF WE QUIT OR ERROR HAS OCCURED THAT WE CANNOT RECOVER FROM
	
	for ( i = 0; i < job_Queue.size; i++ )										//- Destroying the queue ??
	{
		  pop(&job_Queue);														//- Freeing the memory
	}
	destroythreadProgress(thread_Progress);										//- Freeing the allocated memory //- TODO ADD TO AT EXIT FUNCTION
#endif																			//- END OF UNIX CODE
	return 69;
}

///////////////////////////////////////////////////////////////////////////////////
// END OF FILE
///////////////////////////////////////////////////////////////////////////////////
