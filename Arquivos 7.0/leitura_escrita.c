/*
Autores: 2024 - Maicon Chaves Marques - 14593530
Karl Cruz Altenhofen- 14585976
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "help.h"
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

struct dadosI{
    int id;
    long long byteoffset;
};

typedef struct listaDados{
    int tamanho;
    long long byteoffset;
}listaDados;

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

regDados* ler_jogador(long long int byteoffset, FILE* fp){ //Faz a leitura de exclusivamente um unico jogador
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
            int pulaLixo;
            char lixoCifrao;
            if((RegistrosLocais[i] = (regDados*) malloc (sizeof(regDados))) == NULL){
                printf("Falha no processamento do arquivo.61\n");
                fclose(fp);
                return;
            }
            ByteSets[i] = Proxbyteoffset - tamRegLocal;
            RegistrosLocais[i]->removido = teste_remocao;
            RegistrosLocais[i]->tamanhoRegistro = tamRegLocal;
            pulaLixo = tamRegLocal;
            fread(&RegistrosLocais[i]->Prox, sizeof(long long int), 1, fp);
            fread(&RegistrosLocais[i]->id, sizeof(int), 1, fp);
            fread(&RegistrosLocais[i]->idade, sizeof(int), 1, fp);
            fread(&RegistrosLocais[i]->tamNomeJog, sizeof(int), 1, fp);
            pulaLixo -= 25;
            if((RegistrosLocais[i]->nomeJogador = (char*) malloc ((RegistrosLocais[i]->tamNomeJog)*sizeof(char))) == NULL){ 
                printf("Falha no processamento do arquivo.\n");
                fclose(fp);
                return;
            }
            for(int j = 0; j < RegistrosLocais[i]->tamNomeJog; j++){ //Leio byte a byte os caracters
                fread(&RegistrosLocais[i]->nomeJogador[j], sizeof(char), 1, fp);
            }
            pulaLixo=pulaLixo-RegistrosLocais[i]->tamNomeJog;
            fread(&RegistrosLocais[i]->tamNacionalidade, sizeof(int), 1, fp);
            if((RegistrosLocais[i]->nacionalidade = (char*) malloc ((RegistrosLocais[i]->tamNacionalidade)*sizeof(char))) == NULL){ 
                printf("Falha no processamento do arquivo.\n");
                fclose(fp);
                return;
            }
            for(int j = 0; j < RegistrosLocais[i]->tamNacionalidade; j++){ //Leio byte a byte os caracters
            fread(&RegistrosLocais[i]->nacionalidade[j], sizeof(char), 1, fp); 
            }
            pulaLixo= pulaLixo-RegistrosLocais[i]->tamNacionalidade-4;
            fread(&RegistrosLocais[i]->tamNomeClube, sizeof(int), 1, fp);
            if((RegistrosLocais[i]->nomeClube = (char*) malloc ((RegistrosLocais[i]->tamNomeClube)*sizeof(char))) == NULL){
                printf("Falha no processamento do arquivo.\n");
                fclose(fp);
                return;
            }
            for(int j = 0; j < RegistrosLocais[i]->tamNomeClube; j++){ //Leio byte a byte os caracters
                fread(&RegistrosLocais[i]->nomeClube[j], sizeof(char), 1, fp);
            }
            pulaLixo=pulaLixo-RegistrosLocais[i]->tamNomeClube-4;
            for(int i = 0; i < pulaLixo; i++){
                fread(&lixoCifrao, sizeof(char), 1, fp);
            }
            i++;
        }
    }
}

void escreveJogador(regDados* jogador, long long int byteoffset, FILE* fp){
    fseek(fp, byteoffset, SEEK_SET);
    fwrite(&jogador->removido, sizeof(char), 1, fp);
    fwrite(&jogador->tamanhoRegistro, sizeof(int), 1, fp);
    fwrite(&jogador->Prox, sizeof(long long int), 1, fp);
    fwrite(&jogador->id, sizeof(int), 1, fp);
    fwrite(&jogador->idade, sizeof(int), 1, fp);
    fwrite(&jogador->tamNomeJog, sizeof(int), 1, fp);
    if(jogador->tamNomeJog > 0){
        for(int i = 0; i < jogador->tamNomeJog; i++){
            fwrite(&jogador->nomeJogador[i], sizeof(char), 1, fp);
        }
    }
    fwrite(&jogador->tamNacionalidade, sizeof(int), 1, fp);
    if(jogador->tamNacionalidade > 0){
        for(int i = 0; i < jogador->tamNacionalidade; i++){
            fwrite(&jogador->nacionalidade[i], sizeof(char), 1, fp);
        }
    }
    fwrite(&jogador->tamNomeClube, sizeof(int), 1, fp);
    if(jogador->tamNomeClube > 0){
        for(int i = 0; i < jogador->tamNomeClube; i++){
            fwrite(&jogador->nomeClube[i], sizeof(char), 1, fp);
        }
    }
}

bool constroi_indice(indexDados** IndiceLocal, int tamanho, long long int *ByteSets, regDados** RegistrosLocais){ //Pego os arquivos que estão na RAM e gero um indice a partir deles
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

bool constroi_indice_sem_registro(indexDados** IndiceLocal, int tamanho, FILE* fp){ //Não trago nada para a memória primária, apenas crio o indice fazendo freads e fseek's
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
            if((indice = (indexDados*) malloc (sizeof(indexDados))) == NULL){ //Aloco espaço para o indice
                printf("Falha no processamento do arquivo.62\n");
                return false;
            }
            IndiceLocal[i]=indice; //Atualizo os valores que preciso no indice
            IndiceLocal[i]->byteoffset = Proxbyteoffset - tamRegLocal;
            jogador->removido = teste_remocao; //Os valores dos campos continuam sendo lidos para posterior integração a funcionalidade da função 2
            jogador->tamanhoRegistro = tamRegLocal;
            pulaLixo = tamRegLocal;
            fread(&jogador->Prox, sizeof(long long int), 1, fp);
            fread(&jogador->id, sizeof(int), 1, fp);
            IndiceLocal[i]->id = jogador->id;
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
        }
    }

    ordenaIndex(IndiceLocal, i); //Uso um algoritmo de ordenação para ordenar meu indice pela primeira vez, a partir dai, uso insercoes e remocoes já ordenadas
    return true;
}

bool preencherLista(listaDados** listaOrdenada, long long int *byteInicial, int tamanho, int* posicao, FILE* fp){ //Trago os arquivos por demanda, começa do ultimo que já trouxe e trago somente o necessário
    do{
        if(*posicao > 0)
            if(tamanho <= listaOrdenada[*posicao-1]->tamanho) //Significa que não preciso trazer mais elementos, já alcancei o tamanho que eu queria
                return true;

        if(*byteInicial == -1){ //Já trouxe todos os elementos possíveis
            return false;
        }
        fseek(fp, *byteInicial+1, SEEK_SET);
        if((listaOrdenada[(*posicao)] = (listaDados*) malloc (sizeof(listaDados))) == NULL){
            printf("Falha no processamento do arquivo.");
            exit(1);
        }
        fread(&listaOrdenada[*posicao]->tamanho, sizeof(int), 1 , fp); //Leitura dos campos necessários
        fread(&listaOrdenada[*posicao]->byteoffset, sizeof(long long int), 1 , fp);
        *byteInicial = listaOrdenada[*posicao]->byteoffset;
        (*posicao)++;
    }while(listaOrdenada[*posicao-1]->tamanho < tamanho);
    return true;
}

bool escrever_dados_indice(indexDados** IndiceLocal, int numero_indices, FILE* fpIndex){ //Escrevo todos os elementos no arquivo de indice
    char indexCab = '1';
    fseek(fpIndex, 0, SEEK_SET);
    fwrite(&indexCab, sizeof(char), 1, fpIndex);
    for(int i = 0; i < numero_indices; i++){
        fwrite(&IndiceLocal[i]->id, sizeof(int), 1, fpIndex);
        fwrite(&IndiceLocal[i]->byteoffset, sizeof(long long int), 1, fpIndex);
    }
    return true;
}

bool ler_index(indexDados** IndiceLocal, int numero_indices, FILE* fpIndex){ //Traz todos os elementos do indice para memóriaa primária, RAM
    for(int i = 0; i < numero_indices; i++){
        indexDados* elemento;
        if((elemento = (indexDados*) malloc (sizeof(indexDados))) == NULL){
            printf("Falha no processamento do arquivo.");
            return false;
        }
        fread(&elemento->id, sizeof(int), 1, fpIndex);
        fread(&elemento->byteoffset, sizeof(long long int), 1, fpIndex);
        IndiceLocal[i]=elemento;
    }
}

bool remover_arquivo_indice(indexDados** IndiceLocal, int tamanho, regDados* jogador){ //Passo o jogador que quero remover do arquivo de indice
    if(tamanho == 0){ //Se o indice tem tamanho 0, não consigo remover
        return false;
    }
    int indice;
    indice = busca_binaria_insercao(IndiceLocal, jogador->id, 0, tamanho-1); //Procuro pela posição que o elemento está no indice
    free(IndiceLocal[indice]); //Removo o elemento que está nessa posição
    IndiceLocal[indice] = NULL;
    for(int i = indice; i < tamanho-1; i++){ //Desloco todos os elementos do indice para a posição correta e realoco os espaços
        IndiceLocal[i]=IndiceLocal[i+1];
    }
    IndiceLocal[tamanho-1]=NULL;
    return true;
}

void criaJogador(regDados** jogador){ // cria, aloca e preenche um registro
    if(((*jogador) = (regDados*) malloc (sizeof(regDados))) == NULL){ //aloca o registro que sera inserido
        printf("Falha no processamento do arquivo.");
        return;
    }

    (*jogador)->removido = '0'; // preenche o campo removido
    (*jogador)->Prox = -1; // preenche o campo prox
    scanf("%d", &((*jogador)->id)); // preenche o campo id

    char idadetemp[5];
    scanf("%s", idadetemp); // idade e recebida como string pois pode ser NULO
    if(idadetemp[0] == 'N' || idadetemp[0] == 'n')
        (*jogador)->idade = -1; // -1 se NULO
    else
        (*jogador)->idade = atoi(idadetemp); // senao int

    char temp[100];
    scan_quote_string(temp); // recebe nomeJogador na temp
    if(temp == ""){
        (*jogador)->tamNomeJog = 0;
        (*jogador)->nomeJogador = NULL;
    }
    else{
        for(int i = 0; i < 100; i++) // conta o tamanho da temp para preencher o tamanho do nome do jogador
            if(temp[i] == '\0'){
                (*jogador)->tamNomeJog = i;
                break;
            }
        if(((*jogador)->nomeJogador = (char*) malloc ((*jogador)->tamNomeJog * sizeof(char))) == NULL){ // aloca o tamanho exato
            printf("Falha no processamento do arquivo.");
            return;
        }
        for(int i = 0; i < (*jogador)->tamNomeJog; i++){ // preenche o nome do jogador
            (*jogador)->nomeJogador[i] = temp[i];
        }
    }

    scan_quote_string(temp); // recebe nacionalidade na temp
    if(temp == ""){
        (*jogador)->tamNacionalidade = 0;
        (*jogador)->nacionalidade = NULL;
    }
    else{
        for(int i = 0; i < 100; i++) // conta o tamanho da temp para preencher o tamanho da nacionalidade
            if(temp[i] == '\0'){
                (*jogador)->tamNacionalidade = i;
                break;
            }
        if(((*jogador)->nacionalidade = (char*) malloc ((*jogador)->tamNacionalidade * sizeof(char))) == NULL){ // aloca o tamanho exato
            printf("Falha no processamento do arquivo.");
            return;
        }
        for(int i = 0; i < (*jogador)->tamNacionalidade; i++){ // preenche a nacionalidade
            (*jogador)->nacionalidade[i] = temp[i];
        }
    }

    scan_quote_string(temp); // recebe nome do clube na temp
    if(temp == ""){
        (*jogador)->tamNomeClube = 0;
        (*jogador)->nomeClube = NULL;
    }
    else{
        for(int i = 0; i < 100; i++) // conta o tamanho da temp para preencher o tamanho do nome do clube
            if(temp[i] == '\0'){
                (*jogador)->tamNomeClube = i;
                break;
            }
        if(((*jogador)->nomeClube = (char*) malloc ((*jogador)->tamNomeClube * sizeof(char))) == NULL){ // aloca o tamanho exato
            printf("Falha no processamento do arquivo.");
            return;
        }
        for(int i = 0; i < (*jogador)->tamNomeClube; i++){ // preenche a nacionalidade
            (*jogador)->nomeClube[i] = temp[i];
        }
    }
    (*jogador)->tamanhoRegistro = tamanhoreg(*jogador); // preenche o tamanho do registro
}

void resolveindice(indexDados* indice, long long int localdainsercao, indexDados*** IndiceLocal, regCabecalho* CabecalhoLocal){ // realoca e insere ordenadamente um novo indice no indiceLocal

    indice->byteoffset = localdainsercao; // inicializa o ultimo campo desse indice
    int localinsercaoindice = busca_binaria_insercao((*IndiceLocal), indice->id, 0, CabecalhoLocal->nroRegArq-2); // busca binaria para achar o local da insercao
    (*IndiceLocal) = (indexDados**) realloc ((*IndiceLocal), sizeof(indexDados*) * CabecalhoLocal->nroRegArq); // realoca para caber +1
    for(int i = CabecalhoLocal->nroRegArq-1; i > localinsercaoindice; i--){ // desloca os indices ate liberar o local da insercao
        (*IndiceLocal)[i] = (*IndiceLocal)[i-1];
    }
    (*IndiceLocal)[localinsercaoindice] = indice;
}