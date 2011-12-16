#ifdef CLOCK_SERVER_H
#else
#define CLOCK_SERVER_H
	
#define NOTIFIER 0
#define TIME_REQUEST 1
#define DELAY 2
#define DELAY_UNTIL 3
#define TIME_UPDATED "T_UP"
#define DELAY_OVER "DEL_UP"

void initialize_clock_server();
	
void clock_server();
int Delay( int ticks );
int Time();
int DelayUntil( int ticks );
#endif
