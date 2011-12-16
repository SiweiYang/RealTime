#ifdef IO_H
#else
#define IO_H

#define TRAIN_PORT                      0
#define TERM_PORT                       1

#define IO_CTS              2
#define IO_GET				3
#define IO_GETMULT			4
#define IO_PUT				5
#define IO_PUTLINE			6
#define IO_DONE 			7

#define IO_REPLY_LENGTH     4
#define IO_BUFFER_LENGTH 		        8192
#define TRAIN_INPUT_SIZE    32
void initialize_io();

void cts_server();
void uart1_io();
void uart2_io();

int Getc( int channel );
int Getline( int channel, char *line, int length );
int Putc( int channel, char ch );
int Putline( int channel, char *ch, int line_length );
int getLine(char *buff);
#endif
