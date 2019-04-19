#include <unistd.h>
#include <fcntl.h>
#include "../utils/utils.h"

int main() {
    int wr = open("../pipes/rd", O_WRONLY);
    int rd = open("../pipes/wr", O_RDONLY);
    char buff[100];
    int read;
    while(read = readln(0, buff, 100)) {
        read = write(wr, buff, read);
        read = readln(rd, buff, 100);
        write(1, buff, read);
    }
    return 0;
}
