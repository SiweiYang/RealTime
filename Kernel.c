#include <ts7200.h>
#include <bwio.h>
#include <ContextSwitch.h>
#include <Kernel.h>
#include <Scheduler.h>
#include <TrainTask.h>
#include <Interrupt.h>
#include <IO.h>
#include <Log.h>
#include <StringUtil.h>
#include <TrackUtil/TrackMonitor.h>

#include <KernelTester.h>

extern TaskDescriptor volatile *WAIT_INTERRUPT[TOTAL_INTERRUPT_TYPE];
TaskDescriptor volatile TASK[TASK_POOL_SIZE];
unsigned int volatile running_tasks, idle_counter, TID_ASSIGN;        // Assigns TIDs to newly created tasks

extern int current_time;
extern int LogCounter;
char SYS_STATE;

void update_system_state(char state) {
    SYS_STATE = state;
}

void data_exception() {
    bwprintf(TERM_PORT, "Task %d[%s] Got data exception in [%c] mode...\n", (*ACTIVE_TASK).id, (*ACTIVE_TASK).name, SYS_STATE);
}

void print_registers(int *regs) {
	bwprintf(TERM_PORT, "Going to test ContextSwitch\n");
	int i;
	for (i=1;i<15;i++) {
		bwprintf(TERM_PORT, "REG%d: %x\n", i, *(regs+i));
	}
}

int initialize() {
    bwsetfifo( TERM_PORT, OFF );
    bwsetfifo( TRAIN_PORT, OFF );
    bwsetspeed( TRAIN_PORT, 2400 );

    // We still need to set up the space for this task before anything is going to happen
    // We need to store the pc where the code starts to execute
    // We need to give it space to set up its stack (set up stack pointer)
    *(unsigned long *)0x28 = &kernel_entry;
    *(unsigned long *)0x2C = &data_exception;
    *(unsigned long *)0x30 = &data_exception;
    *(unsigned long *)0x38 = &interrupt_entry;

    int i = 0;
    while(1) {
        if (i==TOTAL_INTERRUPT_TYPE)break;
		WAIT_INTERRUPT[i] = NULL;
        i++;
    }
    enable_user_interrupt();

    TID_ASSIGN = 0;
    running_tasks = 0;
    initialize_schedule();
    initiate_task_timeout();

    i = 0;
    while(1) {
		if (i==TASK_POOL_SIZE) {
			TASK[i-1].next_in_queue = NULL;
			break;
		}

		TASK[i].id = i;
		TASK[i].next_in_queue = TASK + i + 1;
		TASK[i].state = EMPTY;
		i++;
    }

    TID_ASSIGN = 0;
#ifdef DEBUG
    bwprintf(TERM_PORT, "Where is TASK : %x\n", &TASK);
    bwprintf(TERM_PORT, "Where is TASK Pointing : %x\n", TASK);
    bwprintf(TERM_PORT, "Starting the Kernel...\n");
    bwprintf(TERM_PORT, "Where is TASK STACK : %x\n", &(TASK[TID_ASSIGN].stack));
    bwprintf(TERM_PORT, "Where is TASK STACK POINTER: %x\n", &(TASK[TID_ASSIGN].stack_pointer));
#endif

    ACTIVE_TASK = create_task( TASK + TID_ASSIGN, TID_ASSIGN++, "Context", 0, 4, &context_switch_tester);
    initiate_available_task_index(TASK + TID_ASSIGN);
    add_to_schedule( ACTIVE_TASK );

    LogCounter = 0;
    return 0;
}

TaskDescriptor *get_task_by_id(int tid) {
    return TASK + getIndex(tid);
}

void save_spsr(int spsr) {
    if ((*ACTIVE_TASK).spsr_flag) {
        (*ACTIVE_TASK).spsr = spsr;
        (*ACTIVE_TASK).spsr_flag = 0;
    }
}

void save_stack_pointer(int *sp) {
    (*ACTIVE_TASK).stack_pointer = sp;
	if (sp < (*ACTIVE_TASK).stack + 0x2000 )bwprintf(TERM_PORT, "Task %d[%s] Running out of STACK in [%c] mode...\n", (*ACTIVE_TASK).id, (*ACTIVE_TASK).name, SYS_STATE);
}

