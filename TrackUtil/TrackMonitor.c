#ifdef TEST
#include <stdio.h>
#endif

#include <TrackInformation.h>
#include <TrackMonitor.h>

char SENSOR_MAP[sensor_map_queue][sensor_module_number*2];
int TIME_MAP[sensor_map_queue];

int TRAIN_PATHS[number_of_trains][max_path_length];
int TRAIN_TIMES[number_of_trains][max_path_length];
volatile train_state TRAIN_STATES[number_of_trains];
int CURRENT_SENSOR_MAP, GENERATION_COUNT;

void initiate_track_monitor() {
    int i, j;
    for ( i = 0 ; i < sensor_map_queue ; i++ ) {
        for ( j = 0 ; j < sensor_module_number*2 ; j++ ) {
            SENSOR_MAP[i][j] = 0;
        }
    }
    for ( i = 0 ; i < number_of_trains ; i++ ) {
        for ( j = 0; j < max_path_length ; j++ ) {
            TRAIN_PATHS[i][j] = -1;
            TRAIN_TIMES[i][j] = -1;
        }
    }
    for ( i = 0 ; i < number_of_trains ; i++ ) {
        TRAIN_STATES[i].train_index = i;
        TRAIN_STATES[i].train_number = -1;
        TRAIN_STATES[i].train_speed = 0;
        TRAIN_STATES[i].train_recovery_speed = 0;

        TRAIN_STATES[i].last_path_index = -1;
        TRAIN_STATES[i].time_stamp = -1;
        TRAIN_STATES[i].last_sensor_id = -1;
        TRAIN_STATES[i].last_last_sensor_id = -1;
        TRAIN_STATES[i].next_sensor_id = -1;


        TRAIN_STATES[i].velocity = 0;
        TRAIN_STATES[i].last_node_id = -1;
        TRAIN_STATES[i].last_offset = 0;

        TRAIN_STATES[i].stop_node_id = -1;
        TRAIN_STATES[i].stop_offset = -1;

        TRAIN_STATES[i].taq.head = 0;
        TRAIN_STATES[i].taq.tail = 0;
    }
    CURRENT_SENSOR_MAP = -1;
    GENERATION_COUNT = 0;
}

void attach_timestamp(int timestamp) {
    TIME_MAP[(CURRENT_SENSOR_MAP + 1) % sensor_map_queue] = timestamp;
}

int sensor_map_empty() {
    int i;
    for ( i = 0 ; i < sensor_module_number*2 ; i++ ) {
        if ( SENSOR_MAP[CURRENT_SENSOR_MAP][i] != 0 ) {
            return 0;
        }
    }
    return 1;
}

void update_sensor(char *sensor_buffer) {
    CURRENT_SENSOR_MAP = (CURRENT_SENSOR_MAP + 1) % sensor_map_queue;
    int i, j, sensor_number;
    for ( i = 0 ; i < sensor_module_number*2 ; i++ ) {
        SENSOR_MAP[CURRENT_SENSOR_MAP][i] = sensor_buffer[i];
    }

    GENERATION_COUNT++;
    // Good for multiple trains
    for ( i = 0 ; i < sensor_module_number*2 ; i++ ) {
        if ( SENSOR_MAP[CURRENT_SENSOR_MAP][i] != 0 ) {
            for ( j = 0 ; j < 8 ; j++ ) {
                if ( ((1 << j) & (SENSOR_MAP[CURRENT_SENSOR_MAP][i])) != 0 ) {
                    sensor_number = i*8 + (7-j);
                    update_with_sensor_id(sensor_number);
                }
            }   
        }
    }
}

void set_up_new_train( int sensor_id ) {
    int i;
    for ( i = 0 ; i < number_of_trains ; i++ ) {
        if ( TRAIN_STATES[i].train_number > -1 && TRAIN_STATES[i].last_sensor_id == -1 ) {
			update_last_sensor(TRAIN_STATES[i].train_number, sensor_id);
            return;
        }
    }
}

void update_with_sensor_id(int sensor_id) {
    int i, jump_sensor_id;
    /* check immediate sensors */
    for ( i = 0 ; i < number_of_trains ; i++ ) {
        if ( TRAIN_STATES[i].train_number > -1 && TRAIN_STATES[i].last_sensor_id > -1 ) {
			if (test_immediate_sensor_id(TRAIN_STATES[i].last_sensor_id, sensor_id)) {
				 update_last_sensor(TRAIN_STATES[i].train_number, sensor_id);
				 return;
			}
			
			if (TRAIN_STATES[i].next_sensor_id == sensor_id) {
				update_last_sensor(TRAIN_STATES[i].train_number, sensor_id);
				return;
			}
        }
    }
    /* check non-immediate sensors */
    //for ( i = 0 ; i < number_of_trains ; i++ ) {
    //    if ( TRAIN_STATES[i].train_number > -1 && TRAIN_STATES[i].last_sensor_id > -1 ) {
    //        /* check next next sensor */
    //        get_next_sensor_id( TRAIN_STATES[i].next_sensor_id, &jump_sensor_id );
    //        if (jump_sensor_id == sensor_id) {
    //            update_last_sensor(TRAIN_STATES[i].train_number, sensor_id);
    //            return;
    //        }
    //    }
    //}
	set_up_new_train( sensor_id );
}

