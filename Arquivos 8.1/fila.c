#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "fila.h"

struct fila_{
    ITEM *fila[100];
    int inicio; /*posicao do 1o elemento da fila*/
    int fim; /*posicao do ultimo elemento da fila*/
    int tamanho;
};

FILA *fila_criar(void){
    FILA *fila = (FILA *) malloc(sizeof(FILA));
    fila->inicio = 0;
    fila->fim = 0;
    fila->tamanho = 0; 
    return (fila);
}

bool fila_cheia(FILA *fila) {
    return (fila->tamanho == 100);
}

bool fila_vazia(FILA *fila) {
    return (fila->tamanho == 0);
}

bool fila_inserir(FILA *fila, ITEM *item){
    if (fila != NULL && (!fila_cheia(fila)) ){
    fila->fila[fila->fim] = item;
    fila->fim = (fila->fim+1) % 100;
    fila->tamanho ++;
    return(true);
    }
    return(false);
    }

ITEM *fila_remover(FILA *fila){
    if (fila != NULL && (!fila_vazia(fila)) ) {
    ITEM *i = fila->fila[fila->inicio];
    fila->fila[fila->inicio] = NULL;
    fila->inicio = (fila->inicio + 1) % 100;
    fila->tamanho --;
    return (i);
    }
    return (NULL);
}

int fila_tamanho(FILA *fila) {
    if (fila != NULL)
    return (fila->tamanho);
    return (-123414124);
}

void fila_apagar(FILA** fila) {
    ITEM* a1;
    if ( ((*fila) != NULL) && (!fila_vazia(*fila)) ) {
        while ((*fila)->tamanho != 0) {
            a1 = fila_remover(*fila);
            item_apagar(&a1);
            free(a1); a1 = NULL;
            }
        }
    free(*fila);
    *fila = NULL;
}