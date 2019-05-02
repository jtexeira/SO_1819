#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdio.h>
#include "../manager/article.h"
#include "../utils/utils.h"
#include <stdlib.h>
#include <string.h>

#define CACHESIZE 50
#define BUFFSIZE 4096

typedef struct stock {
    int codigo;
    ssize_t stock;
} Stock;

typedef struct cache {
    int codigo;
    double preco;
    size_t used;
}Cache;

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

char* articleInfo(int rd, int wr, int id, int* size) {
    int stock = open("stocks", O_RDONLY);
    struct stat info;
    fstat(stock, &info);
    if(((id * sizeof(Stock)) + sizeof(time_t)) >= info.st_size) return NULL;
    char* buff = malloc(BUFFSIZE);
    Stock s;
    pread(stock, &s, sizeof(Stock), id * sizeof(Stock) + sizeof(time_t));
    double preco;
    char miniBuff[BUFFSIZE];
    *size = sprintf(miniBuff, "%d\n", id);
    write(wr, miniBuff, *size);
    read(rd, &preco, sizeof(double));
    *size = sprintf(buff, "%zu %.2f\n", s.stock, preco);
    close(stock);
    return buff;
} 

ssize_t updateStock(int rd, int wr, int id, ssize_t new_stock) {
    int stock = open("stocks", O_RDWR);
    int vendas = open("vendas", O_WRONLY | O_APPEND | O_CREAT, 0700);
    Stock s;
    struct stat info;
    fstat(stock, &info);
    if(((id * sizeof(Stock)) + sizeof(time_t)) >= info.st_size) return -1;
    pread(stock, &s, sizeof(Stock), id * sizeof(Stock) + sizeof(time_t));
    s.stock += new_stock;
    pwrite(stock, &s, sizeof(Stock), id * sizeof(Stock) + sizeof(time_t));
    if(new_stock < 0) {
        char buff[BUFFSIZE];
        double preco;
        char miniBuff[BUFFSIZE];
        int size;
        size = sprintf(miniBuff, "%d\n", id);
        write(wr, miniBuff, size);
        read(rd, &preco, sizeof(double));
        size = sprintf(buff, "%d %zu %.2f\n", id, -new_stock, -new_stock * preco);
        write(vendas, buff, size);
    }
    close(stock);
    close(vendas);
    return s.stock;
}

int cacheComp(const void* a, const void* b) {
    return ((Cache*) b)->used - ((Cache*) a)->used;
}

int main() {
    int idk[2];
    int prices[2];
    pipe(idk);
    pipe(prices);
    if(!fork()) {
        for(;;) {
            int article = open("/tmp/article.pipe", O_RDONLY);
            int read;
            char buff[BUFFSIZE];
            while((read = readln(article, buff, BUFFSIZE))) {
                int stocks = open("stocks", O_WRONLY | O_APPEND);
                Stock s = {0, 0};
                switch(buff[0]) {
                    case 'i':
                        write(stocks, &s, sizeof(Stock));
                        break;
                    case 'p':
                        break;
                }
            }
        }
        return 0;
    }
    if(!fork())
    {
        Cache cache[CACHESIZE] = {0};
        char buff[BUFFSIZE];
        size_t times = 0;
        while(readln(idk[0], buff, BUFFSIZE)) {
            if(buff[0] <= '9' && buff[0] >= '0') {
                int id = atoi(buff);
                int i;
                for(i = 0; i < CACHESIZE && i < times && cache[i].codigo != id; i++);
                if(i == times && times < CACHESIZE) {
                    cache[times] = (Cache) {.codigo = id, 
                        .preco = getArticlePrice(id), 
                        .used = times};
                    printf("%f\n", getArticlePrice(id));
                    write(prices[1], &(cache[times].preco), sizeof(double));
                    times++;
                }
                else {
                    if(i == CACHESIZE) {
                        cache[CACHESIZE-1] = (Cache) {.codigo = id, 
                            .preco = getArticlePrice(id), 
                            .used = times};
                        times++;
                        write(prices[1], &(cache[CACHESIZE-1].preco), sizeof(double));
                    }
                    else {
                        write(prices[1], &(cache[i].preco), sizeof(double));
                        cache[i].used = times++;
                    }
                    qsort(cache, CACHESIZE, sizeof(Cache), cacheComp);
                }
            }
        }
        return 0;
    }
    if(!fork())
    {
        initF();
        char buff[BUFFSIZE];
        int id, size;
        size = sprintf(buff, "%d\n", getpid());
        write(1, buff, size);
        mkfifo("/tmp/rd", 0700);
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
                }
                else {
                    ssize_t quant = atoi(abc);
                    int stock = updateStock(prices[0], idk[1], id, quant);
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
