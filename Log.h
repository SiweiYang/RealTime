#include <TaskDescriptor.h>

#ifdef LOG_H
#else
#define LOG_H

#define         SCHDULE     0
#define         SND_MSG     1
#define         RCV_MSG     2
#define         RLY_MSG     3
#define         HDL_INTR    4

#define LOG_SIZE        4096

typedef struct {
    int id;
    int target_id;
} transmission;

typedef struct {
    int id;
    int event_type;
} interrupt_hook;

typedef struct {
    int type;
    union {
        transmission trans;
        interrupt_hook hook;
        int tid;
    } meta;
} LogEntry;

void initiate_task_timeout();
void process_task_timeout(TaskDescriptor *task);
int get_task_timeout(TaskDescriptor *task);

void log_schedule(TaskDescriptor *task);
void log_messaging(int msg_type, TaskDescriptor *from, TaskDescriptor *to);
void log_listenning(TaskDescriptor *task, int event_type);

int printTaskStatus(char *line, int length, TaskDescriptor *tasks);
#endif