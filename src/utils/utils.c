#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#define BUF_SIZE 100

char* readln(int fildes, int* r) {
    static char local_buf[BUF_SIZE];
    static int i = 0;
    static int rr = 1;
    
    int ind = 0;
    char* buf = malloc(BUF_SIZE);
    
    if (i == 0 || rr == i)
       rr = read(fildes, local_buf, BUF_SIZE);

    while(rr != i && local_buf[i] != '\n') {
        
        for(    ; i < rr && local_buf[i] != '\n'; i++)
            buf[ind++] = local_buf[i];
        if(rr != BUF_SIZE && rr == ind)
            break;
        
        if(i == BUF_SIZE && buf[ind] != '\n') {
            i = 0;
            rr = read(fildes, local_buf, BUF_SIZE);
            buf = realloc(buf, ind + BUF_SIZE);
        }
    }

    i++;
    *r = ind;
    return buf;
}

