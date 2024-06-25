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
#include "fila.h"

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

struct listaDados{
    int tamanho;
    long long byteoffset;
};

typedef struct ArvBcab{
    char status;
    int noRaiz;
    int proxRRN;
    int nroChaves;
}CabArvB;

typedef struct ArvBdados{
    int alturaNo;
    int NroChaves;
    int Chaves[3];
    long long int ByteSets[3];
    int ProxRRN[4];
}NoArvB;

void Driver_ArvB(indexDados ElementoArvB, CabArvB * CabecalhoArvB, FILE* fpArvB);
void imprimirPaginadeDisco(NoArvB* PaginaArvB);
void escrever_PaginaNoDisco(NoArvB* PaginaArvB, int RRN, FILE* fp);
void imprimirPaginadeSplit(int Chaves[], long long int ByteSets[], int ProxRRN[]);
void imprimirArvoreComFila(int RRN, FILE* fp);
void ler_PaginaNoDisco(NoArvB* PaginaArvB, int RRN, FILE* fp);
void split(NoArvB* PaginaArvB, NoArvB* NovaPaginaArvB, int ChavesSplit[], long long int ByteSetsSplit[], int ProxSplit[], int* id_promovido, long long int* ByteSetPromovido, int* paginaFilha, CabArvB * CabecalhoArvB);
void insercao_ArvB(int NroPaginaAtual, int id_buscado, long long int ByteSetDoID, int* id_promovido, long long int* ByteSetPromovido, int* paginaFilha, int* promocao, CabArvB * CabecalhoArvB, FILE* fpArvB);
bool constroi_arvoreB(int tamanho, FILE* fp, CabArvB * CabecalhoArvB, FILE* fpArvB); //Não trago nada para a memória primária, apenas crio o indice fazendo freads e fseek's

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

    long long int *ByteSets;
    ByteSets = (long long int*) malloc(CabecalhoLocal->nroRegArq * sizeof(long long int));
    if (ByteSets == NULL) {
        printf("Falha no processamento do arquivo.");
        fclose(fp);
        return;
    }
    
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

                newRegistro = filtra(newRegistro, &numeroResultados, 1, RegistrodeOffsets, 3, IndiceLocal, fp, 0, ByteSets, tamanho_real);

            }else{  // caso nao seja o primeiro, filtra o conjunto ja filtrado na iteracao anterior, alocando novamente o novo e dando free no anterior
                regDados** tempReg = newRegistro;
                int tempNum = numeroResultados;
                newRegistro = filtra(newRegistro, &numeroResultados, 2, RegistrodeOffsets, 3, IndiceLocal, fp, 0, ByteSets, tamanho_real);
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
    free(ByteSets);
    fclose(fp);
}

void function4(){ //Le os dados de um arquivo ".bin"  e cria um arquivo de indice para esse binário lido
    char arquivo1[30]; //Arquivo de entrada ".bin"
    scanf("%s", arquivo1);
    char arquivo2[30]; //Arquivo de saida indice ".bin"
    scanf("%s", arquivo2);
    FILE *fp; //Ponteiro para o controle do arquivo de dados
    FILE *fpIndex; //Ponteiro para o controle do indice
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
    
    if(CabecalhoLocal->status == '0'){ //Checo se há algum erro no meu arquivo de dados.
        printf("Falha no processamento do arquivo.");
        return;
    }

    if(CabecalhoLocal->nroRegArq == 0){ //Checo se existem registros no meu arquivo de dados.
        printf("Registro inexistente.\n\n");
        return;
    }

    int tamanho_real = CabecalhoLocal->nroRegArq+CabecalhoLocal->nroRegRem; //O tamanho que preciso realmente buscar são os ativos + os removidos
 
    indexDados** IndiceLocal;
    if((IndiceLocal = (indexDados**) malloc ((CabecalhoLocal->nroRegArq)*sizeof(indexDados*))) == NULL){ //Aloco o tamanho exato
        printf("Falha no processamento do arquivo.");
        fclose(fp);
        return;
    }

    constroi_indice_sem_registro(IndiceLocal, tamanho_real, fp); //Contruo meu indice
    fclose(fp);

    if (((fpIndex = fopen(arquivo2, "wb"))) == NULL){ //Abro o binário que sera o destino do indice para escrita
        printf("Falha no processamento do arquivo.");
        return;
    }

    escrever_dados_indice(IndiceLocal, CabecalhoLocal->nroRegArq, fpIndex); //escrevo os dados no meu arquivo de indice
    fclose(fpIndex);

    for(int i = 0; i < CabecalhoLocal->nroRegArq; i++){ //libero a memória que foi alocada
        free(IndiceLocal[i]);
    }
    free(IndiceLocal);

    free(CabecalhoLocal);
    binarioNaTela(arquivo2);
}

