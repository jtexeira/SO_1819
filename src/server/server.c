#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdio.h>
#include "../manager/article.h"
#include "../utils/utils.h"
#include <stdlib.h>
#include <string.h>
#include <sys/inotify.h>

#define CACHESIZE 50
#define SIZEID(id) (off_t)((id * sizeof(Stock)) + sizeof(time_t))

typedef struct stock {
    int codigo;
    size_t stock;
} Stock;

typedef struct cache {
    int codigo;
    double preco;
    size_t used;
} Cache;

void initF() {
    struct stat a;
    int i;
    Stock new;
    int artigos = open("artigos", O_RDONLY);
    fstat(artigos, &a);
    int nArtigos = (a.st_size - sizeof(time_t)) / sizeof(Artigo);
    time_t articleCreate;
    read(artigos, &articleCreate, sizeof(time_t));
    if(stat("stocks", &a) == -1) {
        int stock = open("stocks", O_CREAT | O_WRONLY | O_APPEND, 0600);
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
        int nStock = (a.st_size - sizeof(time_t)) / sizeof(Stock);
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
        else if(nStock < nArtigos) { 
            stock = open("stocks", O_WRONLY | O_APPEND);
            for(i = nStock; i < nArtigos; i++) {
                new.codigo = i;
                new.stock = 0;
                write(stock, &new, sizeof(Stock));
            }
            close(stock);
        }
    }
    close(artigos);
}

char* articleInfo(int rd, int wr, int id, int* size) {
    int stock = open("stocks", O_RDONLY);
    struct stat info;
    fstat(stock, &info);
    if(SIZEID(id) >= info.st_size) return NULL;
    char* buff = malloc(BUFFSIZE);
    Stock s;
    pread(stock, &s, sizeof(Stock), id * sizeof(Stock) + sizeof(time_t));
    double preco;
    char miniBuff[BUFFSIZE];
    *size = sprintf(miniBuff, "%d\n", id);
    if(write(wr, miniBuff, *size) == EAGAIN) 
        preco = getArticlePrice(id);
    else
        read(rd, &preco, sizeof(double));
    *size = sprintf(buff, "%zu %.2f\n", s.stock, preco);
    close(stock);
    return buff;
} 

ssize_t updateStock(int rd, int wr, int id, ssize_t new_stock) {
    int stock = open("stocks", O_RDWR);
    int vendas = open("vendas", O_WRONLY | O_APPEND | O_CREAT, 00600);
    Stock s;
    struct stat info;
    fstat(stock, &info);
    if(SIZEID(id) >= info.st_size) return -1;
    pread(stock, &s, sizeof(Stock), id * sizeof(Stock) + sizeof(time_t));
    if(new_stock + (ssize_t)s.stock >= 0) {
        s.stock += new_stock;
        pwrite(stock, &s, sizeof(Stock), id * sizeof(Stock) + sizeof(time_t));
        if(new_stock < 0) {
            char buff[BUFFSIZE];
            double preco;
            char miniBuff[BUFFSIZE];
            int size;
            size = sprintf(miniBuff, "%d\n", id);
            if(write(wr, miniBuff, size) == EAGAIN) 
                preco = getArticlePrice(id);
            else
                read(rd, &preco, sizeof(double));
            memset(buff, 0, sizeof(buff));
            size = sprintf(buff, "%d %zu %.2f\n", id, -new_stock, -new_stock * preco);
            write(vendas, buff, size);
        }
    }
    close(stock);
    close(vendas);
    return s.stock;
}

int cacheComp(const void* a, const void* b) {
    return ((Cache*) b)->used - ((Cache*) a)->used;
}

void articleSync(int wr) {
    if(!fork()) {
        mkfifo("/tmp/article.pipe", 00600);
        int article;
        for(;;) {
            article = open("/tmp/article.pipe", O_RDONLY);
            int read;
            char buff[BUFFSIZE];
            int newFile = 0;
            while((read = readln(article, buff, BUFFSIZE))) {
                switch(buff[0]) {
                    case 'i':
                        newFile = 1;
                        break;
                    case 'p':
                        while(write(wr, buff, read) == EAGAIN);
                        break;
                }
            }
            if(newFile) 
                initF();
            close(article);
        }
        rename("/dev/null", "/tmp/article.pipe");
        _exit(0);
    }
}

void articleCache(int rd, int wr) {
    if(!fork())
    {
        Cache cache[CACHESIZE] = {0};
        char buff[BUFFSIZE];
        size_t times = 0;
        for(;;)
            while(readln(rd, buff, BUFFSIZE)) {
                if(buff[0] <= '9' && buff[0] >= '0') {
                    int id = atoi(buff);
                    size_t i;
                    for(i = 0; i < CACHESIZE && i < times && cache[i].codigo != id; i++);
                    if(i == times && times < CACHESIZE) {
                        cache[times] = (Cache) {.codigo = id, 
                            .preco = getArticlePrice(id), 
                            .used = times};
                        write(wr, &(cache[times].preco), sizeof(double));
                        times++;
                    }
                    else {
                        if(i == CACHESIZE) {
                            cache[CACHESIZE-1] = (Cache) {.codigo = id, 
                                .preco = getArticlePrice(id), 
                                .used = times};
                            times++;
                            write(wr, &(cache[CACHESIZE-1].preco), sizeof(double));
                        }
                        else {
                            write(wr, &(cache[i].preco), sizeof(double));
                            cache[i].used = times++;
                        }
                        qsort(cache, CACHESIZE, sizeof(Cache), cacheComp);
                    }
                }
                else if(buff[0] == 'p') {
                    char* str[3];
                    str[0] = strtok(buff, " ");
                    str[1] = strtok(NULL, " ");
                    str[2] = strtok(NULL, " ");
                    int id = atoi(str[1]);
                    size_t i;
                    double price = atof(str[2]);
                    for(i = 0; i < CACHESIZE && i < times && cache[i].codigo != id; i++);
                    if(cache[i].codigo == id)
                        cache[i].preco = price;
                }
            }
        _exit(0);
    }
}

void server(int idk[2], int prices[2]) {
    if(!fork())
    {
        initF();
        char buff[BUFFSIZE];
        int id, size;
        mkfifo("/tmp/rd", 0600);
        for(;;) {
            int rd = open("/tmp/rd", O_RDONLY);
            while(readln(rd, buff, BUFFSIZE)) {
                char* pid = strtok(buff, " ");
                char path[BUFFSIZE];
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
                    char* info = articleInfo(prices[0], idk[1], id, &size);
                    if(!info)
                        write(wr, "\b\n", 2); 
                    else 
                        write(wr, info, size + 1);
                    free(info);
                }
                else {
                    if(abc[0] >= 0 && abc[0] <= 9)
                        write(wr, "\b\n", 2);
                    else { 
                        ssize_t quant = atoll(abc);
                        ssize_t stock = updateStock(prices[0], idk[1], id, quant);
                        if(stock == -1) 
                            write(wr, "\b\n", 2);
                        else {
                            size = sprintf(buff, "%ld\n", stock);
                            write(wr, buff, size); 
                        }
                    }
                }
                close(wr);
                memset(buff, 0, sizeof(buff));
            }
            close(rd);
        }
        unlink("/tmp/rd");
        _exit(0);
    }
}

int main() {
    int idk[2];
    int prices[2];
    pipe(idk);
    pipe(prices);
    articleSync(idk[1]); 
    articleCache(idk[0], prices[1]);
    server(idk, prices);
    return 0;
}