void * memcpy(void * dest, const void * src, int len)
{
    char * pdst = (char *) dest;
    char const * psrc = (char const *) src;
    while (len>0)
    {
        *pdst++ = *psrc++;
    len--;
    }

    return (dest);
}

int getnextrequest() {
#ifdef DEBUG
    if ((*ACTIVE_TASK).id==9||1) {
    bwprintf(TERM_PORT, "Context Switching out of kernel into Task %d\n", (*ACTIVE_TASK).id);
    bwprintf(TERM_PORT, "SP points to : %x\n", (*ACTIVE_TASK).stack_pointer);
    bwprintf(TERM_PORT, "On Stack : %x, %x, %x, %x\n", *((int *)(*ACTIVE_TASK).stack_pointer+12), *((int *)(*ACTIVE_TASK).stack_pointer+13), *((int *)(*ACTIVE_TASK).stack_pointer+14), *((int *)(*ACTIVE_TASK).stack_pointer+15));
    bwprintf(TERM_PORT, "Return : %x\n", *((int *)(*ACTIVE_TASK).stack_pointer));
    bwprintf(TERM_PORT, "Switch into Task %d\n", (*ACTIVE_TASK).id);
    }
#endif

    int sp, spsr;
    sp = (*ACTIVE_TASK).stack_pointer;
    spsr = (*ACTIVE_TASK).spsr;
    (*ACTIVE_TASK).spsr_flag = 1;
    //spsr = 0x60000050;
    //bwprintf(TERM_PORT, "Loading Task Stack Pointer : %x, CPSR: %x for Task %d\n", sp, spsr, (*ACTIVE_TASK).id);
    update_system_state('U');
    return context_switch_to_task(sp, spsr);
}

