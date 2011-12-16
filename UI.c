#ifdef TEST
#include <stdio.h>
#endif

#include <ts7200.h>
#include <UI.h>
#include <IO.h>
#include <TrackUtil/track_data.h>
#include <TrackUtil/TrackInformation.h>

char sensor_map[20];
char INITIAL_SENSOR_MAP[10];
char LAST_SENSOR_MAP[10];
int SENSOR_COUNT, DEBUG_COUNT;

void ui_initialize(Buffer *buf) {
	int i;
	for (i=0;i<10;i++) {
		INITIAL_SENSOR_MAP[i] = -1;
		LAST_SENSOR_MAP[i] = 0;
	}
	SENSOR_COUNT = 0;
	DEBUG_COUNT = 0;
	
	//Set to home
	writeBufferString(buf,"\033[H");
	writeBufferString(buf,"-----------------------------------------------------------------------------------------------------\n");
	writeBufferString(buf,"|A Sanjay/Siwei Software |                        |   Successfully Running For :                    |\n");
	writeBufferString(buf,"|---------------------------------------------------------------------------------------------------|\n");
	writeBufferString(buf,"|               Switch Directions                 |   Most Recent Triggered Sensors                 |\n");
	writeBufferString(buf,"|---------------------------------------------------------------------------------------------------|\n");
	writeBufferString(buf,"|      1      |     S    |      10     |     S    |                       |TR:    SP:    LS:        |\n");
	writeBufferString(buf,"|      2      |     S    |      11     |     S    |                       |V:     OF:    NN:        |\n");
	writeBufferString(buf,"|      3      |     S    |      12     |     S    |                       |ST:    SO:    SN:        |\n");
	writeBufferString(buf,"|      4      |     S    |      13     |     S    |                       |                         |\n");
	writeBufferString(buf,"|      5      |     S    |      14     |     S    |                       |TR:    SP:    LS:        |\n");
	writeBufferString(buf,"|      6      |     S    |      15     |     S    |                       |V:     OF:    NN:        |\n");
	writeBufferString(buf,"|      7      |     S    |      16     |     S    |                       |       SO:    SN:        |\n");
	writeBufferString(buf,"|      8      |     S    |      17     |     S    |                       |                         |\n");
	writeBufferString(buf,"|      9      |     S    |      18     |     S    |                       |Last Sen  --->  Next Sen |\n");
	writeBufferString(buf,"|     0x99    |     S    |     0x9B    |     S    |                       |                         |\n");
	writeBufferString(buf,"|     0x9A    |     S    |     0x9C    |     S    |                       |Node/Offset:             |\n");
	writeBufferString(buf,"|---------------------------------------------------------------------------------------------------|\n");
	writeBufferString(buf,"| Type Your Command Here :                                                                          |\n");
	writeBufferString(buf,"|---------------------------------------------------------------------------------------------------|\n");
	writeBufferString(buf,"|                                                                                                   |\n");
	writeBufferString(buf,"|                                                                                                   |\n");
	writeBufferString(buf,"|                                                                                                   |\n");
	writeBufferString(buf,"|                                                                                                   |\n");
	writeBufferString(buf,"|                                                                                                   |\n");
	writeBufferString(buf,"|                                                                                                   |\n");
	writeBufferString(buf,"|                                                                                                   |\n");
	writeBufferString(buf,"|                                                                                                   |\n");
	writeBufferString(buf,"|                                                                                                   |\n");
	writeBufferString(buf,"|                                                                                                   |\n");
	writeBufferString(buf,"-----------------------------------------------------------------------------------------------------\n");
	//Shift cursor to command prompt
	writeBufferString(buf,"\033[18;28H");
}

void ui_initialize_kernel_monitor(Buffer *buf) {
#ifdef TEST
	//printf("Had buffer size: ");
#endif
	//Flush screen
	writeBufferString(buf,"\033[2J");
	//Set to home
	writeBufferString(buf,"\033[30;1H");
	writeBufferString(buf,"-----------------------------------------------------------------------------------------------------\n");
	writeBufferString(buf,"|                                                                                                   |\n");
	writeBufferString(buf,"|                                                                                                   |\n");
	writeBufferString(buf,"|                                                                                                   |\n");
	writeBufferString(buf,"|                                                                                                   |\n");
	writeBufferString(buf,"|                                                                                                   |\n");
	writeBufferString(buf,"|                                                                                                   |\n");
	writeBufferString(buf,"|                                                                                                   |\n");
	writeBufferString(buf,"|                                                                                                   |\n");
	writeBufferString(buf,"|                                                                                                   |\n");
	writeBufferString(buf,"|                                                                                                   |\n");
	writeBufferString(buf,"|                                                                                                   |\n");
	writeBufferString(buf,"|                                                                                                   |\n");
	writeBufferString(buf,"|                                                                                                   |\n");
	writeBufferString(buf,"|                                                                                                   |\n");
	writeBufferString(buf,"|                                                                                                   |\n");
	writeBufferString(buf,"|                                                                                                   |\n");
	writeBufferString(buf,"|                                                                                                   |\n");
	writeBufferString(buf,"|                                                                                                   |\n");
	writeBufferString(buf,"|                                                                                                   |\n");
	writeBufferString(buf,"|                                                                                                   |\n");
	writeBufferString(buf,"-----------------------------------------------------------------------------------------------------\n");
}

