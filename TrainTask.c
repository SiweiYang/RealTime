#ifdef TEST
#include <stdio.h>
#endif

#include <ts7200.h>
#include <ContextSwitch.h>

#include <NameServer.h>
#include <ClockServer.h>
#include <LockServer.h>
#include <Notifier.h>
#include <Tasks.h>

#include <IO.h>
#include <UI.h>
#include <trainAPI.h>
#include <TrainTask.h>
#include <TrackUtil/track_data.h>
#include <TrackUtil/Router.h>
#include <TrackUtil/Reservation.h>
#include <TrackUtil/TrainProfile.h>
#include <TrackUtil/SwitchProfile.h>
#include <TrackUtil/TrackMonitor.h>
#include <TrackUtil/TrackInformation.h>

#define ClearCommandLine Putline(TERM_PORT, "\033[18;28H\033[K\033[18;101H|\033[18;28H", 29);
#define BLINK_ON() Putline(TERM_PORT, "\0337\033[4;90Ho\0338", 12);
#define BLINK_OFF() Putline(TERM_PORT, "\0337\033[4;90HO\0338", 12);

extern unsigned int volatile idle_counter;
extern int volatile current_time;

void go_command() {
    char cmd[2];
    cmd[0] = 96;
    cmd[1] = 0;
    Putline( TRAIN_PORT, cmd, 2 );
}

void stop_command() {
    char stop = 97;
    Putline( TRAIN_PORT, &stop, 1 );
}

void train_set_speed( int train, int speed ) {
    if ((speed&0x0000FFFF)>15)return;
    speed &= 0x0000FFFF;
    char send[2];
    send[0] = speed;
    send[1] = train;
    Putline( TRAIN_PORT, send, 2 );

    train_state *ts = get_train_state(train);
    if (ts) {
        int train_speed = (*ts).train_speed;
        train_speed &= 0x0000FFFF;

        if (speed==15) {
            speed = 0;
            int dist = get_next_node_id((*ts).last_node_id, &((*ts).last_node_id));
            (*ts).last_node_id = get_node_id((*get_node((*ts).last_node_id)).reverse);
            (*ts).last_offset = dist - (*ts).last_offset;
            //(*ts).last_sensor_id = (*ts).last_node_id;
        } else if (speed<train_speed) {
            speed = speed | 0xFFFF0000;
        }
    }
    update_train_speed(train, speed);
}

void train_reverse( int tn ) {
    train_state *ts = get_train_state(tn);
    if (!ts)return;

    int train_speed = (*ts).train_speed;
    train_set_speed(tn,0);

    Delay(300);
    train_set_speed(tn,15);
    Delay(300);

    train_set_speed(tn,train_speed);
}

void set_switch( int switch_number, char direction ) {
    char send[2];
    if ( direction == 'C' ) {
        send[0] = 34;
    }
    else {
        send[0] = 33;
    }
    send[1] = switch_number;
    Putline( TRAIN_PORT, send, 2 );
    Delay(4);
    send[0] = 32;
    send[1] = switch_number;
    Putline( TRAIN_PORT, send, 2 );
    update_switch( switch_number, direction );
}

void set_switches(Buffer *buf, int *switch_map) {
    int i, j;
    char send[4];
    // This will set all of the switches in less than 1 second!
    for ( i = 0; i < total_switch_number; i++ ) {
        if ( i > 17 ) {
            j = i + 135;
        }
        else {
            j = i + 1;
        }

        if (switch_map[i] == DIR_STRAIGHT) {
            send[0] = 33;
            print_switch(buf, j, 'S');
            update_switch(j, 'S');
        } else if (switch_map[i] == DIR_CURVED) {
            send[0] = 34;
            print_switch(buf, j, 'C');
            update_switch(j, 'C');
        } else {
            continue;
        }
        send[1] = j;
        send[2] = send[0];
        send[3] = j;
        Putline( TRAIN_PORT, send, 4 );
        Delay(5);
    }
    Putline(TERM_PORT, buf->line, buf->w_cursor);
    clearBuffer(buf);

    send[0] = 32;
    send[1] = j;
    Putline( TRAIN_PORT, send, 2 );
}

