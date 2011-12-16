#include <fcntl.h>
#include <sys/stat.h>
#include <fstream>
using namespace std;

/*
int main() {
    //mkfifo( "train_out", 777);
    int fd = open ("train_out", O_WRONLY);
    write(fd, "WTF!!!", 6);
    close(fd);

    return 0;
}
*/

int main() {
    fstream in;
    in.open("train_out", fstream::in);
    
    while (1) {
        char ch;
        in >> ch;
        if (in.eof())break;
        printf("%c", ch);
    }
}