int handle(int request) {
    //Serve request here
    if ( request == CREATE ) {
        // we create a new task descriptor and put it in the priority queue
        int given_priority = *((*ACTIVE_TASK).stack_pointer);
        char *task_name = *((*ACTIVE_TASK).stack_pointer+1);
        void (*function) = *((*ACTIVE_TASK).stack_pointer+2);

        //bwprintf(TERM_PORT, "Creating Task  with priority %d\n", given_priority);

        if (given_priority < 0 || given_priority > NUMBER_OF_PRIORITIES - 1) {
            *((*ACTIVE_TASK).stack_pointer) = -1;
            return -1;
        }

        int index = pop_available_task_index();
        // To check if tasks are full
        if ( index == -1 ) {
            *((*ACTIVE_TASK).stack_pointer) = -2;
            return -1;
        }
        // Calculate the next appropriate tid
        if (TID_ASSIGN%TASK_POOL_SIZE>index) {
            TID_ASSIGN += TASK_POOL_SIZE;
        }
        TID_ASSIGN = TID_ASSIGN / TASK_POOL_SIZE * TASK_POOL_SIZE + index;
        //bwprintf(TERM_PORT, "Creating Task: %d\n", TID_ASSIGN);
        TaskDescriptor *new_task;
        if (task_name) {
            new_task = create_task( TASK + index, TID_ASSIGN++, task_name, (*ACTIVE_TASK).id, given_priority, function);
        } else {
            new_task = create_task( TASK + index, TID_ASSIGN++, "ANONYM", (*ACTIVE_TASK).id, given_priority, function);
        }

        //return the child task id
        *((*ACTIVE_TASK).stack_pointer) = (*new_task).id;
        add_to_schedule( new_task );
        running_tasks++;
        //bwprintf(TERM_PORT, "Created Task: %d\n", (*new_task).id);
    }
    else if ( request == MY_TID ) {
        // we basically return the task id to the active task
        *((*ACTIVE_TASK).stack_pointer) = (*ACTIVE_TASK).id;
    }
    else if ( request == MY_PARENT_TID ) {
        // we return the task id of the parent
        *((*ACTIVE_TASK).stack_pointer) = (*ACTIVE_TASK).parent;
    }
    else if ( request == PASS ) {
        // just move the task to the back of the priority queue
        pass_from_schedule( ACTIVE_TASK );
    }
    else if ( request == EXIT ) {
        running_tasks--;
        // remove the task from the priority queue (we dont have to worry about the memory allocated for this task)
        remove_active_task( ACTIVE_TASK );
		(*ACTIVE_TASK).id = -1;
        push_available_task_index( ACTIVE_TASK );
        (*ACTIVE_TASK).state = EMPTY;
    }
    else if ( request == SEND ) {
        int tid = *((*ACTIVE_TASK).stack_pointer);
        char *msg = *((*ACTIVE_TASK).stack_pointer+1);
        int msglen = *((*ACTIVE_TASK).stack_pointer+2);
        // getIndex will return the index of TID in TASK[]
        int index = getIndex(tid);
#ifdef DEBUG
        //if (tid==9) {
        if (1) {
            char *reply = *((*ACTIVE_TASK).stack_pointer+3);
            int replylen = *((*ACTIVE_TASK).stack_pointer+16);
            bwprintf(TERM_PORT, "This is a send from task %d to task %d with msg_len : %s\n", (*ACTIVE_TASK).id, tid, msglen);
            bwprintf(TERM_PORT, "The reply has a length limit %d\n", replylen);
            bwprintf(TERM_PORT, "Send to task at : %d\n", index);
        }
#endif
        if ( index == -1 ) {
            return -2;
        }

        (*ACTIVE_TASK).state = RECEIVE_BLOCKED;
        remove_active_task( ACTIVE_TASK );

        if ( TASK[index].state != SEND_BLOCKED ) {
#ifdef DEBUG
            bwprintf(TERM_PORT, "Pushing to the send_queue!\n");
#endif
            push_to_send_queue( &TASK[index], ACTIVE_TASK );
        }
        else if ( TASK[index].state == SEND_BLOCKED && send_queue_is_empty( &TASK[index] ) ) {
            //bwprintf(TERM_PORT, "Sending msg right now!!!\n");
            // Transfer all the information over (memcpy)
            TaskDescriptor *receiver = &TASK[index];
            int *sender_tid = *((*receiver).stack_pointer);
            char *receiver_msg = *((*receiver).stack_pointer+1);
            int receiver_msglen = *((*receiver).stack_pointer+2);
            int actual_msglen = minimum(receiver_msglen, msglen);

            memcpy( receiver_msg, msg, actual_msglen);
            //bwprintf(TERM_PORT, "Msg to send : %s\nMsg received : %s\n", msg, receiver_msg);

            (*ACTIVE_TASK).state = REPLY_BLOCKED;
            TASK[index].state = READY;
            add_to_schedule( &TASK[index] );

            // This should be the size of the message supplied
            //bwprintf(TERM_PORT, "Sender %d with msg_len %d!\n", (*ACTIVE_TASK).id, actual_msglen);
            *sender_tid = (*ACTIVE_TASK).id;
            *((*receiver).stack_pointer) = actual_msglen;
            return 0;
        }
        else if ( TASK[index].state == SEND_BLOCKED ) {
            push_to_send_queue( &TASK[index], ACTIVE_TASK );
        }
        else {
            // Do nothing
        }
    }
    else if ( request == RECEIVE ) {
        int *tid = *((*ACTIVE_TASK).stack_pointer);
        char *msg = *((*ACTIVE_TASK).stack_pointer+1);
        int msglen = *((*ACTIVE_TASK).stack_pointer+2);
#ifdef DEBUG
        bwprintf(TERM_PORT, "This is a receive!\n");
#endif

        (*ACTIVE_TASK).state = SEND_BLOCKED;
        if ( send_queue_is_empty( ACTIVE_TASK ) ) {
            // We do nothing
            remove_active_task( ACTIVE_TASK );
            //bwprintf(TERM_PORT, "Nothing to receive!\n");

        }
        else {
            // Get all the information (memcpy)
#ifdef DEBUG
            bwprintf(TERM_PORT, "Something in the send queue!\n");
#endif
            TaskDescriptor * sender = pop_from_send_queue( ACTIVE_TASK );
                char *sender_msg = *((*sender).stack_pointer+1);
                int sender_msglen = *((*sender).stack_pointer+2);
            int actual_msglen = minimum(sender_msglen, msglen);
#ifdef DEBUG
            bwprintf(TERM_PORT, "Copy %d from Sender at %x to Receiver at %x\n", actual_msglen, sender_msg, msg);
#endif

            memcpy(msg, sender_msg, actual_msglen);
            (*tid) = (*sender).id;

            (*ACTIVE_TASK).state = READY;
            (*sender).state = REPLY_BLOCKED;

            *((*ACTIVE_TASK).stack_pointer) = actual_msglen;
#ifdef DEBUG
            bwprintf(TERM_PORT, "Sender is Task %d\n", (*sender).id);
#endif
            // This should return the size of the msg sent
            return 0;
        }
    }
    else if ( request == REPLY ) {
        int tid = *((*ACTIVE_TASK).stack_pointer);
        int index = getIndex(tid);
#ifdef DEBUG
        char *reply = *((*ACTIVE_TASK).stack_pointer+1);
        int replylen = *((*ACTIVE_TASK).stack_pointer+2);
        bwprintf(TERM_PORT, "This is a reply to task %d with reply : %s\n", tid, reply);
        bwprintf( TERM_PORT, "Reply to Task at index %d\n", index);
#endif
        if ( index == -1 ) {
            return -2;
        }
        if ( TASK[index].state == REPLY_BLOCKED ) {
#ifdef DEBUG
            bwprintf(TERM_PORT, "A REPLY IS BEING COPIED OVER!\n");
#endif
            // Transfer all the information over (memcpy)
            int actual_msglen = minimum(*((*ACTIVE_TASK).stack_pointer+2), *((TASK[index]).stack_pointer+15));
            memcpy(*((TASK[index]).stack_pointer+3), *((*ACTIVE_TASK).stack_pointer+1), actual_msglen) ;

            TASK[index].state == READY;
            add_to_schedule( &TASK[index] );
            *((TASK[index]).stack_pointer) = actual_msglen;
            return 0;
        }
        else {
           return -3;
        }
    }
    else if ( request == AWAIT_EVENT ) {
        int event = *((*ACTIVE_TASK).stack_pointer);
        event = event & 0xFFFF;
        handle_interrupt_type(event, ACTIVE_TASK);

        // remove the task from the priority queue (we dont have to worry about the memory allocated for
        // this task)
        remove_active_task( ACTIVE_TASK );
        (*ACTIVE_TASK).state = AWAIT_EVENT_BLOCKED;
    }
    else {
        // invalid request number
        pass_from_schedule( ACTIVE_TASK );
    }

    return request;
}

