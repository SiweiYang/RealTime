#ifdef TEST
    #include <stdio.h>
#endif
#include <TrackInformation.h>
#include <TrainProfile.h>
#include <SwitchProfile.h>
#include <track_data.h>

track_node track[TRACK_MAX];
int switch_position[total_switch_number];

void initialize_track(int track_id) {
	if (track_id) {
		init_trackb( track );
	} else {
		init_tracka( track );
	}
}

void initialize_track_information() {
    int i;
    for ( i = 0 ; i < 22 ; i++ ) {
        switch_position[i] = 0;
    }
    initialize_track(0);
    switch_profile( 0, switch_position );
    initialize_train_profiles();
    curve_compensation( track );
}

track_node *get_node(int id) {
    return track + id;
}

track_edge *get_edge(track_node *node) {
    if ( (*node).type == NODE_EXIT ) {
		return NULL;
    } else if ( (*node).type == NODE_BRANCH ) {
        int switch_id = (*node).num;
        if ( switch_id > 18 ) {
            switch_id -= 134;
        }
        switch_id -= 1; // set up to check our array
		return &((*node).edge[ switch_position[ switch_id ] ]);
    } else {
		return &((*node).edge[ DIR_AHEAD ]);
    }
}

int get_switch( int switch_id ) {
    return switch_position[switch_id];
}

int *get_switch_map() {
    return switch_position;
}

void update_switch( int switch_id, char status ) {
    if ( switch_id > 18 ) {
         switch_id -= 134;
    }
    switch_id -= 1;

    if (status == 'C'||status == 'c') {
	switch_position[switch_id] = DIR_CURVED;
    } else {
	switch_position[switch_id] = DIR_STRAIGHT;
    }
}

track_node *get_next_node(track_node *component, int *distance) {
    track_edge *edge = get_edge(component);
    if ( edge ) {
		*distance = (*edge).dist;
		return (*edge).dest;
    }
    
    *distance = 0;
    return NULL;
}

track_node *get_next_sensor( track_node *sensor, int *distance ) {
    *distance = 0;
    int extra_distance;
    track_node *next_node = sensor;
    while(1) {
        next_node = get_next_node(next_node, &extra_distance);
        *distance += extra_distance;
        if ((*next_node).type==NODE_EXIT)return NULL;
        if ((*next_node).type==NODE_SENSOR)break;
    }
    return next_node;
}

track_node *get_location( int train_number, int train_speed, track_node *component, int start_location, int start_velocity, int *end_location, int *end_velocity ) {
	int max_velocity, acceleration, deceleration;
	track_edge *edge = get_edge(component);
	max_velocity = velocity_profile(train_number, train_speed) * 10;

	deceleration = deceleration_profile(train_number);
    acceleration = acceleration_profile(train_number);
    if (start_velocity > max_velocity) {
		start_velocity -= deceleration;
	} else {
		start_velocity += acceleration;
	}
	if (start_velocity<0)start_velocity = 0;
	*end_location = start_location + start_velocity / 1000;
	*end_velocity = start_velocity;

	if (edge) {
		if (*end_location > (*edge).dist) {
			*end_location -= (*edge).dist;
			component = get_next_node(component, &start_location);
			if ((*component).type==NODE_SENSOR) {
				//update_with_sensor_id(get_node_id(component));
				//writeFormatString(buf.line, 90, "Switch %d -> Straight\n", i+1);
				//print_debug_string("Jumpping over a sensor");
			}
			return component;
		}
	} else {
		return NULL;
	}
	return component;
}

int get_node_id(track_node *node) {
    return node - track;
}

int get_reverse_node_id(track_node *node) {
	track_edge * temp_edge = get_edge( node );
	track_node * temp_node = (*temp_edge).dest;
	temp_node = (*temp_node).reverse;
	return temp_node - track;
}

int get_next_node_id(int node_id, int *next_node_id) {
	int total_distance;
	*next_node_id = get_node_id(get_next_node(get_node(node_id), &total_distance));
	return total_distance;
}

