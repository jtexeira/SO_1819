#include "article.h"
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>

char* getArticleName(int strings, int artigos, int id) {
    char* buff = malloc(100);
    Artigo a;
    struct stat b;
    fstat(artigos, &b);
    if(id * sizeof(Artigo) >= b.st_size) return NULL;
    pread(artigos, &a, sizeof(Artigo), SPOT(id));
    pread(strings, buff, 100, a.name);
    return buff;
}

double getArticlePrice(int id) {
    Artigo a;
    int artigos = open("artigos", O_RDONLY);
    struct stat b;
    fstat(artigos, &b);
    if(SPOT(id) >= b.st_size) return -1;
    pread(artigos, &a, sizeof(Artigo), SPOT(id));
    close(artigos);
    return a.price;
}
