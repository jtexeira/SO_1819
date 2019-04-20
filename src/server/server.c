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
    close(stock);
}

char* articleInfo(int id, int* size) {
    int stock = open("stocks", O_RDONLY);
    struct stat info;
    fstat(stock, &info);
    if((id + 1) * sizeof(Stock) >= info.st_size) return NULL;
    char* buff = malloc(100);
    Stock s;
    pread(stock, &s, sizeof(Stock), id * sizeof(Stock));
    int artigos = open("../manager/artigos", O_RDONLY);
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
    if((id + 1) * sizeof(Stock) >= info.st_size) return -1;
    pread(stock, &s, sizeof(Stock), id * sizeof(Stock));
    s.stock += new_stock;
    pwrite(stock, &s, sizeof(Stock), id * sizeof(Stock));
    if(new_stock < 0) {
        char buff[200];
        int artigos = open("../manager/artigos", O_RDONLY);
        double price = getArticlePrice(artigos, id);
        close(artigos);
        int read = sprintf(buff, "%d %zu %.2f\n", id, -new_stock, -new_stock * price);
        write(vendas, buff, read + 1);
    }
    close(stock);
    close(vendas);
    return s.stock;
}

int main() {
    if(!fork()) {
        initF();
        char buff[100];
        int id, size = 0;
        mkfifo("../pipes/rd", 0700);
        mkfifo("../pipes/wr", 0700);
        int rd = open("../pipes/rd", O_RDONLY);
        int wr = open("../pipes/wr", O_WRONLY);
        while(readln(rd, buff, 100)) {
            if(buff[0] < '0' || buff[0] > '9') {
                write(wr, "\n", 2);
                continue;
            }
            id = atoi(strtok(buff, " "));
            char* abc = strtok(NULL, " ");
            if(!abc) {
                char* info = articleInfo(id, &size);
                if(!info)
                    write(wr, "\n", 2); 
                else 
                    write(wr, info, size + 1);
            }
            else {
                ssize_t quant = atoi(abc);
                updateStock(id, quant);
                write(wr, "\n", 2); 
            }
        }
        close(rd);
        close(wr);
        unlink("../pipes/rd");
        unlink("../pipes/wr");
        return 0;
    }
    return 0;
}
