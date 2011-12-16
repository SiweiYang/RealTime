#include <track_node.h>
#include <TrainProfile.h>
#include <TrackMonitor.h>
#include <TrackInformation.h>

train_profile TRAIN_PROFILES[ TRAIN_PROFILE_NUMBER ];
void initialize_train_profiles() {
    TRAIN_PROFILES[0].train_number = 39;
    TRAIN_PROFILES[0].acceleration = 30;
    TRAIN_PROFILES[0].deceleration = 31;

    TRAIN_PROFILES[0].stopping_distance[ ASCENDING ][ 0 ] = 100;
    TRAIN_PROFILES[0].stopping_distance[ ASCENDING ][ 1 ] = 100;
    TRAIN_PROFILES[0].stopping_distance[ ASCENDING ][ 2 ] = 100;
    TRAIN_PROFILES[0].stopping_distance[ ASCENDING ][ 3 ] = 100;
    TRAIN_PROFILES[0].stopping_distance[ ASCENDING ][ 4 ] = 200;
    TRAIN_PROFILES[0].stopping_distance[ ASCENDING ][ 5 ] = 300;
    TRAIN_PROFILES[0].stopping_distance[ ASCENDING ][ 6 ] = 300;
    TRAIN_PROFILES[0].stopping_distance[ ASCENDING ][ 7 ] = 300;
    TRAIN_PROFILES[0].stopping_distance[ ASCENDING ][ 8 ] = 400;
    TRAIN_PROFILES[0].stopping_distance[ ASCENDING ][ 9 ] = 500;
    TRAIN_PROFILES[0].stopping_distance[ ASCENDING ][ 10 ] = 690; // T39
    TRAIN_PROFILES[0].stopping_distance[ ASCENDING ][ 11 ] = 700;
    TRAIN_PROFILES[0].stopping_distance[ ASCENDING ][ 12 ] = 750; // T39
    TRAIN_PROFILES[0].stopping_distance[ ASCENDING ][ 13 ] = 810;
    TRAIN_PROFILES[0].stopping_distance[ ASCENDING ][ 14 ] = 860;

    TRAIN_PROFILES[0].stopping_distance[ DESCENDING ][ 0 ] = 100;
    TRAIN_PROFILES[0].stopping_distance[ DESCENDING ][ 1 ] = 100;
    TRAIN_PROFILES[0].stopping_distance[ DESCENDING ][ 2 ] = 100;
    TRAIN_PROFILES[0].stopping_distance[ DESCENDING ][ 3 ] = 100;
    TRAIN_PROFILES[0].stopping_distance[ DESCENDING ][ 4 ] = 200;
    TRAIN_PROFILES[0].stopping_distance[ DESCENDING ][ 5 ] = 300;
    TRAIN_PROFILES[0].stopping_distance[ DESCENDING ][ 6 ] = 300;
    TRAIN_PROFILES[0].stopping_distance[ DESCENDING ][ 7 ] = 300;
    TRAIN_PROFILES[0].stopping_distance[ DESCENDING ][ 8 ] = 400;
    TRAIN_PROFILES[0].stopping_distance[ DESCENDING ][ 9 ] = 500;
    TRAIN_PROFILES[0].stopping_distance[ DESCENDING ][ 10 ] = 635; // T39
    TRAIN_PROFILES[0].stopping_distance[ DESCENDING ][ 11 ] = 760;
    TRAIN_PROFILES[0].stopping_distance[ DESCENDING ][ 12 ] = 760; // T39
    TRAIN_PROFILES[0].stopping_distance[ DESCENDING ][ 13 ] = 820;
    TRAIN_PROFILES[0].stopping_distance[ DESCENDING ][ 14 ] = 890;

    TRAIN_PROFILES[0].velocities[ ASCENDING ][ 0 ] = 0;
    TRAIN_PROFILES[0].velocities[ ASCENDING ][ 1 ] = 20;
    TRAIN_PROFILES[0].velocities[ ASCENDING ][ 2 ] = 70;
    TRAIN_PROFILES[0].velocities[ ASCENDING ][ 3 ] = 150;
    TRAIN_PROFILES[0].velocities[ ASCENDING ][ 4 ] = 220;
    TRAIN_PROFILES[0].velocities[ ASCENDING ][ 5 ] = 270;
    TRAIN_PROFILES[0].velocities[ ASCENDING ][ 6 ] = 320;
    TRAIN_PROFILES[0].velocities[ ASCENDING ][ 7 ] = 380;
    TRAIN_PROFILES[0].velocities[ ASCENDING ][ 8 ] = 430;
    TRAIN_PROFILES[0].velocities[ ASCENDING ][ 9 ] = 485;
    TRAIN_PROFILES[0].velocities[ ASCENDING ][ 10 ] = 520; // T39
    TRAIN_PROFILES[0].velocities[ ASCENDING ][ 11 ] = 570;
    TRAIN_PROFILES[0].velocities[ ASCENDING ][ 12 ] = 610; // T39
    TRAIN_PROFILES[0].velocities[ ASCENDING ][ 13 ] = 660;
    TRAIN_PROFILES[0].velocities[ ASCENDING ][ 14 ] = 720;

    TRAIN_PROFILES[0].velocities[ DESCENDING ][ 0 ] = 0;
    TRAIN_PROFILES[0].velocities[ DESCENDING ][ 1 ] = 10;
    TRAIN_PROFILES[0].velocities[ DESCENDING ][ 2 ] = 60;
    TRAIN_PROFILES[0].velocities[ DESCENDING ][ 3 ] = 130;
    TRAIN_PROFILES[0].velocities[ DESCENDING ][ 4 ] = 210;
    TRAIN_PROFILES[0].velocities[ DESCENDING ][ 5 ] = 270;
    TRAIN_PROFILES[0].velocities[ DESCENDING ][ 6 ] = 320;
    TRAIN_PROFILES[0].velocities[ DESCENDING ][ 7 ] = 380;
    TRAIN_PROFILES[0].velocities[ DESCENDING ][ 8 ] = 430;
    TRAIN_PROFILES[0].velocities[ DESCENDING ][ 9 ] = 485;
    TRAIN_PROFILES[0].velocities[ DESCENDING ][ 10 ] = 530; // T39
    TRAIN_PROFILES[0].velocities[ DESCENDING ][ 11 ] = 570;
    TRAIN_PROFILES[0].velocities[ DESCENDING ][ 12 ] = 620; // T39
    TRAIN_PROFILES[0].velocities[ DESCENDING ][ 13 ] = 670;
    TRAIN_PROFILES[0].velocities[ DESCENDING ][ 14 ] = 720;

    TRAIN_PROFILES[1].train_number = 21;
    TRAIN_PROFILES[1].acceleration = 30;
    TRAIN_PROFILES[1].deceleration = 31;

    TRAIN_PROFILES[1].stopping_distance[ ASCENDING ][ 0 ] = 100;
    TRAIN_PROFILES[1].stopping_distance[ ASCENDING ][ 1 ] = 100;
    TRAIN_PROFILES[1].stopping_distance[ ASCENDING ][ 2 ] = 100;
    TRAIN_PROFILES[1].stopping_distance[ ASCENDING ][ 3 ] = 100;
    TRAIN_PROFILES[1].stopping_distance[ ASCENDING ][ 4 ] = 100;
    TRAIN_PROFILES[1].stopping_distance[ ASCENDING ][ 5 ] = 200;
    TRAIN_PROFILES[1].stopping_distance[ ASCENDING ][ 6 ] = 300;
    TRAIN_PROFILES[1].stopping_distance[ ASCENDING ][ 7 ] = 300;
    TRAIN_PROFILES[1].stopping_distance[ ASCENDING ][ 8 ] = 400;
    TRAIN_PROFILES[1].stopping_distance[ ASCENDING ][ 9 ] = 500;
    TRAIN_PROFILES[1].stopping_distance[ ASCENDING ][ 10 ] = 690; // T39
    TRAIN_PROFILES[1].stopping_distance[ ASCENDING ][ 11 ] = 700;
    TRAIN_PROFILES[1].stopping_distance[ ASCENDING ][ 12 ] = 750; // T39
    TRAIN_PROFILES[1].stopping_distance[ ASCENDING ][ 13 ] = 810;
    TRAIN_PROFILES[1].stopping_distance[ ASCENDING ][ 14 ] = 860;

    TRAIN_PROFILES[1].stopping_distance[ DESCENDING ][ 0 ] = 100;
    TRAIN_PROFILES[1].stopping_distance[ DESCENDING ][ 1 ] = 100;
    TRAIN_PROFILES[1].stopping_distance[ DESCENDING ][ 2 ] = 100;
    TRAIN_PROFILES[1].stopping_distance[ DESCENDING ][ 3 ] = 100;
    TRAIN_PROFILES[1].stopping_distance[ DESCENDING ][ 4 ] = 100;
    TRAIN_PROFILES[1].stopping_distance[ DESCENDING ][ 5 ] = 200;
    TRAIN_PROFILES[1].stopping_distance[ DESCENDING ][ 6 ] = 300;
    TRAIN_PROFILES[1].stopping_distance[ DESCENDING ][ 7 ] = 300;
    TRAIN_PROFILES[1].stopping_distance[ DESCENDING ][ 8 ] = 400;
    TRAIN_PROFILES[1].stopping_distance[ DESCENDING ][ 9 ] = 500;
    TRAIN_PROFILES[1].stopping_distance[ DESCENDING ][ 10 ] = 635; // T39
    TRAIN_PROFILES[1].stopping_distance[ DESCENDING ][ 11 ] = 760;
    TRAIN_PROFILES[1].stopping_distance[ DESCENDING ][ 12 ] = 760; // T39
    TRAIN_PROFILES[1].stopping_distance[ DESCENDING ][ 13 ] = 820;
    TRAIN_PROFILES[1].stopping_distance[ DESCENDING ][ 14 ] = 890;

    TRAIN_PROFILES[1].velocities[ ASCENDING ][ 0 ] = 0;
    TRAIN_PROFILES[1].velocities[ ASCENDING ][ 1 ] = 0;
    TRAIN_PROFILES[1].velocities[ ASCENDING ][ 2 ] = 110;
    TRAIN_PROFILES[1].velocities[ ASCENDING ][ 3 ] = 160;
    TRAIN_PROFILES[1].velocities[ ASCENDING ][ 4 ] = 220;
    TRAIN_PROFILES[1].velocities[ ASCENDING ][ 5 ] = 270;
    TRAIN_PROFILES[1].velocities[ ASCENDING ][ 6 ] = 310;
    TRAIN_PROFILES[1].velocities[ ASCENDING ][ 7 ] = 370;
    TRAIN_PROFILES[1].velocities[ ASCENDING ][ 8 ] = 450;
    TRAIN_PROFILES[1].velocities[ ASCENDING ][ 9 ] = 520;
    TRAIN_PROFILES[1].velocities[ ASCENDING ][ 10 ] = 570; // T39
    TRAIN_PROFILES[1].velocities[ ASCENDING ][ 11 ] = 610;
    TRAIN_PROFILES[1].velocities[ ASCENDING ][ 12 ] = 660; // T39
    TRAIN_PROFILES[1].velocities[ ASCENDING ][ 13 ] = 720;
    TRAIN_PROFILES[1].velocities[ ASCENDING ][ 14 ] = 770;

    TRAIN_PROFILES[1].velocities[ DESCENDING ][ 0 ] = 0;
    TRAIN_PROFILES[1].velocities[ DESCENDING ][ 1 ] = 0;
    TRAIN_PROFILES[1].velocities[ DESCENDING ][ 2 ] = 150;
    TRAIN_PROFILES[1].velocities[ DESCENDING ][ 3 ] = 210;
    TRAIN_PROFILES[1].velocities[ DESCENDING ][ 4 ] = 270;
    TRAIN_PROFILES[1].velocities[ DESCENDING ][ 5 ] = 320;
    TRAIN_PROFILES[1].velocities[ DESCENDING ][ 6 ] = 380;
    TRAIN_PROFILES[1].velocities[ DESCENDING ][ 7 ] = 460;
    TRAIN_PROFILES[1].velocities[ DESCENDING ][ 8 ] = 520;
    TRAIN_PROFILES[1].velocities[ DESCENDING ][ 9 ] = 535;
    TRAIN_PROFILES[1].velocities[ DESCENDING ][ 10 ] = 570; // T39
    TRAIN_PROFILES[1].velocities[ DESCENDING ][ 11 ] = 610;
    TRAIN_PROFILES[1].velocities[ DESCENDING ][ 12 ] = 670; // T39
    TRAIN_PROFILES[1].velocities[ DESCENDING ][ 13 ] = 720;
    TRAIN_PROFILES[1].velocities[ DESCENDING ][ 14 ] = 780;
    
    TRAIN_PROFILES[2].train_number = 33;
    TRAIN_PROFILES[2].acceleration = 30;
    TRAIN_PROFILES[2].deceleration = 31;

    TRAIN_PROFILES[2].stopping_distance[ ASCENDING ][ 0 ] = 100;
    TRAIN_PROFILES[2].stopping_distance[ ASCENDING ][ 1 ] = 100;
    TRAIN_PROFILES[2].stopping_distance[ ASCENDING ][ 2 ] = 100;
    TRAIN_PROFILES[2].stopping_distance[ ASCENDING ][ 3 ] = 100;
    TRAIN_PROFILES[2].stopping_distance[ ASCENDING ][ 4 ] = 100;
    TRAIN_PROFILES[2].stopping_distance[ ASCENDING ][ 5 ] = 200;
    TRAIN_PROFILES[2].stopping_distance[ ASCENDING ][ 6 ] = 300;
    TRAIN_PROFILES[2].stopping_distance[ ASCENDING ][ 7 ] = 300;
    TRAIN_PROFILES[2].stopping_distance[ ASCENDING ][ 8 ] = 400;
    TRAIN_PROFILES[2].stopping_distance[ ASCENDING ][ 9 ] = 500;
    TRAIN_PROFILES[2].stopping_distance[ ASCENDING ][ 10 ] = 690; // T39
    TRAIN_PROFILES[2].stopping_distance[ ASCENDING ][ 11 ] = 700;
    TRAIN_PROFILES[2].stopping_distance[ ASCENDING ][ 12 ] = 750; // T39
    TRAIN_PROFILES[2].stopping_distance[ ASCENDING ][ 13 ] = 810;
    TRAIN_PROFILES[2].stopping_distance[ ASCENDING ][ 14 ] = 860;

    TRAIN_PROFILES[2].stopping_distance[ DESCENDING ][ 0 ] = 100;
    TRAIN_PROFILES[2].stopping_distance[ DESCENDING ][ 1 ] = 100;
    TRAIN_PROFILES[2].stopping_distance[ DESCENDING ][ 2 ] = 100;
    TRAIN_PROFILES[2].stopping_distance[ DESCENDING ][ 3 ] = 100;
    TRAIN_PROFILES[2].stopping_distance[ DESCENDING ][ 4 ] = 100;
    TRAIN_PROFILES[2].stopping_distance[ DESCENDING ][ 5 ] = 200;
    TRAIN_PROFILES[2].stopping_distance[ DESCENDING ][ 6 ] = 300;
    TRAIN_PROFILES[2].stopping_distance[ DESCENDING ][ 7 ] = 300;
    TRAIN_PROFILES[2].stopping_distance[ DESCENDING ][ 8 ] = 400;
    TRAIN_PROFILES[2].stopping_distance[ DESCENDING ][ 9 ] = 500;
    TRAIN_PROFILES[2].stopping_distance[ DESCENDING ][ 10 ] = 635; // T39
    TRAIN_PROFILES[2].stopping_distance[ DESCENDING ][ 11 ] = 760;
    TRAIN_PROFILES[2].stopping_distance[ DESCENDING ][ 12 ] = 760; // T39
    TRAIN_PROFILES[2].stopping_distance[ DESCENDING ][ 13 ] = 820;
    TRAIN_PROFILES[2].stopping_distance[ DESCENDING ][ 14 ] = 890;

    TRAIN_PROFILES[2].velocities[ ASCENDING ][ 0 ] = 0;
    TRAIN_PROFILES[2].velocities[ ASCENDING ][ 1 ] = 0;
    TRAIN_PROFILES[2].velocities[ ASCENDING ][ 2 ] = 110;
    TRAIN_PROFILES[2].velocities[ ASCENDING ][ 3 ] = 160;
    TRAIN_PROFILES[2].velocities[ ASCENDING ][ 4 ] = 220;
    TRAIN_PROFILES[2].velocities[ ASCENDING ][ 5 ] = 270;
    TRAIN_PROFILES[2].velocities[ ASCENDING ][ 6 ] = 310;
    TRAIN_PROFILES[2].velocities[ ASCENDING ][ 7 ] = 370;
    TRAIN_PROFILES[2].velocities[ ASCENDING ][ 8 ] = 450;
    TRAIN_PROFILES[2].velocities[ ASCENDING ][ 9 ] = 520;
    TRAIN_PROFILES[2].velocities[ ASCENDING ][ 10 ] = 570; // T39
    TRAIN_PROFILES[2].velocities[ ASCENDING ][ 11 ] = 610;
    TRAIN_PROFILES[2].velocities[ ASCENDING ][ 12 ] = 660; // T39
    TRAIN_PROFILES[2].velocities[ ASCENDING ][ 13 ] = 720;
    TRAIN_PROFILES[2].velocities[ ASCENDING ][ 14 ] = 770;

    TRAIN_PROFILES[2].velocities[ DESCENDING ][ 0 ] = 0;
    TRAIN_PROFILES[2].velocities[ DESCENDING ][ 1 ] = 0;
    TRAIN_PROFILES[2].velocities[ DESCENDING ][ 2 ] = 150;
    TRAIN_PROFILES[2].velocities[ DESCENDING ][ 3 ] = 210;
    TRAIN_PROFILES[2].velocities[ DESCENDING ][ 4 ] = 270;
    TRAIN_PROFILES[2].velocities[ DESCENDING ][ 5 ] = 320;
    TRAIN_PROFILES[2].velocities[ DESCENDING ][ 6 ] = 380;
    TRAIN_PROFILES[2].velocities[ DESCENDING ][ 7 ] = 460;
    TRAIN_PROFILES[2].velocities[ DESCENDING ][ 8 ] = 520;
    TRAIN_PROFILES[2].velocities[ DESCENDING ][ 9 ] = 535;
    TRAIN_PROFILES[2].velocities[ DESCENDING ][ 10 ] = 570; // T39
    TRAIN_PROFILES[2].velocities[ DESCENDING ][ 11 ] = 610;
    TRAIN_PROFILES[2].velocities[ DESCENDING ][ 12 ] = 670; // T39
    TRAIN_PROFILES[2].velocities[ DESCENDING ][ 13 ] = 720;
    TRAIN_PROFILES[2].velocities[ DESCENDING ][ 14 ] = 780;

}

