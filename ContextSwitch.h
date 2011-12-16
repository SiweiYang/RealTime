#ifdef CONTEXT_SWITCH_H
#else
#define CONTEXT_SWITCH_H

#define RETURN_ARGUMENT_OFFSET      0
#define ARGUMENT_OFFSET		        0

int Create(int priority, char *task_name, void (*code)());
int MyTid();
int MyParentTid();
void Pass();
void Exit();

int Send( int tid, char *msg, int msglen, char *reply, int replylen );
int Receive( int *tid, char *msg, int msglen );
int Reply( int tid, char *reply, int replylen );

int AwaitEvent( int event );

int SystemShutdown();

int context_switch_to_task(int stack_pointer, int spsr);
int kernel_entry();
int interrupt_entry();

void dump_registers();
#endif
