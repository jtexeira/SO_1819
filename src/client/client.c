#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "../utils/utils.h"

int main() {
    char buff[130];
    char buffA[130];
    sprintf(buffA, "../pipes/%d.pipe", getpid());
    mkfifo(buffA, 00700);
    int rd = open(buffA, O_RDWR);
    int wr = open("../pipes/rd", O_WRONLY);
    int read;
    while((read = readln(0, buff, 130))) {
        char buffR[150];
        read = sprintf(buffR, "%d.pipe %s", getpid(), buff);
        memset(buff, 0, sizeof(char) * 130);
        while(write(wr, buffR, read) == EAGAIN);
        read = readln(rd, buff, 100);
        write(1, buff, read);
        memset(buff, 0, sizeof(char) * 130);
    }
    sprintf(buff, "../pipes/%d", getpid());
    unlink(buff);
    return 0;
}
