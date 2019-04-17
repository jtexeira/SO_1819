#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>

typedef struct artigo {
    size_t name;
    double price;
} Artigo;

int addArticle(char* name, double price) {
    int strings, artigos, id;
    strings = open("STRINGS", O_RDWR | O_APPEND | O_CREAT, 00700);
    artigos = open("ARTIGOS", O_RDWR | O_APPEND | O_CREAT, 00700);
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

int updateName(int id, char* new_name) {
    int strings, artigos;
    Artigo a;
    struct stat b;
    strings = open("STRINGS", O_RDWR | O_APPEND);
    artigos = open("ARTIGOS", O_RDWR);
    pread(artigos, &a, sizeof(Artigo), id * sizeof(Artigo));
    fstat(strings, &b);
    a.name = b.st_size;
    pwrite(artigos, &a, sizeof(Artigo), id * sizeof(Artigo));
    write(strings, new_name, strlen(new_name) + 1);
    return 0;
}

int updateArticle(int id, double new_price) {
    int artigos = open("ARTIGOS", O_RDWR | O_CREAT, 00700);
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
    pread(artigos, &a, sizeof(Artigo), id * sizeof(Artigo));
    close(artigos);
    int strings = open("STRINGS", O_RDONLY);
    pread(strings, buff, 100, a.name);
    return buff;
}

double getArticlePrice(int id) {
    Artigo a;
    int artigos = open("ARTIGOS", O_RDONLY);
    pread(artigos, &a, sizeof(Artigo), id * sizeof(Artigo));
    close(artigos);
    return a.price;
}

int main() {
    addArticle("Abc", 10);
    addArticle("Defg", 32);
}
