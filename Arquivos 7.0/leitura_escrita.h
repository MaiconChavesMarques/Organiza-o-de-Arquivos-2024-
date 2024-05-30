/*
Autores: 2024 - Maicon Chaves Marques - 14593530
Karl Cruz Altenhofen- 14585976
*/

#ifndef _LEITURA_ESCRITA_H
#define _LEITURA_ESCRITA_H
#include <stdio.h>
#include "ordenacao_binaria.h"

  typedef struct cab regCabecalho;
  typedef struct reg regDados;
  typedef struct dadosI indexDados;
  typedef struct listaDados listaDados;

  void escrever_dados(regCabecalho* CabecalhoLocal, regDados** RegistrosLocais, FILE* fp);
  void ler_dados(regCabecalho* CabecalhoLocal, regDados*** RegistrosLocais, FILE* fp);
  void ler_binario(regDados** RegistrosLocais, int tamanho_real, FILE* fp, long long int *ByteSets);
  int tamanhoreg(regDados *jogador);
  regDados* ler_jogador(long long int byteoffset, FILE* fp);
  void escreveJogador(regDados* jogador, long long int byteoffset, FILE* fp);
  bool constroi_indice(indexDados** IndiceLocal, int tamanho, long long int *ByteSets, regDados** RegistrosLocais);
  bool constroi_indice_sem_registro(indexDados** IndiceLocal, int tamanho, FILE* fp);
  bool escrever_dados_indice(indexDados** IndiceLocal, int numero_indices, FILE* fpIndex);
  bool remover_arquivo_indice(indexDados** IndiceLocal, int tamanho, regDados* jogador);
  bool preencherLista(listaDados** listaOrdenada, long long int *byteInicial, int tamanho, int* posicao, FILE* fp);
  bool ler_index(indexDados** IndiceLocal, int numero_indices, FILE* fpIndex);
  void criaJogador(regDados** jogador);
  void resolveindice(indexDados* indice, long long int localdainsercao, indexDados*** IndiceLocal, regCabecalho* CabecalhoLocal);
  
#endif