int main() {
    main_function();
    return 0;
}

int main_function() {
    int request;
    disable_interrupt_type(TIMER3_INTERRUPT_TYPE);
    disable_interrupt_type(UART1_RECEIVE_INTERRUPT_TYPE);
    disable_interrupt_type(UART1_TRANSMIT_INTERRUPT_TYPE);
    disable_interrupt_type(UART1_INTERRUPT_TYPE);
    disable_interrupt_type(UART2_RECEIVE_INTERRUPT_TYPE);
    disable_interrupt_type(UART2_TRANSMIT_INTERRUPT_TYPE);

	Buffer buf;
    clearBuffer(&buf);
    // We initialize task 0s stack, pc, etc, and other important stuff
    initialize();
    while (1) {
        // What the real code is gonna look like
        if (!get_next_scheduled_task())break;

        request = getnextrequest();
        update_system_state('K');
        if ( request == SYSTEM_SHUTDOWN ) {
            break;
        }
        //bwprintf(TERM_PORT, "THE NEXT REQUEST IS: %d\n", request);
        handle(request);
    }

    disable_interrupt_type(TIMER3_INTERRUPT_TYPE);
    disable_interrupt_type(UART1_RECEIVE_INTERRUPT_TYPE);
    disable_interrupt_type(UART1_TRANSMIT_INTERRUPT_TYPE);
    disable_interrupt_type(UART1_INTERRUPT_TYPE);
    disable_interrupt_type(UART2_RECEIVE_INTERRUPT_TYPE);
    disable_interrupt_type(UART2_TRANSMIT_INTERRUPT_TYPE);
    bwprintf(TERM_PORT, "/033[2JAll done, idle counter: %d\n", idle_counter);
    return 0;
}