void print_task(Buffer *buf, char *task_name, int task_index, int task_state) {
	int i, j;
	char *buffer = buf->line + buf->w_cursor;
	j = BUFFER_SIZE - buf->w_cursor;
	if (BUFFER_SIZE - hasBuffer(buf) < j)j = BUFFER_SIZE - hasBuffer(buf);
	i = writeFormatString(buffer, j, "\0337\033[%d;3H%s: %d\0338", task_index + 31, task_name, task_state);
	buf->w_cursor = (buf->w_cursor + i) % BUFFER_SIZE;
}

void print_time(Buffer *buf, int micro_second_elapsed) {
	char time[10];
	writeFormatString(time, 10, "%d.%d", micro_second_elapsed/100, micro_second_elapsed%100);
	printAt(2,82,time,buf);
}

void Print_Time( int time ) {
	char buffer[60];
	int i = writeFormatString(buffer, 60, "\0337\033[2;82H%d.%d\0338", time/100, time%100);
	Putline( TERM_PORT, buffer, i );
}

void print_train_number(Buffer *buf, int train_index, int train_number) {
	int i;
	char buffer[50];
	i = writeFormatString(buffer, 50, "\0337\033[%d;80H%2d\0338", train_index*4 + 6, train_number);
	Putline( TERM_PORT, buffer, i );
}

void print_train_speed(Buffer *buf, int train_index, int train_speed) {
	int i;
	char buffer[50];
	i = writeFormatString(buffer, 50, "\0337\033[%d;87H%2d\0338", train_index*4 + 6, train_speed & 0x0000FFFF);
	Putline( TERM_PORT, buffer, i );
}

void print_train_segment(int train_index, int last_sensor_number) {
	int i;
	char buffer[100];
	i = writeFormatString(buffer, 100, "\0337\033[%d;94H%3d\0338", train_index*4 + 6, last_sensor_number);
	Putline( TERM_PORT, buffer, i );
}

void print_train_offset(Buffer *buf, int train_index, int velocity, int node_id, int offset) {
	int i;
	char buffer[100];
	i = writeFormatString(buffer, 100, "\0337\033[%d;79H%4d\033[%d;87H%3d\033[%d;94H%3d\0338", train_index*4 + 6 + 1, velocity, train_index*4 + 6 + 1, offset, train_index*4 + 6 + 1, node_id);
	Putline( TERM_PORT, buffer, i );
}

void print_train_stop(Buffer *buf, int train_index, int stop_node_id, int stop_offset) {
	int i;
	char buffer[100];
	i = writeFormatString(buffer, 100, "\0337\033[%d;87H%3d\033[%d;94H%3d\0338", train_index*4 + 6 + 2, stop_offset, train_index*4 + 6 + 2, stop_node_id);
	Putline( TERM_PORT, buffer, i );
}

void print_train_stack(Buffer *buf, int train_index, int stack_size) {
	int i;
	char buffer[100];
	i = writeFormatString(buffer, 100, "\0337\033[%d;80H%3d\0338", train_index*4 + 6 + 2, stack_size);
	Putline( TERM_PORT, buffer, i );
}

void print_route(Buffer *buf, int train_number, int start_node_id, int stop_node_id) {
	char line[32];
	writeFormatString(line, 32, "t%d @ N%d -> N%d  \0", train_number, start_node_id, stop_node_id);
	printAt(8,80,line,buf);
}

void print_reservations( int * reservations ) {
	int i, j;
	int sensor[6];
	char output_buffer[100];

	j = 0;
	for ( i = 0 ; i < TRACK_MAX && j < 6 ; i++ ) {
		if (!(reservations[i]==-1)) {
			sensor[j++] = i;
		}
	}
	while (j<6) {
		sensor[j++] = 0;
	}

	i = writeFormatString( output_buffer, 100, "\0337\033[20;3H %d, %d, %d, %d, %d, %d\033[K\033[20;101H|\0338", sensor[0], sensor[1], sensor[2], sensor[3], sensor[4], sensor[5]);

	Putline( TERM_PORT, output_buffer, i );
}

