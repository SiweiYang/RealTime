#ifdef TESTING_KERNEL_H
#else
#define TESTING_KERNEL_H

#include <uC++.h>
#include <uSemaphore.h>

#include <TaskDescriptor.h>

_Task RealTimeTask {
public:
    int id;
    char name[TASK_NAME_SIZE+1];
    int volatile state;
    int priority;
    int parent;
    
    RealTimeTask * volatile receive_queue;
    RealTimeTask * volatile next_task;
    uSemaphore service_lock;
    
    void (*code)();
    
    RealTimeTask(int id, char *task_name, int priority, int parent, void (*code)()) : id(id), state(READY), priority(priority), parent(parent), service_lock(0), code(code) {
        strncpy(name, task_name, TASK_NAME_SIZE+1);
        receive_queue = NULL;
        next_task = NULL;
    }
public:
    int * volatile tid;
    char *msg;
    int msglen;
    char *reply;
    int replylen;
    void main() {
        (*code)();
    }
};
#endif