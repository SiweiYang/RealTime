#include <TaskDescriptor.h>

#ifdef INTERRUPT_H
#else
#define INTERRUPT_H

#define TEST_INTERRUPT_TYPE                     0
#define TIMER1_INTERRUPT_TYPE                   4
#define TIMER2_INTERRUPT_TYPE                   5
#define TIMER3_INTERRUPT_TYPE                   51
#define UART1_RECEIVE_INTERRUPT_TYPE      	23
#define UART1_TRANSMIT_INTERRUPT_TYPE      	24
#define UART1_INTERRUPT_TYPE      	        52
#define UART2_RECEIVE_INTERRUPT_TYPE      	25
#define UART2_TRANSMIT_INTERRUPT_TYPE      	26
#define UART2_INTERRUPT_TYPE      	        54
    #define UART_PUT                            0
    #define UART_GET                            1

#define TOTAL_INTERRUPT_TYPE        64
#define LOW_INT                     0
#define HIGH_INT                    1

#define	VIC1_BASE	            0x800B0000
#define	VIC2_BASE	            0x800C0000

#define IRQ_STATUS_OFFSET           0x0
#define RAW_INT_OFFSET              0x8
#define INT_SELECT_OFFSET           0xC
#define INT_ENABLE_OFFSET           0x10
    #define ENABLE_MASK             0xF97865FB
#define INT_ENABLE_CLEAR_OFFSET     0x14
    #define CLEAR_MASK              0xF97865FB
#define SOFT_INT_OFFSET             0x18
#define SOFT_INT_CLEAR_OFFSET       0x1C
#define PROTECTION_OFFSET           0x20
    #define PROTECTION_MASK         0x1

void enable_user_interrupt();

void enable_interrupt_type(int type);
void disable_interrupt_type(int type);
void simulate_interrupt_type(int type);
void unsimulate_interrupt_type(int type);

void handle_interrupt_type(int type, TaskDescriptor *task);

int get_interrupt_type();
void handle_interrupt();
#endif
