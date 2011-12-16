#ifdef TASK_DESCRIPTOR_H
#else
#define TASK_DESCRIPTOR_H

#ifndef TEST
#define NULL                    0
#endif

#define TASK_STACK_SIZE         0x18000
#define TASK_POOL_SIZE          16
#define NUMBER_OF_PRIORITIES    8

#define EMPTY                   0
#define INIT                    1
#define ACTIVE                  2
#define READY                   3
#define ZOMBIE                  4
#define SEND_BLOCKED            5
#define RECEIVE_BLOCKED         6
#define REPLY_BLOCKED           7
#define AWAIT_EVENT_BLOCKED     8

#define TASK_NAME_SIZE          32
struct TaskDescriptor {
    int id;
    char name[TASK_NAME_SIZE+1];
    int volatile state;
    int priority;
    int parent;

    int volatile *stack_pointer;
    int volatile spsr;
    int volatile spsr_flag;
    int *stack[TASK_STACK_SIZE];
    struct TaskDescriptor *send_queue_head;
    struct TaskDescriptor *send_queue_tail;
    struct TaskDescriptor *next_in_queue;
};
typedef struct TaskDescriptor TaskDescriptor;

TaskDescriptor *create_task(TaskDescriptor *task, int tid, char *task_name, int parent_tid, int priority, void (*code)());

// Simple queue debugging functions
int queue_length(TaskDescriptor *head);
void queue_print(TaskDescriptor *head);

// generic queue functions
void push_to_queue( TaskDescriptor ** head, TaskDescriptor ** tail, TaskDescriptor * to_add );
TaskDescriptor * pop_from_queue( TaskDescriptor ** head, TaskDescriptor ** tail );

int send_queue_is_empty( TaskDescriptor * td );
void push_to_send_queue( TaskDescriptor * td, TaskDescriptor * push_this );
TaskDescriptor * pop_from_send_queue( TaskDescriptor * td );
int minimum( int a, int b );

int getIndex(int TID);
void initiate_available_task_index(TaskDescriptor *task);
void push_available_task_index(TaskDescriptor *task);
int pop_available_task_index();
#endif
