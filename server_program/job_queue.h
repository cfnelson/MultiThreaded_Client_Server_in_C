////////////////////////////////////////////////////////////////////////////////////
// Filename:		job_queue.h
// Author:			Charles Nelson - s2884951
// Date Modified:	01/10/2014 Created
//					29/10/2014 Updated 
// Description:		This file contains the prototypes for the job_queue struct/class
////////////////////////////////////////////////////////////////////////////////////

#ifndef job_queue
#define job_queue

#include  "shared_memory.h"

	typedef struct Job							//- Contains the slot(clientQueryResponse) they are associated with & array of the numbers to factorise after they have been bitshifted
	{
		int querySlot;							//- Where to place the factors for the client to read
		//int maxSize;							//- Starting size of the list
		int originalNumber;						//- CThe original query number provided by the client
		unsigned long numsToFactorise;			//- Number that needs to be factorised 
												
	} Job;

	typedef struct queue_Node 
	{
		Job Job_item;							//- Contains a number that is to be factorised with trial division
		struct queue_Node* next;				//- Pointer to the next Job Item
	}   queue_Node;

	typedef struct Queue 
	{
		int size;								//- Current size of the queue
		queue_Node* head;						//- Start of queue
		queue_Node* tail;						//- End of queue

		Job (*pop) (struct Queue*);				//- Gets the queue_Node from the head and removes it from queue
		Job (*peek) (struct Queue*);			//- Gets the queue_Node from the head, But keeps it in queue
    
		void (*push) (	struct Queue*, int, 
						unsigned long, int );	//- Add the queue_Node to tail of the queue
		void (*display) (struct Queue*);		//- Displays all the queue_Nodes in the queue
   
	}   Queue;

	Job pop (Queue* queue);						//- Returns and remove the first item.
	Job peek (Queue* queue);					//- Returns the first item in the queue but doesnt, pop it off

	void push (	Queue* queue, int querySlot, 
				unsigned long numsToFactorise, 
				int originalNumber);			//- Pushes the node into the queue
	void display (Queue* queue);				//- Displays all the current items in the queue

	Queue createQueue ();						//- Creates and initiates the Queue

#endif											//- #ifndef job_queue.h
	
///////////////////////////////////////////////////////////////////////////////////
// END OF FILE
///////////////////////////////////////////////////////////////////////////////////
