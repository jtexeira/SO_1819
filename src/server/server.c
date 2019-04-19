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
    int stock;
} Stock;

int initF() {
    struct stat a;
    int i;
    Stock new;
    int artigos = open("../manager/artigos", O_RDONLY);
    int stock = open("stocks", O_CREAT | O_WRONLY, 0700);
    fstat(artigos, &a);
    int nArtigos = a.st_size / sizeof(Artigo);
    for(i = 0; i < nArtigos; i++) {
        new.codigo = i;
        new.stock = 0;
        pwrite(stock, &new, sizeof(Stock), i * sizeof(Stock));
    }
    close(artigos);
    return stock;
}

char* articleInfo(int id, int* size) {
    int stock = open("stocks", O_RDONLY);
    struct stat info;
    fstat(stock, &info);
    if(id * sizeof(Stock) >= info.st_size) return NULL;
    char* buff = malloc(100);
    Stock s;
    pread(stock, &s, sizeof(Stock), id * sizeof(Stock));
    int artigos = open("../manager/artigos", O_RDONLY);
    *size = sprintf(buff, "%d %.2f\n", s.stock, getArticlePrice(artigos, id));
    close(artigos);
    return buff;
} 

int updateStock(int id, int new_stock) {
    int stock = open("stocks", O_RDWR);
    int vendas = open("vendas", O_WRONLY | O_APPEND | O_CREAT, 0700);
    Stock s;
    struct stat info;
    fstat(stock, &info);
    if(id * sizeof(Stock) >= info.st_size) return -1;
    pread(stock, &s, sizeof(Stock), id * sizeof(Stock));
    s.stock += new_stock;
    pwrite(stock, &s, sizeof(Stock), id * sizeof(Stock));
    if(new_stock < 0) {
        char buff[200];
        int artigos = open("../manager/artigos", O_RDONLY);
        double price = getArticlePrice(artigos, id);
        close(artigos);
        int read = sprintf(buff, "%d %d %.2f\n", id, -new_stock, -new_stock * price);
        write(vendas, buff, read);
    }
    return s.stock;
}

int main() {
    int stock = open("stocks", O_RDONLY);
    if(errno == ENOENT) {
        close(stock);
        stock = initF();
        close(stock);
    }
    char buff[100];
    int id, size = 0;
    mkfifo("../pipes/rd", 0700);
    mkfifo("../pipes/wr", 0700);
    int rd = open("../pipes/rd", O_RDONLY);
    int wr = open("../pipes/wr", O_WRONLY);
    while(readln(rd, buff, 100)) {
        id = atoi(strtok(buff, " "));
        char* abc = strtok(NULL, " ");
        if(!abc && buff[0] != '\n') {
            char* info = articleInfo(id, &size);
            if(!info)
                write(wr, "\n", 2); 
            else 
                write(wr, info, size);
        }
        else if(buff[0] != '\n') {
            int quant = atoi(abc);
            quant = updateStock(id, quant);
            char quanti[100];
            size = sprintf(quanti, "%d\n", quant);
            write(wr, quanti, size);
        }
        else
           write(wr, "\n", 2); 
    }
    unlink("../pipes/rd");
    unlink("../pipes/wr");
    return 0;
}
