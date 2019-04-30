#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdio.h>
#include "../manager/article.h"
#include "../utils/utils.h"
#include <stdlib.h>
#include <string.h>

typedef struct stock {
    int codigo;
    ssize_t stock;
} Stock;

void initF() {
    struct stat a;
    int i;
    Stock new;
    int artigos = open("artigos", O_RDONLY);
    fstat(artigos, &a);
    int nArtigos = (a.st_size - sizeof(time_t)) / sizeof(Artigo);
    time_t articleCreate;
    read(artigos, &articleCreate, sizeof(time_t));
    if(stat("stocks", &a)) {
        int stock = open("stocks", O_CREAT | O_WRONLY | O_APPEND, 0700);
        write(stock, &articleCreate, sizeof(time_t));
        for(i = 0; i < nArtigos; i++) {
            new.codigo = i;
            new.stock = 0;
            write(stock, &new, sizeof(Stock));
        }
        close(stock);
    }
    else {
        int stock = open("stocks", O_RDONLY);
        time_t stockDate;
        read(stock, &stockDate, sizeof(time_t));
        close(stock);
        if(stockDate != articleCreate) {
            stock = open("stocks", O_WRONLY | O_TRUNC | O_APPEND);
            write(stock, &articleCreate, sizeof(time_t));
            for(i = 0; i < nArtigos; i++) {
                new.codigo = i;
                new.stock = 0;
                write(stock, &new, sizeof(Stock));
            }
            close(stock);
        }
        int nStock = (a.st_size - sizeof(time_t)) / sizeof(Stock);
        if(nStock < nArtigos) { 
            stock = open("stocks", O_WRONLY | O_APPEND);
            for(i = nStock; i < nArtigos; i++) {
                new.codigo = i;
                new.stock = 0;
                write(stock, &new, sizeof(Stock));
            }
        }
        close(artigos);
    }
}

char* articleInfo(int id, int* size) {
    int stock = open("stocks", O_RDONLY);
    struct stat info;
    fstat(stock, &info);
    if(id * sizeof(Stock) >= info.st_size) return NULL;
    char* buff = malloc(100);
    Stock s;
    pread(stock, &s, sizeof(Stock), id * sizeof(Stock) + sizeof(time_t));
    int artigos = open("artigos", O_RDONLY);
    *size = sprintf(buff, "%zu %.2f\n", s.stock, getArticlePrice(artigos, id));
    close(artigos);
    close(stock);
    return buff;
} 

ssize_t updateStock(int id, ssize_t new_stock) {
    int stock = open("stocks", O_RDWR);
    int vendas = open("vendas", O_WRONLY | O_APPEND | O_CREAT, 0700);
    Stock s;
    struct stat info;
    fstat(stock, &info);
    if(id * sizeof(Stock) >= info.st_size) return -1;
    pread(stock, &s, sizeof(Stock), id * sizeof(Stock) + sizeof(time_t));
    s.stock += new_stock;
    pwrite(stock, &s, sizeof(Stock), id * sizeof(Stock) + sizeof(time_t));
    if(new_stock < 0) {
        char buff[200];
        int artigos = open("artigos", O_RDONLY);
        double price = getArticlePrice(artigos, id);
        close(artigos);
        int read = sprintf(buff, "%d %zu %.2f\n", id, -new_stock, -new_stock * price);
        write(vendas, buff, read);
    }
    close(stock);
    close(vendas);
    return s.stock;
}

int main() {
    int idk[2];
    pipe(idk);
    if(!fork()) {
        for(;;) {
            int article = open("/tmp/article.pipe", O_RDONLY);
            int read;
            char buff[100];
            while((read = readln(article, buff, 100)));
        }
    }
    //if(!fork())
    {
        initF();
        char buff[150];
        int id, size;
        size = sprintf(buff, "%d\n", getpid());
        write(1, buff, size);
        mkfifo("/tmp/rd", 0700);
        for(;;) {
            int rd = open("/tmp/rd", O_RDONLY);
            while(readln(rd, buff, 150)) {
                char* pid = strtok(buff, " ");
                char path[100];
                sprintf(path, "/tmp/%s", pid);
                int wr = open(path, O_WRONLY);
                char* cid = strtok(NULL, " ");
                if(cid[0] < '0' || cid[0] > '9') {
                    write(wr, "\b\n", 2);
                    close(wr);
                    continue;
                }
                id = atoi(cid);
                char* abc = strtok(NULL, " ");
                if(!abc) {
                    char* info = articleInfo(id, &size);
                    if(!info)
                        write(wr, "\b\n", 2); 
                    else 
                        write(wr, info, size + 1);
                }
                else {
                    ssize_t quant = atoi(abc);
                    int stock = updateStock(id, quant);
                    size = sprintf(buff, "%d\n", stock);
                    write(wr, buff, size); 
                }
                close(wr);
            }
            close(rd);
        }
        unlink("/tmp/rd");
        return 0;
    }
    return 0;
}
