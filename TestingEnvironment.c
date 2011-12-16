#include <uC++.h>
#include <uSemaphore.h>
#include <IO.h>

#include <fcntl.h>
#include <sys/stat.h>

#include <stdio.h>
#include <unistd.h>
#include <fstream>
#include <iostream>
using namespace std;

#include <TestingKernel.h>

extern RealTimeTask *ACTIVE_TASK;
extern uSemaphore task_lock;
uSemaphore get_lock(1);
uSemaphore put_lock(1);
// IO
int Getline(int channel, char *line, int line_length) {
	if (channel==TRAIN_PORT)return 0;
	
    //get_lock.P();
	task_lock.V();
	cin >> noskipws;
	for (int i=0; i<line_length; i++) {
		cin >> line[i];
	}
	task_lock.P();
    //get_lock.V();
}
int Putline(int channel, char *ch, int line_length) {
	if (channel==TRAIN_PORT) {
	    fstream out;
	    out.open("train_out", fstream::out);
	    out.write(ch, line_length);
        out.close();
        
        return 0;
	}
	
	//put_lock.P();
	for (int i=0; i<line_length; i++) {
		cout << ch[i];
	}
    cout.flush();
    //put_lock.V();
}

int getLine(char *buff) {
	RealTimeTask *task = ACTIVE_TASK;
	task_lock.V();
	char next_char;
	int i = 0;
	cin >> noskipws;
	while (1) {
		next_char = getchar();
		if (next_char == -1)continue;
		//printf("\n\nchar: %d == %c\n", next_char, next_char);
		if (next_char == '\r' || next_char == '\n')break;
		if (i == TRAIN_INPUT_SIZE)break;
		buff[i] = next_char;
		i++;
	}
	buff[i] = 0;
	task_lock.P();
	ACTIVE_TASK = task;
	return i;
}


int DelayUntil( int ticks ){}

