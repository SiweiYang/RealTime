#include <bwio.h>
#include <ts7200.h>
#include <Kernel.h>
#include <ContextSwitch.h>
#include <TaskDescriptor.h>
#include <NameServer.h>
#include <Notifier.h>
#include <Interrupt.h>
#include <IO.h>

int UART1_IO_TID = -1;
int UART2_IO_TID = -1;
char *TASK_GET_SIZE[TASK_POOL_SIZE];

TaskDescriptor *UART1_QUEUE_HEAD;
TaskDescriptor *UART1_QUEUE_TAIL;

TaskDescriptor *UART2_QUEUE_HEAD;
TaskDescriptor *UART2_QUEUE_TAIL;

char UART1_GET_BUFFER[IO_BUFFER_LENGTH];
char UART1_PUT_BUFFER[IO_BUFFER_LENGTH];
int UART1_GET_BUFFER_HEAD;
int UART1_GET_BUFFER_TAIL;
int UART1_PUT_BUFFER_HEAD;
int UART1_PUT_BUFFER_TAIL;

char UART2_GET_BUFFER[IO_BUFFER_LENGTH];
char UART2_PUT_BUFFER[IO_BUFFER_LENGTH]; 
int UART2_GET_BUFFER_HEAD;
int UART2_GET_BUFFER_TAIL;
int UART2_PUT_BUFFER_HEAD;
int UART2_PUT_BUFFER_TAIL;

int * volatile flags = (int *)( UART1_BASE + UART_FLAG_OFFSET );
int volatile UART1_CTS;

void initialize_io() {
	int *high, *low;
	high = (int *)( UART1_BASE + UART_LCRM_OFFSET );
	low = (int *)( UART1_BASE + UART_LCRL_OFFSET );
	*high = 0x0;
	*low = 0xbf;
	
	int * line,buf;
	
	line = (int *)( UART1_BASE + UART_LCRH_OFFSET );
	buf = *line;
	//Two stop bits for Train
	buf = buf | WLEN_MASK | STP2_MASK & ~PEN_MASK;
	*line = buf;
	
	//Request to Send data
	line = (int *)( UART1_BASE + UART_MDMCTL_OFFSET );

	buf = *line;
	buf = buf | LOOP_MASK | RTS_MASK;
	*line = buf;
	
	//enable interrupt
	line = (int *)( UART1_BASE + UART_CTLR_OFFSET );

	buf = *line;
	buf = buf & ~LBEN_MASK | MSIEN_MASK | RIEN_MASK | TIEN_MASK;
	//buf = buf & ~LBEN_MASK | MSIEN_MASK;
	*line = buf;
	
	line = (int *)( UART2_BASE + UART_CTLR_OFFSET );

	buf = *line;
	buf = buf & ~LBEN_MASK | RIEN_MASK | TIEN_MASK;
	*line = buf;
	
	disable_interrupt_type(UART1_RECEIVE_INTERRUPT_TYPE);
	disable_interrupt_type(UART1_TRANSMIT_INTERRUPT_TYPE);
	disable_interrupt_type(UART1_INTERRUPT_TYPE);
	disable_interrupt_type(UART2_RECEIVE_INTERRUPT_TYPE);
	disable_interrupt_type(UART2_TRANSMIT_INTERRUPT_TYPE);
	
	UART1_QUEUE_HEAD = NULL;
	UART1_QUEUE_TAIL = NULL;
	UART2_QUEUE_HEAD = NULL;
	UART2_QUEUE_TAIL = NULL;

	UART1_GET_BUFFER_HEAD = 0;
	UART1_GET_BUFFER_TAIL = 0;

	UART1_PUT_BUFFER_HEAD = 0;
	UART1_PUT_BUFFER_TAIL = 0;

	UART2_GET_BUFFER_HEAD = 0;
	UART2_GET_BUFFER_TAIL = 0;

	UART2_PUT_BUFFER_HEAD = 0;
	UART2_PUT_BUFFER_TAIL = 0;
	
	UART1_CTS = 0;
}

