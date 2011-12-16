#ifdef UI_H
#else
#define UI_H
#include <StringUtil.h>

#define CURSOR_UP		91
#define CURSOR_DOWN		92
#define CURSOR_RIGHT		93
#define CURSOR_LEFT		94

#define SENSOR_BUFFER_SIZE      11
void ui_initialize(Buffer *buf);
void ui_initialize_kernel_monitor(Buffer *buf);
void print_task(Buffer *buf, char *task_name, int task_index, int task_state);

void print_time(Buffer *buf, int micro_second_elapsed);
void print_train_number(Buffer *buf, int train_index, int train_number);
void print_train_speed(Buffer *buf, int train_index, int train_speed);
void print_train_segment(int train_index, int last_sensor_number);

void print_train_offset(Buffer *buf, int train_index, int velocity, int node_id, int offset);
void print_train_stop(Buffer *buf, int train_index, int stop, int last_train_speed);
void print_train_stack(Buffer *buf, int train_index, int stack_size);
void print_route(Buffer *buf, int train_number, int start_node_id, int stop_node_id);

void Print_Time( int time );



void print_switch(Buffer *buf, int switch_number, char switch_direction);

int print_sensors(Buffer *buf, int sensor_buffer_count, char *sensor_buffer);

void Print_Sensors( char * new_sensor_map );

void print_reservations( int * reservations );

void print_debug_string(char *debug_info);

void clear_command(Buffer *buf);

void exit(Buffer *buf);
#endif
