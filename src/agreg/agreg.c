#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include "../utils/utils.h"

typedef struct venda {
    int id;
    int used;
    int quant;
    double preco;
} Venda;

int main() {
    Venda* vendas = malloc(sizeof(Venda) * 50);
    memset(vendas, 0, sizeof(Venda) * 50);
    int size = 50;
    char buff[100]; 
    while(readln(0, buff, 100)) {
        char* tmp[3];
        int z = 0;
        tmp[z++] = strtok(buff, " ");
        for(; z < 3 && (tmp[z] = strtok(NULL, " ")); z++);
        if(z != 3)
            break;
        int id = atoi(tmp[0]);
        if(id >= size) {
            vendas = realloc(vendas, id * 2);
            memset(vendas + size, 0, ((id*2) - size) * sizeof(Venda));
            size = id * 2;
        }
        int quant = atoi(tmp[1]);
        double preco = atof(tmp[2]);
        vendas[id].id = id;
        vendas[id].used = 1;
        vendas[id].preco += preco;
        vendas[id].quant += quant;
    }
    int i;
    for(i = 0; i < size; i++) {
        if(vendas[i].used) {
            int writeS = sprintf(buff, "%d %d %.2f\n", vendas[i].id, vendas[i].quant, vendas[i].preco);
            write(1, buff, writeS);
        }
    }
    free(vendas);
    return 0;
}
