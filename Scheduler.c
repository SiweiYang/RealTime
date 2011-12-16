#include "TaskDescriptor.h"
#include "Scheduler.h"
#include <bwio.h>
#include <ts7200.h>
#include <Kernel.h>

// This has 2 because the first points to head of queue and 
// the second points to the tail of the queue.
// Also lowest priority is 0 and highest is 7(NUMBER_OF_PRIORITIES-1).
TaskDescriptor * volatile TASKS_PQ[NUMBER_OF_PRIORITIES][2];
// This points to the highest priority task
int volatile HIGHEST_ACTIVE_PRIORITY;

void initialize_schedule() {
    int i;
    HIGHEST_ACTIVE_PRIORITY = -1;
    for( i = 0; i < NUMBER_OF_PRIORITIES ; i++ ) {
        TASKS_PQ[i][0] = NULL;
        TASKS_PQ[i][1] = NULL;
    }
}

// Returns a pointer to the Highest Priority Task or NULL if nothing is scheduled
TaskDescriptor * get_next_scheduled_task() {
    if ( HIGHEST_ACTIVE_PRIORITY == -1 ) {
        return NULL;
    }
    //bwprintf(TRAIN_PORT, "%d %d\n", HIGHEST_ACTIVE_PRIORITY, (*TASKS_PQ[HIGHEST_ACTIVE_PRIORITY][0]).id );
    //bwprintf(TRAIN_PORT, "%d\n", (*TASKS_PQ[HIGHEST_ACTIVE_PRIORITY][0]).id );
    ACTIVE_TASK = TASKS_PQ[HIGHEST_ACTIVE_PRIORITY][0];
    return TASKS_PQ[HIGHEST_ACTIVE_PRIORITY][0];
}

// Adds the task td to the respective queue
void add_to_schedule( TaskDescriptor * td ) {
    int priority = (*td).priority;
    //bwprintf(TERM_PORT, "ADDING TASK: %d with priority %d INTO SCHEDULER with HAP is %d\n", (*td).id, priority, HIGHEST_ACTIVE_PRIORITY);
    (*td).next_in_queue = NULL;
    if ( TASKS_PQ[priority][0] == NULL ) {
	    TASKS_PQ[priority][0] = td;
	    TASKS_PQ[priority][1] = td;
	    // adjust HIGHEST_ACTIVE_PRIORITY
	    if ( priority > HIGHEST_ACTIVE_PRIORITY ) {
		    HIGHEST_ACTIVE_PRIORITY = priority;
		    //bwprintf(TERM_PORT, "ADDING TASK: %d updated highest priority %d INTO SCHEDULER\n", (*td).id, HIGHEST_ACTIVE_PRIORITY);
	    }
    }
    else {
	    (*TASKS_PQ[priority][1]).next_in_queue = td;
	    TASKS_PQ[priority][1] = td;
    }
}

// Moves the task td from the beginning of its queue to the back of its queue
void pass_from_schedule( TaskDescriptor *td ) {
    if ( (*TASKS_PQ[(*td).priority][0]).next_in_queue == NULL ) {
	// Do nothing since it is the only one in the queue
    }
    else {
	(*TASKS_PQ[(*td).priority][1]).next_in_queue = td;
	TASKS_PQ[(*td).priority][0] = (*td).next_in_queue;
	TASKS_PQ[(*td).priority][1] = td;
	(*td).next_in_queue = NULL;
    }
}

// Removes the active task from the ready queue
// This function assumes that the active task is at the front of the queue
void remove_active_task( TaskDescriptor *active_td ) {
    int volatile next_highest;
    
    if ( active_td == NULL ) {
	    return;
    }

    TASKS_PQ[(*active_td).priority][0] = (*active_td).next_in_queue;
    (*active_td).next_in_queue = NULL;

    if ( TASKS_PQ[HIGHEST_ACTIVE_PRIORITY][0] == NULL ) {
	HIGHEST_ACTIVE_PRIORITY -=1; 
	while ( HIGHEST_ACTIVE_PRIORITY != -1 ) {

		if ( TASKS_PQ[HIGHEST_ACTIVE_PRIORITY][0] != NULL ) {
			//bwprintf(TERM_PORT, "After removing task %d, next highest priority %d\n", (*active_td).id, HIGHEST_ACTIVE_PRIORITY );
			return;
		}
		HIGHEST_ACTIVE_PRIORITY--;
	}
	//bwprintf(TERM_PORT, "HIGHEST_ACTIVE_PRIORITY: %d, PQ IS COMPLETELY EMPTY!\n", HIGHEST_ACTIVE_PRIORITY);
    }
}

// Removes the task td from the priority queue
void remove_from_schedule( TaskDescriptor *td ) {
    int next_highest;
    int prio = (*td).priority;
    TaskDescriptor *temp = NULL;
    TaskDescriptor *temp2 = NULL;
    
    if ( td == NULL ) {
	    return;
    }

    if ( TASKS_PQ[prio][0] == td && TASKS_PQ[prio][1] == td ) {
	TASKS_PQ[prio][0] = NULL;
	TASKS_PQ[prio][1] = NULL;
    }   
    else if ( TASKS_PQ[prio][0] == td ) {
	TASKS_PQ[prio][0] = (*td).next_in_queue;
	(*td).next_in_queue = NULL;
    }
    else if ( TASKS_PQ[prio][1] == td ) {
	temp = TASKS_PQ[prio][0];
	while ( (*temp).next_in_queue != td ) {
	    temp = (*temp).next_in_queue;
	}
	(*td).next_in_queue = NULL;
	TASKS_PQ[prio][1] = temp;
    }
    else {
	temp = TASKS_PQ[prio][0];
	while ( (*temp).next_in_queue != td ) {
	    if ( (*temp).next_in_queue == NULL ) {
		return;
	    }
	    temp = (*temp).next_in_queue;
	}
	temp2 = (*temp).next_in_queue;
	temp2 = (*temp2).next_in_queue;
	(*td).next_in_queue = NULL;
    }

    if ( TASKS_PQ[(*td).priority][0] == NULL ) {
	TASKS_PQ[(*td).priority][1] = NULL;
	
	next_highest = HIGHEST_ACTIVE_PRIORITY; 
	while ( next_highest != -1 ) {
		if ( TASKS_PQ[next_highest][0] != NULL ) {
			HIGHEST_ACTIVE_PRIORITY = next_highest;
			return;
		}
		next_highest--;
	}
    }

}