void push_to_buffer( char * char_array, int * head, int * tail, char to_push ) {	
	char_array[*tail] = to_push;
	*tail = (*tail + 1)%IO_BUFFER_LENGTH;
}

char pop_from_buffer( char * char_array, int * head, int * tail ) {
	if ( *head == *tail ) {
		return NULL;
	}
	else {
		char to_return = char_array[*head];
		*head = (*head + 1)%IO_BUFFER_LENGTH;
		//bwprintf(TRAIN_PORT, "pop %c from buffer\n", to_return);
		return to_return;
	}
}

int buffer_size( int * head, int * tail ) {
	return (IO_BUFFER_LENGTH + *tail - *head) % IO_BUFFER_LENGTH;
}

int buffer_empty( int * head, int * tail ) {
	if ( *head == *tail ) {
		//bwprintf(TRAIN_PORT, "buffer empty\n");
		return 1;
	}
	else {
		//bwprintf(TERM_PORT, "buffer not empty\n");
		return 0;
	}
}

void clear_buffer( int * head, int * tail ) {
	*head = *tail;
}

void cts_server() {
	while (RegisterAs("CTS")==-1) {
		bwprintf(TERM_PORT, "register to name_server failed\n");
	}
	
	int cts_notifier_tid = Create( 7, "UART1_CTS", &notifier_io );
	int received_tid = -1;
	int interrupt_type = UART1_INTERRUPT_TYPE;
	int rcv_len, i, len = 16;
	char msg[IO_BUFFER_LENGTH];
	int *op;
	char *real_msg;
	op = (int *)msg;
	real_msg = msg + 4;
	char reply[len];
	
	Send( cts_notifier_tid, (char *)&interrupt_type, 4, reply, len );
	while (1) {
		//bwprintf(TERM_PORT, "Waiting for request...\n");
		rcv_len = Receive( &received_tid, msg, IO_BUFFER_LENGTH );
		//bwprintf(TERM_PORT, "Task %d return with op code %d...\n", received_tid, *op);
		switch ( (*op) ) {
			case NOTIFIER_START:
				*op = NOTIFIER_GET;
				Reply( received_tid, msg, 4 );
				// drop all char not received
				break;
			case NOTIFIER_RETURN:
				bwprintf(TERM_PORT, "hold CTS\n");
				UART1_CTS = *(int *)real_msg;
				while (UART1_CTS==(*flags&CTS_MASK))*(int *)real_msg = (*flags&CTS_MASK);
				UART1_CTS=(*flags&CTS_MASK);
				bwprintf(TERM_PORT, "updated CTS\n");
				*op = NOTIFIER_GET;
				Reply( received_tid, msg, 4 );
				break;
		}
	}
}

int uart1_putable(int transmit_notifier_tid) {
	return (*get_task_by_id( transmit_notifier_tid )).state == REPLY_BLOCKED && !buffer_empty( &UART1_PUT_BUFFER_HEAD, &UART1_PUT_BUFFER_TAIL ) && *flags&CTS_MASK;
}

