#ifdef STRING_UTIL_H
#else
#define STRING_UTIL_H

#define MAX_LOCATION_LENGTH 	13
#define BUFFER_SIZE		4096

typedef struct {
	char line[BUFFER_SIZE];
	//Set to 0
	int w_cursor;
	//Set to BUFFER_SIZE - 1
	int r_cursor;
} Buffer;

void writeBuffer(Buffer *buf, char ch);

void writeBufferString(Buffer *buf, char *line);

void writeBufferNumber(Buffer *buf, int i);

int hasBuffer(Buffer *buf);

char readBuffer(Buffer *buf);

void moveBuffer(Buffer *buf, int movement);

int flushBuffer(Buffer *buf, char *line);

void clearBuffer(Buffer *buf);

int printAt( int r, int c, char *line, Buffer *buf );

int printNumberAt( int r, int c, int num, Buffer *buf );

int readNumber(char *line, int *num);

int readDigit(char ch);

int badCommand();

int writeChar(char *line, int limit, char ch);
int writeString(char *line, int limit, char *part);
int writeDigits(char *line, int limit, unsigned int num);
int writeInt(char *line, int limit, int num);
int writeUInt(char *line, int limit, unsigned int num);
int writeFormatString(char *line, int limit, char *format, ...);
#endif

