/*
Autores: 2024 - Maicon Chaves Marques - 14593530
Karl Cruz Altenhofen- 14585976
*/

#ifndef _BUSCA_IMPRESSAO_H
#define _BUSCA_IMPRESSAO_H
#include <stdio.h>
#include "ArvB.h"

  typedef struct cab regCabecalho;
  typedef struct reg regDados;
  typedef struct dadosI indexDados;
  typedef struct listaDados listaDados;

  void imprimirjogador(regDados* jogador);
  void copiaJogador(regDados** newRegistro, regDados* jogador, int newNumero);
  int busca_binaria_insercao(indexDados** IndiceLocal, int idLocal, int inicio, int fim);
  regDados** filtra(regDados** RegistroFiltrado, int* numeroResultados, int pesquisa, long long int RegistrodeOffsets[], int function, indexDados** IndiceLocal, FILE *fp, int tamIndex, long long int *ByteSets, int tamanho_real, CabArvB * CabecalhoArvB, FILE* fpArvB);
  int busca_anterior(listaDados** listaOrdenada, int posicao, int tamanho);
  void antecipa_id();

  
#endif
