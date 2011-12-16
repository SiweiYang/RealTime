#ifdef TEST
#include <stdio.h>
#endif

#include <ts7200.h>
#include <ContextSwitch.h>
#include <NameServer.h>

int LOCK_SERVER_TID = -1;
void lock_server() {
	RegisterAs("LCK");
    LOCK_SERVER_TID = WhoIs("LCK");
	int received_tid = -1;
	int len = 8;
	char msg[len];
	int *op1, *op2;
	op1 = (int *)msg;
	op2 = op1 + 1;
	char reply[len];
	while (1) {
		Receive( &received_tid, (char *)msg, len );

        Reply(received_tid, (char *)msg, len);
		
		Send( received_tid, msg, len, reply, len );
	}
}

// Send to lock server
int Acquire() {
	if (LOCK_SERVER_TID == -1) {
		LOCK_SERVER_TID = WhoIs("LCK");
		if (LOCK_SERVER_TID == -1) {
#ifdef TEST
            printf("Missing Lock Server\n");
#endif
			return -1;
		}
	}
	int len = 8;
	char msg[len];
	int *op1, *op2;
	op1 = (int *)msg;
	op2 = op1 + 1;
	(*op1) = 1;	
	(*op2) = 0;
	char reply[len];
	Send( LOCK_SERVER_TID, msg, len, reply, len );
	return *(int *)reply; 
}

// Receive and Reply to lock server
int Release() {
	int len = 8;
	char msg[len];
	int *op1, *op2;
	op1 = (int *)msg;
	op2 = op1 + 1;
	int received_tid;
	Receive( &received_tid, (char *)msg, len );
	if (LOCK_SERVER_TID == -1) {
		LOCK_SERVER_TID = WhoIs("LCK");
		if (LOCK_SERVER_TID == -1) {
#ifdef TEST
            printf("Missing Lock Server\n");
#endif
			return -1;
		}
		
		if (LOCK_SERVER_TID != received_tid) {
#ifdef TEST
            printf("Target not Lock Server\n");
#endif
			return -1;
		}
	}
	
	char reply[len];
	Reply( received_tid, reply, len );
	return 0; 
}
