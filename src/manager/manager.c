#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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

ssize_t readln(int fildes, void *buff, size_t nbyte) {
    size_t i;
    ssize_t r;
    for(i = 0; (r = read(fildes, buff+i,1)) > 0 && i < nbyte && *(char*)(buff+i) != '\n'; i++);
    if(*(char*)(buff+i) == '\n' && r) i++;
    return i;
}

int main() {
    char buff[200];
    while(readln(1, buff, 200))
        switch(buff[0]) {
            case 'i':
                strtok(buff, " ");
                char* name = strtok(NULL, " ");
                double price = atof(strtok(NULL, " "));
                addArticle(name, price);
                break;
            case 'n':
                strtok(buff, " ");
                int id = atoi(strtok(NULL, " "));
                name = strtok(NULL, " ");
                updateName(id, name);
                break;
            case 'p':
                strtok(buff, " ");
                id = atoi(strtok(NULL, " "));
                price = atof(strtok(NULL, " "));
                updateArticle(id, price);
                break;
        }
    return 0;
}

