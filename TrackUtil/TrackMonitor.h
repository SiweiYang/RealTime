#ifdef TRACK_MONITOR_H
#else
#define TRACK_MONITOR_H

#define sensor_map_queue    	100
#define max_path_length			32
#define number_of_trains 		2

#define train_action_queue_size	64
typedef struct {
	#define action_wait_velocity_eq						0
	#define action_wait_node_id_offset					1
	#define action_wait_stop_for_node_id_offset			2
	#define action_set_speed							3
	int action_id;
	int node_id;
	int node_offset;
	int speed;
	int velocity;
} train_action;

typedef struct {
	train_action pool[train_action_queue_size];
	int head;
	int tail;
} train_action_queue;

typedef struct {
	// for the train
	int volatile train_index;
	int volatile train_number;
	int volatile train_speed;
	int volatile train_recovery_speed;
	// from sensor signal
	int volatile last_path_index;
	int volatile last_sensor_id;
	int volatile last_last_sensor_id;
	int volatile next_sensor_id;
	int volatile time_stamp;
        // computed/inferred
	int volatile velocity;
	int volatile last_node_id;
	int volatile last_offset;
	// from request
	int volatile stop_node_id;
	int volatile stop_offset;
	// action queue
	train_action_queue taq;
} train_state;

void initiate_track_monitor();
int sensor_map_empty();

void attach_timestamp(int timestamp);
void update_sensor(char *sensor_buffer);

// all about train control
void update_with_sensor_id(int sensor_id);
void update_train_state();
void update_train_speed(int train_number, int speed);
void update_last_sensor(int train_number, int last_sensor_id);
void update_stop_sensor(int speed, int train_number, int node_id);

// train status inspection
train_state *get_train_state(int train_number);
train_state *get_train_states();
int query_train_velocity(int i);
int query_train_path(int i, int *path, int *time);

int check_upcoming_sensors( char * aware_sensors );
int check_stop_train(int train_number);

void clear_action_queue(train_action_queue *taq);
void add_action(train_action_queue *taq, int action_id, int node_id, int node_offset, int speed, int velocity);
train_action *get_current_action(train_action_queue *taq);
train_action *dump_current_action(train_action_queue *taq);
#endif
