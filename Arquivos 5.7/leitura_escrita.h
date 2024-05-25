/*
Autores: 2024 - Maicon Chaves Marques - 14593530
Karl Cruz Altenhofen- 14585976
*/

#ifndef _LEITURA_ESCRITA_H
#define _LEITURA_ESCRITA_H
#include <stdio.h>

  typedef struct cab regCabecalho;
  typedef struct reg regDados;

  void escrever_dados(regCabecalho* CabecalhoLocal, regDados** RegistrosLocais, FILE* fp);
  void ler_dados(regCabecalho* CabecalhoLocal, regDados*** RegistrosLocais, FILE* fp);
  void ler_binario(regDados** RegistrosLocais, int tamanho_real, FILE* fp, long long int *ByteSets);
  regDados* ler_jogador(long long int byteoffset, FILE* fp);
  
#endif
