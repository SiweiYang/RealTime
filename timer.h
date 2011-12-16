#ifdef TIMER_H
#else
#define TIMER_H

#define TIMER1          4
#define TIMER2          5
#define TIMER3          51

void init_timer();

unsigned int count_timer();

void reset_timer(int timer_id);
#endif
