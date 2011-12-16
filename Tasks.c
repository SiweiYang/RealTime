#include <ts7200.h> 
#include <bwio.h>
#include <ContextSwitch.h>
#include <NameServer.h>
#include <Tasks.h>
#include <timer.h>
#include <ClockServer.h>
#include <LockServer.h>
#include <IO.h>
#include <TrainTask.h>
#include <TrackUtil/TrackMonitor.h>
#include <TrackUtil/TrackInformation.h>


#include <Interrupt.h>
extern int CLOCK_SERVER_TID;
extern int NAME_SERVER_TID;
extern unsigned int running_tasks;
extern unsigned int volatile idle_counter;

void test_io() {
    NAME_SERVER_TID = Create(6, "NAMESERVER", &name_server);
    initialize_clock_server();
    Create(6, "LCK_SERVER", &lock_server);
    Create(5, "CLK_SERVER", &clock_server);
    
    Create(3, "IDLE", &idle);
    initialize_io();
    Create(6, "UART1", &uart1_io);
    Create(6, "UART2", &uart2_io);
    
    //Delay(100);
    initialize_track_information();
    initiate_track_monitor();
    Create(3, "TRAIN_IN", &train_input);
    Create(3, "SENSOR", &sensor_reader);
    Delay(100);
    Create(5, "TIMER", &update_timer);
    
    Exit();
}

void the_first_user_task_k3() {
    bwprintf(TERM_PORT, "Starting User Task\n");
    
    int task1_tid, task2_tid, task3_tid, task4_tid, ns_tid, cs_tid, idle_tid;
    int len = 8;
    char reply[len];
    int *op1, *op2, tid;
    op1 = reply;
    op2 = reply+4;

    task1_tid = Create( 4, "TASK1", &first_client_k3 );
    task2_tid = Create( 3, "TASK2", &first_client_k3 );
    task3_tid = Create( 2, "TASK3", &first_client_k3 );
    task4_tid = Create( 1, "TASK4", &first_client_k3 );
    
    idle_tid = Create( 0, "IDLE", &idle );
    
    ns_tid = Create( 6, "NAMESERVER", &name_server );
    NAME_SERVER_TID = ns_tid;
    cs_tid = Create( 5, "CLK_SERVER", &clock_server );
    CLOCK_SERVER_TID = cs_tid;

    Receive( &tid, reply, len );
    (*op1) = 10;
    (*op2) = 20;
    Reply( tid, reply, len );
    Receive( &tid, reply, len );
    (*op1) = 23;
    (*op2) = 9;
    Reply( tid, reply, len );
    Receive( &tid, reply, len );
    (*op1) = 33;
    (*op2) = 6;
    Reply( tid, reply, len );
    Receive( &tid, reply, len );
    (*op1) = 71;
    (*op2) = 3;
    Reply( tid, reply, len );

    Exit();
}

void first_client_k3() {
    int my_tid = MyTid();
    int len = 8;
    int i;
    char reply[len];
    int *op1, *op2;
    op1 = reply;
    op2 = reply+4;
    
    Send( MyParentTid(), "GET DELAY INFO", len, reply, len );
    bwprintf(TERM_PORT, "My TID: %d , My delay interval: %d, Delay number: %d\n", my_tid, (*op1), (*op2));

    for (i = 0; i < *op2; i++) {
        Delay( *op1 );
        bwprintf(TERM_PORT, "TID: %d, interval: %d, number: %d\n", my_tid, (*op1), i+1);
    }

    Exit();
}

void idle() {
    idle_counter = 100;
    while(1) {
        idle_counter++;
    }
    
    Exit();
}

void the_first_user_task () {
    bwprintf(TERM_PORT, "Entering Bootstrap.\n");
        init_timer();
    
    int *vic1, *vic2;
    
    vic1 = VIC1_BASE + INT_SELECT_OFFSET;
    vic2 = VIC2_BASE + INT_SELECT_OFFSET;
    bwprintf(TERM_PORT, "SELECT -> IRQ1: %x, IRQ2: %x\n", *vic1, *vic2);
    enable_interrupt_type(0);
    
    vic1 = VIC1_BASE + INT_ENABLE_OFFSET;
    vic2 = VIC2_BASE + INT_ENABLE_OFFSET;
    bwprintf(TERM_PORT, "ENABLE -> IRQ1: %x, IRQ2: %x\n", *vic1, *vic2);
    simulate_interrupt_type(0);
    
    
    vic1 = VIC1_BASE + SOFT_INT_OFFSET;
    vic2 = VIC2_BASE + SOFT_INT_OFFSET;
    bwprintf(TERM_PORT, "SOFT -> IRQ1: %x, IRQ2: %x\n", *vic1, *vic2);
    
    vic1 = VIC1_BASE;
    vic2 = VIC2_BASE;
    bwprintf(TERM_PORT, "SIGNAL -> IRQ1: %x, IRQ2: %x\n", *vic1, *vic2);
    
    vic1 = VIC1_BASE + RAW_INT_OFFSET;
    vic2 = VIC2_BASE + RAW_INT_OFFSET;
    bwprintf(TERM_PORT, "RAW -> IRQ1: %x, IRQ2: %x\n", *vic1, *vic2);

    int new_tid;
    new_tid = Create( 3, "RPS_SERVER", &rps_server );
    new_tid = Create( 2, "RPS_CLT1", &client_one );
    new_tid = Create( 1, "RPS_CLT1", &client_two );
    bwprintf(TERM_PORT, "Created RPS.\n");
    new_tid = Create( 5, "NAMESERVER", &name_server );
    NAME_SERVER_TID = new_tid;
    bwprintf(TERM_PORT, "Exiting Bootstrap.\n");
    Exit();
}

