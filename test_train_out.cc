#include <fcntl.h>
#include <sys/stat.h>
#include <fstream>
using namespace std;

/*
int main() {
    //mkfifo( "train_out", 0777);
    int fd = open ("./train_out", O_WRONLY | O_NONBLOCK);
    printf("FD: %d\n", fd);
    write(fd, "WTF!!!", 6);
    //close(fd);

    return 0;
}
*/

int main() {
    fstream out;
    out.open("train_out", fstream::out);
    
    out << "WTF!!!!" << endl;
}
