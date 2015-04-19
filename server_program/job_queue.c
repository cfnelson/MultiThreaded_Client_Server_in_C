////////////////////////////////////////////////////////////////////////////////////
// Filename:		job_queue.c
// Author:			Charles Nelson - s2884951
// Date Modified:	01/10/2014 Created
//					29/10/2014 Updated 
// Description:		This file contains the functions for the Queue struct/class
////////////////////////////////////////////////////////////////////////////////////
#include "job_queue.h"

/**
 * Push an item into queue, if this is the first item,
 * both queue->head and queue->tail will point to it,
 * otherwise the oldtail->next and tail will point to it.
 */
void push (Queue* queue, int querySlot, unsigned long numsToFactorise, int originalNumber ) 
{
	int i;
    queue_Node* q_node = (queue_Node*) malloc (sizeof(queue_Node));		//- Creating a q_node to be added to the queue
    
	q_node->Job_item.querySlot			= querySlot;					//- Setting the query response slot number
	q_node->Job_item.numsToFactorise	= numsToFactorise;				//- Setting the numberToFactorise
	q_node->Job_item.originalNumber		= originalNumber;				//- Setting the originalNumber

	q_node->next = NULL;												//- Setting the queue nodes next* to NULL as it is the last elment

    if ( queue->head == NULL )											//- Checks to see if the Head has been created, if not then we know this is the first element
	{ 
        queue->head = q_node;											//- Inserting the first element
    } 
	else
	{
        queue->tail->next = q_node;										//- Setting the Queues's tail* to the next q_node
    }
    queue->tail = q_node;
    queue->size++;
}

/**
 * Return and remove the first Job item (the 32 numbers to factorise).
 */
Job pop ( Queue* queue )												//- Retrieves the first Job Item
{
    
    queue_Node* head = queue->head;										//- Retrieving the current head of the queue
    Job item = head->Job_item;											//- Retrieving the first job item in the queue
   
    queue->head = head->next;											//- Move head pointer to next node, decrease size
    queue->size--;														//- Decreasing the queue's size
   
    free(head);															//- Frees the memory of original head
    return item;														//- Returning the job item
}

/**
 * Returns but does not remove the first Job item.
 */
Job peek (Queue* queue) 
{
    queue_Node* head = queue->head;
    return head->Job_item;
}

/**
 * Show all items in queue.
 */
void display ( Queue* queue ) 
{
    printf("\nDisplay: ");
    
    if (queue->size == 0)												//- Checks to see if there is any items
	{
        printf("No Job item (Number to factorise) in queue.\n");		
	}
	else																//- Jobs exist
	{ 
		int i;
		int size = queue->size;											//- Get the queues the current size
        queue_Node* head = queue->head;									//- Go to the item to look
		printf("%d item(s):\n", queue->size);

        for ( i = 0; i < size; i++ ) 
		{
			int k;
			printf("\t QuerySlot[%d] numToFactorise: %lu\n", head->Job_item.querySlot,head->Job_item.numsToFactorise);
			head = head->next;
        }
    }
    printf("\n\n");
}

/**
 * Create and initiate a Queue
 */
Queue createQueue () 
{
    Queue queue;
    queue.size		= 0;
    queue.head		= NULL;
    queue.tail		= NULL;
    queue.push		= &push;
    queue.pop		= &pop;
    queue.peek		= &peek;
    queue.display	= &display;
    return queue;
}

///////////////////////////////////////////////////////////////////////////////////
// END OF FILE
///////////////////////////////////////////////////////////////////////////////////
