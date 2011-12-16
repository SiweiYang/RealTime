#include <ts7200.h>
#include <bwio.h>
#include <timer.h>
#include <Kernel.h>
#include <Scheduler.h>
#include <ContextSwitch.h>
#include <Interrupt.h>

#include <IO.h>
extern TaskDescriptor *TASK;
TaskDescriptor volatile * volatile WAIT_INTERRUPT[TOTAL_INTERRUPT_TYPE];

int *vic1, *vic2;

void enable_user_interrupt() {
	int *base, *protection;
	base = VIC1_BASE;
	protection = base + PROTECTION_OFFSET;
	*protection = *protection | PROTECTION_MASK;

	base = VIC2_BASE;
	protection = base + PROTECTION_OFFSET;
	*protection = *protection | PROTECTION_MASK;

	vic1 = VIC1_BASE + INT_ENABLE_OFFSET;
	vic2 = VIC2_BASE + INT_ENABLE_OFFSET;
}

void enable_interrupt_type(int type) {
	type = type % TOTAL_INTERRUPT_TYPE;

	int *enable;
	int mask;
	if (type < 32) {
		enable = VIC1_BASE + INT_ENABLE_OFFSET;
		mask = 0x1 << type;
	} else {
		enable = VIC2_BASE + INT_ENABLE_OFFSET;
		type -= 32;
		mask = 0x1 << type;
	}
	*enable = *enable | mask;

	//bwprintf(TERM_PORT, "Enable Interrupt for type: %d with %x with mask %x at %x\n", type, *enable, mask, enable);
}

void disable_interrupt_type(int type) {
	type = type % TOTAL_INTERRUPT_TYPE;
	//WAIT_INTERRUPT[type] = NULL;

	int *enable;
	int mask;
	if (type < 32) {
		enable = VIC1_BASE + INT_ENABLE_CLEAR_OFFSET;;
		mask = 0x1 << type;
	} else {
		enable = VIC2_BASE + INT_ENABLE_CLEAR_OFFSET;;
		type -= 32;
		mask = 0x1 << type;
	}
	*enable = *enable | mask;
}

int ready_interrupt_type(int type) {
	type = type % TOTAL_INTERRUPT_TYPE;
	return WAIT_INTERRUPT[type] && WAIT_INTERRUPT[type]->state == AWAIT_EVENT_BLOCKED;
}

int enabled_interrupt_type(int type) {
	type = type % TOTAL_INTERRUPT_TYPE;
	if (type < 32) {
		return (*vic1 >> type) % 2;
	} else {
		type -= 32;
		return (*vic2 >> type) % 2;
	}
}

void simulate_interrupt_type(int type) {
	type = type % TOTAL_INTERRUPT_TYPE;

	int *enable;
	int mask;
	if (type < 32) {
		enable = VIC1_BASE + SOFT_INT_OFFSET;
		mask = 0x1 << type;
	} else {
		enable = VIC2_BASE + SOFT_INT_OFFSET;
		type -= 32;
		mask = 0x1 << type;
	}
	*enable = *enable | mask;

	bwprintf(TERM_PORT, "Faking Interrupt for type: %d with %x with mask %x at %x\n", type, *enable, mask, enable);
}

void unsimulate_interrupt_type(int type) {
	type = type % TOTAL_INTERRUPT_TYPE;

	int *enable;
	int mask;
	if (type < 32) {
		enable = VIC1_BASE + SOFT_INT_CLEAR_OFFSET;
		mask = 0x1 << type;
	} else {
		enable = VIC2_BASE + SOFT_INT_CLEAR_OFFSET;
		type -= 32;
		mask = 0x1 << type;
	}
	*enable = *enable | mask;
}

void handle_interrupt_type(int event, TaskDescriptor *task) {
	enable_interrupt_type(event);
	if(!WAIT_INTERRUPT[event])WAIT_INTERRUPT[event] = task;
	//WAIT_INTERRUPT[event] = task;
	//bwprintf(TERM_PORT, "Register Task %d for Listening Interrupt %d\n", task->id, event);
}

int get_interrupt_type() {
	int *signal, interrupt_type;

	signal = VIC2_BASE + IRQ_STATUS_OFFSET;
	if (*signal) {
		//bwprintf(TERM_PORT, "Checking Interrupt Controller 2\n");
		interrupt_type = 0;
		while (*signal>>interrupt_type++);

		return 32 + interrupt_type - 2;
	}

	signal = VIC1_BASE + IRQ_STATUS_OFFSET;
	if (*signal) {
		//bwprintf(TERM_PORT, "Checking Interrupt Controller 1\n");
		interrupt_type = 0;
		while (*signal>>interrupt_type++);

		return interrupt_type - 2;
	}

	return -1;
}