void uart1_io() {
	while (RegisterAs("UART1")==-1) {
		bwprintf(TERM_PORT, "register to name_server failed\n");
	}
	int * volatile flags = (int *)( UART1_BASE + UART_FLAG_OFFSET );
	
	int receive_notifier_tid = Create( 7, "UART1_RCV", &notifier_io );
	int transmit_notifier_tid = Create( 7, "UART1_TRAN", &notifier_io );
	
    int received_tid = -1;
	int interrupt_type1 = UART1_RECEIVE_INTERRUPT_TYPE;
	int interrupt_type2 = UART1_TRANSMIT_INTERRUPT_TYPE;
	int rcv_len, i, len = 16;
	char msg[IO_BUFFER_LENGTH];
	int *op;
	char *real_msg;
	op = (int *)msg;
	real_msg = msg + 4;
	char reply[len];
	
	Send( receive_notifier_tid, (char *)&interrupt_type1, 4, reply, len );
	Send( transmit_notifier_tid, (char *)&interrupt_type2, 4, reply, len );
	while (1) {
		//bwprintf(TERM_PORT, "Waiting for request...\n");
		rcv_len = Receive( &received_tid, msg, IO_BUFFER_LENGTH );
		//bwprintf(TERM_PORT, "Task %d return with op code %d...\n", received_tid, *op);
		switch ( (*op) ) {
			case NOTIFIER_START:
				if ( received_tid == transmit_notifier_tid ) {
					if ( !buffer_empty( &UART1_PUT_BUFFER_HEAD, &UART1_PUT_BUFFER_TAIL ) ) {
						*op = NOTIFIER_PUT;
						*real_msg = pop_from_buffer( UART1_PUT_BUFFER, &UART1_PUT_BUFFER_HEAD, &UART1_PUT_BUFFER_TAIL );
						Reply( received_tid, msg, 5 );
					}
				} else {
					*op = NOTIFIER_GET;
 					Reply( received_tid, msg, 4 );
				}
				// drop all char not received
				break;
			
			case NOTIFIER_RETURN:
				if ( received_tid == receive_notifier_tid ) {
					*op = NOTIFIER_GET;
 					Reply( receive_notifier_tid, msg, 4 );					
					// if timeout
					if (real_msg[2]|real_msg[3]) {
						bwprintf(TERM_PORT, "A timeout???!!!\n");
					}
					// success
					else {
						//TASK_GET_SIZE[getIndex((*UART1_QUEUE_HEAD).id)] = 1;
						push_to_buffer( UART1_GET_BUFFER, &UART1_GET_BUFFER_HEAD, &UART1_GET_BUFFER_TAIL, *real_msg );
						// the listener ready to serve
						if ( UART1_QUEUE_HEAD != (TaskDescriptor *)NULL && !(TASK_GET_SIZE[getIndex((*UART1_QUEUE_HEAD).id)] > buffer_size(&UART1_GET_BUFFER_HEAD, &UART1_GET_BUFFER_TAIL))) {
							TaskDescriptor *task = UART1_QUEUE_HEAD;
							UART1_QUEUE_HEAD = (*UART1_QUEUE_HEAD).next_in_queue;
							i = 0;
							while (i<TASK_GET_SIZE[getIndex((*task).id)]) {
								reply[i] = pop_from_buffer( UART1_GET_BUFFER, &UART1_GET_BUFFER_HEAD, &UART1_GET_BUFFER_TAIL );
								i++;
							}
							TASK_GET_SIZE[getIndex((*task).id)] = -1;
							Reply( (*task).id, reply, i );
						}
					}
				}
				else if ( received_tid == transmit_notifier_tid ) {
					//bwprintf(TERM_PORT, "Transmitter???!!!\n");
					if ( uart1_putable(transmit_notifier_tid) ) {
						*op = NOTIFIER_PUT;
						*real_msg = pop_from_buffer( UART1_PUT_BUFFER, &UART1_PUT_BUFFER_HEAD, &UART1_PUT_BUFFER_TAIL );
						Reply( received_tid, msg, 5 );
					}
				}
				else {
					// ERROR
				}
				break;
			case IO_CTS:
				//if ((*get_task_by_id( transmit_notifier_tid )).state != REPLY_BLOCKED) {
				//	bwprintf(TERM_PORT, "where is tramsmitter...\n");
				//}
				//if (!buffer_empty( &UART1_PUT_BUFFER_HEAD, &UART1_PUT_BUFFER_TAIL )) {
				//	bwprintf(TERM_PORT, "no buffer...\n");
				//}
				if ( uart1_putable(transmit_notifier_tid) ) {
					*op = NOTIFIER_PUT;
					*real_msg = pop_from_buffer( UART1_PUT_BUFFER, &UART1_PUT_BUFFER_HEAD, &UART1_PUT_BUFFER_TAIL );
					Reply( transmit_notifier_tid, msg, 5 );
				}
				Reply( received_tid, reply, 1 );
				break;
			case IO_GET:
				if ( buffer_empty( &UART1_GET_BUFFER_HEAD, &UART1_GET_BUFFER_TAIL ) ) {
					TASK_GET_SIZE[getIndex(received_tid)] = 1;
					push_to_queue( &UART1_QUEUE_HEAD, &UART1_QUEUE_TAIL, get_task_by_id( received_tid ) );
				}
				else {
					//bwprintf(TERM_PORT, "getc from buffer\n");
					*reply = pop_from_buffer( UART1_GET_BUFFER, &UART1_GET_BUFFER_HEAD, &UART1_GET_BUFFER_TAIL );
					Reply( received_tid, reply, 1 );
				}
				break;
			case IO_GETMULT:
				TASK_GET_SIZE[getIndex(received_tid)] = *(op+1);
				if (TASK_GET_SIZE[getIndex(received_tid)]>buffer_size(&UART1_GET_BUFFER_HEAD, &UART1_GET_BUFFER_TAIL)) {
					push_to_queue( &UART1_QUEUE_HEAD, &UART1_QUEUE_TAIL, get_task_by_id( received_tid ) );
				} else {
					i = 0;
					while (i<TASK_GET_SIZE[getIndex(received_tid)]) {
						reply[i] = pop_from_buffer( UART1_GET_BUFFER, &UART1_GET_BUFFER_HEAD, &UART1_GET_BUFFER_TAIL );
						i++;
					}
					TASK_GET_SIZE[getIndex(received_tid)] = -1;
					Reply( received_tid, reply, i );
				}
				break;
			case IO_PUT:
				if ( *real_msg == 133 ) {
					clear_buffer( &UART1_GET_BUFFER_HEAD, &UART1_GET_BUFFER_TAIL );
				}
				push_to_buffer( UART1_PUT_BUFFER, &UART1_PUT_BUFFER_HEAD, &UART1_PUT_BUFFER_TAIL, *real_msg );
				if ( uart1_putable(transmit_notifier_tid) ) {
					*op = NOTIFIER_PUT;
					*real_msg = pop_from_buffer( UART1_PUT_BUFFER, &UART1_PUT_BUFFER_HEAD, &UART1_PUT_BUFFER_TAIL );
					Reply( transmit_notifier_tid, msg, 5 );
				}
				
				Reply( received_tid, NULL, 0 );
				break;
			case IO_PUTLINE:
				if ( rcv_len == 6 && msg[5] == 133 ) {
					clear_buffer( &UART1_GET_BUFFER_HEAD, &UART1_GET_BUFFER_TAIL );
				}
				i = 4;
				while(i<rcv_len) {
					push_to_buffer( UART1_PUT_BUFFER, &UART1_PUT_BUFFER_HEAD, &UART1_PUT_BUFFER_TAIL, msg[i] );
					i++;
				}
				if ( uart1_putable(transmit_notifier_tid) ) {
					*op = NOTIFIER_PUT;
					*real_msg = pop_from_buffer( UART1_PUT_BUFFER, &UART1_PUT_BUFFER_HEAD, &UART1_PUT_BUFFER_TAIL );
					Reply( transmit_notifier_tid, msg, 5 );
				}
				
				Reply( received_tid, &i, 4 );
				break;
			default:
				// ERROR
				break;
		}
		//bwprintf(TERM_PORT, "finished\n");
	}
}

