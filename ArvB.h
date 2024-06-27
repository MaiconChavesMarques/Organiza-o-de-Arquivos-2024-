/*
Autores: 2024 - Maicon Chaves Marques - 14593530
Karl Cruz Altenhofen- 14585976
*/

#ifndef ARVB_H
	#define ARVB_H
	#include <stdio.h>

	typedef struct ArvBcab CabArvB;
	typedef struct ArvBdados NoArvB;
	typedef struct dadosI indexDados;

	bool criar_arvoreB(int tamanho, FILE* fp, CabArvB * CabecalhoArvB, FILE* fpArvB);
	void inserir_chave_ArvB(int id, long long byteoffset, CabArvB * CabecalhoArvB, FILE* fpArvB);
	void insercao_ArvB(int NroPaginaAtual, int id_buscado, long long int ByteSetDoID, int* id_promovido, long long int* ByteSetPromovido, int* paginaFilha, int* promocao, CabArvB * CabecalhoArvB, FILE* fpArvB);
	void split(NoArvB* PaginaArvB, NoArvB* NovaPaginaArvB, int ChavesSplit[], long long int ByteSetsSplit[], int ProxSplit[], int* id_promovido, long long int* ByteSetPromovido, int* paginaFilha, CabArvB * CabecalhoArvB);
	void Driver_ArvB(indexDados ElementoArvB, CabArvB * CabecalhoArvB, FILE* fpArvB);
	void ler_PaginaNoDisco(NoArvB* PaginaArvB, int RRN, FILE* fp);
	void escrever_PaginaNoDisco(NoArvB* PaginaArvB, int RRN, FILE* fp);
	void escreverCabArvB(CabArvB * CabecalhoArvB, FILE* fpArvB);
	void imprimirPaginadeDisco(NoArvB* PaginaArvB);
	void imprimirPaginadeSplit(int C[], long long int Pr[], int P[]);
	long long int busca_id_ArvB(int id, int RRNno, FILE* fpArvB);
#endif
