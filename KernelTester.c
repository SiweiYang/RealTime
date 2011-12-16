#include <ts7200.h>
#include <bwio.h>
#include <ContextSwitch.h>
#include <KernelTester.h>

void context_switch_tester() {
    while (1) {
        //dump_registers();
        bwprintf(TERM_PORT, "My TID: %d\n", MyTid());
        Pass();
        //dump_registers();
    }
}

void SRR_Sender() {
    int i = 0, len = 8, tester_id, receiver_id;
    char msg[len], reply[len];
    
    Receive( &tester_id, msg, len);
    receiver_id = *(int *)msg;
    Reply( tester_id, reply, len);
    
    while (1) {
        *(int *)msg = i;
        bwprint(TERM_PORT, "Testing Send...\n");
        dump_registers();
        Send( receiver_id, msg, len, reply, len );
        dump_registers();
        i = *(int *)reply;
        i++;
    }
}

void SRR_Receiver() {
    int i, len = 8, tester_id, sender_id;
    char msg[len], reply[len];
    
    //Receive( &tester_id, msg, len);
    //sender_id = *(int *)msg;
    //Reply( tester_id, reply, len);
    
    while (1) {
        Receive( &sender_id, msg, len );
        i = *(int *)msg;
        i++;
        *(int *)reply = i;
        Reply( sender_id, reply, len )
    }
}

void SRR_tester() {
    int sender_id, receiver_id, len = 8;
    sender_id = Create(5, "SRR_SND", &SRR_Sender);
    receiver_id = Create(5, "SRR_RCV", &SRR_Receiver);
    
    char msg[len], reply[len];
    *(int *)msg = receiver_id;
    Send( sender_id, msg, len, reply, len);
    
    bwprint(TERM_PORT, "SRR setup finished...\n");
}

void interrupt_tester() {
    
}