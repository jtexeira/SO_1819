#ifndef ___MANAGER_H___
#define ___MANAGER_H___

#include <unistd.h>

#define OFFSET(x) x/sizeof(Artigo)
#define SPOT(x) x * sizeof(Artigo)

typedef struct artigo {
    size_t name;
    double price;
} Artigo;

char* getArticleName(int);

double getArticlePrice(int);

#endif
