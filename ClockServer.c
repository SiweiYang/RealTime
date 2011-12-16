#include <bwio.h>
#include <ts7200.h>
#include <timer.h>
#include <Kernel.h>
#include <ContextSwitch.h>
#include <ClockServer.h>
#include <NameServer.h>
#include <TaskDescriptor.h>
#include <Notifier.h>
#include <Interrupt.h>

int CLOCK_SERVER_TID = -1;
int volatile current_time = 0;
int TASK_DELAYS[TASK_POOL_SIZE];

TaskDescriptor *DELAY_QUEUE_HEAD;
TaskDescriptor *DELAY_QUEUE_TAIL;


void add_to_delay_queue( TaskDescriptor *task, int delay_until ) {
	int adding_task_index = getIndex( (*task).id );
	TASK_DELAYS[adding_task_index] = delay_until;
	
	if ( DELAY_QUEUE_HEAD == NULL ) {
		DELAY_QUEUE_HEAD = task;
		(*task).next_in_queue = NULL;
		DELAY_QUEUE_TAIL = task;
	}
	else {
		if ( TASK_DELAYS[getIndex( (*DELAY_QUEUE_HEAD).id )] >= delay_until ) {
			// new task ahead everything
			(*task).next_in_queue = DELAY_QUEUE_HEAD;
			DELAY_QUEUE_HEAD = task;	
		}
		else if ( TASK_DELAYS[getIndex( (*DELAY_QUEUE_TAIL).id )] < delay_until ) {
			//new task behand everything
			(*DELAY_QUEUE_TAIL).next_in_queue = task;
			DELAY_QUEUE_TAIL = task;
			(*DELAY_QUEUE_TAIL).next_in_queue = NULL;
		}
		else {
			TaskDescriptor *current_task = DELAY_QUEUE_HEAD;
			int next_index = getIndex( (*((TaskDescriptor *)(*current_task).next_in_queue)).id );
			//bwprintf(TERM_PORT, "Inserting to the delay queue\n");
			while (1) {
				//queue_print(DELAY_QUEUE_HEAD);
				if ( TASK_DELAYS[next_index] >= delay_until ) {
					(*task).next_in_queue = (*current_task).next_in_queue;
					(*current_task).next_in_queue = task;
					//queue_print(DELAY_QUEUE_HEAD);
					break; 	
				}
				current_task = (*current_task).next_in_queue;
				next_index = getIndex( (*((TaskDescriptor *)(*current_task).next_in_queue)).id );
			}
			//bwprintf(TERM_PORT, "Insertion finished\n");
		} 
	}
}

void initialize_clock_server() {
	int i;	
	for ( i = 0; i < TASK_POOL_SIZE ; i++ ) {
		TASK_DELAYS[i] = -1;
	}
	current_time = 0;
	DELAY_QUEUE_HEAD = NULL;
	DELAY_QUEUE_TAIL = NULL;
	
}

void clock_server() {
	init_timer();
	RegisterAs("CLK");
	int notifier_tid = Create( 6, "CLK_NOTIF", &notifier );
	int received_tid = -1;
	int interrupt_type = TIMER1_INTERRUPT_TYPE;
	int len = 8;
	char msg[len];
	int *op1, *op2;
	op1 = msg;
	op2 = msg + 4;
	char reply[len];
	Send( notifier_tid, (char *)&interrupt_type, 4, reply, len );
	while (1) {
		Receive( &received_tid, (char *)msg, len );
		//bwprintf(TERM_PORT, "Got new request\n");
		switch ( (*op1) ) {
 			case NOTIFIER:
				current_time++;
				Reply( notifier_tid, TIME_UPDATED, 1 );
				break;
			case TIME_REQUEST:
				Reply( received_tid, (char *)&current_time, 4 );
				break;
			case DELAY:
				// Add to list of suspended tasks
				//bwprintf(TERM_PORT, "DelayRequest\n");
				//queue_print(DELAY_QUEUE_HEAD);
				add_to_delay_queue( get_task_by_id(received_tid), *op2 + current_time );
				//bwprintf(TERM_PORT, "DelayRequest awaiting %d,-new task tid: %d, delay: %d\n", queue_length(DELAY_QUEUE_HEAD), received_tid, *op2);
				break;
			case DELAY_UNTIL:
				add_to_delay_queue( get_task_by_id(received_tid), *op2 );
				break;
			default:
				// ERROR
				break;
		}
		// Check list of suspended tasks and reply
		//check_delay_queue();
		//bwprintf(TERM_PORT, "DelayRequest awaiting %d\n", queue_length(DELAY_QUEUE_HEAD));
		// Wake up all ready tasks
		while ( DELAY_QUEUE_HEAD && TASK_DELAYS[getIndex( (*DELAY_QUEUE_HEAD).id )] <= current_time ) {
			TaskDescriptor *task = (*DELAY_QUEUE_HEAD).next_in_queue;
			Reply( (*DELAY_QUEUE_HEAD).id, DELAY_OVER, 8 );
			//bwprintf(TERM_PORT, "Waken: Task %d...\n", (*DELAY_QUEUE_HEAD).id);
			DELAY_QUEUE_HEAD = task;
		}
		//queue_print(DELAY_QUEUE_HEAD);
	}
}

int Time() {
	if (CLOCK_SERVER_TID == -1) {
		CLOCK_SERVER_TID = WhoIs("CLK");
		if (CLOCK_SERVER_TID == -1) {
			bwprintf(TERM_PORT, "Missing ClockServer\n");
			return -1;
		}
	}
	int len = 8;
	char msg[len];
	int *op1, *op2;
	op1 = msg;
	op2 = msg+4;
	(*op1) = 1;	
	(*op2) = 0;
	char reply[len];
	//bwprintf(TERM_PORT, "Going to request a delay\n");
	Send( CLOCK_SERVER_TID, msg, len, reply, len );
	return *(int *)reply; 
}

int Delay( int ticks ) {
	if (CLOCK_SERVER_TID == -1) {
		CLOCK_SERVER_TID = WhoIs("CLK");
		if (CLOCK_SERVER_TID == -1) {
			bwprintf(TERM_PORT, "Missing ClockServer\n");
			return -1;
		}
	}
	int len = 8;
	char msg[len];
	int *op1, *op2;
	op1 = msg;
	op2 = msg+4;
	(*op1) = 2;	
	(*op2) = ticks;
	char reply[len];
	Send( CLOCK_SERVER_TID, msg, len, reply, len );
	return 0; 
}

int DelayUntil( int ticks ) {
 	if (CLOCK_SERVER_TID == -1) {
		CLOCK_SERVER_TID = WhoIs("CLK");
		if (CLOCK_SERVER_TID == -1) {
			bwprintf(TERM_PORT, "Missing ClockServer\n");
			return -1;
		}
	}
	int len = 8;
	char msg[len];
	int *op1, *op2;
	op1 = msg;
	op2 = msg+4;
	(*op1) = 3;	
	(*op2) = ticks;
	char reply[len];
	Send( CLOCK_SERVER_TID, msg, len, reply, len );
	return 0; 
}