void train_input() {
    Buffer buf;
    clearBuffer(&buf);
    
    ui_initialize_kernel_monitor(&buf);
    Putline(TERM_PORT, buf.line, buf.w_cursor);
    clearBuffer(&buf);
    
    ui_initialize(&buf);
    Putline(TERM_PORT, buf.line, buf.w_cursor);
    clearBuffer(&buf);
    set_switches(&buf, get_switch_map());
    
    initialize_track_route_map();
    initialize_reservations();

    int cmd_code, tn, ts, sn, sd, gm, gs;
    int i, len, sensor_count = 0;

    // For train status
    train_state *trs;
    // For routing
    route_edge route_map[TRACK_MAX];
    int switch_map[total_switch_number];
    // For command
    char input[TRAIN_INPUT_SIZE+1];

    go_command();
    while ( 1 ) {
        len = getLine(input);
        cmd_code = checkCommand(input);
        if (cmd_code==QUIT_PROGRAM)break;
        Acquire();
        switch (cmd_code) {
            case UNKNOWN_COMMAND:
                break;
            case TRAIN_SPEED:
                parseTrainSpeed(input, &tn, &ts);
                train_set_speed(tn, ts);
                trs = get_train_state(tn);
                print_train_number(&buf, (*trs).train_index, (*trs).train_number);
                print_train_speed(&buf, (*trs).train_index, (*trs).train_speed);
                Putline(TERM_PORT, buf.line, buf.w_cursor);
                clearBuffer(&buf);
                break;
            case TRAIN_REVERSE:
                parseTrainReverse(input, &tn);
                trs = get_train_state(tn);
                if (!trs)break;

                ts = (*trs).train_speed;
                add_action(&((*trs).taq), action_set_speed, 0, 0, 0, 0);
                add_action(&((*trs).taq), action_wait_velocity_eq, 0, 0, 0, 0);
                add_action(&((*trs).taq), action_set_speed, 0, 0, 15, 0);
                add_action(&((*trs).taq), action_set_speed, 0, 0, ts, 0);
                break;
            case SWITCH_TURN:
                parseSwitchTurn(input, &sn, &sd);
                print_switch(&buf, sn, sd);
                Putline(TERM_PORT, buf.line, buf.w_cursor);
                clearBuffer(&buf);
                set_switch(sn, sd);
                break;
            case GO_TO:
                if (parseGoTo(input, &tn, &sn)) {
                    trs = get_train_state(tn);
                    if (!trs)break;

                    add_action(&((*trs).taq), action_wait_stop_for_node_id_offset, sn, 0, 0, 0);
                    add_action(&((*trs).taq), action_set_speed, 0, 0, 0, 0);
                    print_train_stop(&buf, (*trs).train_index, (*trs).stop_node_id, (*trs).stop_offset);
                    Putline(TERM_PORT, buf.line, buf.w_cursor);
                    clearBuffer(&buf);
                }
                
                break;
            case TRAIN_STOP_AT:
                if (parseTrainStop(input, &tn, &sn)) {
                    trs = get_train_state(tn);
                    if (!trs)break;
                    
                    add_action(&((*trs).taq), action_wait_node_id_offset, sn, 0, 0, 0);
                    add_action(&((*trs).taq), action_set_speed, 0, 0, 0, 0);
                    print_train_stop(&buf, (*trs).train_index, sn, ts);
                    Putline(TERM_PORT, buf.line, buf.w_cursor);
                    clearBuffer(&buf);
                }
                
                break;
            case ROUTE:
                if (parseRoute(input, &tn, &ts, &sn)) {
                    trs = get_train_state(tn);
                    if (!trs)break;
                    
                    //writeFormatString(buf.line, 100, "Train %d @ %d finding path to %d", (*trs).train_number, (*trs).last_sensor_id, sn);
                    //print_debug_string(buf.line);
                    // find out where the train is
                    mark_start_node(route_map, (*trs).last_sensor_id);
                    while(add_nearest_node_to_map(route_map)) {
                        if (route_map[sn].length>-1) {
                            print_debug_string("Found a path...");
                            break;
                        }
                    }
                    // route from the train to destination sensor
                    update_switches_to_route(route_map, sn, switch_map);
                    for (i=0;i<total_switch_number;i++){
                        if (switch_map[i]==DIR_STRAIGHT) {
                            writeFormatString(buf.line, 90, "Switch %d -> Straight\n", i+1);
                            print_debug_string(buf.line);
                        } else if (switch_map[i]==DIR_CURVED) {
                            writeFormatString(buf.line, 90, "Switch %d -> Curved\n", i+1);
                            print_debug_string(buf.line);
                        } else {
                            continue;
                        }
                    }
                    set_switches(&buf, switch_map);
                    Putline(TERM_PORT, buf.line, buf.w_cursor);
                    clearBuffer(&buf);
                    fill_action_queue_to_route(route_map, sn, ts, &((*trs).taq));
                }
                
                break;
            case SCHE_TO:
                // parse train number and destination sensor number
                // find out where the train is
                // route from the train to destination sensor
                for (i = 0; i<TRACK_MAX; i++){
                    route_map[i].length = -1;
                    route_map[i].from = -1;
                }
                route_map[3].length = 0;
                route_map[3].from = 3;
                while(add_nearest_node_to_map(route_map)) {
                    if (route_map[sn].length>-1)break;
                }

                update_switches_to_route(route_map, sn, switch_map);
                set_switches(&buf, switch_map);
                Putline(TERM_PORT, buf.line, buf.w_cursor);
                clearBuffer(&buf);
                break;
            case LOAD_TRACK:
                if (parseLoadTrack(input, &tn)) {
                    initialize_track(tn);
                }
                break;
            case LOAD_SWITCHES:
                if (parseLoadSwitches(input, &sn)) {
                    switch_profile(sn, get_switch_map());
                    set_switches(&buf, get_switch_map());
                }
                break;
            case TRAIN_SENSOR:
                if (parseTrainSensor(input, &tn, &sn)) {
                    trs = get_train_state(tn);
                    if (!trs)break;
                    
                    update_with_sensor_id(sn);
                    handle_sensor_update();
                }
                break;
            case TRAIN_CLEAR:
                if (parseTrainClear(input, &tn)) {
                    trs = get_train_state(tn);
                    if (!trs)break;
                    
                    clear_action_queue(&(*trs).taq);
                    train_set_speed(tn, 0);
                    (*trs).train_recovery_speed = -1;
                    print_train_speed(&buf, (*trs).train_index, (*trs).train_speed);
                    Putline(TERM_PORT, buf.line, buf.w_cursor);
                    clearBuffer(&buf);
                }
                break;
            default:
                break;
        }
        Release();
        ClearCommandLine;
        
    }
    stop_command();
    //char output[200];
    //int percent = ((idle_counter/current_time)/(135865.597))*100;
    //int len_output = writeFormatString(output, 200, "\033[2J\033[0;0HAll done, Idle Counter: %d Clock Time: %d Idle Percent: %d %", idle_counter, current_time, percent);
    //Putline(TERM_PORT, output, len_output);

    Delay(5);
}