void rps_server() {
    int sender_tid1;
    int sender_tid2;
    char msg1[30];
    char msg2[30];
    //bwprintf(TERM_PORT, "RPS SERVER: up \n");
    
    int status = RegisterAs("RPS");
    bwprintf(TERM_PORT, "RPS SERVER: successfully registered with return %d\n", status);

    bwprintf(TERM_PORT, "RPS SERVER: Waiting for players... \n");

    Receive( &sender_tid1, msg1, 30 );
    bwprintf( TERM_PORT, "RPS SERVER: Player with tid %d has joined the game.\n", sender_tid1 ); 
    Receive( &sender_tid2, msg2, 30 );
    bwprintf( TERM_PORT, "RPS SERVER: Player with tid %d has joined the game.\n", sender_tid2 ); 
    
    bwprintf(TERM_PORT, "RPS SERVER: Okay! Let's play! \nRPS SERVER: What are your choices?\n");

    Reply( sender_tid1, REQUEST_CHOICE, 30 );
    Reply( sender_tid2, REQUEST_CHOICE, 30 );

    Receive( &sender_tid1, msg1, 30 );
    Receive( &sender_tid2, msg2, 30 );

    // Do the algorithm
    while ( !(string_cmp(msg1, QUIT, 1)||string_cmp(msg2, QUIT, 1)) ) {
    bwprintf(TERM_PORT, "RPS SERVER: Player 1 chose to %s!\n", msg1 );
    bwprintf(TERM_PORT, "RPS SERVER: Player 2 chose to %s!\n", msg2 );
    if ( (string_cmp(msg1, ROCK, 1) && string_cmp(msg2, SCISSORS, 1)) 
      || (string_cmp(msg1, PAPER, 1) && string_cmp(msg2, ROCK, 1)) 
      || (string_cmp(msg1, SCISSORS, 1) && string_cmp(msg2, PAPER, 1)) ) {
            Reply( sender_tid1, RESULT2, 30 );
        Reply( sender_tid2, RESULT1, 30 );
    }
    if ( (string_cmp(msg2, ROCK, 1) && string_cmp(msg1, SCISSORS, 1))
      || (string_cmp(msg2, PAPER, 1) && string_cmp(msg1, ROCK, 1)) 
      || (string_cmp(msg2, SCISSORS, 1) && string_cmp(msg1, PAPER, 1)) ) { 
        Reply( sender_tid1, RESULT1, 30 );
        Reply( sender_tid2, RESULT2, 30 );
    }
    if ( (string_cmp(msg1, ROCK, 1) && string_cmp(msg2, ROCK, 1)) 
      || (string_cmp(msg1, PAPER, 1) && string_cmp(msg2, PAPER, 1)) 
      || (string_cmp(msg1, SCISSORS, 1) && string_cmp(msg2, SCISSORS, 1)) ) { 
        Reply( sender_tid1, RESULT3, 30 );
        Reply( sender_tid2, RESULT3, 30 );
    }
    Receive( &sender_tid1, msg1, 30 );
    Receive( &sender_tid2, msg2, 30 );
    bwprintf(TERM_PORT, "RPS SERVER: Player responses received.\n");
    }
    
    if ( string_cmp( msg1, QUIT, 1 ) ) {
    bwprintf(TERM_PORT, "RPS SERVER: Tells Player 1 that it is okay to quit.\n");
    Reply( sender_tid1, CONFIRM_QUIT, 30 );
    bwprintf(TERM_PORT, "RPS SERVER: Informs Player 2 that Client one quit.\n");
    Reply( sender_tid2, INFORM_OF_QUIT, 30 );
    }
    else {
    bwprintf(TERM_PORT, "RPS SERVER: Tells Player 2 that it is okay to quit.\n");
    Reply( sender_tid2, CONFIRM_QUIT, 30 );
    bwprintf(TERM_PORT, "RPS SERVER: Informs Player 1 that Client two quit.\n");
     Reply( sender_tid1, INFORM_OF_QUIT, 30 );
    }
    
    Exit();
}