int acceleration_profile(int train_number) {
    int i;
    for ( i = 0 ; i < TRAIN_PROFILE_NUMBER ; i++ ) {
        if ( TRAIN_PROFILES[ i ].train_number == train_number ) {
            return TRAIN_PROFILES[ i ].acceleration;
        }
    }
    return -1;
}
int deceleration_profile(int train_number) {
    int i;
    for ( i = 0 ; i < TRAIN_PROFILE_NUMBER ; i++ ) {
        if ( TRAIN_PROFILES[ i ].train_number == train_number ) {
            return TRAIN_PROFILES[ i ].deceleration;
        }
    }
    return -1;
}

int velocity_profile(int train_number, int train_speed) {
    int i, speed = train_speed & 0x0000FFFF;

    for ( i = 0 ; i < TRAIN_PROFILE_NUMBER ; i++ ) {
        if ( TRAIN_PROFILES[ i ].train_number == train_number ) {
            if (train_speed>>16) {
                return TRAIN_PROFILES[ i ].velocities[ DESCENDING ][ speed ];
            }
            else {
                return TRAIN_PROFILES[ i ].velocities[ ASCENDING ][ speed ];
            }
        }
    }
    return -1;
}

int stopping_distance_profile(int train_number, int train_speed) {
    int delay, velocity, deceleration, st_dist;
	delay = 35;
	velocity = (*get_train_state(train_number)).velocity;
	deceleration = deceleration_profile(train_number);
	st_dist = ( velocity * delay + velocity * velocity / 2 / deceleration ) / 1000;
    return st_dist;
}