void sensor_reader() {
    int i;
    char sensor_map[sensor_module_number*2];
    for (i=0;i<sensor_module_number*2;i++) {
        sensor_map[i] = 0;
    }
    i = 0;
    while (1) {
        Getline(TRAIN_PORT, sensor_map, sensor_module_number*2);
        if (i>3) {
            Acquire();
            update_sensor(sensor_map);
            Print_Sensors(sensor_map);
            
            handle_sensor_update();
            Release();
        }

        i++;
        if ( i%10 == 0 ) {
            BLINK_ON();
        }
        if ( i%10 == 5 ) {
            BLINK_OFF();
        }
        
        Delay(1);
    }
}

void handle_sensor_update() {
    Buffer buf;
    clearBuffer(&buf);
    volatile train_state * volatile train_profiles, * volatile ts;
    int i, k, train_speed, reservation_status;
    
    train_profiles = get_train_states();
    for ( i = 0 ; i < number_of_trains ; i++ ) {
        ts = train_profiles + i;
        if ((*ts).train_number>-1 && (*ts).last_sensor_id>-1) {
            if ((*ts).train_index > number_of_trains - 1)print_debug_string("Index problem again...");
            
            print_train_segment((*ts).train_index, (*ts).last_sensor_id);

            //if (train_speed > 0 || (*ts).velocity > 200) {
            //    reservation_status = train_at((*ts).train_number, stopping_distance_profile( (*ts).train_number, (*ts).train_speed), (*ts).last_sensor_id);
            //    //print_debug_string("*** Updated Reservation Area ***");
            //    train_speed = (*ts).train_speed & 0x0000FFFF;
            //    if ( reservation_status == RESERV_FAIL && train_speed > 0 ) {
            //        (*ts).train_recovery_speed = (*ts).train_speed;
            //        train_set_speed( (*ts).train_number, 0 );
            //        
            //        print_train_speed(&buf, (*ts).train_index, (*ts).train_speed);
            //        Putline(TERM_PORT, buf.line, buf.w_cursor);
            //        clearBuffer(&buf);
            //        writeFormatString(buf.line, BUFFER_SIZE, "*** Collision Detected ***: Train[%d] at %d with speed %d",  (*ts).train_number, (*ts).last_sensor_id, (*ts).train_speed);
            //        print_debug_string(buf.line);
            //    }
            //} else {
            //    //if ( reservation_status == RESERV_SUCC_NOCHANGE ) {
            //    //    k = writeFormatString(buf.line, BUFFER_SIZE, "Train %d acquired sensor %d, %d and beyound...", (*ts).train_number, (*ts).last_sensor_id, (*ts).next_sensor_id);
            //    //    print_debug_string(buf.line);
            //    //}
            //    reservation_status = train_at((*ts).train_number, 100, (*ts).last_sensor_id);
            //    if ( reservation_status == RESERV_SUCC && (*ts).train_recovery_speed > 0 ) {
            //        train_set_speed( (*ts).train_number, (*ts).train_recovery_speed );
            //        (*ts).train_recovery_speed = 0;
            //        
            //        print_train_speed(&buf, (*ts).train_index, (*ts).train_speed);
            //        Putline(TERM_PORT, buf.line, buf.w_cursor);
            //        clearBuffer(&buf);
            //        print_debug_string("*** Restoring Speed ***");
            //    }
            //}
        }
    }
}

