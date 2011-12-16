#include <StringUtil.h>
#include <trainAPI.h>
#include <ts7200.h>

int checkCommand(char *line) {
	int ret = UNKNOWN_COMMAND;
	switch(line[0]) {
		case 't':
			if (line[1]=='r')ret = TRAIN_SPEED;
			if (line[1]=='s')ret = TRAIN_SENSOR;
			if (line[1]=='c')ret = TRAIN_CLEAR;
			break;
		case 'r':
			if (line[1]=='v')ret = TRAIN_REVERSE;
			if (line[1]=='t')ret = ROUTE;
			break;
		case 's':
			if (line[1]=='w')ret = SWITCH_TURN;
			if (line[1]=='t')ret = TRAIN_STOP_AT;
			break;
		case 'q':
			ret = QUIT_PROGRAM;
			break;
		case 'g':
			if (line[1]=='t')ret = GO_TO;
			break;
		case 'l':
			if (line[1]=='t')ret = LOAD_TRACK;
			if (line[1]=='s')ret = LOAD_SWITCHES;
			break;
		default:
			break;
	}

	return ret;
}

int parseTrainSpeed(char *line, int *train_number, int *train_speed) {
	//int train_number = 0, train_speed = -1;
	readNumber(line+2+readNumber(line+2, train_number), train_speed);

	if (*train_number<1 || *train_number>80 || *train_speed<0 || *train_speed>14)return 0;

	return 1;
}

int parseTrainReverse(char *line, int *train_number) {
	readNumber(line+2, train_number);

	if (*train_number<1 || *train_number>80)return 0;
	return 1;
}

int parseSwitchTurn(char *line, int *switch_number, int *switch_direction) {
	int i = 2 + readNumber(line+2, switch_number);

	while (line[i]==' ')i++;

	if (line[i]=='S') {
		*switch_direction = 'S';
	} else if (line[i]=='C') {
		*switch_direction = 'C';
	} else {
		return 0;
	}

	if (*switch_number<0 || *switch_number>255)return 0;
	return 1;
}

int parseGoTo( char *line, int *tn, int *sn ) {
	int i = 2 + readNumber(line+2, tn);

	readNumber( line+i+2, sn );
	if ((*sn) < 0 || (*sn) > 16 ) {
		return 0;
	}

	i++;
	if ( (line[i] - 'A') < 0 || (line[i] - 'A') > 4 ) {
		return 0;
	}
	*sn = *sn + (line[i] - 'A') * 16 - 1;

	return 1;
}

int parseTrainStop( char *line, int *tn, int *sn ) {
	return parseGoTo(line, tn, sn);
}

int parseRoute( char *line, int *tn, int *ts, int *sn ) {
	int i = 2 + readNumber(line+2, tn);
	i += readNumber( line+i, ts );

	readNumber( line+i+2, sn );
	if ((*sn) < 0 || (*sn) > 16 ) {
		return 0;
	}

	i++;
	if ( (line[i] - 'A') < 0 || (line[i] - 'A') > 4 ) {
		return 0;
	}
	*sn = *sn + (line[i] - 'A') * 16 - 1;
	
	return 1;
}

int parseLoadTrack(char *line, int *track_number) {
	readNumber(line+2, track_number);

	if (*track_number<1 || *track_number>2)return 0;
	return 1;
}

int parseLoadSwitches(char *line, int *switch_profile) {
	readNumber(line+2, switch_profile);

	if (*switch_profile<0)return 0;
	return 1;
}

int parseTrainSensor(char *line, int *tn, int *sn) {
	return parseGoTo(line, tn, sn);
}

int parseTrainClear(char *line, int *tn) {
	return parseTrainReverse(line, tn);
}
