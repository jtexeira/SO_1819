#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "manager.h"

#define OFFSET(x) x/sizeof(Artigo)
#define SPOT(x) x * sizeof(Artigo)

typedef struct artigo {
    size_t name;
    double price;
} Artigo;

static int addArticle(char* name, double price) {
    int strings, artigos, id;
    strings = open("STRINGS", O_WRONLY | O_APPEND | O_CREAT, 00700);
    artigos = open("ARTIGOS", O_WRONLY | O_APPEND | O_CREAT, 00700);
    struct stat a;
    fstat(strings, &a);
    write(strings, name, strlen(name) + 1);
    close(strings);
    Artigo b = {a.st_size, price};
    fstat(artigos, &a);
    write(artigos, &b, sizeof(Artigo));
    close(artigos);
    id = a.st_size / sizeof(Artigo);
    return id;
}

static int updateName(int id, char* new_name) {
    int strings, artigos;
    Artigo a;
    struct stat b;
    strings = open("STRINGS", O_WRONLY | O_APPEND);
    artigos = open("ARTIGOS", O_RDWR);
    fstat(artigos, &b);
    if(SPOT(id) > b.st_size) return -1;
    pread(artigos, &a, sizeof(Artigo), id * sizeof(Artigo));
    fstat(strings, &b);
    a.name = b.st_size;
    strtok(new_name, "\n");
    pwrite(artigos, &a, sizeof(Artigo), id * sizeof(Artigo));
    write(strings, new_name, strlen(new_name) + 1);
    return 0;
}

static int updateArticle(int id, double new_price) {
    int artigos = open("ARTIGOS", O_RDWR);
    struct stat b;
    fstat(artigos, &b);
    if(SPOT(id) > b.st_size) return -1;
    Artigo a;
    pread(artigos, &a, sizeof(Artigo), id * sizeof(Artigo));
    a.price = new_price;
    pwrite(artigos, &a, sizeof(Artigo), id * sizeof(Artigo));
    return 0;
}

char* getArticleName(int id) {
    char* buff = malloc(100);
    Artigo a;
    int artigos = open("ARTIGOS", O_RDONLY);
    struct stat b;
    fstat(artigos, &b);
    if(SPOT(id) > b.st_size) return NULL;
    pread(artigos, &a, sizeof(Artigo), id * sizeof(Artigo));
    close(artigos);
    int strings = open("STRINGS", O_RDONLY);
    pread(strings, buff, 100, a.name);
    return buff;
}

double getArticlePrice(int id) {
    Artigo a;
    int artigos = open("ARTIGOS", O_RDONLY);
    struct stat b;
    fstat(artigos, &b);
    if(SPOT(id) > b.st_size) return -1;
    pread(artigos, &a, sizeof(Artigo), id * sizeof(Artigo));
    close(artigos);
    return a.price;
}

static ssize_t readln(int fildes, void *buff, size_t nbyte) {
    size_t i;
    ssize_t r;
    for(i = 0; (r = read(fildes, buff+i,1)) > 0 && i < nbyte && *(char*)(buff+i) != '\n'; i++);
    if(*(char*)(buff+i) == '\n' && r) i++;
    return i;
}

int main() {
    char buff[200];
    char tmp[200];
    while(readln(0, buff, 200))
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
            case 'c':
                strtok(buff, " ");
                id = atoi(strtok(NULL, " "));
                price = getArticlePrice(id);
                id = sprintf(tmp, "%.2f\n", price);
                write(1, tmp, id);
                break;
        }
    return 0;
}