void update_timer() {
    Buffer buf;
    clearBuffer(&buf);
    char sensor_update_command[2];
    sensor_update_command[0] = 192;
    sensor_update_command[1] = 133;

    int i, j, t = Time();
    train_state *train_profiles, *ts;
    int train_path[max_path_length], time_stamp[max_path_length];
    while (1) {
        if (!(t%8)) {
            Putline(TRAIN_PORT, sensor_update_command, 2);
            attach_timestamp(t);
        }
        
        Acquire();
        train_profiles = get_train_states();
        for ( i = 0; i < number_of_trains; i++ ) {
            ts = train_profiles+i;
            if ((*ts).train_index > number_of_trains - 1)print_debug_string("Index problem in Timer...");
            if ((*ts).train_number>-1 && (*ts).last_sensor_id>-1) {
                track_node *new_node = get_location(
                    (*ts).train_number,
                    (*ts).train_speed,
                    get_node((*ts).last_node_id),
                    (*ts).last_offset,
                    (*ts).velocity,
                    &((*ts).last_offset),
                    &((*ts).velocity)
                );
                if (!new_node) {
                    print_debug_string("Hitting a dead end...");
                    (*ts).last_sensor_id = -1;
                    (*ts).last_node_id = -1;
                    continue;
                }
                
                (*ts).last_node_id = get_node_id(new_node);
                if (!(t%5))print_train_offset(&buf, (*ts).train_index, (*ts).velocity/10, (*ts).last_node_id, (*ts).last_offset);
                if (!(t%5))print_train_stack(&buf, (*ts).train_index, ((*ts).taq.tail + train_action_queue_size - (*ts).taq.head) % train_action_queue_size);
        
                train_action *ta = get_current_action(&((*ts).taq));
                if (ta) {
                    switch ((*ta).action_id) {
                        case action_wait_velocity_eq:
                            if ((*ts).velocity==(*ta).velocity) {
                                ta = dump_current_action(&((*ts).taq));
                            }
                            break;
                        case action_wait_node_id_offset:
                            (*ts).stop_node_id = (*ta).node_id;
                            (*ts).stop_offset = (*ta).node_offset;
                            if (check_stop_train((*ts).train_number)) {
                                ta = dump_current_action(&((*ts).taq));
                            }
                            break;
                        case action_wait_stop_for_node_id_offset:
                            update_stop_sensor((*ts).train_speed, (*ts).train_number, (*ta).node_id);
                            if (check_stop_train((*ts).train_number))ta = dump_current_action(&((*ts).taq));
                            break;
                        case action_set_speed:
                            train_set_speed((*ts).train_number, (*ta).speed);
                            print_train_speed(&buf, (*ts).train_index, (*ts).train_speed);
                            if (((*ta).speed&0x0000FFFF)<15)print_train_speed(&buf, (*ts).train_index, (*ts).train_speed);
                            ta = dump_current_action(&((*ts).taq));
                            break;
                        default:
                            ta = dump_current_action(&((*ts).taq));
                            break;
                    }
                }
            }
        }
        Release();
        
        t += 1;
        Delay(1);
        Print_Time(t);
    }
}

