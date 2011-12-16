#include <track_node.h>
void switch_profile(int id, int *switch_position) {
    switch (id) {
        case 0:
            // Big loop
            switch_position[0] = DIR_STRAIGHT;
            switch_position[1] = DIR_STRAIGHT;
            switch_position[2] = DIR_STRAIGHT;
            switch_position[3] = DIR_STRAIGHT;
            switch_position[4] = DIR_STRAIGHT;
            switch_position[5] = DIR_STRAIGHT;
            switch_position[6] = DIR_STRAIGHT;
            switch_position[7] = DIR_STRAIGHT;
            switch_position[8] = DIR_STRAIGHT;
            switch_position[9] = DIR_STRAIGHT;
            switch_position[10] = DIR_CURVED;
            switch_position[11] = DIR_STRAIGHT;
            switch_position[12] = DIR_STRAIGHT;
            switch_position[13] = DIR_STRAIGHT;
            switch_position[14] = DIR_CURVED;
            switch_position[15] = DIR_STRAIGHT;
            switch_position[16] = DIR_STRAIGHT;
            switch_position[17] = DIR_STRAIGHT;
            switch_position[18] = DIR_STRAIGHT;
            switch_position[19] = DIR_STRAIGHT;
            switch_position[20] = DIR_STRAIGHT;
            switch_position[21] = DIR_STRAIGHT;
            break;
        case 1:
            // Small circle
            switch_position[0] = DIR_CURVED;
            switch_position[1] = DIR_CURVED;
            switch_position[2] = DIR_CURVED;
            switch_position[3] = DIR_CURVED;
            switch_position[4] = DIR_CURVED;
            switch_position[5] = DIR_CURVED;
            switch_position[6] = DIR_CURVED;
            switch_position[7] = DIR_CURVED;
            switch_position[8] = DIR_CURVED;
            switch_position[9] = DIR_CURVED;
            switch_position[10] = DIR_CURVED;
            switch_position[11] = DIR_CURVED;
            switch_position[12] = DIR_CURVED;
            switch_position[13] = DIR_CURVED;
            switch_position[14] = DIR_CURVED;
            switch_position[15] = DIR_CURVED;
            switch_position[16] = DIR_CURVED;
            switch_position[17] = DIR_CURVED;
            switch_position[18] = DIR_CURVED;
            switch_position[19] = DIR_STRAIGHT;
            switch_position[20] = DIR_STRAIGHT;
            switch_position[21] = DIR_CURVED;
            break;
        case 2:
            break;
    }
}
