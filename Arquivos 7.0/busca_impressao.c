/*
Autores: 2024 - Maicon Chaves Marques - 14593530
Karl Cruz Altenhofen- 14585976
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
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

typedef struct listaDados{
    int tamanho;
    long long byteoffset;
}listaDados;

void BuscaNoVetor(char* nomeCampo, regDados** RegistroFiltrado, int* newNumero, regDados** newRegistro, long long int* ByteSets, long long int* Bytetemporario, int numeroResultados, long long int* RegistrodeOffsets);
void BuscaNoRegistro(int tamanho, FILE* fp, long long int* ByteSets, char* nomeCampo, int* newNumero, long long int* Bytetemporario, regDados** newRegistro);

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

void copiaJogador(regDados** newRegistro, regDados* jogador, int newNumero){
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

long long int busca_binaria_indice(indexDados** IndiceLocal, int idLocal, int inicio, int fim){ //Retorno qual é o byteoffset do elemento com o id buscado
    int meio = (inicio+fim)/2;
    if(fim<inicio){
        return -1;
    }else if(IndiceLocal[meio]->id == idLocal){
        return IndiceLocal[meio]->byteoffset;
    }else if(IndiceLocal[meio]->id > idLocal){
        return busca_binaria_indice(IndiceLocal, idLocal, inicio, meio-1);
    }else{
        return busca_binaria_indice(IndiceLocal, idLocal, meio+1, fim);
    }
}

int busca_binaria_insercao(indexDados** IndiceLocal, int idLocal, int inicio, int fim){ //Busca binaria no indice, retorno posição do indice ideal para inserir um elemento com o id passado
    int meio = (inicio+fim)/2;
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
        return busca_binaria_insercao(IndiceLocal, idLocal, inicio, meio-1);
    }else{
        return busca_binaria_insercao(IndiceLocal, idLocal, meio+1, fim);
    }
}

regDados** filtra(regDados** RegistroFiltrado, int* numeroResultados, int pesquisa, long long int RegistrodeOffsets[], int function, indexDados** IndiceLocal, FILE *fp, int tamIndex, long long int *ByteSets, int tamanho_real){

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
    if(strcmp(nomeCampo, "id") == 0 && function == 5){ //Faço busca binária pelo indice
        int idLocal;
        scanf("%d", &idLocal);
        regDados* jogador;
        long long int bytejogador;
        bytejogador = busca_binaria_indice(IndiceLocal, idLocal, 0, tamIndex-1);
        Bytetemporario[newNumero] = bytejogador;
        jogador = ler_jogador(bytejogador, fp);
        copiaJogador(newRegistro, jogador, 0); //Nessario para manter a modulariazação entre a função 5 e 3
        free(jogador->nomeJogador);
        free(jogador->nacionalidade);
        free(jogador->nomeClube);
        free(jogador);
        newNumero++;
    }else{ 
        if(pesquisa == 1){
            BuscaNoRegistro(tamanho_real, fp, ByteSets, nomeCampo, &newNumero, Bytetemporario, newRegistro);
        }else{
            BuscaNoVetor(nomeCampo, RegistroFiltrado, &newNumero, newRegistro, ByteSets, Bytetemporario, *numeroResultados, RegistrodeOffsets);
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
    }
    *numeroResultados = newNumero; //numeroResultados se torna o tamanho do novo registro
    return newRegistro; //retorna o registro filtrado
}

void BuscaNoRegistro(int tamanho, FILE* fp, long long int* ByteSets, char* nomeCampo, int* newNumero, long long int* Bytetemporario, regDados** newRegistro){
    fseek(fp, 25, SEEK_SET);
    int i = 0; //Indice que representa a quantidade de jogadores não removidos, lidos até o momento
    long long int Proxbyteoffset = 25; //Começo em 25 por considerar o tamanho do cabeçalho

    int idLocal;
    int idadeLocal;
    char nomeLocal[30];
    char nacionalidadeLocal[30];
    char clubeLocal[30];

    if(strcmp(nomeCampo, "id") == 0){
        scanf("%d", &idLocal);
    }else if(strcmp(nomeCampo, "idade") == 0){
        scanf("%d", &idadeLocal);
    }else if(strcmp(nomeCampo, "nomeJogador") == 0){
        scan_quote_string(nomeLocal);
    }else if(strcmp(nomeCampo, "nacionalidade") == 0){
        scan_quote_string(nacionalidadeLocal);
    }else if(strcmp(nomeCampo, "nomeClube") == 0){
        scan_quote_string(clubeLocal);
    }

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
            if((jogador = (regDados*) malloc (sizeof(regDados))) == NULL){ //Aloco espaço para o jogador
                printf("Falha no processamento do arquivo.61\n");
                exit(1);
            }
            ByteSets[i] = Proxbyteoffset - tamRegLocal;
            jogador->removido = teste_remocao; //Os valores dos campos continuam sendo lidos para posterior integração a funcionalidade da função 2
            jogador->tamanhoRegistro = tamRegLocal;
            pulaLixo = tamRegLocal;
            fread(&jogador->Prox, sizeof(long long int), 1, fp);
            fread(&jogador->id, sizeof(int), 1, fp);
            fread(&jogador->idade, sizeof(int), 1, fp);
            fread(&jogador->tamNomeJog, sizeof(int), 1, fp);
            pulaLixo -= 25;
            if((jogador->nomeJogador = (char*) malloc ((jogador->tamNomeJog)*sizeof(char))) == NULL){ 
                printf("Falha no processamento do arquivo.\n");
                exit(1);
            }
            for(int j = 0; j < jogador->tamNomeJog; j++){ //Leio byte a byte os caracters
                fread(&jogador->nomeJogador[j], sizeof(char), 1, fp);
            }
            pulaLixo=pulaLixo-jogador->tamNomeJog;
            fread(&jogador->tamNacionalidade, sizeof(int), 1, fp);
            if((jogador->nacionalidade = (char*) malloc ((jogador->tamNacionalidade)*sizeof(char))) == NULL){ 
                printf("Falha no processamento do arquivo.\n");
                exit(1);
            }
            for(int j = 0; j < jogador->tamNacionalidade; j++){ //Leio byte a byte os caracters
            fread(&jogador->nacionalidade[j], sizeof(char), 1, fp); 
            }
            pulaLixo= pulaLixo-jogador->tamNacionalidade-4;
            fread(&jogador->tamNomeClube, sizeof(int), 1, fp);
            if((jogador->nomeClube = (char*) malloc ((jogador->tamNomeClube)*sizeof(char))) == NULL){
                printf("Falha no processamento do arquivo.\n");
                exit(1);
            }
            for(int j = 0; j < jogador->tamNomeClube; j++){ //Leio byte a byte os caracters
                fread(&jogador->nomeClube[j], sizeof(char), 1, fp);
            }
            pulaLixo=pulaLixo-jogador->tamNomeClube-4;
            for(int i = 0; i < pulaLixo; i++){
                fread(&lixoCifrao, sizeof(char), 1, fp);
            }

            if(strcmp(nomeCampo, "id") == 0){
                    if(idLocal == jogador->id){
                        copiaJogador(newRegistro, jogador, *newNumero);
                        Bytetemporario[*newNumero] = ByteSets[i];
                        *newNumero=(*newNumero)+1;
                        free(jogador->nomeJogador); //Libero a memória do jogador que mantive alocado
                        free(jogador->nacionalidade);
                        free(jogador->nomeClube);
                        free(jogador);
                        break; //se achar um id, encerra a busca
                    }
            }else if(strcmp(nomeCampo, "idade") == 0){
                    if(idadeLocal == jogador->idade){
                        copiaJogador(newRegistro, jogador, *newNumero);
                        Bytetemporario[*newNumero] = ByteSets[i];
                        *newNumero=(*newNumero)+1;
                    }
            }else if(strcmp(nomeCampo, "nomeJogador") == 0){
                    char nomeRegistro[(jogador->tamNomeJog) + 1];
                    for(int a = 0; a < jogador->tamNomeJog; a++){
                        nomeRegistro[a] =  jogador->nomeJogador[a];
                    }
                    nomeRegistro[jogador->tamNomeJog] = '\0';
                    if(strcmp(nomeLocal, nomeRegistro) == 0){
                        copiaJogador(newRegistro, jogador, *newNumero);
                        Bytetemporario[*newNumero] = ByteSets[i];
                        *newNumero=(*newNumero)+1;
                    }
            }else if(strcmp(nomeCampo, "nacionalidade") == 0){
                    char nacionalidadeRegistro[(jogador->tamNacionalidade) + 1];
                    for(int a = 0; a < jogador->tamNacionalidade; a++){
                        nacionalidadeRegistro[a] =  jogador->nacionalidade[a];
                    }
                    nacionalidadeRegistro[jogador->tamNacionalidade] = '\0'; 
                    if(strcmp(nacionalidadeLocal, nacionalidadeRegistro) == 0){
                        copiaJogador(newRegistro, jogador, *newNumero);
                        Bytetemporario[*newNumero] = ByteSets[i];
                        *newNumero=(*newNumero)+1;
                    }
            }else if(strcmp(nomeCampo, "nomeClube") == 0){
                    char clubeRegistro[(jogador->tamNomeClube) + 1];
                    for(int a = 0; a < jogador->tamNomeClube; a++){
                        clubeRegistro[a] =  jogador->nomeClube[a];
                    }
                    clubeRegistro[jogador->tamNomeClube] = '\0';
                    if(strcmp(clubeLocal, clubeRegistro) == 0){
                        copiaJogador(newRegistro, jogador, *newNumero);
                        Bytetemporario[*newNumero] = ByteSets[i];
                        *newNumero=(*newNumero)+1;
                    }
            }
            free(jogador->nomeJogador); //Libero a memória do jogador que mantive alocado
            free(jogador->nacionalidade);
            free(jogador->nomeClube);
            free(jogador);
            i++;
        }
    }
}

void BuscaNoVetor(char* nomeCampo, regDados** RegistroFiltrado, int* newNumero, regDados** newRegistro, long long int* ByteSets, long long int* Bytetemporario, int numeroResultados, long long int* RegistrodeOffsets){
    
    if(strcmp(nomeCampo, "id") == 0){
        int idLocal;
        scanf("%d", &idLocal);
        for(int i = 0; i < numeroResultados; i++){
            if(idLocal == RegistroFiltrado[i]->id){
                copiaJogador(newRegistro, RegistroFiltrado[i], *newNumero);
                Bytetemporario[*newNumero] = RegistrodeOffsets[i];
                *newNumero=(*newNumero)+1;
                break; //se achar um id, encerra a busca
            }
        }
    }else if(strcmp(nomeCampo, "idade") == 0){
        int idadeLocal;
        scanf("%d", &idadeLocal);
        for(int i = 0; i < numeroResultados; i++){
            if(idadeLocal == RegistroFiltrado[i]->idade){
                copiaJogador(newRegistro, RegistroFiltrado[i], *newNumero);
                Bytetemporario[*newNumero] = RegistrodeOffsets[i];
                *newNumero=(*newNumero)+1;
            }
        }

    }else if(strcmp(nomeCampo, "nomeJogador") == 0){
        char nomeLocal[30];
        scan_quote_string(nomeLocal);
        for(int i = 0; i < numeroResultados; i++){
            char nomeRegistro[(RegistroFiltrado[i]->tamNomeJog) + 1];
            for(int a = 0; a < RegistroFiltrado[i]->tamNomeJog; a++){
                nomeRegistro[a] =  RegistroFiltrado[i]->nomeJogador[a];
            }
            nomeRegistro[RegistroFiltrado[i]->tamNomeJog] = '\0';
            if(strcmp(nomeLocal, nomeRegistro) == 0){
                copiaJogador(newRegistro, RegistroFiltrado[i], *newNumero);
                Bytetemporario[*newNumero] = RegistrodeOffsets[i];
                *newNumero=(*newNumero)+1;
            }
        }

    }else if(strcmp(nomeCampo, "nacionalidade") == 0){
        char nacionalidadeLocal[30];
        scan_quote_string(nacionalidadeLocal);
        for(int i = 0; i < numeroResultados; i++){
            char nacionalidadeRegistro[(RegistroFiltrado[i]->tamNacionalidade) + 1];
            for(int a = 0; a < RegistroFiltrado[i]->tamNacionalidade; a++){
                nacionalidadeRegistro[a] =  RegistroFiltrado[i]->nacionalidade[a];
            }
            nacionalidadeRegistro[RegistroFiltrado[i]->tamNacionalidade] = '\0'; 
            if(strcmp(nacionalidadeLocal, nacionalidadeRegistro) == 0){
                copiaJogador(newRegistro, RegistroFiltrado[i], *newNumero);
                Bytetemporario[*newNumero] = RegistrodeOffsets[i];
                *newNumero=(*newNumero)+1;
            }
        }

    }else if(strcmp(nomeCampo, "nomeClube") == 0){
        char clubeLocal[30];
        scan_quote_string(clubeLocal);
        for(int i = 0; i < numeroResultados; i++){
            char clubeRegistro[(RegistroFiltrado[i]->tamNomeClube) + 1];
            for(int a = 0; a < RegistroFiltrado[i]->tamNomeClube; a++){
                clubeRegistro[a] =  RegistroFiltrado[i]->nomeClube[a];
            }
            clubeRegistro[RegistroFiltrado[i]->tamNomeClube] = '\0';
            if(strcmp(clubeLocal, clubeRegistro) == 0){
                copiaJogador(newRegistro, RegistroFiltrado[i], *newNumero);
                Bytetemporario[*newNumero] = RegistrodeOffsets[i];
                *newNumero=(*newNumero)+1;
            }
        }
    }

}

int busca_anterior(listaDados** listaOrdenada, int posicao, int tamanho){
    for(int i = 0; i < posicao; i++){
        if(listaOrdenada[i]->tamanho >= tamanho){
            return i-1;
        }
    }
}