#ifdef RESERVATION_H
#else
#define RESERVATION_H

#define max_number_of_reservations  20
#define RESERV_SUCC                 0
#define RESERV_SUCC_NOCHANGE        1
#define RESERV_FAIL                 2
#define RESERV_FAIL_DEADLOCK        3

typedef struct {
    int train_number;
    int blocked_by;
    int last_sensor_id;
    int last_reservation_status;
} train_reservation;
/* Sets all the reservations to not reserved */
void initialize_reservations();

/* Returns -1 if the train got the reservations required to proceed 
   else this returns 'node num' which is the node number
   that the train must stop at to not collide */
int train_at( int train_number, int speed, int node );
int *get_reservation_map();
void get_reserved_nodes( char * line );

#endif
