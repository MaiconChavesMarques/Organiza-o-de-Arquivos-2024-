/*
Autores: 2024 - Maicon Chaves Marques - 14593530
Karl Cruz Altenhofen- 14585976
*/

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "help.h" //Funções fornecidas pelo run.codes
#include "leitura_escrita.h"
#include "busca_impressao.h"
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

typedef struct listaDados{
    int tamanho;
    long long byteoffset;
}listaDados;

bool ler_index(indexDados** IndiceLocal, int numero_indices, FILE* fpIndex);
bool preencherLista(listaDados** listaOrdenada, long long int *byteInicial, int tamanho, int* posicao, FILE* fp);
int busca_anterior(listaDados** listaOrdenada, int posicao, int tamanho);
bool escrever_dados_indice(indexDados** IndiceLocal, int numero_indices, FILE* fpIndex);
bool remover_arquivo_indice(indexDados** IndiceLocal, int tamanho, regDados* jogador);
void imprimirjogador2(regDados* jogador); //imprima strings caracter por caracter
bool constroi_indice(indexDados** IndiceLocal, int tamanho, long long int *ByteSets, regDados** RegistrosLocais);

void function1(){//Le os dados de um arquivo ".csv"  e escreve os dados em um arquivo ".bin"
    char arquivo1[30]; //Arquivo de entrada ".csv"
    char arquivo2[30]; //Arquivo de saida ".bin"
    scanf("%s", arquivo1);
    scanf("%s", arquivo2);
    FILE *fp;

    if (((fp = fopen(arquivo1, "r"))) == NULL){ //Abre o ".csv" para leitura
        printf("Falha no processamento do arquivo.");
        return;
    }

   regCabecalho* CabecalhoLocal;
    if((CabecalhoLocal = (regCabecalho*) malloc (sizeof(regCabecalho))) == NULL){ //Aloco espaço para o cabecalho
        printf("Falha no processamento do arquivo.");
        fclose(fp);
        return;
    }

    regDados** RegistrosLocais;
    if((RegistrosLocais = (regDados**) malloc (100*sizeof(regDados*))) == NULL){ //Aloco espaço para 100 jogadores inicialmente, e aumento conforme a demanda
        printf("Falha no processamento do arquivo.");
        fclose(fp);
        return;
    }
    
    fseek(fp, 45, SEEK_SET); //Pula a primeira linha, que representa as colunas

    ler_dados(CabecalhoLocal, &RegistrosLocais, fp); //Le os dados do arquivo ".csv" e traz as informações para a memória RAM (regDados e regCababecalho)
    fclose(fp);

    if (((fp = fopen(arquivo2, "wb"))) == NULL){ //Abre o arquivo ".bin" para escrita
        printf("Falha no processamento do arquivo.");
        return;
    }
    escrever_dados(CabecalhoLocal, RegistrosLocais, fp);  //Escreve o cabecalho e todos os registros de dados no arquivo ".bin"
    fclose(fp);

    for(int i = 0; i < CabecalhoLocal->nroRegArq; i++){ //Libera a memória alocada
        free(RegistrosLocais[i]->nomeJogador);
        free(RegistrosLocais[i]->nacionalidade);
        free(RegistrosLocais[i]->nomeClube);
        free(RegistrosLocais[i]);
        RegistrosLocais[i] = NULL;
    }
    free(RegistrosLocais);
    free(CabecalhoLocal);

    binarioNaTela(arquivo2);
}

void function2(){ //Le os dados de um arquivo ".bin"  e imprime todos os seus registros
    char arquivo1[30]; //Arquivo de entrada ".bin"
    scanf("%s", arquivo1);
    FILE *fp;
    if (((fp = fopen(arquivo1, "rb"))) == NULL){ //Abro o binário para leitura
        printf("Falha no processamento do arquivo.");
        return;
    }
    
    regCabecalho* CabecalhoLocal;
    if((CabecalhoLocal = (regCabecalho*) malloc (sizeof(regCabecalho))) == NULL){ //Aloco o cabecalho
        printf("Falha no processamento do arquivo.");
        fclose(fp);
        return;
    }

    fread(&CabecalhoLocal->status, sizeof(char), 1, fp); //Leio as informações do cabecalho
    fread(&CabecalhoLocal->topo, sizeof(long long int), 1, fp);
    fread(&CabecalhoLocal->proxByteOffset, sizeof(long long int), 1, fp);
    fread(&CabecalhoLocal->nroRegArq, sizeof(int), 1, fp);
    fread(&CabecalhoLocal->nroRegRem, sizeof(int), 1, fp);
    
    if(CabecalhoLocal->status == '0'){
        printf("Falha no processamento do arquivo.");
        return;
    }

    if(CabecalhoLocal->nroRegArq == 0){
        printf("Registro inexistente.\n\n");
        return;
    }

    int tamanho_real = CabecalhoLocal->nroRegArq+CabecalhoLocal->nroRegRem; //O tamanho que preciso realmente buscar são os ativos + os removidos

    regDados** RegistrosLocais;
    if((RegistrosLocais = (regDados**) malloc ((CabecalhoLocal->nroRegArq)*sizeof(regDados*))) == NULL){ //Aloco o tamanho exato
        printf("Falha no processamento do arquivo.");
        fclose(fp);
        return;
    }

    long long int *ByteSets;
    ByteSets = (long long int*) malloc(CabecalhoLocal->nroRegArq * sizeof(long long int));
    if (ByteSets == NULL) {
        printf("Falha no processamento do arquivo.");
        fclose(fp);
        return;
    }

    ler_binario(RegistrosLocais, tamanho_real, fp, ByteSets); // Le os dados do arquivo ".bin" e traz as informações para a memória RAM (regDados e regCababecalho)

    for(int i = 0; i < CabecalhoLocal->nroRegArq; i++){
        printf("%lld\n", ByteSets[i]);
    }

    fclose(fp);
    for(int b = 0; b < CabecalhoLocal->nroRegArq; b++){
        //imprimirjogador(RegistrosLocais[b]); // Imprime todos os campos de um jogador
    }

    for(int i = 0; i < CabecalhoLocal->nroRegArq; i++){ // Libero toda a memória alocada
        free(RegistrosLocais[i]->nomeJogador);
        free(RegistrosLocais[i]->nacionalidade);
        free(RegistrosLocais[i]->nomeClube);
        free(RegistrosLocais[i]);
        RegistrosLocais[i] = NULL;
    }
    free(RegistrosLocais);
    free(CabecalhoLocal);
    free(ByteSets);
    
}

