#include <ts7200.h>
#include <bwio.h>
#include <Log.h>
#include <StringUtil.h>

LogEntry	LOGS[LOG_SIZE];
int TASK_TIMEOUT[TASK_POOL_SIZE];
int LogCounter;

void initiate_task_timeout() {
    int i = 0;
    while(1) {
        if (i==TASK_POOL_SIZE) {
	    break;
	}
	TASK_TIMEOUT[i] = 0;
        i++;
    }
}
void process_task_timeout(TaskDescriptor *task) {
    int i = 0;
    while(1) {
        if (i==TASK_POOL_SIZE) {
	    break;
	}
	if (getIndex((*task).id)==i) {
	   TASK_TIMEOUT[i] += 1;
	}
	
        i++;
    }
}
int get_task_timeout(TaskDescriptor *task) {
    return TASK_TIMEOUT[getIndex((*task).id)];
}

void log_schedule(TaskDescriptor *task) {
    LOGS[LogCounter].type = SCHDULE;
    LOGS[LogCounter].meta.tid = task->id;
    
    LogCounter++;
}

void log_messaging(int msg_type, TaskDescriptor *from, TaskDescriptor *to) {
    LOGS[LogCounter].type = msg_type;
    LOGS[LogCounter].meta.trans.id = from->id;
    LOGS[LogCounter].meta.trans.target_id = to->id;
    
    LogCounter++;
}

void log_listenning(TaskDescriptor *task, int event_type) {
    LOGS[LogCounter].type = HDL_INTR;
    LOGS[LogCounter].meta.hook.id = task->id;
    LOGS[LogCounter].meta.hook.event_type = event_type;
    
    LogCounter++;
}

void dump_log(Buffer *buf) {
    int i = 0;
    char line[20];
    LogCounter++;
    while (i < LogCounter) {
        if (hasBuffer(buf) * 2 > BUFFER_SIZE) {
            while (hasBuffer(buf))bwputc(TERM_PORT, readBuffer(buf));
        }
        
        switch (LOGS[LogCounter].type) {
            case SCHDULE:
                break;
            case SND_MSG:
                break;
            case RCV_MSG:
                break;
            case RLY_MSG:
                break;
            case HDL_INTR:
                break;
            
        }
    }    
}

int printTaskStatus(char *line, int length, TaskDescriptor *tasks) {
    int num = length/5;
    
    int i = 0;
    while (i < num) {
        if (tasks[i].id < 10) {
            writeFormatString(line+i*5, 5, " %d:%d,", tasks[i].id, tasks[i].state);
        } else {
            writeFormatString(line+i*5, 5, "%d:%d,", tasks[i].id, tasks[i].state);
        }
        i++;
    }
    
    return i*5;
}