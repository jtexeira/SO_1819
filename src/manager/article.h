#ifndef ___ARTICLE_H___
#define ___ARTICLE_H___

#include <unistd.h>

#define OFFSET(x) x/sizeof(Artigo)
#define SPOT(x) x * sizeof(Artigo)

typedef struct artigo {
    size_t name;
    double price;
} Artigo;

char* getArticleName(int, int, int);

double getArticlePrice(int, int);

#endif