void function3(){ // filtra o arquivo dado pelo usuario atraves de campos e valores tambem dados pelo usuario
    char arquivo1[30];
    scanf("%s", arquivo1);
    FILE *fp;
    if (((fp = fopen(arquivo1, "rb"))) == NULL){ // arquivo binario sera aberto para leitura
        printf("Falha no processamento do arquivo.");
        return;
    }

    regCabecalho* CabecalhoLocal;
    if((CabecalhoLocal = (regCabecalho*) malloc (sizeof(regCabecalho))) == NULL){ // aloca memoria para o cabecalho
        printf("Falha no processamento do arquivo.");
        fclose(fp);
        return;
    }
    fread(&CabecalhoLocal->status, sizeof(char), 1, fp); // traz todos os dados do cabecalho do arquivo para a memoria alocada
    fread(&CabecalhoLocal->topo, sizeof(long long int), 1, fp);
    fread(&CabecalhoLocal->proxByteOffset, sizeof(long long int), 1, fp);
    fread(&CabecalhoLocal->nroRegArq, sizeof(int), 1, fp);
    fread(&CabecalhoLocal->nroRegRem, sizeof(int), 1, fp);
    if(CabecalhoLocal->nroRegArq == 0){ // check se o arquivo esta vazio
        printf("Registro inexistente.\n");
        return;
    }

    int tamanho_real = CabecalhoLocal->nroRegArq+CabecalhoLocal->nroRegRem; // salva o tamanho do (cabecario + registros) para ler o binario

    regDados** RegistrosLocais;
    if((RegistrosLocais = (regDados**) malloc ((CabecalhoLocal->nroRegArq)*sizeof(regDados*))) == NULL){ // aloca memoria para os registros
        printf("Falha no processamento do arquivo.");
        fclose(fp);
        return;
    }

    long long int *ByteSets;
    ByteSets = (long long int*) malloc(CabecalhoLocal->nroRegArq * sizeof(long long int));
    if (ByteSets == NULL) {
        printf("Falha no processamento do arquivo.");
        fclose(fp);
        return;
    }
    
    ler_binario(RegistrosLocais, tamanho_real, fp, ByteSets); // pega todos os registros no arquivo e traz eles para a memoria alocada
    fclose(fp); // arquivo ja esta todo na ram, entao nao precisamos mais ler ele
    
    indexDados** IndiceLocal = NULL;
    int numeroResultadosFixo = CabecalhoLocal->nroRegArq; // salva quantos registros tem no arquivo
    int numeroResultados; // numero de registros que diminui apos serem filtrados
    free(CabecalhoLocal); // cabecalho nao se faz mais necessario 
    int numeroBuscas; // quantidade de buscas a serem feitas
    int numeroCampos; // quantos filtros serão aplicados a uma dada busca
    long long int RegistrodeOffsets[numeroResultadosFixo];
    regDados** newRegistro;

    scanf("%d", &numeroBuscas);
    for(int i = 0; i < numeroBuscas; i++){ // loop em que cada iteracao sera uma busca diferente
        numeroResultados = numeroResultadosFixo; // numero de registros comeca sendo do tamanho de todos os registros
        scanf("%d", &numeroCampos);
        for(int j = 0; j < numeroCampos; j++){ // loop em que cada iteracao sera um filtro diferente a ser aplicado aos registros
            if(j == 0){ // caso seja o primeiro filtro, aplica filtro ao conjunto de registros completo

                newRegistro = filtra(RegistrosLocais, &numeroResultados, 1, RegistrodeOffsets, 3, IndiceLocal, fp, 0, ByteSets);

            }else{  // caso nao seja o primeiro, filtra o conjunto ja filtrado na iteracao anterior, alocando novamente o novo e dando free no anterior
                regDados** tempReg = newRegistro;
                int tempNum = numeroResultados;
                newRegistro = filtra(newRegistro, &numeroResultados, 2, RegistrodeOffsets, 3, IndiceLocal, fp, 0, ByteSets);
                for(int k = 0; k < tempNum; k++){
                    free(tempReg[k]->nomeJogador);
                    free(tempReg[k]->nacionalidade);
                    free(tempReg[k]->nomeClube);
                    free(tempReg[k]);
                    tempReg[k] = NULL;
                }
                free(tempReg);
            }
        }
        printf("Busca %d\n\n", i+1); //prints
        if(numeroResultados == 0){
            printf("Registro inexistente.\n\n");
        }else{
            for(int j = 0; j < numeroResultados; j++){
                //printf("ByteOffSet: %lld\n", RegistrodeOffsets[j]);
                imprimirjogador(newRegistro[j]);
            }
        }
        for(int i = 0; i < numeroResultados; i++){ // da free nos registros da ultima iteracao, os mesmos que foram impressos
            free(newRegistro[i]->nomeJogador);
            free(newRegistro[i]->nacionalidade);
            free(newRegistro[i]->nomeClube);
            free(newRegistro[i]);
            newRegistro[i] = NULL;
        }
        free(newRegistro);
    } // comeca a proxima busca

    for(int i = 0; i < numeroResultadosFixo; i++){ // free nos registros originais que foram retirados do arquivo
        free(RegistrosLocais[i]->nomeJogador);
        free(RegistrosLocais[i]->nacionalidade);
        free(RegistrosLocais[i]->nomeClube);
        free(RegistrosLocais[i]);
        RegistrosLocais[i] = NULL;
    }
    free(RegistrosLocais);
    free(ByteSets);
}

void function4(){ //Le os dados de um arquivo ".bin"  e imprime todos os seus registros
    char arquivo1[30]; //Arquivo de entrada ".bin"
    scanf("%s", arquivo1);
    char arquivo2[30]; //Arquivo de entrada ".bin"
    scanf("%s", arquivo2);
    FILE *fp;
    FILE *fpIndex;
    if (((fp = fopen(arquivo1, "rb"))) == NULL){ //Abro o binário para leitura
        printf("Falha no processamento do arquivo.");
        return;
    }
    
    regCabecalho* CabecalhoLocal;
    if((CabecalhoLocal = (regCabecalho*) malloc (sizeof(regCabecalho))) == NULL){ //Aloco o cabecalho
        printf("Falha no processamento do arquivo.");
        fclose(fp);
        return;
    }

    fread(&CabecalhoLocal->status, sizeof(char), 1, fp); //Leio as informações do cabecalho
    fread(&CabecalhoLocal->topo, sizeof(long long int), 1, fp);
    fread(&CabecalhoLocal->proxByteOffset, sizeof(long long int), 1, fp);
    fread(&CabecalhoLocal->nroRegArq, sizeof(int), 1, fp);
    fread(&CabecalhoLocal->nroRegRem, sizeof(int), 1, fp);
    
    if(CabecalhoLocal->status == '0'){
        printf("Falha no processamento do arquivo.");
        return;
    }

    if(CabecalhoLocal->nroRegArq == 0){
        printf("Registro inexistente.\n\n");
        return;
    }

    int tamanho_real = CabecalhoLocal->nroRegArq+CabecalhoLocal->nroRegRem; //O tamanho que preciso realmente buscar são os ativos + os removidos

    regDados** RegistrosLocais;
    if((RegistrosLocais = (regDados**) malloc ((CabecalhoLocal->nroRegArq)*sizeof(regDados*))) == NULL){ //Aloco o tamanho exato
        printf("Falha no processamento do arquivo.");
        fclose(fp);
        return;
    }
    
    long long int *ByteSets;
    ByteSets = (long long int*) malloc(CabecalhoLocal->nroRegArq * sizeof(long long int));
    if (ByteSets == NULL) {
        printf("Falha no processamento do arquivo.");
        fclose(fp);
        return;
    }

    ler_binario(RegistrosLocais, tamanho_real, fp, ByteSets); // Le os dados do arquivo ".bin" e traz as informações para a memória RAM (regDados e regCababecalho)

    fclose(fp);
 
    char indexCab = '0';
    indexDados** IndiceLocal;
    if((IndiceLocal = (indexDados**) malloc ((CabecalhoLocal->nroRegArq)*sizeof(indexDados*))) == NULL){ //Aloco o tamanho exato
        printf("Falha no processamento do arquivo.");
        fclose(fp);
        return;
    }

    constroi_indice(IndiceLocal, CabecalhoLocal->nroRegArq, ByteSets, RegistrosLocais);

    if (((fpIndex = fopen(arquivo2, "wb"))) == NULL){ //Abro o binário para escrita
        printf("Falha no processamento do arquivo.");
        return;
    }

    escrever_dados_indice(IndiceLocal, CabecalhoLocal->nroRegArq, fpIndex);
    fclose(fpIndex);

    for(int i = 0; i < CabecalhoLocal->nroRegArq; i++){
        free(IndiceLocal[i]);
    }
    free(IndiceLocal);

    for(int i = 0; i < CabecalhoLocal->nroRegArq; i++){ // Libero toda a memória alocada
        free(RegistrosLocais[i]->nomeJogador);
        free(RegistrosLocais[i]->nacionalidade);
        free(RegistrosLocais[i]->nomeClube);
        free(RegistrosLocais[i]);
        RegistrosLocais[i] = NULL;
    }
    free(RegistrosLocais);
    free(ByteSets);
    free(CabecalhoLocal);
    binarioNaTela(arquivo2);

}

