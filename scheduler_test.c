#include <stdio.h>
#include <stdlib.h>
#include "Scheduler.c"

void init_td(TaskDescriptor * a, int b, int c) {
	(*a).id =  b;
    (*a).state = 0;
    (*a).priority = c;
    (*a).parent = 0;
    
    (*a).stack_pointer = 0;
    (*a).next_in_queue = 0;
}

void print_out_head_everything() {
	int i;
	TaskDescriptor * a;
	for (i = 0; i < 8; i++) {
		if (TASKS_PQ[i][0] != 0) {
			a = TASKS_PQ[i][0];
			while( a != 0 ) {
				printf ("%d   ", (*a).id);
				a = (*a).next_in_queue;
			}
			printf("\n");
		}
	} 
}

void separate() {
	printf("**************************************************\n");
}

int main() {

	int i = 0;

	TaskDescriptor one;
	init_td(&one, 11, 5); 
	/*TaskDescriptor two;
	init_td(&two, 12, 5); 
	TaskDescriptor three;
	init_td(&three, 13, 5); 
	TaskDescriptor four;
	init_td(&four, 14, 5); 
	TaskDescriptor five;
	init_td(&five, 15, 5); 
	TaskDescriptor six;
	init_td(&six, 16, 5); 
	TaskDescriptor seven;
	init_td(&seven, 17, 5); 
	TaskDescriptor eight;
	init_td(&eight, 18, 5); 
	TaskDescriptor nine;
	init_td(&nine, 19, 5); 
	TaskDescriptor ten;
	init_td(&ten, 10, 5); */

	initialize_schedule();

	add_to_schedule(&one);
	/*add_to_schedule(&two);
	add_to_schedule(&three);
	add_to_schedule(&four);
	add_to_schedule(&five);
	add_to_schedule(&six);
	add_to_schedule(&seven);
	add_to_schedule(&eight);
	add_to_schedule(&nine);
	add_to_schedule(&ten);*/

	print_out_head_everything();

	separate();

	TaskDescriptor *active;
	for ( i = 0; i < 10 ; i++ ) {
		active = get_next_scheduled_task();
		printf("%d\n", (*active).id);
		pass_from_schedule(active);
	}

	separate();
	
	print_out_head_everything();

	separate();
	printf("BEGINNING OF REMOVES\n");

	for ( i = 0; i < 6 ; i++ ) {
		active = get_next_scheduled_task();
		if (active != NULL) {
			printf("%d\n", (*active).id);
			remove_from_schedule(active);
		}
		else {
			printf("NULL\n");
		}
	}

	separate();

	print_out_head_everything();
	return 0;
}