int get_next_sensor_id( int sensor_id, int *next_sensor_id ) {
    int total_distance;
    *next_sensor_id = get_node_id(get_next_sensor(get_node(sensor_id), &total_distance));
    return total_distance;
}

int get_previous_sensor_id( int sensor_id, int *previous_sensor_id ) {
    int total_distance;
    *previous_sensor_id = get_node_id(get_next_sensor((*get_node(sensor_id)).reverse, &total_distance));
    return total_distance;
}

int test_immediate_sensor(track_node *sensor, track_node *next_sensor) {
	int i;
    if ((*sensor).type!=NODE_SENSOR)return 0;
    if ((*next_sensor).type!=NODE_SENSOR)return 0;
    
    if (sensor==next_sensor)return 1;
    if ((*sensor).reverse==next_sensor)return 1;
    sensor = get_next_sensor(sensor, &i);
    if (sensor==next_sensor)return 1;
	
    return 0;
}

int test_immediate_sensor_id(int sensor_id, int next_sensor_id) {
    return test_immediate_sensor(get_node(sensor_id), get_node(next_sensor_id));
}

void set_sensor_bit( int sensor, char *temp_map ) {
    temp_map[ (sensor/8) ] = temp_map[ (sensor/8) ] | (1 << (7 - sensor%8));
}

void clear_sensor_bit( int sensor, char *temp_map ) {
    temp_map[ (sensor/8) ] = temp_map[ (sensor/8) ] & ~(1 << (7 - sensor%8));
}

int get_sensor_number(char *temp_map) {
	int i, j;
	for (i=0;i<sensor_module_number*2;i++) {
		//printf("Printing i value: %d %x \n", i, (temp_map[i/8])>>7);
		if ( temp_map[i] != 0 ) {
			for (j=0;j<8;j++) {
				if (1<<(7-j)&temp_map[i]) {
					return (i*8)+j;
				}
			}
		}
	}
	return -1;
}

void set_forward_bits( int last_known_sensor, char *temp_map ) {
    int next_sensor;
    int next_next_sensor;

    get_next_sensor_id( last_known_sensor, &next_sensor );
    get_next_sensor_id( next_sensor, &next_next_sensor );
    set_sensor_bit( next_sensor, temp_map );
    set_sensor_bit( next_next_sensor, temp_map );
}

void set_reverse_bits( int last_known_sensor, char *temp_map ) {
    set_forward_bits( get_node_id((*get_node(last_known_sensor)).reverse), temp_map );
}

int get_stop_node_id( int train_number, int train_speed, int start_sensor_id, int stop_sensor_id, int *node_id ) {
	int st_dist = stopping_distance_profile(train_number, train_speed);

	int distance_so_far = 0, distance = 0;
	track_node *node = get_node(start_sensor_id);
	track_node *stop_sensor = get_node(stop_sensor_id);
    while (distance_so_far < st_dist) {
        node = get_next_node(node, &distance);
        distance_so_far += distance;
        while (!(node==stop_sensor)) {
            node = get_next_node(node, &distance);
            distance_so_far += distance;
        }
    }
    
    track_node *stop_node;
	node = get_node(start_sensor_id);
    while (distance_so_far > st_dist) {
        stop_node = node;
        node = get_next_node(stop_node, &distance);
        distance_so_far -= distance;
    }

	(*node_id) = get_node_id(stop_node);
	return distance + distance_so_far - st_dist;
}

void curve_compensation(track_node *nodes) {
    int i = 0;
    while (i<144) {
        nodes[i].edge[DIR_AHEAD].fraction = 0;
        i++;
    }
    i = 0;
    while (i<144) {
        if (nodes[i].type == NODE_BRANCH) {
            nodes[i].edge[DIR_CURVED].fraction = 1;
            (*nodes[i].reverse).edge[DIR_AHEAD].fraction = 1;
        }
        i++;
    }
}
