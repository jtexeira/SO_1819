#include <unistd.h>

ssize_t readln(int fildes, void *buff, size_t nbyte) {
    size_t i;
    ssize_t r;
    for(i = 0; (r = read(fildes, buff+i,1)) > 0 && i < nbyte && *(char*)(buff+i) != '\n'; i++);
    if(*(char*)(buff+i) == '\n' && r) i++;
    return i;
}
