#ifdef SCHEDULER_H
#else
#define SCHEDULER_H

void initialize_schedule();
TaskDescriptor * get_next_scheduled_task();
void add_to_schedule( TaskDescriptor * td );
void pass_from_schedule( TaskDescriptor *td );
void remove_active_task( TaskDescriptor *active_td);
void remove_from_schedule( TaskDescriptor * td );

#endif