void function5(){ //deleta um registro
    char arquivo1[30]; //Arquivo de entrada ".bin"
    scanf("%s", arquivo1);
    char arquivo2[30]; //Arquivo de inidice ".bin"
    scanf("%s", arquivo2);
    FILE *fp;
    FILE *fpIndex;
    if (((fp = fopen(arquivo1, "rb+"))) == NULL){ //Abro o binário para leitura e escrita
        printf("Falha no processamento do arquivo.");
        return;
    }
    
    regCabecalho* CabecalhoLocal;
    if((CabecalhoLocal = (regCabecalho*) malloc (sizeof(regCabecalho))) == NULL){ //Aloco o cabecalho
        printf("Falha no processamento do arquivo.");
        fclose(fp);
        return;
    }

    fread(&CabecalhoLocal->status, sizeof(char), 1, fp); //Leio as informações do cabecalho
    fread(&CabecalhoLocal->topo, sizeof(long long int), 1, fp);
    fread(&CabecalhoLocal->proxByteOffset, sizeof(long long int), 1, fp);
    fread(&CabecalhoLocal->nroRegArq, sizeof(int), 1, fp);
    fread(&CabecalhoLocal->nroRegRem, sizeof(int), 1, fp);
    
    if(CabecalhoLocal->status == '0'){
        printf("Falha no processamento do arquivo.");
        fclose(fp);
        return;
    }

    CabecalhoLocal->status = '0';

    if(CabecalhoLocal->nroRegArq == 0){
        printf("Registro inexistente.\n\n");
        fclose(fp);
        return;
    }

    int tamanho_real = CabecalhoLocal->nroRegArq+CabecalhoLocal->nroRegRem; //O tamanho que preciso realmente buscar são os ativos + os removidos

    regDados** RegistrosLocais;
    if((RegistrosLocais = (regDados**) malloc ((CabecalhoLocal->nroRegArq)*sizeof(regDados*))) == NULL){ //Me preparo para ler, mas não leio ainda, apenas se a busca não envolver o id
        printf("Falha no processamento do arquivo.");
        fclose(fp);
        return;
    }

    long long int *ByteSets; //Guardo aqui os byteoffsets lidos do arquivo.
    ByteSets = (long long int*) malloc(CabecalhoLocal->nroRegArq * sizeof(long long int));
    if (ByteSets == NULL) {
        printf("Falha no processamento do arquivo.");
        fclose(fp);
        return;
    }

    ler_binario(RegistrosLocais, tamanho_real, fp, ByteSets); // Le os dados do arquivo ".bin" e traz as informações para a memória RAM (regDados e regCababecalho)

    fseek(fp, 0, SEEK_SET);
    CabecalhoLocal->status = '0';
    fwrite(&CabecalhoLocal->status, sizeof(char), 1, fp);

    indexDados** IndiceLocal;
    if((IndiceLocal = (indexDados**) malloc ((CabecalhoLocal->nroRegArq)*sizeof(indexDados*))) == NULL){ //Aloco o tamanho exato
        printf("Falha no processamento do arquivo.");
        fclose(fp);//
        return;
    }
    char indexCab;

    if (((fpIndex = fopen(arquivo2, "rb"))) == NULL){ //Abro o indice para leitura e escrita
        indexCab = '1';
        constroi_indice(IndiceLocal, CabecalhoLocal->nroRegArq, ByteSets, RegistrosLocais);
    }else{
        fread(&indexCab, sizeof(char), 1, fpIndex); //Leio as informações do cabecalho
        if(indexCab == '0'){
            printf("Falha no processamento do arquivo.");
            exit(1);
        }
        ler_index(IndiceLocal, CabecalhoLocal->nroRegArq, fpIndex);
        fclose(fpIndex);
    }

    if (((fpIndex = fopen(arquivo2, "wb"))) == NULL){ //Abro o indice para leitura e escrita
        printf("Falha no processamento do arquivo.");
        return;
    }

    indexCab = '0';
    fseek(fpIndex, 0, SEEK_SET);
    fwrite(&indexCab, sizeof(char), 1, fpIndex);

    listaDados** listaOrdenada;
    //printf("A lista ordenada comecou com o tamanho de: %d\n\n", CabecalhoLocal->nroRegRem);
    if((listaOrdenada = (listaDados**) malloc (((CabecalhoLocal->nroRegRem))*sizeof(listaDados*))) == NULL){ //OLHA ISSO AQUIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII
        printf("Falha no processamento do arquivo.");
        fclose(fp);//
        return;
    }

    int DadosEmMemoria = 0;
    int tamMaxBuscado = 0; //é o maior tamanho que foi buscado até o momento
    int posicao = 0; //Diz respeito a qual a proxima posicao que irei gravar no vetor da lista ordenada
    long long int byteinicial = CabecalhoLocal->topo;
    int numeroResultadosFixo = CabecalhoLocal->nroRegArq; // salva quantos registros tem no arquivo
    int numeroResultados; // numero de registros que diminui apos serem filtrados
    int numeroBuscas; // quantidade de buscas a serem feitas
    int numeroCampos; // quantos filtros serão aplicados a uma dada busca
    long long int RegistrodeOffsets[numeroResultadosFixo]; //Vetor onde atualizo e guardo os byteoffsets daqueles que foram encontrados
    regDados** newRegistro;
    /*
    regDados* jogador;
    long long b = 43752;
    jogador = ler_jogador(b, fp);
    imprimirjogador(jogador);
    */

    scanf("%d", &numeroBuscas);
    for(int i = 0; i < numeroBuscas; i++){ // loop em que cada iteracao sera uma busca diferente
        numeroResultados = numeroResultadosFixo; // numero de registros comeca sendo do tamanho de todos os registros
        scanf("%d", &numeroCampos);
        for(int j = 0; j < numeroCampos; j++){ // loop em que cada iteracao sera um filtro diferente a ser aplicado aos registros
            if(j == 0){ // caso seja o primeiro filtro, aplica filtro ao conjunto de registros completo

                newRegistro = filtra(RegistrosLocais, &numeroResultados, 1, RegistrodeOffsets, 5, IndiceLocal, fp, numeroResultadosFixo, ByteSets);

            }else{  // caso nao seja o primeiro, filtra o conjunto ja filtrado na iteracao anterior, alocando novamente o novo e dando free no anterior
                regDados** tempReg = newRegistro;
                int tempNum = numeroResultados;
                newRegistro = filtra(newRegistro, &numeroResultados, 2, RegistrodeOffsets, 5, IndiceLocal, fp, numeroResultadosFixo, ByteSets);
                for(int k = 0; k < tempNum; k++){
                    free(tempReg[k]->nomeJogador);
                    free(tempReg[k]->nacionalidade);
                    free(tempReg[k]->nomeClube);
                    free(tempReg[k]);
                    tempReg[k] = NULL;
                }
                free(tempReg);
            }
        }
        //printf("Busca %d\n\n", i+1); //prints
        if(numeroResultados == 0){
            //printf("Registro inexistente.\n\n");
        }else{
            //printf("Para essa %d° busca eu tive %d resultados\n", i+1, numeroResultados);
            for(int j = 0; j < numeroResultados; j++){
                //printf("%d° resultado\n", j+1);
                newRegistro[j]->removido = '1';
                listaDados* RegistroLista;
                CabecalhoLocal->nroRegRem++;
                CabecalhoLocal->nroRegArq--;
                if((listaOrdenada = (listaDados**) realloc (listaOrdenada,((CabecalhoLocal->nroRegRem))*sizeof(listaDados*))) == NULL){ //Aloco o tamanho exato
                    printf("Falha no processamento do arquivo.");
                    fclose(fp);//
                    exit(1);
                }
                if((RegistroLista = (listaDados*) malloc (sizeof(listaDados))) == NULL){
                    printf("Falha no processamento do arquivo.");
                    exit(1);
                }else{
                    //printf("Responsável 1\n");
                }
                if(newRegistro[j]->tamanhoRegistro > tamMaxBuscado && CabecalhoLocal->topo != -1){
                    tamMaxBuscado = newRegistro[j]->tamanhoRegistro;
                    preencherLista(listaOrdenada, &byteinicial, tamMaxBuscado, &posicao, fp);
                }
                //remove do cabecalho
                //printf("ByteOffSet: %lld\n", RegistrodeOffsets[j]);
                //imprimirjogador(newRegistro[j]);
                if(CabecalhoLocal->topo == -1){ //Caso a lista ainda não exista
                //printf("Caso de lista vazia\n");
                    RegistroLista->tamanho = newRegistro[j]->tamanhoRegistro;
                    RegistroLista->byteoffset = -1;
                    listaOrdenada[0] = RegistroLista;
                    fseek(fp, RegistrodeOffsets[j], SEEK_SET); //Preciso escrever no registro como removido e manter a lista ordenada
                    fwrite(&newRegistro[j]->removido, sizeof(char), 1, fp);
                    fwrite(&newRegistro[j]->tamanhoRegistro, sizeof(int), 1, fp);
                    fwrite(&RegistroLista->byteoffset, sizeof(long long int), 1, fp); //atualizo o proximo para manter a lista encadeada
                    CabecalhoLocal->topo = RegistrodeOffsets[j];
                    //printf("Aumentarei 1 em posicao, que está nesse caso em %d", posicao);
                    posicao++;
                    byteinicial = -1;
                      //              printf("removi1\n");
                }else if(newRegistro[j]->tamanhoRegistro < listaOrdenada[0]->tamanho){ //Tenho uma lista, mas preciso inserir o removido como primeiro elemento
                    //printf("Caso de insercao no comeco\n");
                    for(int i = posicao-1; i >= 0; i--){ //Tem que ser posicao -1, porque posicao sempre aponta para o depois do ultimo
                        listaOrdenada[i+1]=listaOrdenada[i];
                    }
                    RegistroLista->tamanho = newRegistro[j]->tamanhoRegistro;
                    RegistroLista->byteoffset = CabecalhoLocal->topo;
                    listaOrdenada[0] = RegistroLista;
                    CabecalhoLocal->topo = RegistrodeOffsets[j];
                    fseek(fp, RegistrodeOffsets[j], SEEK_SET); //Preciso escrever no registro como removido e manter a lista ordenada
                    fwrite(&newRegistro[j]->removido, sizeof(char), 1, fp);
                    fwrite(&newRegistro[j]->tamanhoRegistro, sizeof(int), 1, fp);
                    fwrite(&RegistroLista->byteoffset, sizeof(long long int), 1, fp); //atualizo o proximo para manter a lista encadeada
                    //printf("Aumentarei 1 em posicao, que está nesse caso em %d", posicao);
                    posicao++;
                        //printf("removi2\n");
                }else if(newRegistro[j]->tamanhoRegistro > listaOrdenada[posicao-1]->tamanho){ //Se o espaço que procuro é maior que o espaco de todo mundo que tenho na lista, entao coloco no fim
                    //printf("Caso de insercao no fim\n");
                    RegistroLista->tamanho = newRegistro[j]->tamanhoRegistro;
                    RegistroLista->byteoffset = -1;
                    listaOrdenada[posicao] = RegistroLista;
                    listaOrdenada[posicao-1]->byteoffset = RegistrodeOffsets[j];
                    if(posicao == 1){
                        fseek(fp, (CabecalhoLocal->topo)+5, SEEK_SET); //se eu tiver so 1 elemento na lista, e voltar 2 ele da segfault
                    }else{
                        fseek(fp, (listaOrdenada[posicao-2]->byteoffset)+5, SEEK_SET); //tenho pelo menos 2, então da para voltar no 0
                    }
                    //printf("O anterior a ele na lista eh: %lld\n", listaOrdenada[posicao-2]->byteoffset);
                    //printf("O byte off set desse jogador eh: %lld", RegistrodeOffsets[j]);
                    fwrite(&RegistrodeOffsets[j], sizeof(long long int), 1, fp);
                    fseek(fp, RegistrodeOffsets[j], SEEK_SET); //Preciso escrever no registro como removido e manter a lista ordenada
                    fwrite(&newRegistro[j]->removido, sizeof(char), 1, fp);
                    fwrite(&newRegistro[j]->tamanhoRegistro, sizeof(int), 1, fp);
                    fwrite(&RegistroLista->byteoffset, sizeof(long long int), 1, fp); //atualizo o proximo para manter a lista encadeada
                    //printf("Aumentarei 1 em posicao, que está nesse caso em %d", posicao);
                    posicao++;
                    byteinicial = -1;
                    //               printf("removi3\n");
                }else{ //Insiro no meio da lista
                    //printf("Caso de insercao no meio\n");
                    //printf("%d\n\n", listaOrdenada[0]->tamanho);
                int indice = busca_anterior(listaOrdenada, posicao, newRegistro[j]->tamanhoRegistro);//retorna o indice do elemento que está logo atrás do elemento a ser inserido
                //printf("Depois da busca\n");
                //printf("Variavel posicao: %d\n", posicao);
                //printf("Variavel indice: %d\n", indice);
                //printf("Tamanho: %d", newRegistro[j]->tamanhoRegistro);
                //printf("Tamanho da cabeça: %d\n", listaOrdenada[0]->tamanho);
                //printf("Tamanho da calda: %d\n", listaOrdenada[1]->tamanho);
                RegistroLista->tamanho = newRegistro[j]->tamanhoRegistro;
                RegistroLista->byteoffset = listaOrdenada[indice]->byteoffset;//Faco ele apontar para onde o anterior a ele apontava
                if(indice == 0){
                    fseek(fp, (CabecalhoLocal->topo)+5, SEEK_SET); //Preciso colocar no proprio registro o anterior ao novo removido apontar para ele
                }else{
                    fseek(fp, (listaOrdenada[indice-1]->byteoffset)+5, SEEK_SET); //Preciso colocar no proprio registro o anterior ao novo removido apontar para ele
                }
                fwrite(&RegistrodeOffsets[j], sizeof(long long int), 1, fp);
                listaOrdenada[indice]->byteoffset = RegistrodeOffsets[j]; //atualizo isso na memoria ram tambem
                fseek(fp, RegistrodeOffsets[j], SEEK_SET); //Preciso escrever no registro como removido e manter a lista ordenada
                fwrite(&newRegistro[j]->removido, sizeof(char), 1, fp);
                fwrite(&newRegistro[j]->tamanhoRegistro, sizeof(int), 1, fp);
                fwrite(&RegistroLista->byteoffset, sizeof(long long int), 1, fp); //atualizo o proximo para manter a lista encadeada
                for(int i = posicao-1; i >= indice+1; i--){ //Tem que ser posicao -1, porque posicao sempre aponta para o depois do ultimo
                    listaOrdenada[i+1]=listaOrdenada[i];
                }
                listaOrdenada[indice+1] = RegistroLista;
                //printf("Aumentarei 1 em posicao, que está nesse caso em %d", posicao);
                posicao++;
                //printf("Caso de remocao no meio\n");
                //printf("removido45\n");
                }
                remover_arquivo_indice(IndiceLocal, CabecalhoLocal->nroRegArq+1, newRegistro[j]);
                IndiceLocal = (indexDados**) realloc (IndiceLocal,(CabecalhoLocal->nroRegArq)*sizeof(indexDados*));
                if(IndiceLocal == NULL){
                    printf("Falha no processamento do arquivo.");
                }
            }
        //*/
        }
        for(int i = 0; i < numeroResultados; i++){ // da free nos registros da ultima iteracao, os mesmos que foram impressos
            free(newRegistro[i]->nomeJogador);
            free(newRegistro[i]->nacionalidade);
            free(newRegistro[i]->nomeClube);
            free(newRegistro[i]);
            newRegistro[i] = NULL;
        }
        free(newRegistro);
    } // comeca a proxima busca

    for(int i = 0; i < posicao; i++){
        free(listaOrdenada[i]);
    }
    free(listaOrdenada);

    //printf("Fim nroRegArq: %d\n", CabecalhoLocal->nroRegArq);
    if(!escrever_dados_indice(IndiceLocal, CabecalhoLocal->nroRegArq, fpIndex)){
        printf("Falha no processamento do arquivo.");
        exit(1);
    }
    fclose(fpIndex);

    for(int i = 0; i < CabecalhoLocal->nroRegArq; i++){
        free(IndiceLocal[i]);
    }
    free(IndiceLocal);

    fseek(fp, 0, SEEK_SET);
    CabecalhoLocal->status = '1';
    fwrite(&CabecalhoLocal->status, sizeof(char), 1, fp);
    fwrite(&CabecalhoLocal->topo, sizeof(long long int), 1, fp);
    fwrite(&CabecalhoLocal->proxByteOffset, sizeof(long long int), 1, fp);
    fwrite(&CabecalhoLocal->nroRegArq, sizeof(int), 1, fp);
    fwrite(&CabecalhoLocal->nroRegRem, sizeof(int), 1, fp);

    for(int i = 0; i < numeroResultadosFixo; i++){ // free nos registros originais que foram retirados do arquivo
        free(RegistrosLocais[i]->nomeJogador);
        free(RegistrosLocais[i]->nacionalidade);
        free(RegistrosLocais[i]->nomeClube);
        free(RegistrosLocais[i]);
        RegistrosLocais[i] = NULL;
    }
    free(RegistrosLocais);
    free(ByteSets);
    free(CabecalhoLocal);

    fclose(fp);
    binarioNaTela(arquivo1);
    binarioNaTela(arquivo2);
}

