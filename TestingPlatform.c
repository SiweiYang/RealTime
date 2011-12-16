#include <uC++.h>

#include <ContextSwitch.h>
#include <NameServer.h>
#include <LockServer.h>
#include <ClockServer.h>

#include <TrainTask.h>
#include <TrackUtil/TrackMonitor.h>
#include <TrackUtil/TrackInformation.h>

extern int CLOCK_SERVER_TID;
extern int NAME_SERVER_TID;
void test_train_task() {
    NAME_SERVER_TID = Create(6, "NAMESERVER", &name_server);
    Create(6, "LCK_SERVER", &lock_server);
    
    //Delay(100);
    initialize_track_information();
    initiate_track_monitor();
    Create(3, "TRAIN_IN", &train_input);
    Create(3, "SENSOR", &sensor_reader);
    Delay(100);
    Create(5, "TIMER", &update_timer);
    
    Exit();
}
