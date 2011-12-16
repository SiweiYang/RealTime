#include <TrackInformation.h>
#include <TrackMonitor.h>
#include <track_node.h>
#include <track_data.h>
#include <Router.h>
#include <stdio.h>

int main() {
      initialize_track_information();
      initialize_track_route_map();
      route_edge route_map[TRACK_MAX];
      int i;
      mark_start_node(route_map, 37);
      while(add_nearest_node_to_map(route_map)) {
            if (route_map[19].length>-1) {
                break;
            }
      }
      
      //for (i=0;i<TRACK_MAX;i++){
      //      printf("Node %d -> (%d, %d)\n", i, route_map[i].length, route_map[i].from);
      //}
      
      printf("The Switches to be Updated...\n");
      int switch_map[total_switch_number];
      update_switches_to_route(route_map, 19, switch_map);
      for (i=0;i<total_switch_number;i++){
            if (switch_map[i]==DIR_STRAIGHT) {
                  printf("Switch %d -> Straight\n", i+1);
            } else if (switch_map[i]==DIR_CURVED) {
                  printf("Switch %d -> Curved\n", i+1);
            } else {
                  continue;
            }
      }
      
      train_action_queue taq;
      taq.head = 0;
      taq.tail = 0;
      fill_action_queue_to_route(route_map, 19, 12, &taq);
      for (;taq.head<taq.tail;taq.head++){
            switch (taq.pool[taq.head].action_id) {
                  case action_wait_velocity_eq:
                        printf("Wait for velocity == %d\n", taq.pool[taq.head].velocity);
                        break;
                  case action_wait_node_id_offset:
                        printf("Wait for node == %d and offset == %d\n", taq.pool[taq.head].node_id, taq.pool[taq.head].node_offset);
                        break;
                  case action_set_speed:
                        printf("Set speed = %d\n", taq.pool[taq.head].speed);
                        break;
                  default:
                        break;
            }
      }
      
      return 0;
}
