#ifdef TRAIN_PROFILE_H
#else
#define TRAIN_PROFILE_H
#include <TrackMonitor.h>
#include <TrackInformation.h>

#define TRAIN_PROFILE_NUMBER		3
typedef struct {
	int train_number;
	int acceleration;
	int deceleration;
	int stopping_distance[ 2 ][ speed_number ];
	int velocities[ 2 ][ speed_number ];
} train_profile;

void initialize_train_profiles();
int acceleration_profile(int train_number);
int deceleration_profile(int train_number);
int velocity_profile(int train_number, int train_speed);
int stopping_distance_profile(int train_number, int train_speed);
#endif
