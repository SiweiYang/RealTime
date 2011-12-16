#include <TrackInformation.h>
#include <TrackMonitor.h>
#include <track_node.h>
#include <stdio.h>
extern char **SENSOR_MAP;

void sensor_associatity_test() {
}

void track_traverse_test(int node_id) {
	track_node *next_node;
	int distance, total = 0;
	printf("Start: %d\n", (*get_node(node_id)).num);
	next_node = get_next_node(get_node(node_id), &distance);
	
	do {
		total += distance;
		printf("Total: %d\n", total);
		if ((*get_node(get_node_id(next_node))).type==NODE_SENSOR) {
			printf("Sensor: %d, %d\n", (*get_node(get_node_id(next_node))).num, distance);
		} else {
			printf("Node: %d, %d\n", (*get_node(get_node_id(next_node))).num, distance);
		}
		next_node = get_next_node(get_node(get_node_id(next_node)), &distance);
	} while ((*get_node(get_node_id(next_node))).num!=node_id);
	
	if ((*get_node(get_node_id(next_node))).type==NODE_SENSOR) {
		printf("Sensor: %d, %d\n", (*get_node(get_node_id(next_node))).num, distance);
	} else {
		printf("Node: %d, %d\n", (*get_node(get_node_id(next_node))).num, distance);
	}
	next_node = get_next_node(get_node(get_node_id(next_node)), &distance);
	printf("Total: %d\n", total);
}

void track_traverse_sensor_test() {
	track_node *next_node;
	int distance;
	printf("Start: %d\n", (*get_node(54)).num);
	next_node = get_next_node(get_node(54), &distance);
	
	do {
		if ((*get_node(get_node_id(next_node))).type==NODE_SENSOR) {
			printf("Sensor: %d, %d\n", (*get_node(get_node_id(next_node))).num, distance);
		} else {
			printf("Node: %d, %d\n", (*get_node(get_node_id(next_node))).num, distance);
		}
		next_node = get_next_node(get_node(get_node_id(next_node)), &distance);
	} while ((*get_node(get_node_id(next_node))).num!=58);
	
	if ((*get_node(get_node_id(next_node))).type==NODE_SENSOR) {
		printf("Sensor: %d, %d\n", (*get_node(get_node_id(next_node))).num, distance);
	} else {
		printf("Node: %d, %d\n", (*get_node(get_node_id(next_node))).num, distance);
	}
	next_node = get_next_node(get_node(get_node_id(next_node)), &distance);
}

void track_run_test() {
    int train_number, start_node, node_id, stop_node_id, velocity, speed, offset;
    train_number = 39;
    speed = 12;
    start_node = 43;
    node_id = 41;
	offset = get_stop_node(39, start_node, node_id, speed, &stop_node_id);
	printf("To stop at sensor %d, set speed 0 at %d inside node %d\n", node_id, offset, stop_node_id);
    
    train_state *ts = get_train_state(0);
    (*ts).time_stamp = 0;
    (*ts).velocity = 0;
    (*ts).last_node_id = start_node;
    (*ts).last_offset = 0;
    (*ts).stop_offset = offset;
    (*ts).stop_node_id = stop_node_id;
    track_node *new_node;
    
    int i = 0, distance;
	new_node = get_node(stop_node_id);
	offset = 0 - offset;
	while (!(get_node_id(new_node)==node_id)) {
		new_node = get_next_node(new_node, &distance);
		offset += distance;
	}
	printf("From stop node to the node is: %d\n", offset);
	
    while (!check_stop_train(train_number)) {
		//printf("At node %d -> %d\n", (*ts).last_node_id, (*ts).last_offset);
        new_node = get_location( 39, speed, get_node((*ts).last_node_id), (*ts).last_offset, (*ts).velocity, &((*ts).last_offset), &((*ts).velocity) );
        (*ts).last_node_id = get_node_id(new_node);
        i++;
    }
	printf("At node %d -> %d\n", (*ts).last_node_id, (*ts).last_offset);
	
	printf("Max speed for 12 is %d...\n", velocity_profile(39));
}

