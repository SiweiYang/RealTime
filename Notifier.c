#include <bwio.h>
#include <ts7200.h>
#include <Kernel.h>
#include <ContextSwitch.h>
#include <TaskDescriptor.h>
#include <NameServer.h>
#include <Notifier.h>
#include <IO.h>

extern int volatile UART1_CTS;

void cts_notifier() {
	while (RegisterAs("CTS_NOTIF")==-1) {
		bwprintf(TERM_PORT, "register to name_server failed\n");
	}
	int * volatile flags = (int *)( UART1_BASE + UART_FLAG_OFFSET );
	int received_tid = -1;
	int rcv_len, i, len = 16;
	char msg[IO_BUFFER_LENGTH];
	int *op;
	char *real_msg;
	op = (int *)msg;
	real_msg = msg + 4;
	char reply[len];
	
	int uart1_tid = WhoIs("UART1");
	
	*op = IO_CTS;
	UART1_CTS = (*flags&CTS_MASK);
	while (1) {
		while (!(*flags&CTS_MASK));
		UART1_CTS = (*flags&CTS_MASK);
		//bwprintf(TERM_PORT, "CTS ON\n");
		Send( uart1_tid, msg, 4, reply, len );
		
		while (UART1_CTS==(*flags&CTS_MASK));
		UART1_CTS = (*flags&CTS_MASK);
		//bwprintf(TERM_PORT, "CTS OFF\n");
	}
}

void notifier () {
	//bwprintf(TERM_PORT, "Notifier Up...\n");
	int server_tid, event_type, data;
	int len = 8;
	char msg[len];
	char reply[len];
	int *op1, *op2;
	op1 = (int *)msg;
	op2 = op1 + 1; 
	Receive( &server_tid, (char *)&event_type, 4 );
	Reply( server_tid, CONFIRM, len );
	while (1) {
		data = AwaitEvent( event_type );
		//bwprintf(TERM_PORT, "Notifier Up...\n");
		*op1 = 0;
		*op2 = data;
		
		Send( server_tid, msg, len, reply, len );
	}
}

void notifier_io () {
	//bwprintf(TERM_PORT, "Notifier Up...\n");
	int server_tid, event_type, data;
	int len = 8;
	char msg[len];
	char reply[len];
	
	int *status, *response, *op;
	status = (int *)msg;
	response = status + 1;
	op = (int *)reply;
	
	//bwprintf(TERM_PORT, "Notifier waiting for initialization...\n");
	Receive( &server_tid, (char *)&event_type, 4 );
	Reply( server_tid, CONFIRM, len );
	
	*status = NOTIFIER_START;
	*response = data;
	
	int ch;
	while (1) {
		//bwprintf(TERM_PORT, "Notifier Up...\n");
		Send( server_tid, msg, len, reply, len );
		//bwprintf(TERM_PORT, "notifier get op code %d\n", *op);
		if (*op == NOTIFIER_PUT) {
			ch = reply[4];
			data = AwaitEvent( ch << 24 | event_type );
		} else if  (*op == NOTIFIER_GET) {
			data = AwaitEvent( event_type );
		}
		*status = NOTIFIER_RETURN;
		*response = data;
	}
}