bool remover_arquivo_indice(indexDados** IndiceLocal, int tamanho, regDados* jogador){
    if(tamanho == 0){
        return false;
    }
    int indice;
    //printf("ALERTA, VOCE MANDOU REMOVER O SEGUINTE JOGADOR:\n");
    //imprimirjogador2(jogador);
    indice = busca_binaria_insercao(IndiceLocal, jogador->id, 0, tamanho-1);
    //printf("Elemento da posicao do indice %d\n", IndiceLocal[indice]->id);
    if(indice+1 < tamanho){
        //printf("Elemento da posicao do indice +1: %d\n", IndiceLocal[indice+1]->id);
    }
    //printf("Indice: %d\n", indice);
    free(IndiceLocal[indice]);
    IndiceLocal[indice] = NULL;
    for(int i = indice; i < tamanho-1; i++){
        IndiceLocal[i]=IndiceLocal[i+1];
    }
    //printf("Elemento da posicao do indice %d\n", IndiceLocal[indice]->id);
    IndiceLocal[tamanho-1]=NULL;
    return true;
}

int busca_anterior(listaDados** listaOrdenada, int posicao, int tamanho){
    for(int i = 0; i < posicao; i++){
        if(listaOrdenada[i]->tamanho >= tamanho){
            return i-1;
        }
    }
}