void uart2_io() {
	while (RegisterAs("UART2")==-1) {
		bwprintf(TERM_PORT, "register to name_server failed\n");
	}
	
	int receive_notifier_tid = Create( 7, "UART2_RCV", &notifier_io );
	int transmit_notifier_tid = Create( 7, "UART2_TRAN", &notifier_io );
	int received_tid = -1;
	int interrupt_type1 = UART2_RECEIVE_INTERRUPT_TYPE;
	int interrupt_type2 = UART2_TRANSMIT_INTERRUPT_TYPE;
	int rcv_len, i, len = 8;
	char msg[IO_BUFFER_LENGTH];
	int *op;
	char *real_msg;
	op = msg;
	real_msg = msg + 4;
	char reply[len];

	Send( receive_notifier_tid, &interrupt_type1, 4, reply, len );
	Send( transmit_notifier_tid, &interrupt_type2, 4, reply, len );
	while (1) {
		rcv_len = Receive( &received_tid, msg, IO_BUFFER_LENGTH );
		//bwprintf(TERM_PORT, "Got new request\n");
		switch ( (*op) ) {
			case NOTIFIER_START:
				if ( received_tid == receive_notifier_tid ) {
					*op = NOTIFIER_GET;
 					Reply( received_tid, msg, 4 );
				}
				// drop all char not received
				break;
			
			case NOTIFIER_RETURN:
				if ( received_tid == receive_notifier_tid ) {
					*op = NOTIFIER_GET;
 					Reply( received_tid, msg, 4 );
			 		if ( UART2_QUEUE_HEAD != NULL ) {
						while ( UART2_QUEUE_HEAD ) {
							TaskDescriptor *task = UART2_QUEUE_HEAD;
							UART2_QUEUE_HEAD = (*UART2_QUEUE_HEAD).next_in_queue;
							
							Reply( (*task).id, real_msg, 1 );
						}
					}
					else {
						// store real char
						push_to_buffer( UART2_GET_BUFFER, &UART2_GET_BUFFER_HEAD, &UART2_GET_BUFFER_TAIL, *real_msg );
					}
				}
				else if ( received_tid == transmit_notifier_tid ) {
					//bwprintf(TRAIN_PORT, "transmiter notifier comes back\n");
					if ( !buffer_empty( &UART2_PUT_BUFFER_HEAD, &UART2_PUT_BUFFER_TAIL ) ) {
						*op = NOTIFIER_PUT;
						*real_msg = pop_from_buffer( UART2_PUT_BUFFER, &UART2_PUT_BUFFER_HEAD, &UART2_PUT_BUFFER_TAIL );
						
						Reply( received_tid, msg, 5 );
					}
				}
				else {
					// ERROR
				}
				break;
			case IO_GET:
				if ( buffer_empty( &UART2_GET_BUFFER_HEAD, &UART2_GET_BUFFER_TAIL ) ) {
					push_to_queue( &UART2_QUEUE_HEAD, &UART2_QUEUE_TAIL, get_task_by_id( received_tid ) );
				}
				else {
					*reply = pop_from_buffer( UART2_GET_BUFFER, &UART2_GET_BUFFER_HEAD, &UART2_GET_BUFFER_TAIL );
					Reply( received_tid, reply, 1 );
				}
				break;
			case IO_PUT:
				if ( (*get_task_by_id( transmit_notifier_tid )).state == REPLY_BLOCKED ) {
					*op = NOTIFIER_PUT;
					//bwprintf(TERM_PORT, "uart2 put to terminal: %c\n", msg[4]);
					//*real_msg = reply[4];
					Reply( transmit_notifier_tid, msg, 5 );
				}
				else {
					push_to_buffer( UART2_PUT_BUFFER, &UART2_PUT_BUFFER_HEAD, &UART2_PUT_BUFFER_TAIL, *real_msg );
				}
				Reply( received_tid, NULL, 0 );
				break;
			case IO_PUTLINE:
				i = 4;
				//bwprintf(TERM_PORT, "putline: %d...\n", rcv_len);
				while(i<rcv_len) {
					push_to_buffer( UART2_PUT_BUFFER, &UART2_PUT_BUFFER_HEAD, &UART2_PUT_BUFFER_TAIL, msg[i] );
					i++;
				}
				if ( (*get_task_by_id( transmit_notifier_tid )).state == REPLY_BLOCKED ) {
					*op = NOTIFIER_PUT;
					*real_msg = pop_from_buffer( UART2_PUT_BUFFER, &UART2_PUT_BUFFER_HEAD, &UART2_PUT_BUFFER_TAIL );
					Reply( transmit_notifier_tid, msg, 5 );
				}
				
				Reply( received_tid, &i, 4 );
				break;
			default:
				// ERROR
				break;
		}
	}
}

