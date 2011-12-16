#include <uC++.h>
#include <uSemaphore.h>

#include <stdio.h>
#include <string.h>
#include <algorithm>
using namespace std;

#include <ContextSwitch.h>
#include <IO.h>
#include <ClockServer.h>
#include <TrainTask.h>

#include <TestingKernel.h>

uSemaphore task_lock(1);
int NEXT_TASK_INDEX;
void test() {
    Putline(TERM_PORT, "WTF!!!!", 8);
    Create(2, "This is me", &test);
    Putline(TERM_PORT, "WTF!!!!", 8);
    Create(2, "This is me", &test);
    Putline(TERM_PORT, "WTF!!!!", 8);
    Create(2, "This is me", &test);
    
    if (NEXT_TASK_INDEX>1)Putline(TERM_PORT, "Created a few tasks...", 8);
    
    while(1)Pass();
}
RealTimeTask *ACTIVE_TASK;
RealTimeTask *TASKS[TASK_POOL_SIZE];
void uMain::main() {
    for (int i=0; i<TASK_POOL_SIZE; i++) {
        TASKS[i] = NULL;
    }
    NEXT_TASK_INDEX = 1;
    task_lock.P();
    ACTIVE_TASK = TASKS[0] = new RealTimeTask(0, "TrainTask", 4, 0, &train_task);
    
    
    for (int i=0; i<TASK_POOL_SIZE; i++) {
        if (TASKS[i] != NULL)delete TASKS[i];
    }
    //printf("All Tasks finished!!!\n");
}

int Create(int priority, char *task_name, void (*code)()) {
    RealTimeTask *task = ACTIVE_TASK;
    int tid = NEXT_TASK_INDEX++;
    //printf("Create task[%s]with id %d!!!\n", task_name, tid);
    ACTIVE_TASK = TASKS[tid] = new RealTimeTask(tid, task_name, priority, task->id, code);
    
    task_lock.P();
    ACTIVE_TASK = task;
    return tid;
}
int MyTid() {
    RealTimeTask *task = ACTIVE_TASK;
    int tid = task->id;
    
    task_lock.V();
    task_lock.P();
    ACTIVE_TASK = task;
    return tid;
}
int MyParentTid() {
    RealTimeTask *task = ACTIVE_TASK;
    int tid = task->parent;
    
    task_lock.V();
    task_lock.P();
    ACTIVE_TASK = task;
    return tid;
}
void Pass() {
    RealTimeTask *task = ACTIVE_TASK;
    
    task_lock.V();
    task_lock.P();
    ACTIVE_TASK = task;
}
void Exit() {
    RealTimeTask *task = ACTIVE_TASK;
    
    task_lock.V();
    task->service_lock.P();
    task_lock.P();
    ACTIVE_TASK = task;
}

int Send( int tid, char *msg, int msglen, char *reply, int replylen ) {
    RealTimeTask *task = ACTIVE_TASK;
    task->state = RECEIVE_BLOCKED;
    task->msg = msg;
    task->msglen = msglen;
    task->reply = reply;
    task->replylen = replylen;
    
    RealTimeTask *sender = task;
    RealTimeTask *receiver = TASKS[tid];
    //printf("Sending from Task %d to Task %d...\n", sender->id, receiver->id);
    if (receiver->state == SEND_BLOCKED) {
        // copy msg to receiver
        receiver->msglen = min(receiver->msglen, sender->msglen);
        //sender->msglen = receiver->msglen;
        strncpy(receiver->msg, sender->msg, receiver->msglen);
        sender->state = REPLY_BLOCKED;
        *(receiver->tid) = sender->id;
        receiver->state = READY;
        //sender->service_lock.V();
        receiver->service_lock.V();
    } else {
        
        if (receiver->receive_queue == NULL) {
            receiver->receive_queue = sender;
        } else {
            RealTimeTask *receive_queue_tail = receiver->receive_queue;
            while (receive_queue_tail->next_task != NULL) {
                receive_queue_tail = receive_queue_tail->next_task;
            }
            receive_queue_tail->next_task = sender;
            sender->next_task = NULL;
        }
    }
    
    task_lock.V();
    task->service_lock.P();
    task_lock.P();
    ACTIVE_TASK = task;
    return sender->replylen;
}
int Receive( int *tid, char *msg, int msglen ) {
    RealTimeTask *task = ACTIVE_TASK;
    task->state = SEND_BLOCKED;
    task->tid = tid;
    task->msg = msg;
    task->msglen = msglen;
    RealTimeTask *receiver = task;
    if (receiver->receive_queue != NULL) {
        RealTimeTask *sender = receiver->receive_queue;
        receiver->receive_queue = receiver->receive_queue->next_task;
        receiver->next_task = NULL;
        
        // copy msg from sender
        receiver->msglen = min(receiver->msglen, sender->msglen);
        //sender->msglen = receiver->msglen;
        strncpy(receiver->msg, sender->msg, receiver->msglen);
        sender->state = REPLY_BLOCKED;
        *(receiver->tid) = sender->id;
        receiver->state = READY;
        //sender->service_lock.V();
        receiver->service_lock.V();
    }
    
    task_lock.V();
    task->service_lock.P();
    task_lock.P();
    //printf("Receiving from Task %d by Task %d...\n", *tid, task->id);
    ACTIVE_TASK = receiver;
    return receiver->msglen;
}
int Reply( int tid, char *reply, int replylen ) {
    RealTimeTask *task = ACTIVE_TASK;
    RealTimeTask *sender = TASKS[tid];
    //printf("Reply from Task %d to Task %d...\n", task->id, sender->id);
    // copy msg to TASKS[tid]
    sender->replylen = min(sender->replylen, replylen);
    replylen = sender->replylen;
    strncpy(sender->reply, reply, sender->replylen);
    // release lock on the other side
    sender->state = READY;
    sender->service_lock.V();
    task_lock.V();
    task_lock.P();
    ACTIVE_TASK = task;
    return replylen;
}

//int AwaitEvent( int event );

// CLK SERVER
int Delay( int ticks ){
    RealTimeTask *task = ACTIVE_TASK;
    task_lock.V();
    
    timespec delta;
    delta.tv_sec = 0;
    delta.tv_nsec = 10000000;
    nanosleep(&delta, NULL);
    
    task_lock.P();
    ACTIVE_TASK = task;
}
int Time(){
	return 0;
}