void function5(){ //Recebe um registro e escreve como logicamente removido no arquivo de dados, e remove realmente do indice
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
    
    if(CabecalhoLocal->status == '0'){ //Testo o status do arquivo que estou lendo
        printf("Falha no processamento do arquivo.");
        fclose(fp);
        return;
    }

    if(CabecalhoLocal->nroRegArq == 0){//Testo se possuo registros no arquivo de dados que quero remover
        printf("Registro inexistente.\n\n");
        fclose(fp);
        return;
    }

    int tamanho_real = CabecalhoLocal->nroRegArq+CabecalhoLocal->nroRegRem; //O tamanho que preciso realmente buscar são os ativos + os removidos

    long long int *ByteSets; //Guardo aqui os byteoffsets lidos do arquivo.
    ByteSets = (long long int*) malloc(CabecalhoLocal->nroRegArq * sizeof(long long int));
    if (ByteSets == NULL) {
        printf("Falha no processamento do arquivo.");
        fclose(fp);
        return;
    }

    fseek(fp, 0, SEEK_SET);
    CabecalhoLocal->status = '0'; //A partir de agora vou editar meu arquivo, então aviso que pode haver falha na integridade do arquivo de dados
    fwrite(&CabecalhoLocal->status, sizeof(char), 1, fp);

    indexDados** IndiceLocal;
    if((IndiceLocal = (indexDados**) malloc ((CabecalhoLocal->nroRegArq)*sizeof(indexDados*))) == NULL){ //Aloco o tamanho exato
        printf("Falha no processamento do arquivo.");
        fclose(fp);//
        return;
    }
    char indexCab;

    if (((fpIndex = fopen(arquivo2, "rb+"))) == NULL){ //Abre o indice para leitura e escrita
        //entra no if se falhar em abrir o arquivo, ou seja, arquivo de indice nao existe
        int tamanho_real = CabecalhoLocal->nroRegArq+CabecalhoLocal->nroRegRem; // salva o numero de registros
        fseek(fp, 25, SEEK_SET);

        constroi_indice_sem_registro(IndiceLocal, tamanho_real, fp); //constroi o indice
 
        if (((fpIndex = fopen(arquivo2, "wb"))) == NULL){ //Abre o indice para leitura e escrita
            printf("Falha no processamento do arquivo.");
            return;
        }
    }else{
        fread(&indexCab, sizeof(char), 1, fpIndex); //Le as informacoes do cabecalho
        if(indexCab == '0'){
            printf("Falha no processamento do arquivo.");
            exit(1);
        }
        ler_index(IndiceLocal, CabecalhoLocal->nroRegArq, fpIndex); // preenche o indiceLocal
    }

    indexCab = '0';
    fseek(fpIndex, 0, SEEK_SET);  //A partir de agora vou editar meu arquivo, então aviso que pode haver falha na integridade do arquivo de indice
    fwrite(&indexCab, sizeof(char), 1, fpIndex);

    listaDados** listaOrdenada;
    if((listaOrdenada = (listaDados**) malloc (((CabecalhoLocal->nroRegRem))*sizeof(listaDados*))) == NULL){ //Como a lista ordenada é leve (12bytes _ int+long int) vou utilizar ela na RAM para evitar fseek's desnecessários que consomem muito processamento
        fclose(fp);
        return;
    }

    int tamMaxBuscado = 0; //é o maior tamanho que foi buscado até o momento, isso evitar trazer elementos novamente, trago elementos apenas por demanda
    int posicao = 0; //Diz respeito a qual a proxima posicao que irei gravar no vetor da lista ordenada, basicamente seu tamanho
    long long int byteinicial = CabecalhoLocal->topo; //Essa variavel diz a partir de qual byte eu já trouxe, para trazer estritamente apenas elementos novos, e só até o tamanho necessário, também é a flag de já trouxe todos
    int numeroResultadosFixo = CabecalhoLocal->nroRegArq; // salva quantos registros tem no arquivo
    int numeroResultados; // numero de registros que diminui apos serem filtrados
    int numeroBuscas; // quantidade de buscas a serem feitas
    int numeroCampos; // quantos filtros serão aplicados a uma dada busca
    long long int RegistrodeOffsets[numeroResultadosFixo]; //Vetor onde atualizo e guardo os byteoffsets daqueles que foram encontrados
    regDados** newRegistro;

    scanf("%d", &numeroBuscas);
    for(int i = 0; i < numeroBuscas; i++){ // loop em que cada iteracao sera uma busca diferente
        numeroResultados = numeroResultadosFixo; // numero de registros comeca sendo do tamanho de todos os registros
        scanf("%d", &numeroCampos);
        for(int j = 0; j < numeroCampos; j++){ // loop em que cada iteracao sera um filtro diferente a ser aplicado aos registros
            if(j == 0){ // caso seja o primeiro filtro, aplica filtro ao conjunto de registros completo

                newRegistro = filtra(newRegistro, &numeroResultados, 1, RegistrodeOffsets, 5, IndiceLocal, fp, numeroResultadosFixo, ByteSets, tamanho_real);

            }else{  // caso nao seja o primeiro, filtra o conjunto ja filtrado na iteracao anterior, alocando novamente o novo e dando free no anterior
                regDados** tempReg = newRegistro;
                int tempNum = numeroResultados;
                newRegistro = filtra(newRegistro, &numeroResultados, 2, RegistrodeOffsets, 5, IndiceLocal, fp, numeroResultadosFixo, ByteSets, tamanho_real);
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
        if(numeroResultados == 0){
            //printf("Registro inexistente.\n\n");
        }else{
            for(int j = 0; j < numeroResultados; j++){ //j irá percorrer todos os meus resultados
                newRegistro[j]->removido = '1'; //Marco o registro encontrado pela busca como removido.
                listaDados* RegistroLista;
                CabecalhoLocal->nroRegRem++; //Diminui o numero de registros ativos no arquivo
                CabecalhoLocal->nroRegArq--; //Aumenta o numero de registros removidos
                if((listaOrdenada = (listaDados**) realloc (listaOrdenada,((CabecalhoLocal->nroRegRem))*sizeof(listaDados*))) == NULL){ //Realoco um tamanho a mais na lista encadeada de arquivos removidos
                    printf("Falha no processamento do arquivo.");
                    exit(1);
                }
                if((RegistroLista = (listaDados*) malloc (sizeof(listaDados))) == NULL){ //Aloco um novo elemento de minha lista ordenada de arquivos removidos
                    printf("Falha no processamento do arquivo.");
                    exit(1);
                }
                if(newRegistro[j]->tamanhoRegistro > tamMaxBuscado && CabecalhoLocal->topo != -1){ //O preenchimento da lista acontece por demanda, então eu só trago para o ram até o ultimo elemento máximo que foi necessário, e mantenho quem já está para uma busca mais rápida.

                    tamMaxBuscado = newRegistro[j]->tamanhoRegistro; //Atualizo o tamanho máximo, para só atualizar a lista se o elemento for maior que ele
                    preencherLista(listaOrdenada, &byteinicial, tamMaxBuscado, &posicao, fp);
                }
                if(CabecalhoLocal->topo == -1){ //Caso em que a lista de Registros removidos ainda não existe, então ela será iniciada
                    RegistroLista->tamanho = newRegistro[j]->tamanhoRegistro; //Tamanho do registro deletado
                    RegistroLista->byteoffset = -1; //O proximo do registro deletado será -1;
                    listaOrdenada[0] = RegistroLista; //Recebo o elemento que irá compor a lista ordenada que está em RAM
                    excluir_registro(newRegistro[j], RegistrodeOffsets[j], fp, &posicao, RegistroLista);
                    CabecalhoLocal->topo = RegistrodeOffsets[j]; //O topo agora aponta para o byteoffset desse elemento
                    byteinicial = -1; //Flag que indica que já trouxe todos os elementos possíveis da lista
                }else if(newRegistro[j]->tamanhoRegistro < listaOrdenada[0]->tamanho){ //Caso em que já existe uma lista de removidos, mas quero adicionar um elemento na primeira posição da lista
                    for(int i = posicao-1; i >= 0; i--){ //Desloco todos os elementos da lista uma posição para frente
                        listaOrdenada[i+1]=listaOrdenada[i];
                    }
                    RegistroLista->tamanho = newRegistro[j]->tamanhoRegistro; //Atualizo o elemento que será inserido na lista de removidos
                    RegistroLista->byteoffset = CabecalhoLocal->topo;
                    listaOrdenada[0] = RegistroLista; //Atualizo a lista própriamente
                    CabecalhoLocal->topo = RegistrodeOffsets[j]; //O topo irá apontar para esse elemento a partir de agora
                    excluir_registro(newRegistro[j], RegistrodeOffsets[j], fp, &posicao, RegistroLista);
                }else if(newRegistro[j]->tamanhoRegistro > listaOrdenada[posicao-1]->tamanho){ //Caso em que vou inserir um elemento no fim da lista, pois seu tamanho é maior que o de todos
                    RegistroLista->tamanho = newRegistro[j]->tamanhoRegistro; //Atualizo o elemento que será inserido na lista de removidos
                    RegistroLista->byteoffset = -1; //Como ele é o ultimo, seu byteoffset deve ser -1
                    listaOrdenada[posicao] = RegistroLista; //Atualizo a lista própriamente
                    listaOrdenada[posicao-1]->byteoffset = RegistrodeOffsets[j]; //Nesse caso possuo pelo menos 1 elemento, então o elemento anterior a ele deve apontar para o byteoffset do atual
                    if(posicao == 1){
                        fseek(fp, (CabecalhoLocal->topo)+5, SEEK_SET); //se eu tiver so 1 elemento na lista, o anterior a ele é o topo
                    }else{
                        fseek(fp, (listaOrdenada[posicao-2]->byteoffset)+5, SEEK_SET); //se eu tiver mais de 1 elemento então basta acessar o byteoffset anterior para endereçar e escrever no registro de dados
                    }
                    fwrite(&RegistrodeOffsets[j], sizeof(long long int), 1, fp);
                    excluir_registro(newRegistro[j], RegistrodeOffsets[j], fp, &posicao, RegistroLista);
                    byteinicial = -1;
                }else{ //Caso de inserção em um lugar qualquer no meio da lista
                    int indice = busca_anterior(listaOrdenada, posicao, newRegistro[j]->tamanhoRegistro); //Procuro o indice do elemento que vem logo antes do elemento que de fato eu quero inserir
                    RegistroLista->tamanho = newRegistro[j]->tamanhoRegistro;  //Atualizo o elemento que será inserido na lista de removidos
                    RegistroLista->byteoffset = listaOrdenada[indice]->byteoffset;
                    if(indice == 0){ //Caso eu queira inserir na segunda posição do vetor [x,o,x,x,x,x]
                        fseek(fp, (CabecalhoLocal->topo)+5, SEEK_SET); //Preciso indereçar a atualização do primeiro elemento com o topo 
                    }else{
                        fseek(fp, (listaOrdenada[indice-1]->byteoffset)+5, SEEK_SET); //Basta eu voltar uma posição já que o elemento anterior aponta para o registro (que é o imediatamente anterior) ao que vou remover
                    }
                    fwrite(&RegistrodeOffsets[j], sizeof(long long int), 1, fp);
                    listaOrdenada[indice]->byteoffset = RegistrodeOffsets[j];  //Atualizo o elemento que será inserido na lista de removidos
                    excluir_registro(newRegistro[j], RegistrodeOffsets[j], fp, &posicao, RegistroLista);
                    for(int i = posicao-2; i >= indice+1; i--){ //Posição-1 porque é um vetor, e os elemento sempre começam em zero
                        listaOrdenada[i+1]=listaOrdenada[i];
                    }
                    listaOrdenada[indice+1] = RegistroLista;
                    }
                    remover_arquivo_indice(IndiceLocal, CabecalhoLocal->nroRegArq+1, newRegistro[j]); //Removo o arquivo do indice e desloco os elementos que for necessário
                    IndiceLocal = (indexDados**) realloc (IndiceLocal,(CabecalhoLocal->nroRegArq)*sizeof(indexDados*)); //Readequo o tamanho do indice local
                    if(IndiceLocal == NULL){
                        printf("Falha no processamento do arquivo.");
                    }
            }
        }
        for(int i = 0; i < numeroResultados; i++){ // da free nos registros da ultima iteracao
            free(newRegistro[i]->nomeJogador);
            free(newRegistro[i]->nacionalidade);
            free(newRegistro[i]->nomeClube);
            free(newRegistro[i]);
            newRegistro[i] = NULL;
        }
        free(newRegistro);
    } // comeca a proxima busca

    for(int i = 0; i < posicao; i++){ //Free nos elementos da lista de removidos
        free(listaOrdenada[i]);
    }
    free(listaOrdenada);

    if(!escrever_dados_indice(IndiceLocal, CabecalhoLocal->nroRegArq, fpIndex)){ //Escrevo os elementos no indice
        printf("Falha no processamento do arquivo.");
        exit(1);
    }
    fclose(fpIndex);

    for(int i = 0; i < CabecalhoLocal->nroRegArq; i++){ //Libero a memória alocada para o indice
        free(IndiceLocal[i]);
    }
    free(IndiceLocal);

    fseek(fp, 0, SEEK_SET);
    CabecalhoLocal->status = '1'; //Escrevo que agora o arquivo está corretamente alterado
    fwrite(&CabecalhoLocal->status, sizeof(char), 1, fp);
    fwrite(&CabecalhoLocal->topo, sizeof(long long int), 1, fp);
    fwrite(&CabecalhoLocal->proxByteOffset, sizeof(long long int), 1, fp);
    fwrite(&CabecalhoLocal->nroRegArq, sizeof(int), 1, fp);
    fwrite(&CabecalhoLocal->nroRegRem, sizeof(int), 1, fp);

    free(ByteSets);
    free(CabecalhoLocal);

    fclose(fp);
    binarioNaTela(arquivo1);
    binarioNaTela(arquivo2);
}

void function6(){ //deleta um registro  
    char arquivo1[30]; //Arquivo de entrada ".bin"
    scanf("%s", arquivo1);
    char arquivo2[30]; //Arquivo de indice ".bin"
    scanf("%s", arquivo2);
    FILE *fp;
    FILE *fpIndex;
    regCabecalho* CabecalhoLocal;
    
    if (((fp = fopen(arquivo1, "rb+"))) == NULL){ //Abre o binario para leitura e escrita
        printf("Falha no processamento do arquivo.");
        return;
    }

    if((CabecalhoLocal = (regCabecalho*) malloc (sizeof(regCabecalho))) == NULL){ //Aloca o cabecalho
        printf("Falha no processamento do arquivo.");
        return;
    }
    fread(&CabecalhoLocal->status, sizeof(char), 1, fp); //Le as informacoes do cabecalho
    fread(&CabecalhoLocal->topo, sizeof(long long int), 1, fp);
    fread(&CabecalhoLocal->proxByteOffset, sizeof(long long int), 1, fp);
    fread(&CabecalhoLocal->nroRegArq, sizeof(int), 1, fp);
    fread(&CabecalhoLocal->nroRegRem, sizeof(int), 1, fp);
    
    if(CabecalhoLocal->status == '0'){
        printf("Falha no processamento do arquivo.");
        fclose(fp);
        return;
    }

    indexDados** IndiceLocal;
    if((IndiceLocal = (indexDados**) malloc ((CabecalhoLocal->nroRegArq)*sizeof(indexDados*))) == NULL){ //Aloca o tamanho exato do indice
        printf("Falha no processamento do arquivo.");
        fclose(fp);//
        return;
    }
    char indexCab;

    if (((fpIndex = fopen(arquivo2, "rb+"))) == NULL){ //Abre o indice para leitura e escrita
        //entra no if se falhar em abrir o arquivo, ou seja, arquivo de indice nao existe
        int tamanho_real = CabecalhoLocal->nroRegArq+CabecalhoLocal->nroRegRem; // salva o numero de registros 

        constroi_indice_sem_registro(IndiceLocal, tamanho_real, fp); //constroi o indice
 
        if (((fpIndex = fopen(arquivo2, "wb"))) == NULL){ //Abre o indice para leitura e escrita
            printf("Falha no processamento do arquivo.");
            return;
        }
    }else{
        fread(&indexCab, sizeof(char), 1, fpIndex); //Le as informacoes do cabecalho
        if(indexCab == '0'){
            printf("Falha no processamento do arquivo.");
            exit(1);
        }
        ler_index(IndiceLocal, CabecalhoLocal->nroRegArq, fpIndex); // preenche o indiceLocal
    }

    indexCab = '0';
    fseek(fpIndex, 0, SEEK_SET);
    fwrite(&indexCab, sizeof(char), 1, fpIndex); //define o bit de status para 0 e escreve ele no arquivo de indice

    listaDados** listaOrdenada;
    if((listaOrdenada = (listaDados**) malloc (((CabecalhoLocal->nroRegRem))*sizeof(listaDados*))) == NULL){ //aloca a lista ordenada de registros removidos
        printf("Falha no processamento do arquivo.");
        return;
    }

    int tamMaxBuscado = 0; //e o maior tamanho que foi buscado ate o momento
    int posicao = 0; //Diz respeito a qual a proxima posicao que irei gravar no vetor da lista ordenada
    long long int byteinicial = CabecalhoLocal->topo; // primeiro byte da lista ordenada de elementos removidos que eu ainda nao acessei
    int numeroInsercoes; // quantidade de insercoes a serem feitas

    scanf("%d", &numeroInsercoes);
    for(int i = 0; i < numeroInsercoes; i++){ // loop em que cada iteracao sera uma insercao diferente

        regDados* jogador;
        criaJogador(&jogador); // registro foi criado, alocado e preenchido

        int tamanhoReal = jogador->tamanhoRegistro;// salva o tamanho do registro que esta sendo inserido
        tamMaxBuscado = tamanhoReal;

        indexDados* indice = (indexDados*) malloc(sizeof(indexDados));// indice que sera inserido no arquivo de indices
        indice->id = jogador->id;
        
        if(preencherLista(listaOrdenada, &byteinicial, tamMaxBuscado, &posicao, fp) == 0){//preencherLista == 0 quando tamanho do registro > maior elemento da lista de removidos
            escreveJogador(jogador, CabecalhoLocal->proxByteOffset, fp); // escreve o registro no arquivo
            long long int localdainsercao = CabecalhoLocal->proxByteOffset; // salva local da insercao para salvar no indice
            CabecalhoLocal->nroRegArq++;
            CabecalhoLocal->proxByteOffset = CabecalhoLocal->proxByteOffset + tamanhoReal;
            resolveindice(indice, localdainsercao, &IndiceLocal, CabecalhoLocal); // realoca e insere ordenadamente o novo indice no indiceLocal
        }else{// o registro sera inserido por cima de um outro registro ja excluido
            
            int tamanhobuscando = listaOrdenada[0]->tamanho;// tamanho do registro excluido atual, vai se tornar o tamanho do registro sobreescrito
            int posicaodeinsercao = 1;//posicao do registro que sera sobreescrito na lista ordenada de registros removidos, 
            while(tamanhobuscando < tamanhoReal){
                tamanhobuscando = listaOrdenada[posicaodeinsercao]->tamanho;
                posicaodeinsercao++;
            }
            posicaodeinsercao--;

            jogador->tamanhoRegistro = tamanhobuscando;// tamanho do registro que sera inserido se torna o tamanho do registro excluido

            char nullchar = '$';
            if(posicaodeinsercao == 0){// o primeiro elemento da lista ordenada de registros excluidos sera sobreescrito
                escreveJogador(jogador, CabecalhoLocal->topo, fp); // escreve registro no arquivo
                long long int localdainsercao = CabecalhoLocal->topo; // salva local da insercao para escrever no indice
                CabecalhoLocal->topo = listaOrdenada[0]->byteoffset; // muda o comeco da lista ordenada de registros removidos
                CabecalhoLocal->nroRegRem--;
                CabecalhoLocal->nroRegArq++;
                for(int i = 0; i < tamanhobuscando - tamanhoReal; i++){
                    fwrite(&nullchar, sizeof(char), 1, fp); // escreve '$' nos espacos que sobraram
                }
                resolveindice(indice, localdainsercao, &IndiceLocal, CabecalhoLocal); // realoca e insere ordenadamente o novo indice no indiceLocal
            }else{ // algum outro elemento da lista ordenada de registros excluidos sera sobreescrito
                long long int byteoffsetdainsercao = listaOrdenada[posicaodeinsercao-1]->byteoffset; // salva byteoffset da insercao para escrever no indice
                listaOrdenada[posicaodeinsercao-1]->byteoffset = listaOrdenada[posicaodeinsercao]->byteoffset;
                escreveJogador(jogador, byteoffsetdainsercao, fp); // escreve registro no arquivo
                CabecalhoLocal->nroRegRem--;
                CabecalhoLocal->nroRegArq++;
                for(int i = 0; i < tamanhobuscando - tamanhoReal; i++){
                    fwrite(&nullchar, sizeof(char), 1, fp); // escreve '$' nos espacos que sobraram
                }
                if(posicaodeinsercao == 1) // altera o elemento anterior para manter a lista encadeada
                    fseek(fp, CabecalhoLocal->topo + sizeof(int) + sizeof(char), SEEK_SET);
                else
                    fseek(fp, listaOrdenada[posicaodeinsercao-2]->byteoffset + sizeof(int) + sizeof(char), SEEK_SET);
                fwrite(&listaOrdenada[posicaodeinsercao]->byteoffset, sizeof(long long int), 1, fp);

                resolveindice(indice, byteoffsetdainsercao, &IndiceLocal, CabecalhoLocal); // realoca e insere ordenadamente o novo indice no indiceLocal
            }

            free(listaOrdenada[posicaodeinsercao]); // free no elemento que foi removido da lista ordenada (sobreescrito)
            for(int j = posicaodeinsercao; j < posicao - 1; j++){ // desloca a lista ordenada de elementos removidos para refletir seu novo encadeamento
                listaOrdenada[j] = listaOrdenada[j+1];
            }
            posicao--; // lista ordenada de elementos removidos tem um elemento a menos
            if(posicaodeinsercao == posicao){ // se o elemento removido for o ultimo, altera o byteinicial
                byteinicial = listaOrdenada[posicao-1]->byteoffset;
            }
    
        }
        free(jogador->nomeJogador); // registro ja foi escrito onde devia, é necessario free antes da proxima iteracao do loop
        free(jogador->nacionalidade);
        free(jogador->nomeClube);
        free(jogador);
    }

    if(!escrever_dados_indice(IndiceLocal, CabecalhoLocal->nroRegArq, fpIndex)){ //escreve os dados no indice
        printf("Falha no processamento do arquivo.15");
        exit(1);
    }

    fseek(fp, 0, SEEK_SET);
    CabecalhoLocal->status = '1'; // escreve todas as mudancas do cabecalho
    fwrite(&CabecalhoLocal->status, sizeof(char), 1, fp);
    fwrite(&CabecalhoLocal->topo, sizeof(long long int), 1, fp);
    fwrite(&CabecalhoLocal->proxByteOffset, sizeof(long long int), 1, fp);
    fwrite(&CabecalhoLocal->nroRegArq, sizeof(int), 1, fp);
    fwrite(&CabecalhoLocal->nroRegRem, sizeof(int), 1, fp);

    for(int i = 0; i < posicao; i++){ // free e fclose de tudo que foi alocado e aberto
        free(listaOrdenada[i]);
    }
    free(listaOrdenada);
    fclose(fp);
    fclose(fpIndex);
    for(int i = 0; i < CabecalhoLocal->nroRegArq; i++){
        free(IndiceLocal[i]);
    }
    free(IndiceLocal);
    free(CabecalhoLocal);

    binarioNaTela(arquivo1); // output 
    binarioNaTela(arquivo2);
}

void function7(){ //Le os dados de um arquivo ".bin"  e cria um arquivo de indice para esse binário lido
    char arquivo1[30]; //Arquivo de entrada ".bin"
    scanf("%s", arquivo1);
    char arquivo2[30]; //Arquivo de saida indice ".bin"
    scanf("%s", arquivo2);
    FILE *fp; //Ponteiro para o controle do arquivo de dados
    FILE *fpArvB; //Ponteiro para o controle do indice
    if (((fp = fopen(arquivo1, "rb+"))) == NULL){ //Abro o binário para leitura
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
    
    if(CabecalhoLocal->status == '0'){ //Checo se há algum erro no meu arquivo de dados.
        printf("Falha no processamento do arquivo.");
        return;
    }

    if(CabecalhoLocal->nroRegArq == 0){ //Checo se existem registros no meu arquivo de dados.
        printf("Registro inexistente.\n\n");
        return;
    }

    int tamanho_real = CabecalhoLocal->nroRegArq+CabecalhoLocal->nroRegRem; //O tamanho que preciso realmente buscar são os ativos + os removidos
 
    CabArvB * CabecalhoArvB;
    CabecalhoArvB = (CabArvB*) malloc (sizeof(CabArvB));
    if(CabecalhoArvB == NULL){
        exit(1);
    }

    if (((fpArvB = fopen(arquivo2, "wb+"))) == NULL){ //Abro o binário para leitura
        printf("Falha no processamento do arquivo.");
        return;
    }

    CabecalhoArvB->status = '0';
    fwrite(&CabecalhoArvB, sizeof(char), 1, fpArvB);
    CabecalhoArvB->noRaiz = -1;
    CabecalhoArvB->proxRRN = 0;
    CabecalhoArvB->nroChaves = 0;

    constroi_arvoreB(tamanho_real, fp, CabecalhoArvB, fpArvB); //Construo meu indice
    fclose(fp);

    CabecalhoArvB->status = '1';
    fseek(fpArvB, 0, SEEK_SET);
    fwrite(&CabecalhoArvB, sizeof(char), 1, fpArvB);

    fclose(fpArvB);
    free(CabecalhoArvB);
    free(CabecalhoLocal);
    binarioNaTela(arquivo2);
}

bool constroi_arvoreB(int tamanho, FILE* fp, CabArvB * CabecalhoArvB, FILE* fpArvB){ //Não trago nada para a memória primária, apenas crio o indice fazendo freads e fseek's
    int i = 0; //Indice que representa a quantidade de jogadores não removidos, lidos até o momento
    long long int Proxbyteoffset = 25; //Começo em 25 por considerar o tamanho do cabeçalho
    for(int a = 0; a < tamanho; a++){
        char teste_remocao;
        int tamRegLocal;
        fread(&teste_remocao, sizeof(char), 1, fp); //Verifico se é um arquivo removido, para não inseri-lo no indice
        fread(&tamRegLocal, sizeof(int), 1, fp);
        Proxbyteoffset+=tamRegLocal;
        if(teste_remocao == '1'){ //Se já estiver removido, vou para o próximo
            fseek(fp, tamRegLocal-5, SEEK_CUR);
        }else{
            int pulaLixo; //Quantos bytes precisso avançar lendo lixo
            char lixoCifrao; //Variavel descartável que recebe lixo
            regDados* jogador;
            indexDados* indice;
            if((jogador = (regDados*) malloc (sizeof(regDados))) == NULL){ //Aloco espaço para o jogador
                printf("Falha no processamento do arquivo.61\n");
                return false;
            }
            indexDados ElementoArvB;
            ElementoArvB.byteoffset = Proxbyteoffset - tamRegLocal;
            jogador->removido = teste_remocao; //Os valores dos campos continuam sendo lidos para posterior integração a funcionalidade da função 2
            jogador->tamanhoRegistro = tamRegLocal;
            pulaLixo = tamRegLocal;
            fread(&jogador->Prox, sizeof(long long int), 1, fp);
            fread(&jogador->id, sizeof(int), 1, fp);
            ElementoArvB.id = jogador->id;
            fread(&jogador->idade, sizeof(int), 1, fp);
            fread(&jogador->tamNomeJog, sizeof(int), 1, fp);
            pulaLixo -= 25;
            if((jogador->nomeJogador = (char*) malloc ((jogador->tamNomeJog)*sizeof(char))) == NULL){ 
                printf("Falha no processamento do arquivo.\n");
                return false;
            }
            for(int j = 0; j < jogador->tamNomeJog; j++){ //Leio byte a byte os caracters
                fread(&jogador->nomeJogador[j], sizeof(char), 1, fp);
            }
            pulaLixo=pulaLixo-jogador->tamNomeJog;
            fread(&jogador->tamNacionalidade, sizeof(int), 1, fp);
            if((jogador->nacionalidade = (char*) malloc ((jogador->tamNacionalidade)*sizeof(char))) == NULL){ 
                printf("Falha no processamento do arquivo.\n");
                return false;
            }
            for(int j = 0; j < jogador->tamNacionalidade; j++){ //Leio byte a byte os caracters
            fread(&jogador->nacionalidade[j], sizeof(char), 1, fp); 
            }
            pulaLixo= pulaLixo-jogador->tamNacionalidade-4;
            fread(&jogador->tamNomeClube, sizeof(int), 1, fp);
            if((jogador->nomeClube = (char*) malloc ((jogador->tamNomeClube)*sizeof(char))) == NULL){
                printf("Falha no processamento do arquivo.\n");
                return false;
            }
            for(int j = 0; j < jogador->tamNomeClube; j++){ //Leio byte a byte os caracters
                fread(&jogador->nomeClube[j], sizeof(char), 1, fp);
            }
            pulaLixo=pulaLixo-jogador->tamNomeClube-4;
            for(int i = 0; i < pulaLixo; i++){
                fread(&lixoCifrao, sizeof(char), 1, fp);
            }
            free(jogador->nomeJogador); //Libero a memória do jogador que mantive alocado
            free(jogador->nacionalidade);
            free(jogador->nomeClube);
            free(jogador);
            i++;
            Driver_ArvB(ElementoArvB, CabecalhoArvB, fpArvB);
            imprimirArvoreComFila(CabecalhoArvB->noRaiz, fpArvB);
            printf("Out\n");
        }
    }

    return true;
}

void insercao_ArvB(int NroPaginaAtual, int id_buscado, long long int ByteSetDoID, int* id_promovido, long long int* ByteSetPromovido, int* paginaFilha, int* promocao, CabArvB * CabecalhoArvB, FILE* fpArvB){
    //NroPaginaAtual = RRN da pagina que estou trazendo no momento; ByteSetdoID = Byteoffset do elemento novo que estou inserindo na arvore, id*_promovido = valor do id, do elemento que está sendo promovido
    //paginaFilha = Pagina a direita, do elemento que está sendo promovido, equivalente a novaPagina.
    if(NroPaginaAtual == -1){
        *id_promovido = id_buscado;
        *ByteSetPromovido = ByteSetDoID;
        *paginaFilha = -1;
        *promocao = 1;
        printf("Retornando split como 1, fim da recursão interna\n");
        return;
    }else{
        //falta atualizar o numero de chaves em cada iteraçao
        long long int RRN_atual = (NroPaginaAtual*60);
        //fseek(fpArvB, RRN_atual, SEEK_SET);
        NoArvB* PaginaArvB;
        PaginaArvB = (NoArvB*) malloc (sizeof(NoArvB));
        if(PaginaArvB == NULL){
            exit(1);
        }
        printf("O numero da pagina atual eh:%d\n", NroPaginaAtual);
        ler_PaginaNoDisco(PaginaArvB, NroPaginaAtual, fpArvB);
        //imprimirPaginadeDisco(PaginaArvB);
        printf("O id buscado eh:%d\n", id_buscado);
        int posicao = 0;
        for(int i = 0; i < PaginaArvB->NroChaves; i++){
            if(PaginaArvB->Chaves[i]>id_buscado){
                break;
            }
            posicao++;
        }
        printf("A posição que devo inserir claramente eh: %d\n", posicao);
        if(posicao < 3){ //Caso em que todos os nós da minha pagina são menores, ai não posso checar a posicão para testar se encontrei o nó
            if(PaginaArvB->Chaves[posicao] == id_buscado){
                *promocao = -1;
                return;
            }
        }
        printf("O proximo RRN na posicao eh: %d\n", PaginaArvB->ProxRRN[posicao]);
        //exit(1); /////////////////////////
        insercao_ArvB(PaginaArvB->ProxRRN[posicao], id_buscado, ByteSetDoID, id_promovido, ByteSetPromovido, paginaFilha, promocao, CabecalhoArvB, fpArvB);
        if(*promocao == 0){
            return;
        }else if(*promocao == 1 && (PaginaArvB->NroChaves < 3)){
                    printf("Não precisei fazer split\n");
            posicao = 0; //Não sei se vou inserir um elemento diferente do elemento que foi buscado inicalmente
            for(int i = 0; i < PaginaArvB->NroChaves; i++){
                if(PaginaArvB->Chaves[i]>*id_promovido){
                    break;
                }
                posicao++;
            }
            imprimirPaginadeDisco(PaginaArvB);
            printf("O numero de chaves eh: %d\n", PaginaArvB->NroChaves);
            printf("A posição que devo inserir claramente eh: %d\n", posicao);
            for(int i = 1; i >= posicao; i--){
                PaginaArvB->Chaves[i+1]=PaginaArvB->Chaves[i];
                PaginaArvB->ByteSets[i+1]=PaginaArvB->ByteSets[i];
                PaginaArvB->ProxRRN[i+2] = PaginaArvB->ProxRRN[i+1];
            }
            PaginaArvB->Chaves[posicao] = *id_promovido;
            PaginaArvB->ByteSets[posicao]= *ByteSetPromovido;
            PaginaArvB->ProxRRN[posicao+1] = *paginaFilha;
            PaginaArvB->NroChaves++;
            *promocao = 0;
            imprimirPaginadeDisco(PaginaArvB);
            escrever_PaginaNoDisco(PaginaArvB, NroPaginaAtual, fpArvB);
        }else{
            printf("Fiz split\n");
            //imprimirPaginadeDisco(PaginaArvB);
            int ChavesSplit[4] = {-1, -1, -1, -1};
            long long int ByteSetsSplit[4] = {-1, -1, -1, -1};
            int ProxSplit[5] = {-1, -1, -1, -1, -1};
            for(int i = 0; i < PaginaArvB->NroChaves; i++){
                ChavesSplit[i] = PaginaArvB->Chaves[i];
                ByteSetsSplit[i] = PaginaArvB->ByteSets[i];
                ProxSplit[i] = PaginaArvB->ProxRRN[i];
            }
            for(int i = 0; i < 5; i++){
                //printf("Chaves split %d\n", ProxSplit[i]);
            }
            for(int i = 0; i < PaginaArvB->NroChaves; i++){
                if(PaginaArvB->Chaves[i]>*id_promovido){
                    break;
                }
                posicao++;
            }
            printf("A posicao do novo elemento deveria ser %d\n", posicao);
            for(int i = 2; i >= posicao; i--){
                ChavesSplit[i+1]=ChavesSplit[i];
                ByteSetsSplit[i+1]=ByteSetsSplit[i];
                ProxSplit[i+2] = ProxSplit[i+1];
            }
            for(int i = 0; i < 3; i++){
                //printf("Chaves split %d\n", ChavesSplit[i]);
            }
            ChavesSplit[posicao] = *id_promovido;
            ByteSetsSplit[posicao] = *ByteSetPromovido;
            ProxSplit[posicao+1] = *paginaFilha;
            NoArvB* NovaPaginaArvB;
            NovaPaginaArvB = (NoArvB*) malloc (sizeof(NoArvB));
            if(NovaPaginaArvB == NULL){
                exit(1);
            }
            NovaPaginaArvB->NroChaves = 2;
            NovaPaginaArvB->alturaNo = PaginaArvB->alturaNo;

            for(int i = 0; i < 5; i++){
                //printf("Chaves split %d\n", ProxSplit[i]);
            }
            imprimirPaginadeSplit(ChavesSplit, ByteSetsSplit, ProxSplit);
            split(PaginaArvB, NovaPaginaArvB, ChavesSplit, ByteSetsSplit, ProxSplit, id_promovido, ByteSetPromovido, paginaFilha, CabecalhoArvB);
            PaginaArvB->NroChaves = 1;
            printf("A pagina antiga: \n");
            imprimirPaginadeDisco(PaginaArvB);
            printf("A pagina nova: \n");
            imprimirPaginadeDisco(NovaPaginaArvB);
            escrever_PaginaNoDisco(PaginaArvB, NroPaginaAtual, fpArvB);
            escrever_PaginaNoDisco(NovaPaginaArvB, CabecalhoArvB->proxRRN, fpArvB);
            *promocao = 1;
            CabecalhoArvB->proxRRN++;
            free(NovaPaginaArvB);
        }
        free(PaginaArvB);
    }
}

void split(NoArvB* PaginaArvB, NoArvB* NovaPaginaArvB, int ChavesSplit[], long long int ByteSetsSplit[], int ProxSplit[], int* id_promovido, long long int* ByteSetPromovido, int* paginaFilha, CabArvB * CabecalhoArvB){
    int posicao_split = 1;
    for(int i = 0; i < PaginaArvB->NroChaves; i++){
        PaginaArvB->Chaves[i] = -1;
        PaginaArvB->ByteSets[i] = -1;
    }
    for(int i = 0; i < PaginaArvB->NroChaves+1; i++){
        PaginaArvB->ProxRRN[i] = -1;
    }
    for(int i = 0; i < 3; i++){
        NovaPaginaArvB->Chaves[i] = -1;
        NovaPaginaArvB->ByteSets[i] = -1;
    }
    for(int i = 0; i < 4; i++){
        NovaPaginaArvB->ProxRRN[i] = -1;
    }
    for(int i = 0; i < posicao_split; i ++){
        PaginaArvB->Chaves[i] = ChavesSplit[i];
        PaginaArvB->ByteSets[i] = ByteSetsSplit[i];
        PaginaArvB->ProxRRN[i] = ProxSplit[i];
    }
    PaginaArvB->ProxRRN[1] = ProxSplit[posicao_split];
    *id_promovido = ChavesSplit [posicao_split];
    *ByteSetPromovido = ByteSetsSplit [posicao_split];
    *paginaFilha = CabecalhoArvB->proxRRN;
    int j = posicao_split+1;
    int i = 0;
    for(int i = 0; i < 4; i++){
        //printf("Chaves split %d\n", ChavesSplit[i]);
    }
    while(i < NovaPaginaArvB->NroChaves){
        NovaPaginaArvB->Chaves[i] = ChavesSplit[j];
        NovaPaginaArvB->ByteSets[i] = ByteSetsSplit[j];
        NovaPaginaArvB->ProxRRN[i] = ProxSplit[j];
        j++;
        i++;
    }
    //printf("Aq %d\n", NovaPaginaArvB->Chaves[1]);
    //printf("i: %d\n", i);
    NovaPaginaArvB->ProxRRN[i] = ProxSplit[j];
    //imprimirPaginadeDisco(NovaPaginaArvB);
}

void ler_PaginaNoDisco(NoArvB* PaginaArvB, int RRN, FILE* fp){
    printf("ler_pagina_De\n");
    printf("O RRN de leitura eh: %d\n", RRN);
    fseek(fp, (RRN*60)+60, SEEK_SET);
    fread(&PaginaArvB->alturaNo, sizeof(int), 1, fp);
    fread(&PaginaArvB->NroChaves, sizeof(int), 1, fp); 
    fread(&PaginaArvB->Chaves[0], sizeof(int), 1, fp); 
    fread(&PaginaArvB->ByteSets[0], sizeof(long long int), 1, fp); 
    fread(&PaginaArvB->Chaves[1], sizeof(int), 1, fp); 
    fread(&PaginaArvB->ByteSets[1], sizeof(long long int), 1, fp); 
    fread(&PaginaArvB->Chaves[2], sizeof(int), 1, fp); 
    fread(&PaginaArvB->ByteSets[2], sizeof(long long int), 1, fp); 
    fread(&PaginaArvB->ProxRRN[0], sizeof(int), 1, fp); 
    fread(&PaginaArvB->ProxRRN[1], sizeof(int), 1, fp); 
    fread(&PaginaArvB->ProxRRN[2], sizeof(int), 1, fp); 
    fread(&PaginaArvB->ProxRRN[3], sizeof(int), 1, fp);
    //imprimirPaginadeDisco(PaginaArvB);
}

void escrever_PaginaNoDisco(NoArvB* PaginaArvB, int RRN, FILE* fp){
    fseek(fp, (RRN*60)+60, SEEK_SET);
    fwrite(&PaginaArvB->alturaNo, sizeof(int), 1, fp);
    fwrite(&PaginaArvB->NroChaves, sizeof(int), 1, fp); 
    fwrite(&PaginaArvB->Chaves[0], sizeof(int), 1, fp); 
    fwrite(&PaginaArvB->ByteSets[0], sizeof(long long int), 1, fp); 
    fwrite(&PaginaArvB->Chaves[1], sizeof(int), 1, fp); 
    fwrite(&PaginaArvB->ByteSets[1], sizeof(long long int), 1, fp); 
    fwrite(&PaginaArvB->Chaves[2], sizeof(int), 1, fp); 
    fwrite(&PaginaArvB->ByteSets[2], sizeof(long long int), 1, fp); 
    fwrite(&PaginaArvB->ProxRRN[0], sizeof(int), 1, fp); 
    fwrite(&PaginaArvB->ProxRRN[1], sizeof(int), 1, fp); 
    fwrite(&PaginaArvB->ProxRRN[2], sizeof(int), 1, fp); 
    fwrite(&PaginaArvB->ProxRRN[3], sizeof(int), 1, fp);
    fflush(fp);
}

void Driver_ArvB(indexDados ElementoArvB, CabArvB * CabecalhoArvB, FILE* fpArvB){
    int id_promovido = -1;
    long long int ByteSetPromovido = -1;
    int paginaFilha = -1;
    int promocao = 0;
    //Sobre a promocao
    // Quando -1 significa ERRO
    // Quando 0 significa SEM PROMOCAO
    // Quando 1 significa COM PROMOCAO
    if(CabecalhoArvB->nroChaves == 0){
        CabecalhoArvB->noRaiz = 0;
        CabecalhoArvB->nroChaves = 1;
        CabecalhoArvB->proxRRN = 1;
        NoArvB* PaginaArvB;
        PaginaArvB = (NoArvB*) malloc (sizeof(NoArvB));
        if(PaginaArvB == NULL){
            exit(1);
        }
        PaginaArvB->alturaNo = 0;
        PaginaArvB->NroChaves = 1;
        for(int i = 0; i < 3; i++){
            PaginaArvB->Chaves[i] = -1;
            PaginaArvB->ByteSets[i] = -1;
        }
        for(int i = 0; i < 4; i++){
            PaginaArvB->ProxRRN[i] = -1;
        }
        PaginaArvB->Chaves[0] = ElementoArvB.id;
        PaginaArvB->ByteSets[0] = ElementoArvB.byteoffset;
        //imprimirPaginadeDisco(PaginaArvB);
        //exit(1);
        escrever_PaginaNoDisco(PaginaArvB, 0, fpArvB);
        printf("Inseri no começo");
        free(PaginaArvB);
        //ler_PaginaNoDisco(PaginaArvB, 0, fpArvB);
        //imprimirPaginadeDisco(PaginaArvB);
    }else{
        printf("Se não\n");
        insercao_ArvB(CabecalhoArvB->noRaiz, ElementoArvB.id, ElementoArvB.byteoffset, &id_promovido, &ByteSetPromovido, &paginaFilha, &promocao, CabecalhoArvB, fpArvB);
            if(promocao == 1){
            printf("Fiz split na RAIZZZ\n");
            //exit(1); /////////////////////////
            NoArvB* PaginaArvB;
            PaginaArvB = (NoArvB*) malloc (sizeof(NoArvB));
            NoArvB* RaizArvB;
            RaizArvB = (NoArvB*) malloc (sizeof(NoArvB));
            if(PaginaArvB == NULL){
                exit(1);
            }
            if(RaizArvB == NULL){
                exit(1);
            }
            ler_PaginaNoDisco(RaizArvB, CabecalhoArvB->noRaiz, fpArvB);
            PaginaArvB->alturaNo = RaizArvB->alturaNo+1;
            PaginaArvB->NroChaves = 1;
            for(int i = 0; i < 3; i++){
                PaginaArvB->Chaves[i] = -1;
                PaginaArvB->ByteSets[i] = -1;
            }
            for(int i = 0; i < 4; i++){
                PaginaArvB->ProxRRN[i] = -1;
            }
            PaginaArvB->Chaves[0] = id_promovido;
            PaginaArvB->ByteSets[0] = ByteSetPromovido;
            PaginaArvB->ProxRRN[0] = CabecalhoArvB->noRaiz;
            PaginaArvB->ProxRRN[1] = paginaFilha;
            CabecalhoArvB->noRaiz = CabecalhoArvB->proxRRN;
            CabecalhoArvB->proxRRN++;
            printf("O novo nó raiz estará em %d\n", CabecalhoArvB->noRaiz);
            escrever_PaginaNoDisco(PaginaArvB, CabecalhoArvB->noRaiz, fpArvB);
            imprimirPaginadeDisco(PaginaArvB);
            free(PaginaArvB);
            free(RaizArvB);
        }  
    }
}

void imprimirPaginadeDisco(NoArvB* PaginaArvB){
    printf("AlturaNo: %d\n", PaginaArvB->alturaNo);
    printf("NroChaves: %d\n", PaginaArvB->NroChaves);
    printf("C1 %d\n", PaginaArvB->Chaves[0]);
    printf("PR1 %lld\n", PaginaArvB->ByteSets[0]);
    printf("C2 %d\n", PaginaArvB->Chaves[1]);
    printf("PR2 %lld\n", PaginaArvB->ByteSets[1]);
    printf("C3 %d\n", PaginaArvB->Chaves[2]);
    printf("PR3 %lld\n", PaginaArvB->ByteSets[2]);
    for(int i = 0; i < 4; i++){
        printf("P%d %d ", i+1, PaginaArvB->ProxRRN[i]);
    }
    printf("\n\n");
}

void imprimirPaginadeSplit(int Chaves[], long long int ByteSets[], int ProxRRN[]){
    printf("C1 %d\n", Chaves[0]);
    printf("PR1 %lld\n", ByteSets[0]);
    printf("C2 %d\n", Chaves[1]);
    printf("PR2 %lld\n", ByteSets[1]);
    printf("C3 %d\n", Chaves[2]);
    printf("PR3 %lld\n", ByteSets[2]);
    printf("C4 %d\n", Chaves[3]);
    printf("PR4 %lld\n", ByteSets[3]);
    for(int i = 0; i < 5; i++){
        printf("P%d %d ", i+1, ProxRRN[i]);
    }
    printf("\n\n");
}

void imprimirArvoreComFila(int RRN, FILE* fp){
    printf("\n\n\nImpressão da arvoreB\n");
    NoArvB* PaginaArvB = (NoArvB*) malloc (sizeof(NoArvB));
    ler_PaginaNoDisco(PaginaArvB, RRN, fp);
    imprimirPaginadeDisco(PaginaArvB);
    FILA* fila = fila_criar();
    ITEM* meuitem;
    for(int i = 0; i < 4; i++){
        if(PaginaArvB->ProxRRN[i] != -1){
            meuitem = item_criar(PaginaArvB->ProxRRN[i]);
            if(fila_inserir(fila, meuitem)){
                printf("Coloquei: %d\n", PaginaArvB->ProxRRN[i]);
            }
        }
    }
    if(fila_vazia(fila)){
        printf("Esta vazia\n");
    }
    while(!fila_vazia(fila)){
        int proximo = item_get_chave(fila_remover(fila));
        printf("tirei: %d\n", proximo);
        ler_PaginaNoDisco(PaginaArvB, proximo, fp);
        imprimirPaginadeDisco(PaginaArvB);
        for(int i = 0; i < 4; i++){
            if(PaginaArvB->ProxRRN[i] != -1){
                meuitem = item_criar(PaginaArvB->ProxRRN[i]);
                fila_inserir(fila, meuitem);
                printf("Coloquei: %d\n", PaginaArvB->ProxRRN[i]);
            }
        }
    }
    printf("\n\n\n");
}

void function8(){ //Le os dados de um arquivo ".bin"  e cria um arquivo de indice para esse binário lido
    char arquivo1[30]; //Arquivo de entrada ".bin"
    scanf("%s", arquivo1);
    char arquivo2[30]; //Arquivo de saida indice ".bin"
    scanf("%s", arquivo2);
    FILE *fp; //Ponteiro para o controle do arquivo de dados
    FILE *fpArvB; //Ponteiro para o controle do indice
    if (((fp = fopen(arquivo1, "rb+"))) == NULL){ //Abro o binário para leitura
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
    
    if(CabecalhoLocal->status == '0'){ //Checo se há algum erro no meu arquivo de dados.
        printf("Falha no processamento do arquivo.");
        return;
    }

    if(CabecalhoLocal->nroRegArq == 0){ //Checo se existem registros no meu arquivo de dados.
        printf("Registro inexistente.\n\n");
        return;
    }

    int tamanho_real = CabecalhoLocal->nroRegArq+CabecalhoLocal->nroRegRem; //O tamanho que preciso realmente buscar são os ativos + os removidos
 
    CabArvB * CabecalhoArvB;
    CabecalhoArvB = (CabArvB*) malloc (sizeof(CabArvB));
    if(CabecalhoArvB == NULL){
        exit(1);
    }

    if (((fpArvB = fopen(arquivo2, "rb+"))) == NULL){ //Abro o binário para leitura
        printf("Falha no processamento do arquivo.");
        return;
    }

    char status;
    int noRaiz;
    int proxRRN;
    int nroChaves;

    fread(&status, sizeof(char), 1, fpArvB);
    fread(&noRaiz, sizeof(int), 1, fpArvB);
    fread(&proxRRN, sizeof(int), 1, fpArvB);
    fread(&nroChaves, sizeof(int), 1, fpArvB);
    printf("No raiz: %d\n", noRaiz);
    imprimirArvoreComFila(noRaiz, fpArvB);
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
        function6();
        break;
    case 7:
        function7();
        break;
    case 8:
        function8();
        break;
    case 9:
        //function9();
        break;
    case 10:
        //function10();
        break;
    default:
        break;
    }

    return 0;
}