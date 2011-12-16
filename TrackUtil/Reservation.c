#ifdef TEST
#include <stdio.h>
#endif
#include <Reservation.h>
#include <TrackInformation.h>
#include <TrainProfile.h>
#include <track_data.h>

int RESERVATIONS[ TRACK_MAX ];
train_reservation volatile TRAIN_RESERVATION[number_of_trains];

void initialize_reservations() {
	int i;
	for ( i = 0 ; i < TRACK_MAX ; i++ ) {
		RESERVATIONS[ i ] = -1;
	}
	
	for (i=0; i<number_of_trains; i++) {
	    TRAIN_RESERVATION[i].train_number = -1;
		TRAIN_RESERVATION[i].blocked_by = -1;
	    TRAIN_RESERVATION[i].last_sensor_id = -1;
	    TRAIN_RESERVATION[i].last_reservation_status = RESERV_FAIL;
    }
}

int reserve_node_id(int train_number, int node_id) {
	if (RESERVATIONS[node_id] != -1 && RESERVATIONS[node_id] != train_number) {
		int i;
		for (i=0; i<number_of_trains; i++) {
			if (TRAIN_RESERVATION[i].train_number == train_number)TRAIN_RESERVATION[i].blocked_by = RESERVATIONS[node_id];
		}
		return RESERV_FAIL;
	}
	RESERVATIONS[node_id] = train_number;
	RESERVATIONS[get_reverse_node_id(get_node(node_id))] = train_number;
	
	return RESERV_SUCC;
}

int free_node_id(int train_number, int node_id) {
    int last_train_number = train_number;
    if ( RESERVATIONS[node_id] != train_number ) {
		last_train_number = RESERVATIONS[node_id];
	}
	RESERVATIONS[node_id] = -1;
	RESERVATIONS[get_reverse_node_id(get_node(node_id))] = -1;

	return last_train_number;
}

int train_at( int train_number, int stopping_distance, int node_id ) {
	int train_index, last_sensor_id=-1, last_reservation_status, distance;
	for (train_index=0; train_index<number_of_trains; train_index++) {
	    if (TRAIN_RESERVATION[train_index].train_number==train_number) {
	        last_sensor_id = TRAIN_RESERVATION[train_index].last_sensor_id;
	        last_reservation_status = TRAIN_RESERVATION[train_index].last_reservation_status;
	        break;
	    }
    }
	
	if (train_index<number_of_trains) {
		// don't redo work
		if (last_sensor_id==node_id && last_reservation_status!=RESERV_FAIL) {
			return (TRAIN_RESERVATION[train_index].last_reservation_status=RESERV_SUCC_NOCHANGE);
		}
	} else {
		// create new profile for new train
		while (1) {
			if (TRAIN_RESERVATION[train_index].train_number<0) {
				TRAIN_RESERVATION[train_index].train_number = train_number;
				last_sensor_id = TRAIN_RESERVATION[train_index].last_sensor_id = node_id;
				last_reservation_status = TRAIN_RESERVATION[train_index].last_reservation_status = RESERV_FAIL;
				break;
			}
			if (train_index==0)return RESERV_FAIL;
			train_index--;
		}
	}
    
    // free node from last sensor to current sensor
    track_node *current_node, *target_node;
    current_node = get_node(last_sensor_id);
    target_node = get_node(node_id);
    while (current_node!=target_node) {
        free_node_id(train_number, get_node_id(current_node));
        current_node = get_next_node(current_node, &distance);
    }
	
	// Find out the reservation result
	/* reserve the necessary nodes infront of the train */
	while (last_reservation_status!=RESERV_SUCC) {
		// stop anytime if fail reservation
		if (reserve_node_id(train_number, get_node_id(current_node))==RESERV_FAIL) {
			int block_by = train_number;
			while (1) {
				int i;
				for (i=0; i<number_of_trains; i++) {
					if (TRAIN_RESERVATION[i].train_number == block_by)block_by = TRAIN_RESERVATION[i].blocked_by;
				}
				
				if (block_by == -1) {
					last_reservation_status=RESERV_FAIL;
					break;
				}
				if (block_by == train_number) {
					last_reservation_status=RESERV_FAIL_DEADLOCK;
					break;
				}
			}
			break;
		}
		// this trick reserve one more sensor ahead
	    if (stopping_distance<=0)last_reservation_status=RESERV_SUCC;

		current_node = get_next_sensor( current_node, &distance );	
		stopping_distance -= distance;	
	}
	
	
	return (TRAIN_RESERVATION[train_index].last_reservation_status = last_reservation_status);
}

int *get_reservation_map() {
	return RESERVATIONS;
}