void client_one() {
    // We would have gotten this from the name server
    int RPS_SERVER = WhoIs("RPS");
    bwprintf(TERM_PORT, "Player 1: Registered with RPS server %d\n", RPS_SERVER);
    int i = 0; 
    char reply[30];
    int random = 0;

    Send( RPS_SERVER, JOIN_PLAY, 30, reply, 30 );
    
    while ( i < 10 ) {
        random = (count_timer())%3;
        if (random == 0) {
            Send( RPS_SERVER, ROCK, 30, reply, 30 );
    }
    else if (random == 1) {
            Send( RPS_SERVER, PAPER, 30, reply, 30 );
    }
    else {
            Send( RPS_SERVER, SCISSORS, 30, reply, 30 );
    }
        if ( string_cmp( reply, RESULT1, 1 ) ) {
            bwprintf(TERM_PORT, "Player 1: I lost!\n");
        }
        else if ( string_cmp( reply, RESULT2, 1 ) ) {
            bwprintf(TERM_PORT, "Player 1: I won!\n");
        }
        else {
            bwprintf(TERM_PORT, "Player 1: It was a tie!\n");
        }
       i++;
    }

    Send( RPS_SERVER, QUIT, 30, reply, 30 );
    bwprintf(TERM_PORT, "Player 1: I quit!\n");
    
    Exit();
}

void client_two() {
    // We would have gotten this from the name server
    int RPS_SERVER = WhoIs("RPS");
    char reply[30];
    int random = 0;

    bwprintf(TERM_PORT, "Player 2: Registered with RPS server %d\n", RPS_SERVER);
    int replylen = Send( RPS_SERVER, JOIN_PLAY, 30, reply, 30 );
    
    while ( !string_cmp( reply, INFORM_OF_QUIT, replylen ) ) {
        random = (count_timer())%3;
        if (random == 0) {
            Send( RPS_SERVER, ROCK, 30, reply, 30 );
    }
    else if (random == 1) {
            Send( RPS_SERVER, PAPER, 30, reply, 30 );
    }
    else {
            Send( RPS_SERVER, SCISSORS, 30, reply, 30 );
    }
        if ( string_cmp( reply, RESULT1, 1 ) ) {
            bwprintf(TERM_PORT, "Player 2: I lost!\n");
        }
        else if ( string_cmp( reply, RESULT2, 1 ) ) {
            bwprintf(TERM_PORT, "Player 2: I won!\n");
        }
        else if ( string_cmp( reply, RESULT3, 1 ) ) {
            bwprintf(TERM_PORT, "Player 2: It was a tie!\n");
        }
    else {

    }
    bwprintf(TERM_PORT, "Press a button to continue to next round.\n");
    bwgetc(TERM_PORT);
    }
    bwprintf(TERM_PORT, "Player 2: Oh no the other guy quit!\n");
    
    Exit();
}

void the_other_user_tasks() {
    int my_tid, parent_tid;
    
    my_tid = MyTid();
    parent_tid = MyParentTid();
    bwprintf(TERM_PORT, "MY TID: %d, MY PARENTS TID: %d\n", my_tid, parent_tid);
    bwprintf(TERM_PORT, "MY TID: %d, MY PARENTS TID: %d\n", my_tid, parent_tid);
    
    char *msg = "I'm a message!!!";
    int replylen = 18;
    char reply[replylen];
    Send( 0, msg, 18, reply, replylen );

    Exit();
}

void test_user_send() {
    //bwprintf(TERM_PORT, "Performance Test Start\n");
    //bwprintf(TERM_PORT, "Sender Up\n");
    init_timer();
    int volatile tick = count_timer();
    bwprintf(TERM_PORT, "Performance Test started with tick %d\n", tick);
    volatile int receiver_tid = Create( 6, NULL, &test_user_receive );
    
    int loops = 5000;
    int len = 64;
    char msg[len];
    char reply[len];
    int volatile i = 0;
    while (i<loops) {
        //bwprintf(TERM_PORT, "Send: %d to Task %d\n", i, receiver_tid);
        Send( receiver_tid, msg, len, reply, len );
        i++;
    }
    bwprintf(TERM_PORT, "Performance Test finished with tick %d\n", count_timer());
    
    bwprintf(TERM_PORT, "Performance Test Done transfer %d bytes %d times in %d milliseconds\n", len, loops, (0x10000+tick-count_timer())%0x10000/2);
    
    Exit();
}

void test_user_receive() {
    bwprintf(TERM_PORT, "Receiver Up\n");
    int loops = 5000001;
    volatile int tid;
    int len = 64;
    char msg[len];
    
    
    int volatile i = 0;
    int volatile result;
    while (i<loops) {
        result = Receive( &tid, msg, len );
        //bwprintf(TERM_PORT, "Receive: %d from Task %d\n", i, tid);
        Reply( tid, msg, len );
        i++;
    }    
    Exit();
}
