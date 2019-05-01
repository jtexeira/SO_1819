#include <fcntl.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "article.h"
#include "../utils/utils.h"

static int addArticle(char* name, double price) {
    int strings, artigos, id;
    strings = open("strings", O_WRONLY | O_APPEND | O_CREAT, 00700);
    struct stat a;
    fstat(strings, &a);
    write(strings, name, strlen(name) + 1);
    close(strings);
    Artigo b = {a.st_size, price};
    if(!stat("artigos", &a)) {
        artigos = open("artigos", O_WRONLY | O_APPEND);
        write(artigos, &b, sizeof(Artigo));
        id = (a.st_size - sizeof(time_t)) / sizeof(Artigo);
        close(artigos);
    }
    else {
        artigos = open("artigos", O_WRONLY | O_APPEND | O_CREAT, 00700);
        fstat(artigos, &a);
        write(artigos, &(a.st_mtim.tv_sec), sizeof(time_t));
        write(artigos, &b, sizeof(Artigo));
        close(artigos);
        id = 0;
    }
    return id;
}

static int updateName(int id, char* new_name) {
    int strings, artigos;
    Artigo a;
    struct stat b;
    strings = open("strings", O_WRONLY | O_APPEND);
    artigos = open("artigos", O_RDWR);
    fstat(artigos, &b);
    if(SPOT(id) >= b.st_size) return -1;
    pread(artigos, &a, sizeof(Artigo), SPOT(id));
    fstat(strings, &b);
    a.name = b.st_size;
    strtok(new_name, "\n");
    pwrite(artigos, &a, sizeof(Artigo), SPOT(id));
    write(strings, new_name, strlen(new_name) + 1);
    return 0;
}

static int updateArticle(int id, double new_price) {
    int artigos = open("artigos", O_RDWR);
    struct stat b;
    fstat(artigos, &b);
    if(SPOT(id) >= b.st_size) return -1;
    Artigo a;
    pread(artigos, &a, sizeof(Artigo), SPOT(id));
    a.price = new_price;
    pwrite(artigos, &a, sizeof(Artigo), SPOT(id));
    return 0;
}

int main() {
    char buff[200];
    int read;
    mkfifo("/tmp/article.pipe", 00700);
    while((read = readln(0, buff, 200))) {
        int pipe = open("/tmp/article.pipe", O_WRONLY | O_NONBLOCK);
        write(pipe, buff, read);
        switch(buff[0]) {
            case 'i':
                strtok(buff, " ");
                char* name = strtok(NULL, " ");
                double price = atof(strtok(NULL, " "));
                int id = addArticle(name, price);
                sprintf(buff, "%d\n", id);
                write(1, buff, strlen(buff) + 1);
                break;
            case 'n':
                strtok(buff, " ");
                id = atoi(strtok(NULL, " "));
                name = strtok(NULL, " ");
                updateName(id, name);
                break;
            case 'p':
                strtok(buff, " ");
                id = atoi(strtok(NULL, " "));
                price = atof(strtok(NULL, " "));
                updateArticle(id, price);
                break;
        }
        close(pipe);
    }
    return 0;
}
