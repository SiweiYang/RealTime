#ifdef KERNEL_H
#else
#define KERNEL_H
#include <TaskDescriptor.h>

#define CREATE 0
#define MY_TID 1
#define MY_PARENT_TID 2
#define PASS 3
#define EXIT 4
#define SEND 5
#define RECEIVE 6
#define REPLY 7
#define AWAIT_EVENT 8
#define SYSTEM_SHUTDOWN 9
#define HARDWARE_INTERRUPT      255

TaskDescriptor volatile * ACTIVE_TASK;
TaskDescriptor *get_task_by_id(int tid);

void update_system_state(char state);
void save_spsr(int spsr);
void save_stack_pointer(int *sp);
void * memcpy(void * dest, const void * src, int len);
int getnextrequest();
int handle(int request);
int main_function();
#endif
