/*
Autores: 2024 - Maicon Chaves Marques - 14593530
Karl Cruz Altenhofen- 14585976
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "help.h" //Funções fornecidas pelo run.codes
#include "busca_impressao.h"
#include "leitura_escrita.h"

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

void imprimirjogador(regDados* jogador){ //Imprime os campos: nome, nacionalidade e clube de um jogador (regDados)
    printf("Nome do Jogador: ");
    if(jogador->tamNomeJog == 0){
        printf("SEM DADO");
    }else{
        for(int i = 0; i < jogador->tamNomeJog; i++){
            printf("%c", jogador->nomeJogador[i]);
        }
    }
    printf("\n");
    printf("Nacionalidade do Jogador: ");
    if(jogador->tamNacionalidade == 0){
        printf("SEM DADO");
    }else{
        for(int i = 0; i < jogador->tamNacionalidade; i++){
            printf("%c", jogador->nacionalidade[i]);
        }
    }
    printf("\n");
    printf("Clube do Jogador: ");
    if(jogador->tamNomeClube == 0){
        printf("SEM DADO");
    }else{
    for(int i = 0; i < jogador->tamNomeClube; i++){
        printf("%c", jogador->nomeClube[i]);
    }
    }
    printf("\n\n");
}

void copiaJogador(regDados** newRegistro, regDados** RegistrosLocais, int newNumero, int i){
    // aloca memoria para um registro individual e depois para 3 arrays de char dentro desse registro
    if((newRegistro[newNumero] = (regDados*) malloc (sizeof(regDados))) == NULL){
        printf("Falha no processamento do arquivo.");
        return;
    }
    if((newRegistro[newNumero]->nomeJogador = (char*) malloc (RegistrosLocais[i]->tamNomeJog * sizeof(char))) == NULL){
        printf("Falha no processamento do arquivo.");
        return;
    }
    if((newRegistro[newNumero]->nacionalidade = (char*) malloc (RegistrosLocais[i]->tamNacionalidade * sizeof(char))) == NULL){
        printf("Falha no processamento do arquivo.");
        return;
    }
    if((newRegistro[newNumero]->nomeClube = (char*) malloc (RegistrosLocais[i]->tamNomeClube * sizeof(char))) == NULL){
        printf("Falha no processamento do arquivo.");
        return;
    }

    // copia os dados de RegistroLocais[i] para newRegistro[newNumero], passando apenas os conteudos
    for(int j = 0; j < RegistrosLocais[i]->tamNomeJog; j++){
        newRegistro[newNumero]->nomeJogador[j] = RegistrosLocais[i]->nomeJogador[j];
    }
    for(int j = 0; j < RegistrosLocais[i]->tamNacionalidade; j++){
        newRegistro[newNumero]->nacionalidade[j] = RegistrosLocais[i]->nacionalidade[j];
    }
    for(int j = 0; j < RegistrosLocais[i]->tamNomeClube; j++){
        newRegistro[newNumero]->nomeClube[j] = RegistrosLocais[i]->nomeClube[j];
    }
    newRegistro[newNumero]->removido = RegistrosLocais[i]->removido;
    newRegistro[newNumero]->tamanhoRegistro = RegistrosLocais[i]->tamanhoRegistro;
    newRegistro[newNumero]->Prox = RegistrosLocais[i]->Prox;
    newRegistro[newNumero]->id = RegistrosLocais[i]->id;
    newRegistro[newNumero]->idade = RegistrosLocais[i]->idade;
    newRegistro[newNumero]->tamNomeJog = RegistrosLocais[i]->tamNomeJog;
    newRegistro[newNumero]->tamNacionalidade = RegistrosLocais[i]->tamNacionalidade;
    newRegistro[newNumero]->tamNomeClube = RegistrosLocais[i]->tamNomeClube;
}


void copiaJogador2(regDados** newRegistro, regDados* jogador, int newNumero){
    // aloca memoria para um registro individual e depois para 3 arrays de char dentro desse registro
    if((newRegistro[newNumero] = (regDados*) malloc (sizeof(regDados))) == NULL){
        printf("Falha no processamento do arquivo.");
        return;
    }
    if((newRegistro[newNumero]->nomeJogador = (char*) malloc (jogador->tamNomeJog * sizeof(char))) == NULL){
        printf("Falha no processamento do arquivo.");
        return;
    }
    if((newRegistro[newNumero]->nacionalidade = (char*) malloc (jogador->tamNacionalidade * sizeof(char))) == NULL){
        printf("Falha no processamento do arquivo.");
        return;
    }
    if((newRegistro[newNumero]->nomeClube = (char*) malloc (jogador->tamNomeClube * sizeof(char))) == NULL){
        printf("Falha no processamento do arquivo.");
        return;
    }

    // copia os dados de RegistroLocais[i] para newRegistro[newNumero], passando apenas os conteudos
    for(int j = 0; j < jogador->tamNomeJog; j++){
        newRegistro[newNumero]->nomeJogador[j] = jogador->nomeJogador[j];
    }
    for(int j = 0; j < jogador->tamNacionalidade; j++){
        newRegistro[newNumero]->nacionalidade[j] = jogador->nacionalidade[j];
    }
    for(int j = 0; j < jogador->tamNomeClube; j++){
        newRegistro[newNumero]->nomeClube[j] = jogador->nomeClube[j];
    }
    newRegistro[newNumero]->removido = jogador->removido;
    newRegistro[newNumero]->tamanhoRegistro = jogador->tamanhoRegistro;
    newRegistro[newNumero]->Prox = jogador->Prox;
    newRegistro[newNumero]->id = jogador->id;
    newRegistro[newNumero]->idade = jogador->idade;
    newRegistro[newNumero]->tamNomeJog = jogador->tamNomeJog;
    newRegistro[newNumero]->tamNacionalidade = jogador->tamNacionalidade;
    newRegistro[newNumero]->tamNomeClube = jogador->tamNomeClube;
}

long long int busca_binaria_indice(indexDados** IndiceLocal, int idLocal, int inicio, int fim){
    //printf("ue\n");
    //printf("Inicio: %d, Fim: %d\n", inicio, fim);
    int meio = (inicio+fim)/2;
    //printf("Meio: %d e \n", meio);
    if(fim<inicio){
        return -1;
    }else if(IndiceLocal[meio]->id == idLocal){
        return IndiceLocal[meio]->byteoffset;
    }else if(IndiceLocal[meio]->id > idLocal){
        //printf("%d > %d\n", IndiceLocal[meio]->id, idLocal);
        return busca_binaria_indice(IndiceLocal, idLocal, inicio, meio-1);
    }else{
        //printf("%d < %d\n", IndiceLocal[meio]->id, idLocal);
        return busca_binaria_indice(IndiceLocal, idLocal, meio+1, fim);
    }
}

int busca_binaria_insercao(indexDados** IndiceLocal, int idLocal, int inicio, int fim){
    //printf("ue\n");
    //printf("Inicio: %d, Fim: %d\n", inicio, fim);
    int meio = (inicio+fim)/2;
    //printf("Meio: %d e \n", meio);
    if(IndiceLocal[meio]->id == idLocal){
        return meio;
    }else if(inicio==fim){
        if(IndiceLocal[fim]->id > idLocal){
            return inicio;
        }else{
            return inicio+1;
        }
    }else if(IndiceLocal[inicio]->id > idLocal){
        return inicio; 
    }else if(IndiceLocal[meio]->id > idLocal){
        //printf("%d > %d\n", IndiceLocal[meio]->id, idLocal);
        return busca_binaria_insercao(IndiceLocal, idLocal, inicio, meio-1);
    }else{
        //printf("%d < %d\n", IndiceLocal[meio]->id, idLocal);
        return busca_binaria_insercao(IndiceLocal, idLocal, meio+1, fim);
    }
}


regDados** filtra(regDados** RegistrosLocais, int* numeroResultados, int pesquisa, long long int RegistrodeOffsets[], int function, indexDados** IndiceLocal, FILE *fp, int tamIndex, long long int *ByteSets){
    /*Para encontrar o ByteOffSet de cada elemento incialmente eu aviso que estou fazendo a pesquisa considerando o banco de dados total
    o que faz o ByteOffSet começar em 25 (tamanho do cabecalho) por padrão, a partir dai, mesmo se não for um elemento que foi filtrado, eu vou somando os 
    tamanhos, para ter o cálculo pronto quando o elemento passar no teste.
    Quando um elemento passa no teste eu pego seu respctivo ByteOffSet e guardo em um vetor temporário, assim esse vetor contém os ByteOffSets dos elementos
    que passaram no teste e vários -1, que representa o vazio.
    No fim eu copio esse vetor temporário para o vetor principal, o que faz o vetor principal ter somente os elementos que passaram no teste, com o mesmo indice do banco de dados
    copia que vai ser retornado por essa funcao.
    Quando eu chamo a pesquisa novamente com pesquisa == 2, quer dizer que não vou contar os bytes de novo, vou apenas guardar os byteoffsets de quem passar no teste, e guardar
    com o indice sincronizado com os elementos que vão compor o novo banco de dados retornado por essa função.
    */


    long long Bytetemporario[*(numeroResultados)]; //Vou guardar temporariamente os byteoffsets dos selecionados;
    for(int i = 0; i < *(numeroResultados); i++){
        Bytetemporario[i] = -1;
    }
    int cont_temporario = 0;
    int n = *numeroResultados; // numero de dados no registro de entrada
    regDados** newRegistro; // o registro de saida
    int newNumero = 0; // numero de dados no registro de saida
    if((newRegistro = (regDados**) malloc (n * sizeof(regDados*))) == NULL){ // aloca o novo registro
        printf("Falha no processamento do arquivo.");
        return NULL;
    }
    
    char nomeCampo[14]; // sera "id", "idade", "nomeJogador", "nacionalidade" ou "nomeClube", com \0 no final
    scanf(" %s", nomeCampo);

    // entra no filtro respectivo
    // caso a entrada seja um array de char, cria outro array com um espaco a mais e coloca '\0' no final
    // checa a entrada contra todos os elementos no registro
    // caso passe, aloca e copia dado a dado dp registro antigo para o novo, aumentando seu numero de elementos em 1
    if(strcmp(nomeCampo, "id") == 0 && function == 5){
        int idLocal;
        scanf("%d", &idLocal);
        regDados* jogador;
        long long int bytejogador;
        bytejogador = busca_binaria_indice(IndiceLocal, idLocal, 0, tamIndex-1);
        Bytetemporario[newNumero] = bytejogador;
        //printf("Byte encontrado: %lld\n", bytejogador);
        jogador = ler_jogador(bytejogador, fp);
        //imprimirjogador(jogador);
        copiaJogador2(newRegistro, jogador, 0);
        free(jogador->nomeJogador);
        free(jogador->nacionalidade);
        free(jogador->nomeClube);
        free(jogador);
        newNumero++;
    }else if(strcmp(nomeCampo, "id") == 0){
        int idLocal;
        scanf("%d", &idLocal);
        for(int i = 0; i < n; i++){
            if(idLocal == RegistrosLocais[i]->id){
                copiaJogador(newRegistro, RegistrosLocais, newNumero, i);
                if(pesquisa == 1){
                Bytetemporario[newNumero] = ByteSets[i];
                }else{
                Bytetemporario[newNumero] = RegistrodeOffsets[i];
                }
                newNumero++;
                break; //se achar um id, encerra a busca
            }
        }
    }else if(strcmp(nomeCampo, "idade") == 0){
        int idadeLocal;
        scanf("%d", &idadeLocal);
        for(int i = 0; i < n; i++){
            if(idadeLocal == RegistrosLocais[i]->idade){
                copiaJogador(newRegistro, RegistrosLocais, newNumero, i);
                if(pesquisa == 1){
                Bytetemporario[newNumero] = ByteSets[i];
                }else{
                Bytetemporario[newNumero] = RegistrodeOffsets[i];
                }
                newNumero++;
            }
        }

    }else if(strcmp(nomeCampo, "nomeJogador") == 0){
        char nomeLocal[30];
        scan_quote_string(nomeLocal);
        for(int i = 0; i < n; i++){
            char nomeRegistro[(RegistrosLocais[i]->tamNomeJog) + 1];
            for(int a = 0; a < RegistrosLocais[i]->tamNomeJog; a++){
                nomeRegistro[a] =  RegistrosLocais[i]->nomeJogador[a];
            }
            nomeRegistro[RegistrosLocais[i]->tamNomeJog] = '\0';
            if(strcmp(nomeLocal, nomeRegistro) == 0){
                copiaJogador(newRegistro, RegistrosLocais, newNumero, i); //
                if(pesquisa == 1){
                Bytetemporario[newNumero] = ByteSets[i];
                }else{
                Bytetemporario[newNumero] = RegistrodeOffsets[i];
                }
                newNumero++;
            }
        }

    }else if(strcmp(nomeCampo, "nacionalidade") == 0){
        char nacionalidadeLocal[30];
        scan_quote_string(nacionalidadeLocal);
        for(int i = 0; i < n; i++){
            char nacionalidadeRegistro[(RegistrosLocais[i]->tamNacionalidade) + 1];
            for(int a = 0; a < RegistrosLocais[i]->tamNacionalidade; a++){
                nacionalidadeRegistro[a] =  RegistrosLocais[i]->nacionalidade[a];
            }
            nacionalidadeRegistro[RegistrosLocais[i]->tamNacionalidade] = '\0'; 
            if(strcmp(nacionalidadeLocal, nacionalidadeRegistro) == 0){
                copiaJogador(newRegistro, RegistrosLocais, newNumero, i);
                if(pesquisa == 1){
                Bytetemporario[newNumero] = ByteSets[i];
                }else{
                Bytetemporario[newNumero] = RegistrodeOffsets[i];
                }
                newNumero++;
            }
        }

    }else if(strcmp(nomeCampo, "nomeClube") == 0){
        char clubeLocal[30];
        scan_quote_string(clubeLocal);
        for(int i = 0; i < n; i++){
            char clubeRegistro[(RegistrosLocais[i]->tamNomeClube) + 1];
            for(int a = 0; a < RegistrosLocais[i]->tamNomeClube; a++){
                clubeRegistro[a] =  RegistrosLocais[i]->nomeClube[a];
            }
            clubeRegistro[RegistrosLocais[i]->tamNomeClube] = '\0';
            if(strcmp(clubeLocal, clubeRegistro) == 0){
                copiaJogador(newRegistro, RegistrosLocais, newNumero, i);
                if(pesquisa == 1){
                Bytetemporario[newNumero] = ByteSets[i];
                }else{
                Bytetemporario[newNumero] = RegistrodeOffsets[i];
                }
                newNumero++;
            }
        }
    }

    if(newNumero != 0){ //realoca o registro para o tamanho exato, se ele nao for vazio
        if((newRegistro = (regDados**) realloc (newRegistro, sizeof(regDados*) * newNumero)) == NULL){
            printf("Falha no processamento do arquivo.");
            return NULL;
        }
    }
    for(int i = 0; i < (*numeroResultados); i++){
        RegistrodeOffsets[i]=Bytetemporario[i];
        //printf("%d° %lld\n", i+1, Bytetemporario[i]);
    }
    *numeroResultados = newNumero; //numeroResultados se torna o tamanho do novo registro
    return newRegistro; //retorna o registro filtrado
}