#ifdef TEST
#include <stdarg.h>
#else
#include <bwio.h>
#endif
#include <StringUtil.h>


void writeBuffer(Buffer *buf, char ch) {
	buf->line[buf->w_cursor] = ch;
	buf->w_cursor = (buf->w_cursor + 1) % BUFFER_SIZE;
}

void writeBufferString(Buffer *buf, char *line) {
	int i = 0;
	while (line[i]) {
		writeBuffer(buf, line[i]);
		i++;
	}
}

void writeBufferDigits(Buffer *buf, int i) {
	if (i==0) {
	} else {
		int digit = i % 10;
		writeBufferDigits(buf,i/10);
		writeBuffer(buf,digit+'0');
	}
}

void writeBufferNumber(Buffer *buf, int i) {
	if (i==0) {
		writeBuffer(buf,'0');
	} else {
		writeBufferDigits(buf,i);
	}
}

int hasBuffer(Buffer *buf) {
	//if ( ( buf->w_cursor - buf->r_cursor + BUFFER_SIZE) % BUFFER_SIZE == 1 )return 0;
	
	return ( BUFFER_SIZE + buf->w_cursor - buf->r_cursor ) % BUFFER_SIZE - 1;
}

char readBuffer(Buffer *buf) {
	//Should not happen
	//has before read
	
	buf->r_cursor = (buf->r_cursor + 1) % BUFFER_SIZE;
	return buf->line[buf->r_cursor];
}

void moveBuffer(Buffer *buf, int movement) {
	//Should not happen
	//has before read
	while (movement<0)movement += BUFFER_SIZE;
	buf->r_cursor = (buf->r_cursor + movement + BUFFER_SIZE) % BUFFER_SIZE;
}

int flushBuffer(Buffer *buf, char *line) {
	int i = 0;
	while (hasBuffer(buf)) {
		line[i] = readBuffer(buf);
		i++;
	}
	
	return i;
}

void clearBuffer(Buffer *buf) {
	buf->w_cursor = 0;
	buf->r_cursor = BUFFER_SIZE - 1;
}
//Return the length of writing
int printAt( int r, int c, char *line, Buffer *buf ) {
    //Save cursor
    writeBuffer( buf, '\033' );
    writeBuffer( buf, '7' );
    
    //Move cursor over
    writeBuffer( buf, '\033' );
    writeBuffer( buf, '[' );
    writeBufferNumber( buf, r );
    writeBuffer( buf, ';' );
    writeBufferNumber( buf, c );
    writeBuffer( buf, 'H' );
    
    //Write string
    while (*line) {
	writeBuffer( buf, *line );
	line++;
    }
    //bwprintf( TERM_PORT, line );
    
    //Return cursor back
    //Save cursor
    writeBuffer( buf, '\033' );
    writeBuffer( buf, '8' );
    
    return 0;
}

int printNumberAt( int r, int c, int num, Buffer *buf ) {
    //Save cursor    
    writeBuffer( buf, '\033' );
    writeBuffer( buf, '7' );
    
    //Move cursor over
    writeBuffer( buf, '\033' );
    writeBuffer( buf, '[' );
    writeBufferNumber( buf, r );
    writeBuffer( buf, ';' );
    writeBufferNumber( buf, c );
    writeBuffer( buf, 'H' );
    
    //Write string
    writeBufferNumber( buf, num );
    writeBuffer(buf, ' ');
    //bwprintf( TERM_PORT, line );
    
    //Return cursor back
    //Save cursor
    writeBuffer( buf, '\033' );
    writeBuffer( buf, '8' );
    
    return 0;
}


int readNumber(char *line, int *n) {
	int digit = 0, num = 0, i = 0;
	
	while(line[i]==' ')i++;
	
	while (1) {
		num = num*10 + digit;
		
		digit = readDigit(line[i]);
		if (digit==-1)break;
		i++;
	}
	*n = num;
	
	return i;
}

int readDigit(char ch) {

	if (ch >= '0' && ch <= '9')
	{
		return (int)(ch - '0');
	}

	return -1;
}

int badCommand() {
	//bwprintf( TERM_PORT, "Waiting for command:\n\r" );

	return 0;
}

int writeChar(char *line, int limit, char ch) {
	if (limit > 0) {
		*line = ch;
		return 1;
	}
	
	return 0;
}

int writeString(char *line, int limit, char *part) {
	int i = 0;
	while (part[i] && i < limit) {
		line[i] = part[i];
		i++;
	}
	
	return i;
}

int writeInt(char *line, int limit, int num) {
	if (!(limit > 0)) {
		return 0;
	}
	
	if (num == 0) {
		*line = '0';
		return 1;
	}
	
	if (num < 0) {
		*line = '-';
		return 1 + writeDigits(line+1, limit-1, (unsigned int)0-num);
	}
	
	return writeDigits(line, limit, (unsigned int)num);
}

int writeUInt(char *line, int limit, unsigned int num) {
	if (!(limit > 0)) {
		return 0;
	}
	
	if (num == 0) {
		*line = '0';
		return 1;
	}
	
	return writeDigits(line, limit, num);
}

int writeDigits(char *line, int limit, unsigned int num) {
	if (!(limit > 0)) {
		return 0;
	}
	
	if (num == 0) {
		return 0;
	}
	
	int length = writeDigits(line, limit, num/10);
	if (length<limit) {
		line[length] = num % 10 + '0';
	} else {
		return length;
	}
	return 1+length;
}

int writeFormatString(char *line, int limit, char *format, ...) {
	if (limit < 1)return 0;
	
	va_list va;
	va_start(va, format);
	
	char ch;
	int i = 0, j;
	while ((ch=*(format++))) {
		if (ch == '%') {
			format += readNumber(format, &j);
			j += i;
			ch=*(format++);
			switch (ch) {
				case 'c':
					#ifdef TEST
                        i += writeChar(line+i, limit-i, va_arg(va, int));
					#else
                        i += writeChar(line+i, limit-i, va_arg(va, char));
					#endif
					break;
				case 'u':
					i += writeUInt(line+i, limit-i, va_arg(va, unsigned int));
					break;
				case 'd':
					i += writeInt(line+i, limit-i, va_arg(va, int));
					break;
				case 's':
					i += writeString(line+i, limit-i, va_arg(va, char *));
					break;
				case '%':
					i += writeChar(line+i, limit-i, ch);
					break;
				default:
					break;
			}
			while (i<j)line[i++] = ' ';
		} else {
			i += writeChar(line+i, limit-i, ch);
		}
		if (i==limit)break;
	}
	if (i<limit)line[i] = 0;
	va_end(va);
	
	return i;
}