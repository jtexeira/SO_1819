#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdio.h>
#include "../manager/manager.h"
#include <stdlib.h>

typedef struct stock {
    int codigo;
    size_t stock;
} Stock;

int initF() {
    struct stat a;
    int artigos = open("../manager/ARTIGOS", O_RDONLY);
    int stock = open("STOCKS", O_CREAT | O_WRONLY, 0700);
    fstat(artigos, &a);
    int nArtigos = a.st_size / sizeof(Artigo);
    for(int i = 0; i < nArtigos; i++) {
        Stock new = {i, 0};
        pwrite(stock, &new, sizeof(new), i * sizeof(new));
    }
    close(artigos);
    return stock;
}

char* articleInfo(int id) {
    int stock = open("STOCKS", O_RDONLY);
    struct stat info;
    fstat(stock, &info);
    if(id > info.st_size / sizeof(Stock)) return -1;
    char* buff = malloc(100);
    Stock s;
    pread(stock, &s, sizeof(Stock), id * sizeof(stock));
    sprintf(buff, "%zu %.2f\n", s.stock, getArticlePrice(id));
    return buff;
} 

int updateStock(int id, int new_stock) {
    int stock = open("STOCKS", O_RDWR);
    int vendas = open("VENDAS", O_WRONLY | O_APPEND | O_CREAT, 0700);
    Stock s;
    struct stat info;
    fstat(stock, &info);
    if(id > info.st_size / sizeof(Stock)) return -1;
    pread(stock, &s, sizeof(Stock), id * sizeof(stock));
    s.stock += new_stock;
    pwrite(stock, &s, sizeof(Stock), id * sizeof(stock));
    if(new_stock < 0) {
        char buff[200];
        double price = getArticlePrice(id);
        int read = sprintf(buff, "%d %d %.2f\n", id, -new_stock, -new_stock * price);
        write(vendas, buff, read);
    }
    return s.stock;
}

int main() {
    int stock = open("STOCKS", O_RDONLY);
    if(stock == ENOENT) {
        close(stock);
        stock = initF();
        close(stock);
    }
}
