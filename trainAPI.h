#ifdef TRAIN_API
#else
#define TRAIN_API

/*
 * The definition file to help with communication with the train
 *
 */

#define TRAIN_GO	0x01100000
#define TRAIN_STOP	0x01100001

#define UNKNOWN_COMMAND 0
#define TRAIN_SPEED     1
#define TRAIN_STOP_AT   2
#define TRAIN_REVERSE   3
#define SWITCH_TURN     4
#define QUIT_PROGRAM 	5
#define GO_TO		    6
#define ROUTE		    7
#define SCHE_TO         8
#define LOAD_TRACK		9
#define LOAD_SWITCHES   10
#define TRAIN_SENSOR    11
#define TRAIN_CLEAR     12

struct Operation {
    int type;
    int train_number;
    int time_out;
    char command[3];
};

int checkCommand(char *line);

int parseTrainSpeed(char *line, int *train_number, int *train_speed);
int parseTrainReverse(char *line, int *train_number);
int parseSwitchTurn(char *line, int *switch_number, int *switch_direction);

int parseGoTo( char *line, int *tn, int *sn );
int parseTrainStop(char *line, int *tn, int *sn);
int parseRoute( char *line, int *tn, int *ts, int *sn );

int parseLoadTrack(char *line, int *track_number);
int parseLoadSwitches(char *line, int *switch_profile);
int parseTrainSensor(char *line, int *tn, int *sn);
int parseTrainClear(char *line, int *tn);
#endif
