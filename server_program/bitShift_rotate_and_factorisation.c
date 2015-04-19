////////////////////////////////////////////////////////////////////////////////////
// Filename:		bitShift_rotate_and_factorisation.c
// Author:			Charles Nelson - s2884951
// Date Modified:	01/10/2014 Created
//					29/10/2014 Updated 
// Description:		This file contains the bitShift_rotate_and_factorisation functions
////////////////////////////////////////////////////////////////////////////////////

#include  "bitShift_rotate_and_factorisation.h"

unsigned long rotateNumberRight(unsigned long in, unsigned long amount)
{
	return 	(in >> amount) | (in << (8*sizeof(unsigned long) - amount)) ;
}

unsigned long rotateNumber(unsigned long in, long amount, byte dir)
{
	return dir == DIR_RIGHT ? 
			(in >> amount) | (in << (8*sizeof(unsigned long) - amount)) : 
			(in << amount) | (in >> (8*sizeof(unsigned long) - amount)) ;
}

int bitShiftClientNum(unsigned long* numToFactorise, unsigned long numFromClient)	//- BitShifts And rotates teh number provided by the client 32 times based,
{
	int i;
	unsigned long k = 0;

	for (i = 0; i < BIT_SHIFT_ROTATE_32; i++,k++ )
	{
		numToFactorise[k] = rotateNumberRight(numFromClient, k);
		//printf("%lu\n",numToFactorise[k]);
	}
	return 1;
}

int thread_trial_division ( Job job_item,  int t_progress_slot )					//- Completes the Trial Division and returns the answers to the CLient's SLots
{
	unsigned long i;
	
	for ( i = 2; i < job_item.numsToFactorise; i++ ) 
	{
		if ( ( job_item.numsToFactorise % i ) == 0 )
		{
		#if WIN32
			printf("Factor: %lu\n",i);
			//- TO DO - WRITING TO THE WINDOWS MAILSLOT
		#else
			m_lock(&job_mailSlot_access[job_item.querySlot]);						//- Locking the mailsot for the query

			while ( SHM_PTR->serverFlag[job_item.querySlot] == NUM_FILLED  )
			{
				usleep(20);															//- Going to sleep for 500 microseconds
			}
			//printf("Factor: %lu\n",i);
			SHM_PTR->clientQueryResponse[job_item.querySlot] = i;					//- Sending the value to the slot.
			SHM_PTR->serverFlag[job_item.querySlot] = NUM_FILLED;					//- Setting the value of the serverFlag to 1, so the client will read from it.
			m_unlock(&job_mailSlot_access[job_item.querySlot]);						//- Access the finished jobs list
		#endif
			
		 }
		thread_Progress[t_progress_slot].percentComplete = ( ( (double) i / (double) job_item.numsToFactorise ) * (double) 100.00 );
	}
	return 1;
}

///////////////////////////////////////////////////////////////////////////////////
// END OF FILE
///////////////////////////////////////////////////////////////////////////////////
