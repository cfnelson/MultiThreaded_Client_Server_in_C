////////////////////////////////////////////////////////////////////////////////////
// Filename:		bitShift_rotate_and_factorisation.h
// Author:			Charles Nelson - s2884951
// Date Modified:	01/10/2014 Created
//					29/10/2014 Updated 
// Description:		This file contains the prototypes for the bitShift and factorisation and trial division
////////////////////////////////////////////////////////////////////////////////////
#ifndef bitShift_rotate_and_factorisation
#define bitShift_rotate_and_factorisation

	#include  "my_thread.h"

	unsigned long rotateNumber		(unsigned long in, long amount, byte dir);
	unsigned long rotateNumberRight	(unsigned long in, unsigned long amount);

	int bitShiftClientNum(unsigned long* numToFactorise, unsigned long numFromClient);		//- BitShifts And rotates teh number provided by the client 32 times based,
	int thread_trial_division ( Job job_item, int );

#endif																						//- #ifndef bitShift_rotate_and_factorisation.h

///////////////////////////////////////////////////////////////////////////////////
// END OF FILE
///////////////////////////////////////////////////////////////////////////////////
