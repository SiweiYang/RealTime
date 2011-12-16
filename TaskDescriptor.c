#include <ts7200.h>
#include <bwio.h>
#include <StringUtil.h>
#include <TaskDescriptor.h>
TaskDescriptor * available_queue;

int getIndex(int TID) {
    return TID%TASK_POOL_SIZE;
}

TaskDescriptor *create_task(TaskDescriptor *task, int tid, char *task_name, int parent_tid, int priority, void (*code)()) {
    task->id = tid;
    task->state = READY;
    task->parent = parent_tid;
    task->priority = priority;
	writeFormatString(task->name, TASK_NAME_SIZE, "%s", task_name);
	//task->name[writeFormatString(task->name, TASK_NAME_SIZE, "%s", task_name)] = 0;

    int *sp;
    sp = task->stack;
    sp += TASK_STACK_SIZE;
    sp--;
    *sp = code;
	sp--;
    *sp = code;
    sp--;
    *sp = sp + 2;
    sp--;
    *sp = sp;
    sp -= 12;
    task->stack_pointer = sp;
    //task->spsr = 0x60000050;
	task->spsr = 0x60000050;
    task->send_queue_head = NULL;
    task->send_queue_tail = NULL;
    task->next_in_queue = NULL;

    //bwprintf(TERM_PORT, "Task %d starting from %x with stack at %x\n", tid, code, sp);
    return task;
}

int queue_length(TaskDescriptor *head) {
	int i = 0;
	while (head) {
		head = (*head).next_in_queue;
		i++;
		//bwprintf(TERM_PORT, "Queue has more %x -> %x at %d\n", head, (*head).next_in_queue, i);
	}
	return i;
}

void queue_print(TaskDescriptor *head) {
	int i = 0;
	while (head) {
		bwprintf(TERM_PORT, "Queue[%d] %x -> %x\n", i, head, (*head).next_in_queue);
		head = (*head).next_in_queue;
		i++;
	}
}

int send_queue_is_empty( TaskDescriptor * td ) {
    if ( (*td).send_queue_head == NULL ) {
	return 1;
    }
    return 0;
}

void push_to_send_queue( TaskDescriptor * td, TaskDescriptor * push_this ) {
    if ( (*td).send_queue_head == NULL ) {
        (*td).send_queue_head = push_this;
	(*td).send_queue_tail = push_this;
    }
    else {
	TaskDescriptor * temp = (*td).send_queue_tail;
	(*temp).next_in_queue = push_this;
	(*td).send_queue_tail = push_this;
    }
}

TaskDescriptor * pop_from_send_queue( TaskDescriptor * td ) {
    TaskDescriptor * to_return = NULL;
    if ( (*td).send_queue_head != NULL && (*td).send_queue_head == (*td).send_queue_tail ) {
	to_return = (*td).send_queue_head;
	(*td).send_queue_head = NULL;
	(*td).send_queue_tail = NULL;
	return to_return;
    }
    else if ( (*td).send_queue_head != NULL ) {
	TaskDescriptor * temp = (*td).send_queue_head;
	to_return = (*td).send_queue_head;
	(*td).send_queue_head = (*temp).next_in_queue;
	(*to_return).next_in_queue = NULL;
	return to_return;
    }
    else {
	return NULL;
    }
}

int minimum( int a, int b ) {
    if ( a < b ) {
	return a;
    }
    return b;
}

void initiate_available_task_index(TaskDescriptor *task) {
    available_queue = task;
}

void push_available_task_index(TaskDescriptor *task) {
    (*task).next_in_queue = available_queue;
    available_queue = task;
}

int pop_available_task_index() {
    if (available_queue==NULL) {
	bwprintf(TERM_PORT, "Run out of TaskDescriptors\n");
	return -1;
    }
    int index = getIndex((*available_queue).id);
    available_queue = (*available_queue).next_in_queue;

    return index;
}

void push_to_queue( TaskDescriptor ** head, TaskDescriptor ** tail, TaskDescriptor * to_add ) {
	if ( *head == NULL ) {
		*head = to_add;
		*tail = to_add;
	}
	else {
		(**tail).next_in_queue = to_add;
		*tail = to_add;
	}
}

TaskDescriptor * pop_from_queue( TaskDescriptor ** head, TaskDescriptor ** tail ) {
	TaskDescriptor * temp = *head;
	if ( *head == NULL ) {
		return NULL;
	}
	else {
		*head = (*temp).next_in_queue;
		(*temp).next_in_queue = NULL;
		return temp;
	}
}
