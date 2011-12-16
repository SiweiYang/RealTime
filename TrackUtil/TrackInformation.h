#ifdef TRACK_INFORMATION_H
#else
#define TRACK_INFORMATION_H
#include <track_node.h>

#define sensor_module_number    5
#define total_switch_number     22
#define speed_number            15

#define ASCENDING 		        0
#define DESCENDING		        1
void initialize_track(int track_id);
void initialize_track_information();
void update_switch( int switch_id, char status );
track_node *get_node(int id);
int get_node_id(track_node *node);
int get_reverse_node_id(track_node *node);
int get_switch( int switch_id );
int *get_switch_map();
track_node *get_next_node(track_node *component, int *distance);
int get_next_node_id(int node_id, int *next_node_id);

track_edge * get_edge( track_node *node );

track_node *get_next_sensor( track_node *sensor, int *distance );
int get_next_sensor_id( int sensor_id, int *next_sensor_id );
int get_previous_sensor_id( int sensor_id, int *previous_sensor_id );

// Assumes one time unit passed for each invocation
track_node *get_location( int train_number, int train_speed, track_node *component, int start_location, int start_velocity, int *end_location, int *end_velocity );

void set_sensor_bit( int sensor, char *temp_map );
void clear_sensor_bit( int sensor, char *temp_map );
int get_sensor_number(char *temp_map);

int test_immediate_sensor(track_node *sensor, track_node *next_sensor);
int test_immediate_sensor_id(int sensor_id, int next_sensor_id);

// helper function for TrackMonitor
void set_reverse_bits( int last_known_sensor, char *temp_map );
void set_forward_bits( int last_known_sensor, char *temp_map );

// return node id and distance offset
int get_stop_node_id( int train_number, int train_speed, int start_sensor_id, int stop_sensor_id, int *node_id );

void curve_compensation(track_node *nodes);
#endif
