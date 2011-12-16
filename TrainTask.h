#ifdef TRAINTASK_H
#else
#define TRAINTASK_H

#define LARGEST_COMMAND_LENGTH 10
#define NUM_TRAINS 80

void train_input();

void sensor_reader();

void handle_sensor_update();

void update_timer();

//void delay_command_queue();
void train_task();
#endif