// First char is the number of commands in total, second char is the output device (TERM_PORT/TRAIN_PORT)
// Then we repeat for as many commands as you want this sequence of chars
// len, op1, op2, delay
// len is either 1 or 2 (number of operations)
// op is the character to send (op1 and op2 are sent one after the other guarenteed)
// delay is the delay before we execute this command (delay is the delay between commands)


//void delay_command_queue() {
//    int received_tid, i;
//    int buffer_max_length = 1024;
//    char commands[buffer_max_length];
//    char to_send[2];
//
//    Receive(&received_tid, commands, buffer_max_length);
//    Reply(received_tid, commands, 0);
//    for ( i = 0; i < commands[0]; i++ ) {
//        if ( commands[1] == TERM_PORT ) {
//            Delay(i*4 + 5);
//            if ( commands[i*4 + 2] == 1 ) {
//                Putline(TERM_PORT, commands + i*4 + 3, 1);
//            }
//            else {
//                to_send[0] = commands[i*4 + 3];
//                to_send[1] = commands[i*4 + 4];
//                Putline(TERM_PORT, to_send, 2);
//            }
//        }
//        if ( commands[1] == TRAIN_PORT ) {
//            Delay(i*4 + 5);
//            if ( commands[i*4 + 2] == 1 ) {
//                Putline(TRAIN_PORT, commands + i*4 + 3, 1);
//            }
//            else {
//                to_send[0] = commands[i*4 + 3];
//                to_send[1] = commands[i*4 + 4];
//                Putline(TRAIN_PORT, to_send, 2);
//            }
//        }
//    }
//}
extern int volatile NAME_SERVER_TID;
void train_task() {
    NAME_SERVER_TID = Create(6, "NAMESERVER", &name_server);
#ifndef TEST
    initialize_clock_server();
    Create(5, "CLK_SERVER", &clock_server);
    Create(0, "IDLE", &idle);
    initialize_io();
    
    Create(6, "UART1", &uart1_io);
    Create(6, "UART2", &uart2_io);
    Create(3, "CTS", &cts_notifier);
    //Create(6, "CTS", &cts_server);
    //Create(6, "CTS", &cts_notifier);
#endif    
    Create(6, "LCK_SERVER", &lock_server);
    
    initialize_track_information();
    initiate_track_monitor();
    Create(3, "TRAIN_IN", &train_input);
    Create(3, "SENSOR", &sensor_reader);
    //Delay(100);
    Create(5, "TIMER", &update_timer);
    
    //Create(5, "TIMER", &update_timer);
    //Create(5, "TIMER", &update_timer);
    //Create(5, "TIMER", &update_timer);
    
    Exit();
}