void handle_interrupt() {
	update_system_state('I');
	int interrupt_type = get_interrupt_type();
	//enable_interrupt_type(23);

	//bwprintf(TERM_PORT, "Getting Interrupt type: %d\n", interrupt_type);
/*
	bwprintf(TERM_PORT, "Getting Interrupt type: %d\n", interrupt_type);
	bwprintf(TERM_PORT, "ENABLE -> IRQ1: %x, IRQ2: %x\n", *vic1, *vic2);
*/


	int * volatile flags, * volatile data;
	switch (interrupt_type) {
		case TEST_INTERRUPT_TYPE:
			bwprintf(TERM_PORT, "Handling Test Interrupt\n");
			WAIT_INTERRUPT[interrupt_type]->state == READY;
			add_to_schedule( WAIT_INTERRUPT[interrupt_type] );
			*(WAIT_INTERRUPT[interrupt_type]->stack_pointer+RETURN_ARGUMENT_OFFSET) = 1024;

			unsimulate_interrupt_type(interrupt_type);
			bwprintf(TERM_PORT, "Clear Test Interrupt signal\n");

			break;

		case TIMER1_INTERRUPT_TYPE:
			//Only do if the interrupt is listened
			if (ready_interrupt_type(interrupt_type)) {
				//bwprintf(TERM_PORT, "Rescheduling Notifier Task: %d for Timer Interrupt\n", WAIT_INTERRUPT[interrupt_type]->id);
				WAIT_INTERRUPT[interrupt_type]->state == READY;

				add_to_schedule( WAIT_INTERRUPT[interrupt_type] );
				disable_interrupt_type(interrupt_type);
			}
			//Clear the timer signal here
			reset_timer(interrupt_type);
			break;

		case TIMER2_INTERRUPT_TYPE:
			//Only do if the interrupt is listened
			if (ready_interrupt_type(interrupt_type)) {
				WAIT_INTERRUPT[interrupt_type]->state == READY;

				add_to_schedule( WAIT_INTERRUPT[interrupt_type] );
				disable_interrupt_type(interrupt_type);
			}

			//Clear the timer signal here
			reset_timer(interrupt_type);
			break;

		case TIMER3_INTERRUPT_TYPE:
			//Only do if the interrupt is listened
			if (ready_interrupt_type(interrupt_type)) {
				WAIT_INTERRUPT[interrupt_type]->state == READY;

				add_to_schedule( WAIT_INTERRUPT[interrupt_type] );
				disable_interrupt_type(interrupt_type);
			}

			//Clear the timer signal here
			reset_timer(interrupt_type);
			break;

		case UART1_RECEIVE_INTERRUPT_TYPE:
			flags = (int *)( UART1_BASE + UART_FLAG_OFFSET );
			data = (int *)( UART1_BASE + UART_DATA_OFFSET );
			//bwprintf(TERM_PORT, "Receiving uart1 char\n");
			//Only do if the interrupt is listened
			if (ready_interrupt_type(interrupt_type)) {
				WAIT_INTERRUPT[interrupt_type]->state == READY;
				*(WAIT_INTERRUPT[interrupt_type]->stack_pointer+RETURN_ARGUMENT_OFFSET) = *data;

				add_to_schedule( WAIT_INTERRUPT[interrupt_type] );
				disable_interrupt_type(interrupt_type);
			}
			//Clear the signal here
			*flags = *flags | TXFE_MASK;
			break;

		case UART1_TRANSMIT_INTERRUPT_TYPE:
			flags = (int *)( UART1_BASE + UART_FLAG_OFFSET );
			data = (int *)( UART1_BASE + UART_DATA_OFFSET );
			
			//bwprintf(TERM_PORT, "Receiving uart1 transmit with flag %x\n", *flags);
			//Only do if the interrupt is listened
			if (ready_interrupt_type(interrupt_type)) {
				if ((!(*flags & TXFF_MASK)) && (*flags & CTS_MASK)) {
					int *ch = WAIT_INTERRUPT[interrupt_type]->stack_pointer+RETURN_ARGUMENT_OFFSET;
					WAIT_INTERRUPT[interrupt_type]->state == READY;
					*data = *ch >> 24;
					*(WAIT_INTERRUPT[interrupt_type]->stack_pointer+RETURN_ARGUMENT_OFFSET) = 0;
				}
				add_to_schedule( WAIT_INTERRUPT[interrupt_type] );
				disable_interrupt_type(interrupt_type);
			}
			
			break;

		case UART1_INTERRUPT_TYPE:
			
			// CTS
			//bwprintf(TERM_PORT, "Receiving uart1 modem with flag %x\n", *flags & CTS_MASK);
			if (ready_interrupt_type(UART1_RECEIVE_INTERRUPT_TYPE) && (*flags & TXFF_MASK)) {
				flags = (int *)( UART1_BASE + UART_FLAG_OFFSET );
				data = (int *)( UART1_BASE + UART_DATA_OFFSET );
				
				WAIT_INTERRUPT[UART1_RECEIVE_INTERRUPT_TYPE]->state == READY;
				*(WAIT_INTERRUPT[UART1_RECEIVE_INTERRUPT_TYPE]->stack_pointer+RETURN_ARGUMENT_OFFSET) = *data;

				add_to_schedule( WAIT_INTERRUPT[UART1_RECEIVE_INTERRUPT_TYPE] );
				disable_interrupt_type(UART1_RECEIVE_INTERRUPT_TYPE);
				*flags = *flags | TXFE_MASK;
				
			} else if (ready_interrupt_type(UART1_TRANSMIT_INTERRUPT_TYPE) && (!(*flags & TXFF_MASK)) && (*flags & CTS_MASK)) {
				flags = (int *)( UART1_BASE + UART_FLAG_OFFSET );
				data = (int *)( UART1_BASE + UART_DATA_OFFSET );
				
				int *ch = WAIT_INTERRUPT[UART1_TRANSMIT_INTERRUPT_TYPE]->stack_pointer+RETURN_ARGUMENT_OFFSET;
				WAIT_INTERRUPT[UART1_TRANSMIT_INTERRUPT_TYPE]->state == READY;
				*data = *ch >> 24;
				*(WAIT_INTERRUPT[UART1_TRANSMIT_INTERRUPT_TYPE]->stack_pointer+RETURN_ARGUMENT_OFFSET) = 0;

				add_to_schedule( WAIT_INTERRUPT[UART1_TRANSMIT_INTERRUPT_TYPE] );
				disable_interrupt_type(UART1_TRANSMIT_INTERRUPT_TYPE);
			} else if (ready_interrupt_type(interrupt_type)) {
				flags = (int *)( UART1_BASE + UART_FLAG_OFFSET );
				data = (int *)( UART1_BASE + UART_INTR_OFFSET );
				if (ready_interrupt_type(UART1_TRANSMIT_INTERRUPT_TYPE)&&(*flags & CTS_MASK))enable_interrupt_type(UART1_TRANSMIT_INTERRUPT_TYPE);
				
				//bwprintf(TERM_PORT, "Receiving uart1 modem\n");
				*(WAIT_INTERRUPT[interrupt_type]->stack_pointer+RETURN_ARGUMENT_OFFSET) = *flags & CTS_MASK;
				if (*data) {
					*data = 0;
				} else {
					*data = 0xF;
				}
				
				WAIT_INTERRUPT[interrupt_type]->state == READY;
				add_to_schedule( WAIT_INTERRUPT[interrupt_type] );
			}
			//Clear the signal here
			//*flags = *flags | TXFE_MASK;
			break;

		case UART2_RECEIVE_INTERRUPT_TYPE:
			flags = (int *)( UART2_BASE + UART_FLAG_OFFSET );
			data = (int *)( UART2_BASE + UART_DATA_OFFSET );
			//Only do if the interrupt is listened
			if (ready_interrupt_type(interrupt_type) && ( *flags & RXFF_MASK )) {
				WAIT_INTERRUPT[interrupt_type]->state == READY;
				*(WAIT_INTERRUPT[interrupt_type]->stack_pointer+RETURN_ARGUMENT_OFFSET) = *data;

				add_to_schedule( WAIT_INTERRUPT[interrupt_type] );

			}
			disable_interrupt_type(interrupt_type);
			//Clear the timer signal here
			*flags = *flags | TXFE_MASK;
			break;

		case UART2_TRANSMIT_INTERRUPT_TYPE:
			flags = (int *)( UART2_BASE + UART_FLAG_OFFSET );
			data = (int *)( UART2_BASE + UART_DATA_OFFSET );
			//bwprintf(TERM_PORT, "put to terminal\n");
			//Only do if the interrupt is listened
			if (ready_interrupt_type(interrupt_type) && (!(*flags & TXFF_MASK))) {
				WAIT_INTERRUPT[interrupt_type]->state == READY;
				int *ch = WAIT_INTERRUPT[interrupt_type]->stack_pointer+RETURN_ARGUMENT_OFFSET;
				*data = *ch >> 24;
				// return code 0
				//bwprintf(TERM_PORT, "interrupt put from %d to terminal: %c...\n", WAIT_INTERRUPT[interrupt_type]->id, *ch >> 24);
				*(WAIT_INTERRUPT[interrupt_type]->stack_pointer+RETURN_ARGUMENT_OFFSET) = 0;
				add_to_schedule( WAIT_INTERRUPT[interrupt_type] );
			}
			disable_interrupt_type(interrupt_type);
			break;

		default:
			break;
	}

	return;
}