int Getc( int channel ) {
	int len = 4;
	char msg[len];
	int *op1;
	op1 = msg;
	(*op1) = IO_GET;
	char reply[IO_REPLY_LENGTH];
	if ( channel == TRAIN_PORT ) {
		if (UART1_IO_TID==-1) {
			UART1_IO_TID = WhoIs("UART1");
		}
		Send( UART1_IO_TID, msg, len, reply, IO_REPLY_LENGTH );
	}
	if ( channel == TERM_PORT ) {
		if (UART2_IO_TID==-1) {
			UART2_IO_TID = WhoIs("UART2");
		}
		Send( UART2_IO_TID, msg, len, reply, IO_REPLY_LENGTH );
	}
	return *reply;
}

int Getline( int channel, char *line, int length ) {
	int len = 8;
	char msg[len];
	int *op1,*op2;
	op1 = msg;
	op2 = msg + 4;
	(*op1) = IO_GETMULT;
	(*op2) = length;
	
	char reply[length];
	int i = -1;
	if ( channel == TRAIN_PORT ) {
		if (UART1_IO_TID==-1) {
			UART1_IO_TID = WhoIs("UART1");
		}
		length = Send( UART1_IO_TID, msg, len, reply, length );
		while (++i<length) {
			line[i] = reply[i];
		}		
	}
	if ( channel == TERM_PORT ) {
		if (UART2_IO_TID==-1) {
			UART2_IO_TID = WhoIs("UART2");
		}
	}
	return length;
}

