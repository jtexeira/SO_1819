#ifndef ___UTILS_H___
#define ___UTILS_H___

#include <unistd.h>

#define BUFFSIZE 4096

ssize_t readln(int fildes, void *buff, size_t nbyte); 

#endif