bool preencherLista(listaDados** listaOrdenada, long long int *byteInicial, int tamanho, int* posicao, FILE* fp){
    do{
        if(*byteInicial == -1){
            return false;
        }
        fseek(fp, *byteInicial+1, SEEK_SET);
        //printf("Posicaao: %d", *posicao);
        if((listaOrdenada[(*posicao)] = (listaDados*) malloc (sizeof(listaDados))) == NULL){
            printf("Falha no processamento do arquivo.");
            exit(1);
        }else{
            //printf("Responsável 2\n");
        }
        //printf("agora fread\n");
        fread(&listaOrdenada[*posicao]->tamanho, sizeof(int), 1 , fp);
        fread(&listaOrdenada[*posicao]->byteoffset, sizeof(long long int), 1 , fp);
        *byteInicial = listaOrdenada[*posicao]->byteoffset;
        (*posicao)++; //AQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ
        //printf("fread deu certo\n");
        //printf("Posicaao: %d", *posicao);
    }while(listaOrdenada[*posicao-1]->tamanho < tamanho);
    return true;
}

bool escrever_dados_indice(indexDados** IndiceLocal, int numero_indices, FILE* fpIndex){
    char indexCab = '1';
    fseek(fpIndex, 0, SEEK_SET);
    //printf("\n");
    fwrite(&indexCab, sizeof(char), 1, fpIndex);
    for(int i = 0; i < numero_indices; i++){
        //printf("%d id: %d\n", i, IndiceLocal[i]->id);
        //printf("byteoffset: %lld\n", IndiceLocal[i]->byteoffset);
    }
    for(int i = 0; i < numero_indices; i++){
        fwrite(&IndiceLocal[i]->id, sizeof(int), 1, fpIndex);
        fwrite(&IndiceLocal[i]->byteoffset, sizeof(long long int), 1, fpIndex);
    }
    return true;
}