int Putc( int channel, char ch ) {
	int len = 5;
	char msg[len];
	int *op1;
	op1 = msg;
	(*op1) = IO_PUT;
	msg[4] = ch;
	char reply[IO_REPLY_LENGTH];
	
	//bwprintf(TERM_PORT, "putc: %c\n", msg[4]);
	if ( channel == TRAIN_PORT ) {
		if (UART1_IO_TID==-1) {
			UART1_IO_TID = WhoIs("UART1");
		}
		Send( UART1_IO_TID, msg, len, reply, IO_REPLY_LENGTH );
	}
	if ( channel == TERM_PORT ) {
		if (UART2_IO_TID==-1) {
			UART2_IO_TID = WhoIs("UART2");
		}
		Send( UART2_IO_TID, msg, len, reply, IO_REPLY_LENGTH );
	}
	
	return 0;
}

int Putline( int channel, char *ch, int line_length ) {
	if (!line_length)return 0;
        //bwprintf(TERM_PORT, "Putline: %d\n", line_length);
	int len = 4;
	char msg[IO_BUFFER_LENGTH];
	int *op1;
	op1 = msg;
	(*op1) = IO_PUTLINE;
	while (1) {
		msg[len] = *ch;
		ch++;
		len++;
		if (len == line_length + 4)break;
		if (len == IO_BUFFER_LENGTH)break;
	}
	char reply[IO_REPLY_LENGTH];
	
	if ( channel == TRAIN_PORT ) {
		if (UART1_IO_TID==-1) {
			UART1_IO_TID = WhoIs("UART1");
		}
		Send( UART1_IO_TID, msg, len, reply, IO_REPLY_LENGTH );
	}
	if ( channel == TERM_PORT ) {
		if (UART2_IO_TID==-1) {
			UART2_IO_TID = WhoIs("UART2");
		}
		Send( UART2_IO_TID, msg, len, reply, IO_REPLY_LENGTH );
	}
	
	return *(int *)reply;
}

int getLine(char *buff) {
	char next_char;
	int i = 0;
	while (1) {
		next_char = Getc(TERM_PORT);
		if (next_char == '\r')break;
		if (i == TRAIN_INPUT_SIZE)break;
		buff[i] = next_char;
		Putc(TERM_PORT, next_char);
		i++;
	}
	buff[i] = 0;
	return i;
}
