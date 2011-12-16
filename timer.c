#include <ts7200.h>
#include <timer.h>

void init_timer() {
        unsigned int *line, *value, buf;
        line = TIMER1_BASE + CRTL_OFFSET;
        value = TIMER1_BASE + LDR_OFFSET;
        buf = *line;
        //Enable clock
        //Periodic running
        //2kHz
        buf = buf & ~ENABLE_MASK | MODE_MASK & ~CLKSEL_MASK;
        *line = buf;
        *value = 20;
        
        buf = buf | ENABLE_MASK;
        *line = buf;
        
        line = TIMER2_BASE + CRTL_OFFSET;
        value = TIMER2_BASE + LDR_OFFSET;
        buf = *line;
        buf = buf & ~ENABLE_MASK | MODE_MASK & ~CLKSEL_MASK;
        *line = buf;
        *value = 40;
        
        buf = buf | ENABLE_MASK;
        *line = buf;
        
        line = TIMER3_BASE + CRTL_OFFSET;
        value = TIMER3_BASE + LDR_OFFSET;
        buf = *line;
        buf = buf & ~ENABLE_MASK | MODE_MASK & ~CLKSEL_MASK;
        *line = buf;
        *value = 40;
        
        buf = buf | ENABLE_MASK;
        *line = buf;
}

unsigned int count_timer() {
        unsigned int *line, buf;
        line = TIMER1_BASE + VAL_OFFSET;
        buf = *line;
        
        //Clear bits
        buf = buf << 16 >> 16;
        return buf;
}

void reset_timer(int timer_id) {
        int *line;
        switch (timer_id) {
                case TIMER1:
                        line = TIMER1_BASE + CLR_OFFSET;
                        break;
                case TIMER2:
                        line = TIMER2_BASE + CLR_OFFSET;
                        break;
                case TIMER3:
                        line = TIMER3_BASE + CLR_OFFSET;
                        break;
        }
        
        *line = 0;
}