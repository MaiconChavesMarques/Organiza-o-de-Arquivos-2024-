/*
Autores: 2024 - Maicon Chaves Marques - 14593530
Karl Cruz Altenhofen- 14585976
*/

#ifndef _BUSCA_IMPRESSAO_H
#define _BUSCA_IMPRESSAO_H
#include <stdio.h>

  typedef struct cab regCabecalho;
  typedef struct reg regDados;
  typedef struct dadosI indexDados;

  void imprimirjogador(regDados* jogador);
  void copiaJogador(regDados** newRegistro, regDados** RegistrosLocais, int newNumero, int i);
  int busca_binaria_insercao(indexDados** IndiceLocal, int idLocal, int inicio, int fim);
  regDados** filtra(regDados** RegistrosLocais, int* numeroResultados, int pesquisa, long long int RegistrodeOffsets[], int function, indexDados** IndiceLocal, FILE *fp, int tamIndex, long long int *ByteSets);
  
#endif
