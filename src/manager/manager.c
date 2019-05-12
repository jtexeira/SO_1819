#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "article.h"
#include "../utils/utils.h"
#include <errno.h>
#include <time.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>

#ifdef __APPLE__
    #define st_mtim st_mtimespec
#endif

static int addArticle(char* name, double price) {
    int strings, artigos, id;
    strings = open("strings", O_WRONLY | O_APPEND | O_CREAT, 00666);
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
    int artigos = open("artigos", O_RDONLY);
    rename("strings", "ree");
    int strings = open("ree", O_RDONLY);
    char buff[BUFFSIZE];
    int newStrings = open("strings", O_WRONLY | O_CREAT | O_APPEND | O_TRUNC, 00666);
    struct stat b;
    fstat(artigos, &b);
    int id;
    Artigo a;
    for(id = 0; SPOT(id) < b.st_size; id++) {
        pread(artigos, &a, sizeof(Artigo), SPOT(id));
        pread(strings, buff, BUFFSIZE, a.name);
        updateName(id, buff);
    }
    unlink("ree");
    close(artigos);
    close(newStrings);
    close(strings);
}

static int runAg() {
    if(!fork()) {
        int vendas = open("vendas", O_RDONLY);
        struct stat a;
        fstat(vendas, &a);
        char* vendasS = malloc(a.st_size);
        read(vendas, vendasS, a.st_size);
        close(vendas);
        int pipes[2];
        pipe(pipes);
        fcntl(pipes[1], F_SETFL, O_NONBLOCK);
        size_t newI = 3000;
        for(ssize_t i = 0; i < a.st_size;){
            while(vendasS[newI++] != '\n');
            int ree[2];
            pipe(ree);
            write(ree[1], vendasS + i, newI - i);
            i = newI;
            newI += 1000;
            if(!fork()) {
                close(ree[1]);
                close(pipes[0]);
                dup2(ree[0], 0);
                close(ree[0]);
                dup2(pipes[1], 1);
                close(pipes[1]);
                execl("./ag","./ag", NULL);
                perror("Small ag went wrong");
                _exit(1);
            }
            close(ree[0]);
            close(ree[1]);
        }
        free(vendasS);
        close(pipes[1]);
        time_t timeAg = time(NULL);
        struct tm tm = *localtime(&timeAg);
        char buff[BUFFSIZE];
        sprintf(buff, "%d-%02d-%02dT%02d:%02d:%02d",
                tm.tm_year + 1900, 
                tm.tm_mon + 1, 
                tm.tm_mday, 
                tm.tm_hour, 
                tm.tm_min, 
                tm.tm_sec);
        int agFile = open(buff, O_WRONLY | O_CREAT, 00600);
        dup2(pipes[0], 0);
        close(pipes[0]);
        dup2(agFile, 1);
        close(agFile);
        execl("./ag","./ag", NULL);
        perror("Small ag went wrong");
        _exit(1);

    }
    _exit(0);
}
    
int main() {
    char buff[BUFFSIZE];
    char cpy[BUFFSIZE];
    int read;
    char* str[3];
    int i;
    size_t strings, articles; 
    struct stat a;
    strings = articles = 0;
    if(!stat("artigos", &a))
        strings = articles = (a.st_size - sizeof(time_t)) / sizeof(Artigo); 
    int pipe = open("/tmp/article.pipe", O_WRONLY | O_NONBLOCK);
    while((read = readln(0, buff, BUFFSIZE))) {
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
                int r = updateName(id, str[2]);
                strings++;
                if(!r && ((double) articles)/strings < 0.8) {
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
    }
    close(pipe);
    unlink("tmp/article.pipe");
    return 0;
}
