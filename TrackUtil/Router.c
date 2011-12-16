#ifdef TEST
#include <stdio.h>
#endif

#include <TrackInformation.h>
#include <track_node.h>
#include <track_data.h>
#include <Router.h>
route_edge TrackMap[TRACK_MAX];
void initialize_track_route_map() {
    int i;
    for (i = 0; i<TRACK_MAX; i++){
          TrackMap[i].length = NODE_AVAILABLE;
          TrackMap[i].from = -1;
    }
}

void mark_start_node(route_edge *route_map, int node_id) {
    int i, reverse_node_id;
    track_node *node;
    node = get_node(node_id);
    reverse_node_id = get_node_id(node->reverse);
    for (i = 0; i<TRACK_MAX; i++){
          route_map[i].length = NODE_AVAILABLE;
          route_map[i].from = -1;
          
          if (i==node_id || i==reverse_node_id) {
            route_map[i].length = 0;
            route_map[i].from = node_id;
          }
          if (TrackMap[i].length!=NODE_AVAILABLE)route_map[i].length = TrackMap[i].length;
    }
}

int add_nearest_node_to_map(route_edge *route_map) {
    int from, to, distance;
    int route_length, new_from, new_to, total_length = MAX_ROUTE_LENGTH;
    
    track_node *node;
    for (from = 0; from < TRACK_MAX; from++) {
        route_length = route_map[from].length;
        // if not in map, do not process
        if (route_length<0)continue;
        
        node = get_node(from);
        switch (node->type) {
            case NODE_EXIT:
                continue;
                break;
            case NODE_BRANCH:
                // Reject if already in map
                if (route_map[get_node_id(node->edge[ DIR_STRAIGHT ].dest)].length>=0) {
                    distance = node->edge[ DIR_CURVED ].dist;
                    to = get_node_id(node->edge[ DIR_CURVED ].dest);
                    break;
                }
                // Reject if already in map
                if (route_map[get_node_id(node->edge[ DIR_CURVED ].dest)].length>=0) {
                    distance = node->edge[ DIR_STRAIGHT ].dist;
                    to = get_node_id(node->edge[ DIR_STRAIGHT ].dest);
                    break;
                }
                
                distance = node->edge[ DIR_STRAIGHT ].dist;
                to = get_node_id(node->edge[ DIR_STRAIGHT ].dest);
                if (distance>node->edge[ DIR_CURVED ].dist) {
                    distance = node->edge[ DIR_CURVED ].dist;
                    to = get_node_id(node->edge[ DIR_CURVED ].dest);
                }
                break;
            default:
                distance = node->edge[ DIR_AHEAD ].dist;
                to = get_node_id(node->edge[ DIR_AHEAD ].dest);
                break;
        }
        // Reject if already in map
        if (route_map[to].length>=0 || route_map[to].length==NODE_UNAVAILABLE)continue;
        if (distance+route_length>total_length) {
            // Reject if too long
            continue;
        } else {
            // Only case the new edge can be updated
            new_from = from;
            new_to = to;
            total_length = distance+route_length;
        }
    }
    // success if there is new edge
    if (total_length<MAX_ROUTE_LENGTH) {
        route_map[new_to].length = total_length;
        route_map[new_to].from = new_from;
        
        new_from = new_to;
        new_to = get_node_id(get_node(new_to)->reverse);
        route_map[new_to].length = total_length;
        route_map[new_to].from = new_from;
        return 1;
    }
    // failure
    return 0;
}

void update_switches_to_route(route_edge *route_map, int node_id, int *switch_map) {
    int from, offset;
    track_node *to_node, *from_node;
    for (from=0;from<total_switch_number;from++)switch_map[from] = DIR_IGNORE;
    
    from = route_map[node_id].from;
    from_node = get_node(node_id);
    while(1) {
        to_node = from_node;
        from = route_map[from].from;
        from_node = get_node(from);
        if (to_node==from_node)break;
        if (to_node->reverse==from_node)continue;
        
        if (from_node->type==NODE_BRANCH) {
            offset = 1;
            if (from_node->num>17)offset = 135;
            if (to_node==from_node->edge[ DIR_STRAIGHT ].dest) {
                switch_map[from_node->num-offset] = DIR_STRAIGHT;
            } else {
                switch_map[from_node->num-offset] = DIR_CURVED;
            }
        }
    }
}

void fill_action_queue_to_route(route_edge *route_map, int node_id, int speed, train_action_queue *taq) {
    int from, i = 0, reverse_list[TRACK_MAX];
    track_node *to_node, *from_node;
    
    add_action(taq, action_set_speed, 0, 0, speed, 0);
    
    from = node_id;
    from_node = get_node(node_id);
    while(1) {
        to_node = from_node;
        from = route_map[from].from;
        from_node = get_node(from);
        if (to_node==from_node)break;
        if (to_node->reverse==from_node) {
            reverse_list[i] = from;
            i++;
        }
    }
    
    while (i--) {
        add_action(taq, action_wait_stop_for_node_id_offset, reverse_list[i], 0, 0, 0);
        //add_action(taq, action_wait_node_id_offset, get_node_id(get_next_node(get_node(reverse_list[i]), &offset)), 0, 0, 0);
        add_action(taq, action_set_speed, 0, 0, 0, 0);
        add_action(taq, action_wait_velocity_eq, 0, 0, 0, 0);
        add_action(taq, action_set_speed, 0, 0, 15, 0);
        add_action(taq, action_set_speed, 0, 0, speed, 0);
    }
    add_action(taq, action_wait_node_id_offset, node_id, 0, 0, 0);
    add_action(taq, action_set_speed, 0, 0, 0, 0);
}