void train_path_test() {
	int i,j;
    train_state *ts;
	char sensor_map[sensor_module_number*2];
	int train_path[max_path_length], time_stamp[max_path_length];
	for( i = 0; i < sensor_module_number*2; i++ ) {
		sensor_map[i] = 0;
	}
	// D11
	set_sensor_bit(58, sensor_map);
	attach_timestamp(1);
	update_sensor(sensor_map);
	clear_sensor_bit(58, sensor_map);
	printf("Update...\n");
    ts = get_train_state(0);
    printf("Train is in %d ---> %d", (*ts).last_sensor_id, (*ts).next_sensor_id);
	
	//C16
	set_sensor_bit(47, sensor_map);
	attach_timestamp(2);
	update_sensor(sensor_map);
	clear_sensor_bit(47, sensor_map);
	j = query_train_path(0, train_path, time_stamp);
	printf("Update %d...\n", j);
	ts = get_train_state(0);
    printf("Train is in %d ---> %d", (*ts).last_sensor_id, (*ts).next_sensor_id);
	
	//C6
	set_sensor_bit(37, sensor_map);
	attach_timestamp(3);
	update_sensor(sensor_map);
	clear_sensor_bit(37, sensor_map);
	ts = get_train_state(0);
    printf("Train is in %d ---> %d", (*ts).last_sensor_id, (*ts).next_sensor_id);
	
	//B15
	set_sensor_bit(30, sensor_map);
	attach_timestamp(4);
	update_sensor(sensor_map);
	clear_sensor_bit(30, sensor_map);
	ts = get_train_state(0);
    printf("Train is in %d ---> %d", (*ts).last_sensor_id, (*ts).next_sensor_id);
	
	//A3
	set_sensor_bit(2, sensor_map);
	attach_timestamp(5);
	update_sensor(sensor_map);
	clear_sensor_bit(2, sensor_map);
    ts = get_train_state(0);
    printf("Train is in %d ---> %d", (*ts).last_sensor_id, (*ts).next_sensor_id);
	
	//C13
	set_sensor_bit(44, sensor_map);
	attach_timestamp(6);
	update_sensor(sensor_map);
	clear_sensor_bit(44, sensor_map);
    ts = get_train_state(0);
    printf("Train is in %d ---> %d", (*ts).last_sensor_id, (*ts).next_sensor_id);
	
	//E7
	set_sensor_bit(70, sensor_map);
	attach_timestamp(7);
	update_sensor(sensor_map);
	clear_sensor_bit(70, sensor_map);
    ts = get_train_state(0);
    printf("Train is in %d ---> %d", (*ts).last_sensor_id, (*ts).next_sensor_id);
	
	//D7
	set_sensor_bit(54, sensor_map);
	attach_timestamp(8);
	update_sensor(sensor_map);
	clear_sensor_bit(54, sensor_map);
    ts = get_train_state(0);
    printf("Train is in %d ---> %d", (*ts).last_sensor_id, (*ts).next_sensor_id);
	
	//D9
	set_sensor_bit(56, sensor_map);
	attach_timestamp(9);
	update_sensor(sensor_map);
	clear_sensor_bit(56, sensor_map);
    ts = get_train_state(0);
    printf("Train is in %d ---> %d", (*ts).last_sensor_id, (*ts).next_sensor_id);
	
	//E12
	set_sensor_bit(75, sensor_map);
	attach_timestamp(10);
	update_sensor(sensor_map);
	clear_sensor_bit(75, sensor_map);
    ts = get_train_state(0);
    printf("Train is in %d ---> %d", (*ts).last_sensor_id, (*ts).next_sensor_id);
	
	j = query_train_path(0, train_path, time_stamp);
	printf("Update %d...\n", j);
	for (i=0;i<j;i++) {
		printf("train 1 hit sensor %d at time %d...\n", train_path[i], time_stamp[i]);
	}
}

int main() {
	initialize_track_information();
	int i,j;
	char sensor_map[sensor_module_number*2];
	initiate_track_monitor();
	
	if (sensor_map_empty()) {
		printf("true, it's empty...\n");
	}
	
	//train_path_test();
	track_traverse_test(37);
	//track_traverse_sensor_test();
    //track_run_test();
	return 0;
}
