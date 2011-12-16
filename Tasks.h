#ifdef TASKS_H
#else
#define TASKS_H

#define JOIN_PLAY "I would like to sign up!"
#define REQUEST_CHOICE "What is your pick?"
#define ROCK "Rock"
#define PAPER "Paper"
#define SCISSORS "Scissors"
#define RESULT1 "LOSE!"
#define RESULT2 "WIN!"
#define RESULT3 "TIE!"
#define QUIT "Quit"
#define CONFIRM_QUIT "Okay!"
#define INFORM_OF_QUIT "Other player has quit!"

void test_io();

void the_first_user_task_k3();
void first_client_k3();
void second_client_k3();
void third_client_k3();
void fourth_client_k3();

void idle();

void the_first_user_task();
void rps_server();
void client_one();
void client_two();

void test_user();
void the_other_user_tasks();
void the_first_user_task ();
void test_user_send();
void test_user_receive();
void test_user_receive_send();
void test_user_send_receive();
#endif
