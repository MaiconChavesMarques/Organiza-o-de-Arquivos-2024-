/*
Autores: 2024 - Maicon Chaves Marques - 14593530
Karl Cruz Altenhofen- 14585976
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "leitura_escrita.h"
#include "busca_impressao.h"

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

int tamanhoreg(regDados *jogador){ //Soma os sizeoffs dos campos para descobrir o tamanho do registro
    return (sizeof(jogador->removido)+sizeof(jogador->tamanhoRegistro)+sizeof(jogador->Prox)+sizeof(jogador->id)+sizeof(jogador->idade)+sizeof(jogador->tamNomeJog)
    +jogador->tamNomeJog+sizeof(jogador->tamNacionalidade)+jogador->tamNacionalidade+sizeof(jogador->tamNomeClube)+jogador->tamNomeClube);
}
int tamanhoCab(){ //Calcula o tamanho do cabecalho
    return (sizeof(char)+sizeof(long long int)+sizeof(long long int)+sizeof(int)+sizeof(int));
}

void ler_dados(regCabecalho* CabecalhoLocal, regDados*** RegistrosLocais, FILE* fp){ //Le os dados de um arquivo ".csv" e traz as informações para a memória RAM (regDados e regCababecalho)
    CabecalhoLocal->status = '0';
    CabecalhoLocal->topo = -1;
    CabecalhoLocal->proxByteOffset = tamanhoCab();
    CabecalhoLocal->nroRegArq = 0;
    CabecalhoLocal->nroRegRem = 0;
    long long int prox_local = 0;

    while (1){ //O teste do eof é feito, esperando que a flag de eof dispare dentro do while
        int id_local;
        int idade_local = -1; //Inicio sempre com -1, caso o jogador não tenha o campo idade
        fscanf(fp, "%d , ", &id_local);
        if(feof(fp)){ //Teste de parada
            break;
        }
        fscanf(fp, "%d", &idade_local); //Le a idade e guarda localmente, se não ler nada continua -1
        int c = (char)getc(fp);

        (CabecalhoLocal->nroRegArq)++;
        if(((CabecalhoLocal->nroRegArq) % 100)==0){ //Uso realloc de jogadores sempre de 100 em 100
            if(((*RegistrosLocais) = (regDados**) realloc ((*RegistrosLocais), ((CabecalhoLocal->nroRegArq)+100)*sizeof(regDados*))) == NULL){
                printf("Falha no processamento do arquivo.");
                fclose(fp);
                return;
            }
        }
        regDados* jogador;
        if((jogador = (regDados*) malloc (sizeof(regDados))) == NULL){
            printf("Falha no processamento do arquivo.");
            fclose(fp);
            return;
        }

        jogador->removido = '0';
        jogador->id = id_local;
        jogador->idade = idade_local;

        //Ler o nome do Jogador
        int count;
        char NomeTemp[40]; //Guardo temporáriamente o nome do jogador
        count = 0;
        c = 'a';
        while(c != ','){
            fread(&NomeTemp[count], sizeof(char), 1, fp);
            c = NomeTemp[count];
            count++;
        }
        jogador->tamNomeJog = count-1; //Para desconsiderar a vírgula que veio
        if((jogador->nomeJogador = (char*) malloc ((jogador->tamNomeJog)*sizeof(char))) == NULL){
            printf("Falha no processamento do arquivo.");
            fclose(fp);
            return;
        }
        for(int i = 0; i < (jogador->tamNomeJog); i++){
            jogador->nomeJogador[i] = NomeTemp[i];
        }

        //Ler o nome do Pais do jogador
        char NacionalidadeTemp[40]; //Guarda a nacionalidade temporáriamente
        c = 'a';
        count = 0;
        while(c != ','){
            fread(&NacionalidadeTemp[count], sizeof(char), 1, fp);
            c = NacionalidadeTemp[count];
            count++;
        }
        jogador->tamNacionalidade = count-1; //Para desconsiderar a vírgula que veio
        if((jogador->nacionalidade = (char*) malloc ((jogador->tamNacionalidade)*sizeof(char))) == NULL){
            printf("Falha no processamento do arquivo.");
            fclose(fp);
            return;
        }
        for(int i = 0; i < (jogador->tamNacionalidade); i++){
            jogador->nacionalidade[i] = NacionalidadeTemp[i];
        }

        //Ler o nome do Time do jogador
        char ClubeTemp[40]; //Guarda a nacionalidade temporáriamente
        c = 'a';
        count = 0;
        while(c != '\n' && c != '\r'){
            fread(&ClubeTemp[count], sizeof(char), 1, fp);
            c = ClubeTemp[count];
            count++;
        }
        jogador->tamNomeClube = count-1; //Para desconsiderar o "/n" que veio da função fgets
        if((jogador->nomeClube = (char*) malloc ((jogador->tamNomeClube)*sizeof(char))) == NULL){ 
            printf("Falha no processamento do arquivo.");
            fclose(fp);
            return;
        }
        for(int i = 0; i < (jogador->tamNomeClube); i++){
            jogador->nomeClube[i] = ClubeTemp[i];
        }

        jogador->tamanhoRegistro = tamanhoreg(jogador); //Calculo o tamanho para esse jogador em específico
        (prox_local) = (prox_local) + jogador->tamanhoRegistro; //Para o cáculo do proximo byteoffset
        jogador->Prox = -1;

        (*RegistrosLocais)[(CabecalhoLocal->nroRegArq)-1] = jogador;
    }

    CabecalhoLocal->proxByteOffset += prox_local;
}

regDados* ler_jogador(long long int byteoffset, FILE* fp){
    regDados* jogador;
    fseek(fp, byteoffset, SEEK_SET);
    if((jogador = (regDados*) malloc (sizeof(regDados))) == NULL){
        printf("Falha no processamento do arquivo.");
        fclose(fp);
        exit(1);
    }
    fread(&jogador->removido, sizeof(char), 1, fp);
    fread(&jogador->tamanhoRegistro, sizeof(int), 1, fp);
    fread(&jogador->Prox, sizeof(long long int), 1, fp);
    fread(&jogador->id, sizeof(int), 1, fp);
    fread(&jogador->idade, sizeof(int), 1, fp);
    fread(&jogador->tamNomeJog, sizeof(int), 1, fp);
    if((jogador->nomeJogador = (char*) malloc ((jogador->tamNomeJog)*sizeof(char))) == NULL){ 
        printf("Falha no processamento do arquivo.");
        fclose(fp);
        exit(1);
    }
    for(int j = 0; j < jogador->tamNomeJog; j++){ //Leio byte a byte os caracters
        fread(&jogador->nomeJogador[j], sizeof(char), 1, fp);
    }
    fread(&jogador->tamNacionalidade, sizeof(int), 1, fp);
    if((jogador->nacionalidade = (char*) malloc ((jogador->tamNacionalidade)*sizeof(char))) == NULL){ 
        printf("Falha no processamento do arquivo.");
        fclose(fp);
        exit(1);        
    }
    for(int j = 0; j < jogador->tamNacionalidade; j++){ //Leio byte a byte os caracters
    fread(&jogador->nacionalidade[j], sizeof(char), 1, fp); 
    }
    fread(&jogador->tamNomeClube, sizeof(int), 1, fp);
    if((jogador->nomeClube = (char*) malloc ((jogador->tamNomeClube)*sizeof(char))) == NULL){
        printf("Falha no processamento do arquivo.");
        fclose(fp);
        exit(1);
    }
    for(int j = 0; j < jogador->tamNomeClube; j++){ //Leio byte a byte os caracters
        fread(&jogador->nomeClube[j], sizeof(char), 1, fp);
    }
    //imprimirjogador(jogador);
    return jogador;
}

void escrever_dados(regCabecalho* CabecalhoLocal, regDados** RegistrosLocais, FILE* fp){ //Escreve o cabecalho e todos os registros de dados no arquivo ".bin"

    fwrite(&CabecalhoLocal->status, sizeof(char), 1, fp);
    fwrite(&CabecalhoLocal->topo, sizeof(long long int), 1, fp);
    fwrite(&CabecalhoLocal->proxByteOffset, sizeof(long long int), 1,fp);
    fwrite(&CabecalhoLocal->nroRegArq, sizeof(int), 1, fp);
    fwrite(&CabecalhoLocal->nroRegRem, sizeof(int), 1, fp);

    for(int i = 0; i < CabecalhoLocal->nroRegArq; i++){
        fwrite(&RegistrosLocais[i]->removido, sizeof(char), 1, fp);
        fwrite(&RegistrosLocais[i]->tamanhoRegistro, sizeof(int), 1, fp);
        fwrite(&RegistrosLocais[i]->Prox, sizeof(long long int), 1, fp);
        fwrite(&RegistrosLocais[i]->id, sizeof(int), 1, fp);
        fwrite(&RegistrosLocais[i]->idade, sizeof(int), 1, fp);
        fwrite(&RegistrosLocais[i]->tamNomeJog, sizeof(int), 1, fp);
        for(int j = 0; j < RegistrosLocais[i]->tamNomeJog; j++){
            fwrite(&RegistrosLocais[i]->nomeJogador[j], sizeof(char), 1, fp);
        }
        fwrite(&RegistrosLocais[i]->tamNacionalidade, sizeof(int), 1, fp);
        for(int j = 0; j < RegistrosLocais[i]->tamNacionalidade; j++){
           fwrite(&RegistrosLocais[i]->nacionalidade[j], sizeof(char), 1, fp); 
        }
        fwrite(&RegistrosLocais[i]->tamNomeClube, sizeof(int), 1, fp);
        for(int j = 0; j < RegistrosLocais[i]->tamNomeClube; j++){
            fwrite(&RegistrosLocais[i]->nomeClube[j], sizeof(char), 1, fp);
        }
    }
    
    CabecalhoLocal->status = '1';
    fseek(fp, 0, SEEK_SET);
    fwrite(&CabecalhoLocal->status, sizeof(char), 1, fp);
}

void ler_binario(regDados** RegistrosLocais, int tamanho_real, FILE* fp, long long int *ByteSets){ //Le os dados de um arquivo ".bin" e traz as informações para a memória RAM (regDados e regCababecalho)
    int i = 0; //Indice que representa a quantidade de jogadores não removidos, lidos até o momento
    long long int Proxbyteoffset = 25;
    for(int a = 0; a < tamanho_real; a++){
        char teste_remocao;
        int tamRegLocal;
        fread(&teste_remocao, sizeof(char), 1, fp);
        fread(&tamRegLocal, sizeof(int), 1, fp);
        Proxbyteoffset+=tamRegLocal;
        if(teste_remocao == '1'){ //Se já estiver removido, vou para o próximo
            fseek(fp, tamRegLocal-5, SEEK_CUR);
        }else{
            if((RegistrosLocais[i] = (regDados*) malloc (sizeof(regDados))) == NULL){
                printf("Falha no processamento do arquivo.");
                fclose(fp);
                return;
            }
            ByteSets[i] = Proxbyteoffset - tamRegLocal;
            RegistrosLocais[i]->removido = teste_remocao;
            RegistrosLocais[i]->tamanhoRegistro = tamRegLocal;
            fread(&RegistrosLocais[i]->Prox, sizeof(long long int), 1, fp);
            fread(&RegistrosLocais[i]->id, sizeof(int), 1, fp);
            fread(&RegistrosLocais[i]->idade, sizeof(int), 1, fp);
            fread(&RegistrosLocais[i]->tamNomeJog, sizeof(int), 1, fp);
            if((RegistrosLocais[i]->nomeJogador = (char*) malloc ((RegistrosLocais[i]->tamNomeJog)*sizeof(char))) == NULL){ 
                printf("Falha no processamento do arquivo.");
                fclose(fp);
                return;
            }
            for(int j = 0; j < RegistrosLocais[i]->tamNomeJog; j++){ //Leio byte a byte os caracters
                fread(&RegistrosLocais[i]->nomeJogador[j], sizeof(char), 1, fp);
            }
            fread(&RegistrosLocais[i]->tamNacionalidade, sizeof(int), 1, fp);
            if((RegistrosLocais[i]->nacionalidade = (char*) malloc ((RegistrosLocais[i]->tamNacionalidade)*sizeof(char))) == NULL){ 
                printf("Falha no processamento do arquivo.");
                fclose(fp);
                return;
            }
            for(int j = 0; j < RegistrosLocais[i]->tamNacionalidade; j++){ //Leio byte a byte os caracters
            fread(&RegistrosLocais[i]->nacionalidade[j], sizeof(char), 1, fp); 
            }
            fread(&RegistrosLocais[i]->tamNomeClube, sizeof(int), 1, fp);
            if((RegistrosLocais[i]->nomeClube = (char*) malloc ((RegistrosLocais[i]->tamNomeClube)*sizeof(char))) == NULL){
                printf("Falha no processamento do arquivo.");
                fclose(fp);
                return;
            }
            for(int j = 0; j < RegistrosLocais[i]->tamNomeClube; j++){ //Leio byte a byte os caracters
                fread(&RegistrosLocais[i]->nomeClube[j], sizeof(char), 1, fp);
            }
            i++;
        }
    }
}