// Initialize/Update a train state
void update_train_speed(int train_number, int speed) {
	int train_index;
	for (train_index=0;train_index<number_of_trains;train_index++) {
		if (TRAIN_STATES[train_index].train_number==train_number) {
			TRAIN_STATES[train_index].train_speed = speed;
			return;
		}
	}

	for (train_index=0;train_index<number_of_trains;train_index++) {
		if (TRAIN_STATES[train_index].train_number==-1) {
			// initialize a train state
			TRAIN_STATES[train_index].train_number = train_number;
			TRAIN_STATES[train_index].train_speed = speed;
			return;
		}
	}
}

void update_last_sensor(int train_number, int last_sensor_id) {
	int train_index;
	for (train_index=0;train_index<number_of_trains;train_index++) {
		if (TRAIN_STATES[train_index].train_number==train_number) {
            TRAIN_STATES[train_index].last_path_index += 1;
            TRAIN_STATES[train_index].last_sensor_id = last_sensor_id;
            get_next_sensor_id(TRAIN_STATES[train_index].last_sensor_id, &(TRAIN_STATES[train_index].next_sensor_id));
            TRAIN_STATES[train_index].time_stamp = TIME_MAP[CURRENT_SENSOR_MAP];
                        
            TRAIN_STATES[train_index].last_node_id = TRAIN_STATES[train_index].last_sensor_id;
            TRAIN_STATES[train_index].last_offset = 0;
                        
            TRAIN_PATHS[train_index][ TRAIN_STATES[train_index].last_path_index ] = last_sensor_id;
            TRAIN_TIMES[train_index][ TRAIN_STATES[train_index].last_path_index ] = TIME_MAP[CURRENT_SENSOR_MAP];
			return;
		}
	}
}

void update_stop_sensor(int speed, int train_number, int node_id) {
    int train_index = 0, node_start;
	for (train_index=0;train_index<number_of_trains;train_index++) {
		if (TRAIN_STATES[train_index].train_number==train_number) {
			node_start =TRAIN_STATES[ train_index ].last_sensor_id;
			TRAIN_STATES[train_index].stop_offset = get_stop_node_id(train_number, speed, node_start, node_id, &(TRAIN_STATES[train_index].stop_node_id));
		}
	}
}

train_state *get_train_state(int train_number) {
    int train_index;
	for (train_index=0;train_index<number_of_trains;train_index++) {
		if (TRAIN_STATES[train_index].train_number==train_number) {
			return TRAIN_STATES + train_index;
		}
	}
    return NULL;
}

train_state *get_train_states() {
    return TRAIN_STATES;
}

int query_train_path(int i, int *path, int *time) {
	if (TRAIN_STATES[i].last_path_index == -1)return -1;

	int j;
	for (j = 0;TRAIN_STATES[i].last_path_index-j>-1&&j<max_path_length;j++) {
		path[j] = TRAIN_PATHS[i][ (TRAIN_STATES[i].last_path_index-j)%max_path_length ];
		time[j] = TRAIN_TIMES[i][ (TRAIN_STATES[i].last_path_index-j)%max_path_length ];
	}

	return j;
}

int check_upcoming_sensors( char * aware_sensors ) {
	int last_sensor = TRAIN_STATES[0].last_sensor_id;

	int next_sensor, next_next_sensor;
	get_next_sensor_id( last_sensor, &next_sensor );
	get_next_sensor_id( next_sensor, &next_next_sensor );

	int aware_sensor = get_sensor_number( aware_sensors );

	if ( aware_sensor == last_sensor || aware_sensor == next_sensor || aware_sensor == next_next_sensor ) {
		return 1;
	}
	return 0;
}

int check_stop_train(int train_number) {
    int train_index = 0;
	for (train_index = 0;train_index<number_of_trains;train_index++) {
		if (TRAIN_STATES[train_index].stop_node_id>-1) {
			if (TRAIN_STATES[train_index].last_node_id == TRAIN_STATES[train_index].stop_node_id)return TRAIN_STATES[train_index].stop_offset < (TRAIN_STATES[train_index].last_offset);
			if (TRAIN_STATES[train_index].last_node_id == get_node_id(get_next_node(get_node(TRAIN_STATES[train_index].stop_node_id), &train_number)))return 1;
		}
	}

    return 0;
}

void clear_action_queue(train_action_queue *taq) {
	(*taq).head = 0;
    (*taq).tail = 0;
}

void add_action(train_action_queue *taq, int action_id, int node_id, int node_offset, int speed, int velocity) {
	(*taq).pool[(*taq).tail].action_id = action_id;
	(*taq).pool[(*taq).tail].node_id = node_id;
	(*taq).pool[(*taq).tail].node_offset = node_offset;
	(*taq).pool[(*taq).tail].speed = speed;
	(*taq).pool[(*taq).tail].velocity = velocity;
	(*taq).tail = ((*taq).tail+1)%train_action_queue_size;
}

train_action *get_current_action(train_action_queue *taq) {
	if ((*taq).head==(*taq).tail)return NULL;

	return (*taq).pool + (*taq).head;
}

train_action *dump_current_action(train_action_queue *taq) {
	if ((*taq).head==(*taq).tail)return NULL;
	int head = (*taq).head;
	(*taq).head = (head+1)%train_action_queue_size;
	return (*taq).pool + head;
}
