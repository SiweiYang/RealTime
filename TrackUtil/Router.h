#ifdef ROUTER_H
#else
#define ROUTER_H
#include <TrackMonitor.h>

#define MAX_ROUTE_LENGTH        0xFFFF
#define NODE_AVAILABLE          -1
#define NODE_UNAVAILABLE        -2
#define NODE_LOCKED             -3
typedef struct {
    int length;
    int from;
} route_edge;
void initialize_track_route_map();
void mark_start_node(route_edge *route_map, int node_id);

int add_nearest_node_to_map(route_edge *route_map);
void update_switches_to_route(route_edge *route_map, int node_id, int *switch_map);
void fill_action_queue_to_route(route_edge *route_map, int node_id, int speed, train_action_queue *taq);
#endif
