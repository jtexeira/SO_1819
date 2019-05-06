#include <fcntl.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "article.h"
#include "../utils/utils.h"
#include <errno.h>
#include <time.h>

static int addArticle(char* name, double price) {
    int strings, artigos, id;
    strings = open("strings", O_WRONLY | O_APPEND | O_CREAT, 00600);
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
        artigos = open("artigos", O_WRONLY | O_APPEND | O_CREAT, 00600);
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
    close(artigos);
    close(strings);
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
    close(artigos);
    return 0;
}

static void strCleaner() {
    int artigos = open("artigos", O_RDWR);
    struct stat b;
    fstat(artigos, &b);
    int id = (b.st_size - sizeof(time_t)) / sizeof(Artigo);
    Artigo all[id];
    pread(artigos, all, sizeof(Artigo) * id, SPOT(0)); 
    int strings = open("strings", O_RDONLY);
    fstat(strings, &b);
    char string[b.st_size];
    read(strings, string, b.st_size);
    close(strings);
    strings = open("strings", O_WRONLY | O_APPEND | O_TRUNC);
    for(int i = 0; i < id; i++) {
        fstat(strings, &b);
        write(strings, &string[all[i].name], strlen(&string[all[i].name]) + 1);
        all[id].name = b.st_size;
    }
    pwrite(artigos, all, sizeof(all), sizeof(time_t));
}

int runAg() {
    if(!fork()) {
        int vendas = open("vendas", O_RDONLY);
        dup2(vendas, 0);
        close(vendas);
        time_t timeAg = time(NULL);
        struct tm tm = *localtime(&timeAg);
        char buff[BUFFSIZE];
        sprintf(buff, "%d-%d-%dT%d:%d:%d",
                tm.tm_year + 1900, 
                tm.tm_mon + 1, 
                tm.tm_mday, 
                tm.tm_hour, 
                tm.tm_min, 
                tm.tm_sec);
        int agFile = open(buff, O_WRONLY | O_CREAT, 00600);
        dup2(agFile, 1);
        close(agFile);
        execl("./ag","./ag", NULL);
        return 0;
    }
    return 0;
}

int main() {
    char buff[BUFFSIZE];
    char cpy[BUFFSIZE];
    int read;
    char* str[3];
    int i;
    int strings, articles; 
    strings = articles = 0;
    mkfifo("/tmp/article.pipe", 00600);
    while((read = readln(0, buff, BUFFSIZE))) {
        int pipe = open("/tmp/article.pipe", O_WRONLY | O_NONBLOCK);
        switch(buff[0]) {
            case 'i':
                str[0] = strtok(buff, " ");
                for(i = 0; i < 2 && str[i]; i++)
                    str[i+1] = strtok(NULL, " ");
                if(i != 2 || !str[2]) break;
                while(write(pipe, buff, read) == EAGAIN);
                char* name = str[1];
                double price = atof(str[2]);
                int id = addArticle(name, price);
                sprintf(buff, "%d\n", id);
                write(1, buff, strlen(buff) + 1);
                articles++;
                strings++;
                break;
            case 'n':
                str[0] = strtok(buff, " ");
                str[1] = strtok(NULL, " ");
                str[2] = strtok(NULL, " ");
                if(!str[1] || !str[2]) break;
                id = atoi(str[1]);
                updateName(id, str[2]);
                strings++;
                if(articles/strings < 0.8) {
                    strCleaner();
                    strings = articles;
                }
                break;
            case 'p':
                strncpy(cpy, buff, BUFFSIZE);
                str[0] = strtok(buff, " ");
                str[1] = strtok(NULL, " ");
                str[2] = strtok(NULL, " ");
                if(!str[1] || !str[2]) break;
                id = atoi(str[1]);
                price = atof(str[2]);
                updateArticle(id, price);
                while(write(pipe, cpy, read) == EAGAIN);
                break;
            case 'a':
                runAg();
                break;
        }
        //close(pipe);
    }
    return 0;
}
