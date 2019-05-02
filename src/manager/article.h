#ifndef ___ARTICLE_H___
#define ___ARTICLE_H___

#include <unistd.h>

#define OFFSET(x) ((x-sizeof(time_t))/sizeof(Artigo))
#define SPOT(x) ((x * sizeof(Artigo)) + sizeof(time_t))

typedef struct artigo {
    size_t name;
    double price;
} Artigo;

char* getArticleName(int, int, int);

double getArticlePrice(int);

#endif