bool ler_index(indexDados** IndiceLocal, int numero_indices, FILE* fpIndex){
    for(int i = 0; i < numero_indices; i++){
        indexDados* elemento; //Um elemento do indice
        if((elemento = (indexDados*) malloc (sizeof(indexDados))) == NULL){
            printf("Falha no processamento do arquivo.");
            return false;
        }
        fread(&elemento->id, sizeof(int), 1, fpIndex);
        fread(&elemento->byteoffset, sizeof(long long int), 1, fpIndex);
        IndiceLocal[i]=elemento;
    }
}

bool constroi_indice(indexDados** IndiceLocal, int tamanho, long long int *ByteSets, regDados** RegistrosLocais){
    for(int i = 0; i < tamanho; i++){
        if((IndiceLocal[i] = (indexDados*) malloc (sizeof(indexDados))) == NULL){ //Aloco o cabecalho
            printf("Falha no processamento do arquivo.");
            exit(1);
        }
        IndiceLocal[i]->id = RegistrosLocais[i]->id;
        IndiceLocal[i]->byteoffset = ByteSets[i];
    }

    ordenaIndex(IndiceLocal, tamanho); //Uso um algoritmo de ordenação para ordenar meu indice pela primeira vez, a partir dai, uso insercoes e remocoes já ordenadas
    return true;
}


int main(){ 
    int operacao; //Operação que será feita pelo usuário
    scanf("%d", &operacao);

    switch (operacao){
    case 1:
        function1(); //Le os dados de um arquivo ".csv" e escreve os dados em um arquivo ".bin"
        break;
    case 2:
        function2(); //Le os dados de um arquivo ".bin" e imprime todos os seus registros
        break;
    case 3:
        function3();
        break;
    case 4:
        function4();
        break;
    case 5:
        function5();
        break;
    case 6:
        //function6();
        break;
    case 7:
        //function7();
        break;
    default:
        break;
    }

    return 0;
}

void imprimirjogador2(regDados* jogador){ //imprima strings caracter por caracter
    printf("tamanhoRegistro: %d\n", jogador->tamanhoRegistro);
    printf("Prox: %lld\n", jogador->Prox);
    printf("id: %d\n", jogador->id);
    printf("idade: %d\n", jogador->idade);
    printf("tamNomeJog: %d\n", jogador->tamNomeJog);
    printf("Nome do jogador: ");
    for(int i = 0; i < jogador->tamNomeJog; i++){
        printf("%c", jogador->nomeJogador[i]);
    }
    printf("\n");
    printf("tamNacionalidade: %d\n", jogador->tamNacionalidade);
     printf("Nacionalidade: ");
    for(int i = 0; i < jogador->tamNacionalidade; i++){
        printf("%c", jogador->nacionalidade[i]);
    }
    printf("\n");
    printf("tamNomeClube: %d\n", jogador->tamNomeClube);
     printf("Nome do Clube: ");
    for(int i = 0; i < jogador->tamNomeClube; i++){
        printf("%c", jogador->nomeClube[i]);
    }
    printf("\n");


    printf("\n\n");
}

