#include <StringUtil.h>
#include <iostream>
#include <string>
using namespace std;
int main() {
	cout << "WTF" << endl;
	char line[100];
	int i = writeFormatString(line,100,"Number 1024: %d.%d, Number -2048: %d, SubString 'abcde': %15s...", 1024/100, 1024%100, -2048, "abcde");
	cout << line << endl;
	cout << "EOL -> " << (int)line[i] << endl;
	line[i] = 0;
	
	i = writeFormatString(line,100,"  \0", 1024/100, 1024%100, -2048, "abcde");
	cout << "EOL@" << i << " -> " << (int)line[i] << endl;
	
	cout << "EOL -> " << (int)'\0' << endl;
	
	line[i] = 0;
	cout << line << endl;
	
	cout << "Parsing Test..." << endl;
	int m,n;
	char *str = "gt 39 A7";
	i = readNumber(str+2, &n);
	readNumber(str+i+3, &m);
	cout << n << " ends at " << i+2 << " also " << m << endl;
	
	return 0;
}
