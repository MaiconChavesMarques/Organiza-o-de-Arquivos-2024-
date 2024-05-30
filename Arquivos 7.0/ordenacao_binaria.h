/*
Autores: 2024 - Maicon Chaves Marques - 14593530
Karl Cruz Altenhofen- 14585976
*/

#ifndef _ORDENACAO_BINARIA_H
#define _ORDENACAO_BINARIA_H
#include <stdio.h>
#include <stdbool.h>

  typedef struct cab regCabecalho;
  typedef struct reg regDados;
  typedef struct dadosI indexDados;
  typedef struct listaDados listaDados;

  void ordenaIndex(indexDados** IndiceLocal, int tamanho_index);
  
#endif