void function6(){ //deleta um registro
    char arquivo1[30]; //Arquivo de entrada ".bin"
    scanf("%s", arquivo1);
    char arquivo2[30]; //Arquivo de entrada ".bin"
    scanf("%s", arquivo2);
    FILE *fp;
    FILE *fpIndex;
    
    regCabecalho* CabecalhoLocal;
    if((CabecalhoLocal = (regCabecalho*) malloc (sizeof(regCabecalho))) == NULL){ //Aloco o cabecalho
        printf("Falha no processamento do arquivo.");
        return;
    }

    fread(&CabecalhoLocal->status, sizeof(char), 1, fp); //Leio as informações do cabecalho
    fread(&CabecalhoLocal->topo, sizeof(long long int), 1, fp);
    fread(&CabecalhoLocal->proxByteOffset, sizeof(long long int), 1, fp);
    fread(&CabecalhoLocal->nroRegArq, sizeof(int), 1, fp);
    fread(&CabecalhoLocal->nroRegRem, sizeof(int), 1, fp);
    
    if(CabecalhoLocal->status == '0'){
        printf("Falha no processamento do arquivo.");
        fclose(fp);
        return;
    }

    int dadosMemoria = 0;
    indexDados** IndiceLocal;
    if((IndiceLocal = (indexDados**) malloc ((CabecalhoLocal->nroRegArq)*sizeof(indexDados*))) == NULL){ //Aloco o tamanho exato
        printf("Falha no processamento do arquivo.");
        fclose(fp);//
        return;
    }
    char indexCab;

    FILE* fpIndex;
    if (((fpIndex = fopen(arquivo2, "rb"))) == NULL){ //Abro o indice para leitura e escrita
        int tamanho_real = CabecalhoLocal->nroRegArq+CabecalhoLocal->nroRegRem; // salva o tamanho do (cabecario + registros) para ler o binario

        regDados** RegistrosLocais;
        if((RegistrosLocais = (regDados**) malloc ((CabecalhoLocal->nroRegArq)*sizeof(regDados*))) == NULL){ // aloca memoria para os registros
            printf("Falha no processamento do arquivo.");
            fclose(fp);
            return;
        }

        long long int *ByteSets;
        ByteSets = (long long int*) malloc(CabecalhoLocal->nroRegArq * sizeof(long long int));
        if (ByteSets == NULL) {
            printf("Falha no processamento do arquivo.");
            fclose(fp);
            return;
        }
        
        ler_binario(RegistrosLocais, tamanho_real, fp, ByteSets); // pega todos os registros no arquivo e traz eles para a memoria alocada
        fclose(fp); // arquivo ja esta todo na ram, entao nao precisamos mais ler ele
    
        indexCab = '1';
        constroi_indice(IndiceLocal, CabecalhoLocal->nroRegArq, ByteSets, RegistrosLocais);
        dadosMemoria = 1;
    }else{
        fread(&indexCab, sizeof(char), 1, fpIndex); //Leio as informações do cabecalho
        if(indexCab == '0'){
            printf("Falha no processamento do arquivo.");
            exit(1);
        }
        ler_index(IndiceLocal, CabecalhoLocal->nroRegArq, fpIndex);
        fclose(fpIndex);
    }

    if (((fpIndex = fopen(arquivo2, "wb"))) == NULL){ //Abro o indice para leitura e escrita
        printf("Falha no processamento do arquivo.");
        return;
    }

    indexCab = '0';
    fseek(fpIndex, 0, SEEK_SET);
    fwrite(&indexCab, sizeof(char), 1, fpIndex);

    listaDados** listaOrdenada;
    //printf("A lista ordenada comecou com o tamanho de: %d\n\n", CabecalhoLocal->nroRegRem);
    if((listaOrdenada = (listaDados**) malloc (((CabecalhoLocal->nroRegRem))*sizeof(listaDados*))) == NULL){ //OLHA ISSO AQUIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII
        printf("Falha no processamento do arquivo.");
        fclose(fp);//
        return;
    }

    int DadosEmMemoria = 0;
    int tamMaxBuscado = 0; //é o maior tamanho que foi buscado até o momento
    int posicao = 0; //Diz respeito a qual a proxima posicao que irei gravar no vetor da lista ordenada
    long long int byteinicial = CabecalhoLocal->topo;
    int numeroResultadosFixo = CabecalhoLocal->nroRegArq; // salva quantos registros tem no arquivo
    int numeroResultados; // numero de registros que diminui apos serem filtrados
    int numeroBuscas; // quantidade de buscas a serem feitas
    int numeroCampos; // quantos filtros serão aplicados a uma dada busca
    long long int RegistrodeOffsets[numeroResultadosFixo]; //Vetor onde atualizo e guardo os byteoffsets daqueles que foram encontrados
    regDados** newRegistro;
    /*
    regDados* jogador;
    long long b = 43752;
    jogador = ler_jogador(b, fp);
    imprimirjogador(jogador);
    */

    scanf("%d", &numeroBuscas);
    for(int i = 0; i < numeroBuscas; i++){ // loop em que cada iteracao sera uma busca diferente
        numeroResultados = numeroResultadosFixo; // numero de registros comeca sendo do tamanho de todos os registros
        scanf("%d", &numeroCampos);
        for(int j = 0; j < numeroCampos; j++){ // loop em que cada iteracao sera um filtro diferente a ser aplicado aos registros
            if(j == 0){ // caso seja o primeiro filtro, aplica filtro ao conjunto de registros completo

                newRegistro = filtra(RegistrosLocais, &numeroResultados, 1, RegistrodeOffsets, 5, IndiceLocal, fp, numeroResultadosFixo, ByteSets);

            }else{  // caso nao seja o primeiro, filtra o conjunto ja filtrado na iteracao anterior, alocando novamente o novo e dando free no anterior
                regDados** tempReg = newRegistro;
                int tempNum = numeroResultados;
                newRegistro = filtra(newRegistro, &numeroResultados, 2, RegistrodeOffsets, 5, IndiceLocal, fp, numeroResultadosFixo, ByteSets);
                for(int k = 0; k < tempNum; k++){
                    free(tempReg[k]->nomeJogador);
                    free(tempReg[k]->nacionalidade);
                    free(tempReg[k]->nomeClube);
                    free(tempReg[k]);
                    tempReg[k] = NULL;
                }
                free(tempReg);
            }
        }
        //printf("Busca %d\n\n", i+1); //prints
        if(numeroResultados == 0){
            //printf("Registro inexistente.\n\n");
        }else{
            //printf("Para essa %d° busca eu tive %d resultados\n", i+1, numeroResultados);
            for(int j = 0; j < numeroResultados; j++){
                //printf("%d° resultado\n", j+1);
                newRegistro[j]->removido = '1';
                listaDados* RegistroLista;
                CabecalhoLocal->nroRegRem++;
                CabecalhoLocal->nroRegArq--;
                if((listaOrdenada = (listaDados**) realloc (listaOrdenada,((CabecalhoLocal->nroRegRem))*sizeof(listaDados*))) == NULL){ //Aloco o tamanho exato
                    printf("Falha no processamento do arquivo.");
                    fclose(fp);//
                    exit(1);
                }
                if((RegistroLista = (listaDados*) malloc (sizeof(listaDados))) == NULL){
                    printf("Falha no processamento do arquivo.");
                    exit(1);
                }else{
                    //printf("Responsável 1\n");
                }
                if(newRegistro[j]->tamanhoRegistro > tamMaxBuscado && CabecalhoLocal->topo != -1){
                    tamMaxBuscado = newRegistro[j]->tamanhoRegistro;
                    preencherLista(listaOrdenada, &byteinicial, tamMaxBuscado, &posicao, fp);
                }
                //remove do cabecalho
                //printf("ByteOffSet: %lld\n", RegistrodeOffsets[j]);
                //imprimirjogador(newRegistro[j]);
                if(CabecalhoLocal->topo == -1){ //Caso a lista ainda não exista
                //printf("Caso de lista vazia\n");
                    RegistroLista->tamanho = newRegistro[j]->tamanhoRegistro;
                    RegistroLista->byteoffset = -1;
                    listaOrdenada[0] = RegistroLista;
                    fseek(fp, RegistrodeOffsets[j], SEEK_SET); //Preciso escrever no registro como removido e manter a lista ordenada
                    fwrite(&newRegistro[j]->removido, sizeof(char), 1, fp);
                    fwrite(&newRegistro[j]->tamanhoRegistro, sizeof(int), 1, fp);
                    fwrite(&RegistroLista->byteoffset, sizeof(long long int), 1, fp); //atualizo o proximo para manter a lista encadeada
                    CabecalhoLocal->topo = RegistrodeOffsets[j];
                    //printf("Aumentarei 1 em posicao, que está nesse caso em %d", posicao);
                    posicao++;
                    byteinicial = -1;
                      //              printf("removi1\n");
                }else if(newRegistro[j]->tamanhoRegistro < listaOrdenada[0]->tamanho){ //Tenho uma lista, mas preciso inserir o removido como primeiro elemento
                    //printf("Caso de insercao no comeco\n");
                    for(int i = posicao-1; i >= 0; i--){ //Tem que ser posicao -1, porque posicao sempre aponta para o depois do ultimo
                        listaOrdenada[i+1]=listaOrdenada[i];
                    }
                    RegistroLista->tamanho = newRegistro[j]->tamanhoRegistro;
                    RegistroLista->byteoffset = CabecalhoLocal->topo;
                    listaOrdenada[0] = RegistroLista;
                    CabecalhoLocal->topo = RegistrodeOffsets[j];
                    fseek(fp, RegistrodeOffsets[j], SEEK_SET); //Preciso escrever no registro como removido e manter a lista ordenada
                    fwrite(&newRegistro[j]->removido, sizeof(char), 1, fp);
                    fwrite(&newRegistro[j]->tamanhoRegistro, sizeof(int), 1, fp);
                    fwrite(&RegistroLista->byteoffset, sizeof(long long int), 1, fp); //atualizo o proximo para manter a lista encadeada
                    //printf("Aumentarei 1 em posicao, que está nesse caso em %d", posicao);
                    posicao++;
                        //printf("removi2\n");
                }else if(newRegistro[j]->tamanhoRegistro > listaOrdenada[posicao-1]->tamanho){ //Se o espaço que procuro é maior que o espaco de todo mundo que tenho na lista, entao coloco no fim
                    //printf("Caso de insercao no fim\n");
                    RegistroLista->tamanho = newRegistro[j]->tamanhoRegistro;
                    RegistroLista->byteoffset = -1;
                    listaOrdenada[posicao] = RegistroLista;
                    listaOrdenada[posicao-1]->byteoffset = RegistrodeOffsets[j];
                    if(posicao == 1){
                        fseek(fp, (CabecalhoLocal->topo)+5, SEEK_SET); //se eu tiver so 1 elemento na lista, e voltar 2 ele da segfault
                    }else{
                        fseek(fp, (listaOrdenada[posicao-2]->byteoffset)+5, SEEK_SET); //tenho pelo menos 2, então da para voltar no 0
                    }
                    //printf("O anterior a ele na lista eh: %lld\n", listaOrdenada[posicao-2]->byteoffset);
                    //printf("O byte off set desse jogador eh: %lld", RegistrodeOffsets[j]);
                    fwrite(&RegistrodeOffsets[j], sizeof(long long int), 1, fp);
                    fseek(fp, RegistrodeOffsets[j], SEEK_SET); //Preciso escrever no registro como removido e manter a lista ordenada
                    fwrite(&newRegistro[j]->removido, sizeof(char), 1, fp);
                    fwrite(&newRegistro[j]->tamanhoRegistro, sizeof(int), 1, fp);
                    fwrite(&RegistroLista->byteoffset, sizeof(long long int), 1, fp); //atualizo o proximo para manter a lista encadeada
                    //printf("Aumentarei 1 em posicao, que está nesse caso em %d", posicao);
                    posicao++;
                    byteinicial = -1;
                    //               printf("removi3\n");
                }else{ //Insiro no meio da lista
                    //printf("Caso de insercao no meio\n");
                    //printf("%d\n\n", listaOrdenada[0]->tamanho);
                int indice = busca_anterior(listaOrdenada, posicao, newRegistro[j]->tamanhoRegistro);//retorna o indice do elemento que está logo atrás do elemento a ser inserido
                //printf("Depois da busca\n");
                //printf("Variavel posicao: %d\n", posicao);
                //printf("Variavel indice: %d\n", indice);
                //printf("Tamanho: %d", newRegistro[j]->tamanhoRegistro);
                //printf("Tamanho da cabeça: %d\n", listaOrdenada[0]->tamanho);
                //printf("Tamanho da calda: %d\n", listaOrdenada[1]->tamanho);
                RegistroLista->tamanho = newRegistro[j]->tamanhoRegistro;
                RegistroLista->byteoffset = listaOrdenada[indice]->byteoffset;//Faco ele apontar para onde o anterior a ele apontava
                if(indice == 0){
                    fseek(fp, (CabecalhoLocal->topo)+5, SEEK_SET); //Preciso colocar no proprio registro o anterior ao novo removido apontar para ele
                }else{
                    fseek(fp, (listaOrdenada[indice-1]->byteoffset)+5, SEEK_SET); //Preciso colocar no proprio registro o anterior ao novo removido apontar para ele
                }
                fwrite(&RegistrodeOffsets[j], sizeof(long long int), 1, fp);
                listaOrdenada[indice]->byteoffset = RegistrodeOffsets[j]; //atualizo isso na memoria ram tambem
                fseek(fp, RegistrodeOffsets[j], SEEK_SET); //Preciso escrever no registro como removido e manter a lista ordenada
                fwrite(&newRegistro[j]->removido, sizeof(char), 1, fp);
                fwrite(&newRegistro[j]->tamanhoRegistro, sizeof(int), 1, fp);
                fwrite(&RegistroLista->byteoffset, sizeof(long long int), 1, fp); //atualizo o proximo para manter a lista encadeada
                for(int i = posicao-1; i >= indice+1; i--){ //Tem que ser posicao -1, porque posicao sempre aponta para o depois do ultimo
                    listaOrdenada[i+1]=listaOrdenada[i];
                }
                listaOrdenada[indice+1] = RegistroLista;
                //printf("Aumentarei 1 em posicao, que está nesse caso em %d", posicao);
                posicao++;
                //printf("Caso de remocao no meio\n");
                //printf("removido45\n");
                }
                remover_arquivo_indice(IndiceLocal, CabecalhoLocal->nroRegArq+1, newRegistro[j]);
                IndiceLocal = (indexDados**) realloc (IndiceLocal,(CabecalhoLocal->nroRegArq)*sizeof(indexDados*));
                if(IndiceLocal == NULL){
                    printf("Falha no processamento do arquivo.");
                }
            }
        //*/
        }
        for(int i = 0; i < numeroResultados; i++){ // da free nos registros da ultima iteracao, os mesmos que foram impressos
            free(newRegistro[i]->nomeJogador);
            free(newRegistro[i]->nacionalidade);
            free(newRegistro[i]->nomeClube);
            free(newRegistro[i]);
            newRegistro[i] = NULL;
        }
        free(newRegistro);
    } // comeca a proxima busca

    for(int i = 0; i < posicao; i++){
        free(listaOrdenada[i]);
    }
    free(listaOrdenada);

    //printf("Fim nroRegArq: %d\n", CabecalhoLocal->nroRegArq);
    if(!escrever_dados_indice(IndiceLocal, CabecalhoLocal->nroRegArq, fpIndex)){
        printf("Falha no processamento do arquivo.");
        exit(1);
    }
    fclose(fpIndex);

    for(int i = 0; i < CabecalhoLocal->nroRegArq; i++){
        free(IndiceLocal[i]);
    }
    free(IndiceLocal);

    fseek(fp, 0, SEEK_SET);
    CabecalhoLocal->status = '1';
    fwrite(&CabecalhoLocal->status, sizeof(char), 1, fp);
    fwrite(&CabecalhoLocal->topo, sizeof(long long int), 1, fp);
    fwrite(&CabecalhoLocal->proxByteOffset, sizeof(long long int), 1, fp);
    fwrite(&CabecalhoLocal->nroRegArq, sizeof(int), 1, fp);
    fwrite(&CabecalhoLocal->nroRegRem, sizeof(int), 1, fp);

    for(int i = 0; i < numeroResultadosFixo; i++){ // free nos registros originais que foram retirados do arquivo
        free(RegistrosLocais[i]->nomeJogador);
        free(RegistrosLocais[i]->nacionalidade);
        free(RegistrosLocais[i]->nomeClube);
        free(RegistrosLocais[i]);
        RegistrosLocais[i] = NULL;
    }
    free(RegistrosLocais);
    free(ByteSets);
    free(CabecalhoLocal);

    fclose(fp);
    binarioNaTela(arquivo1);
    binarioNaTela(arquivo2);
}

//reescreva o topo do cabecalho local, voce nao fez isso