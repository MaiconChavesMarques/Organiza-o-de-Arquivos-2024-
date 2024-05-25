/*
Autores: 2024 - Maicon Chaves Marques - 14593530
Karl Cruz Altenhofen- 14585976
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ordenacao_binaria.h"

struct cab{
    char status;
    long long int topo;
    long long int proxByteOffset;
    int nroRegArq;
    int nroRegRem;
};

struct reg{
    char removido;
    int tamanhoRegistro;
    long long int Prox;
    int id;
    int idade;
    int tamNomeJog;
    char *nomeJogador;
    int tamNacionalidade;
    char *nacionalidade;
    int tamNomeClube;
    char *nomeClube;
};

struct dadosI{
    int id;
    long long byteoffset;
};

void mergeSort(indexDados** vetor, int inicio, int fim);
void merge(indexDados** vetor, int inicio, int meio, int fim);

void ordenaIndex(indexDados** IndiceLocal, int tamanho_index){
    mergeSort(IndiceLocal, 0, tamanho_index-1);
}

void merge(indexDados** vetor, int inicio, int meio, int fim) {
    int com1 = inicio; 
    int com2 = meio+1; 
    int comAux = 0; 
    indexDados* vetAux[(fim - inicio)+1];
    while(com1<=meio && com2<=fim){
        if(vetor[com1]->id <= vetor[com2]->id){
            vetAux[comAux] = vetor[com1];
            com1++;
    }else{
        vetAux[comAux] = vetor[com2];
        com2++; 
        }
    comAux++; 
    }
    while(com1<=meio){ 
        vetAux[comAux] = vetor[com1];
        comAux++;
        com1++; 
    }
    while(com2<=fim){
        vetAux[comAux] = vetor[com2];
        comAux++;com2++; 
    }
    for(comAux=inicio;comAux<=fim;comAux++){
        vetor[comAux] = vetAux[comAux - inicio];
    }
}

void mergeSort(indexDados** vetor, int inicio, int fim){
    if (inicio < fim){
        int meio = (inicio+fim)/2;
        mergeSort(vetor, inicio, meio);
        mergeSort(vetor, meio+1, fim);
        merge(vetor, inicio, meio, fim);
    }
}