void print_switch(Buffer *buf, int switch_number, char switch_direction) {
	int r,c;

	if (switch_number>0&&switch_number<10) {
		r = 5 + switch_number;
		c = 21;
	} else if (switch_number>9&&switch_number<19) {
		r = switch_number - 4;
		c = 46;
	} else if (switch_number==153) {
		r = 15;
		c = 21;
	} else if (switch_number==154) {
		r = 16;
		c = 21;
	} else if (switch_number==155) {
		r = 15;
		c = 46;
	} else if (switch_number==156) {
		r = 16;
		c = 46;
	} else {
		return;
	}

	char line[2];
	line[0] = switch_direction;
	line[1] = '\0';
	printAt(r,c,line,buf);
}

int print_sensors(Buffer *buf, int sensor_buffer_count, char *sensor_buffer) {
	int i = 0;
	while (i<sensor_module_number) {
		int sensor_status = sensor_buffer[2*i] | (sensor_buffer[2*i+1] << 8);
		int last_status = sensor_map[i*4] | (sensor_map[i*4 + 1] << 8);
		sensor_map[i*4] = sensor_buffer[2*i];
		sensor_map[i*4 + 1] = sensor_buffer[2*i+1];

		int module_number = i;
		int sensor_number = 0;
		while (sensor_number<16) {
			if (sensor_status%2==1 && last_status%2==0)break;

			sensor_status /= 2;
			last_status /= 2;
			sensor_number++;
		}
		if (sensor_number==16) {

		} else {
			if (sensor_number<8) {
				sensor_number = 8 - sensor_number;
			} else {
				sensor_number -= 8;
				sensor_number = 8 - sensor_number;
				sensor_number += 8;
			}

			char *format = "ModA, Sen 00\0";
			char line[13];
			int l = 0;
			while (l<13) {
				line[l] = format[l];
				l++;
			}

			line[3] = module_number+'A';

			if (sensor_number/10) {
				line[10] = sensor_number/10+'0';
			}
			if (sensor_number%10) {
				line[11] = sensor_number%10+'0';
			}

			printAt(6+sensor_buffer_count%SENSOR_BUFFER_SIZE,55,line,buf);
			sensor_buffer_count++;
		}

		i++;
	}

	return sensor_buffer_count;
}

void Print_Sensor_To_UI( int i, int j, int print_counter ) {
	char buffer[32];
	if (j<10) {
		i = writeFormatString(buffer, 32, "\0337\033[%d;63H%c0%d\0338", 6+print_counter%SENSOR_BUFFER_SIZE, i + 'A', j);
	} else {
		i = writeFormatString(buffer, 32, "\0337\033[%d;63H%c%d\0338", 6+print_counter%SENSOR_BUFFER_SIZE, i + 'A', j);
	}

	Putline( TERM_PORT, buffer, i );
}

void Print_Sensors( char *new_sensor_map ) {
	int i, j;
	if (SENSOR_COUNT == 0) {
		for ( i = 0; i < sensor_module_number*2 ; i++ ) {
			INITIAL_SENSOR_MAP[i] = new_sensor_map[i];
		}
	}

	for ( i = 0; i < sensor_module_number*2 ; i++ ) {
		if ( new_sensor_map[i] != 0 && new_sensor_map[i] != LAST_SENSOR_MAP[i] ) {
			for ( j = 0; j < 8 ; j++ ) {
				if ( (1<<j & new_sensor_map[i]) > 0 &&  (1<<j & LAST_SENSOR_MAP[i]) == 0 ) {
					Print_Sensor_To_UI( i/2, i%2*8+8-j, SENSOR_COUNT );
					SENSOR_COUNT++;
				}
			}
		}
	}

	for ( i = 0; i < sensor_module_number*2 ; i++ ) {
		LAST_SENSOR_MAP[i] = new_sensor_map[i];
	}
}

void print_debug_string(char *debug_info) {
	char buffer[256];
	int i = 0, j = DEBUG_COUNT;
	i += writeFormatString(buffer+i, 256, "\0337\033[%d;3H%6d: \033[K", 20 + j % 10, DEBUG_COUNT);
	i += writeFormatString(buffer+i, 90, "%s", debug_info);
	i += writeFormatString(buffer+i, 256 - i, "\033[%d;101H|\0338", 20 + j % 10);
	Putline( TERM_PORT, buffer, i );
	DEBUG_COUNT++;
}
