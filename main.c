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
#include "ArvB.h"

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

struct ArvBcab{
    char status;
    int noRaiz;
    int proxRRN;
    int nroChaves;
};

struct ArvBdados{
    int alturaNo;
    int NroChaves;
    int C[3]; //Chave (id)
    long long int Pr[3]; //Página no Registro (byteoffset)
    int P[4]; //Próximo RRN (ProxRRN)
};

void function1(){//Le os dados de um arquivo ".csv"  e escreve os dados em um arquivo ".bin"
    char arquivo1[30]; //Arquivo de entrada ".csv"
    char arquivo2[30]; //Arquivo de saida ".bin"
    scanf("%s", arquivo1);
    scanf("%s", arquivo2);
    FILE *fp;

    if (((fp = fopen(arquivo1, "r"))) == NULL){ //Abre o ".csv" para leitura
        printf("Falha no processamento do arquivo.\n");
        return;
    }

   regCabecalho* CabecalhoLocal;
    if((CabecalhoLocal = (regCabecalho*) malloc (sizeof(regCabecalho))) == NULL){ //Aloco espaço para o cabecalho
        printf("Falha no processamento do arquivo.\n");
        fclose(fp);
        return;
    }

    regDados** RegistrosLocais;
    if((RegistrosLocais = (regDados**) malloc (100*sizeof(regDados*))) == NULL){ //Aloco espaço para 100 jogadores inicialmente, e aumento conforme a demanda
        printf("Falha no processamento do arquivo.\n");
        fclose(fp);
        return;
    }
    
    fseek(fp, 45, SEEK_SET); //Pula a primeira linha, que representa as colunas

    ler_dados(CabecalhoLocal, &RegistrosLocais, fp); //Le os dados do arquivo ".csv" e traz as informações para a memória RAM (regDados e regCababecalho)
    fclose(fp);

    if (((fp = fopen(arquivo2, "wb"))) == NULL){ //Abre o arquivo ".bin" para escrita
        printf("Falha no processamento do arquivo.\n");
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
        printf("Falha no processamento do arquivo.\n");
        return;
    }
    
    regCabecalho* CabecalhoLocal;
    if((CabecalhoLocal = (regCabecalho*) malloc (sizeof(regCabecalho))) == NULL){ //Aloco o cabecalho
        printf("Falha no processamento do arquivo.\n");
        fclose(fp);
        return;
    }

    fread(&CabecalhoLocal->status, sizeof(char), 1, fp); //Leio as informações do cabecalho
    fread(&CabecalhoLocal->topo, sizeof(long long int), 1, fp);
    fread(&CabecalhoLocal->proxByteOffset, sizeof(long long int), 1, fp);
    fread(&CabecalhoLocal->nroRegArq, sizeof(int), 1, fp);
    fread(&CabecalhoLocal->nroRegRem, sizeof(int), 1, fp);
    
    if(CabecalhoLocal->status == '0'){
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    if(CabecalhoLocal->nroRegArq == 0){
        printf("Registro inexistente.\n\n");
        return;
    }

    int tamanho_real = CabecalhoLocal->nroRegArq+CabecalhoLocal->nroRegRem; //O tamanho que preciso realmente buscar são os ativos + os removidos

    regDados** RegistrosLocais;
    if((RegistrosLocais = (regDados**) malloc ((CabecalhoLocal->nroRegArq)*sizeof(regDados*))) == NULL){ //Aloco o tamanho exato
        printf("Falha no processamento do arquivo.\n");
        fclose(fp);
        return;
    }

    long long int *ByteSets;
    ByteSets = (long long int*) malloc(CabecalhoLocal->nroRegArq * sizeof(long long int));
    if (ByteSets == NULL) {
        printf("Falha no processamento do arquivo.\n");
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
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    regCabecalho* CabecalhoLocal;
    if((CabecalhoLocal = (regCabecalho*) malloc (sizeof(regCabecalho))) == NULL){ // aloca memoria para o cabecalho
        printf("Falha no processamento do arquivo.\n");
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
        printf("Falha no processamento do arquivo.\n");
        fclose(fp);
        return;
    }
    
    indexDados** IndiceLocal = NULL;
    CabArvB * CabecalhoArvB = NULL;
    FILE* fpArvB = NULL;
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

                newRegistro = filtra(newRegistro, &numeroResultados, 1, RegistrodeOffsets, 3, IndiceLocal, fp, 0, ByteSets, tamanho_real, CabecalhoArvB, fpArvB);

            }else{  // caso nao seja o primeiro, filtra o conjunto ja filtrado na iteracao anterior, alocando novamente o novo e dando free no anterior
                regDados** tempReg = newRegistro;
                int tempNum = numeroResultados;
                newRegistro = filtra(newRegistro, &numeroResultados, 2, RegistrodeOffsets, 3, IndiceLocal, fp, 0, ByteSets, tamanho_real, CabecalhoArvB, fpArvB);
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
        printf("Falha no processamento do arquivo.\n");
        return;
    }
    
    regCabecalho* CabecalhoLocal;
    if((CabecalhoLocal = (regCabecalho*) malloc (sizeof(regCabecalho))) == NULL){ //Aloco o cabecalho
        printf("Falha no processamento do arquivo.\n");
        fclose(fp);
        return;
    }

    fread(&CabecalhoLocal->status, sizeof(char), 1, fp); //Leio as informações do cabecalho
    fread(&CabecalhoLocal->topo, sizeof(long long int), 1, fp);
    fread(&CabecalhoLocal->proxByteOffset, sizeof(long long int), 1, fp);
    fread(&CabecalhoLocal->nroRegArq, sizeof(int), 1, fp);
    fread(&CabecalhoLocal->nroRegRem, sizeof(int), 1, fp);
    
    if(CabecalhoLocal->status == '0'){ //Checo se há algum erro no meu arquivo de dados.
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    if(CabecalhoLocal->nroRegArq == 0){ //Checo se existem registros no meu arquivo de dados.
        printf("Registro inexistente.\n\n");
        return;
    }

    int tamanho_real = CabecalhoLocal->nroRegArq+CabecalhoLocal->nroRegRem; //O tamanho que preciso realmente buscar são os ativos + os removidos
 
    indexDados** IndiceLocal;
    if((IndiceLocal = (indexDados**) malloc ((CabecalhoLocal->nroRegArq)*sizeof(indexDados*))) == NULL){ //Aloco o tamanho exato
        printf("Falha no processamento do arquivo.\n");
        fclose(fp);
        return;
    }

    constroi_indice_sem_registro(IndiceLocal, tamanho_real, fp); //Contruo meu indice
    fclose(fp);

    if (((fpIndex = fopen(arquivo2, "wb"))) == NULL){ //Abro o binário que sera o destino do indice para escrita
        printf("Falha no processamento do arquivo.\n");
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
        printf("Falha no processamento do arquivo.\n");
        return;
    }
    
    regCabecalho* CabecalhoLocal;
    if((CabecalhoLocal = (regCabecalho*) malloc (sizeof(regCabecalho))) == NULL){ //Aloco o cabecalho
        printf("Falha no processamento do arquivo.\n");
        fclose(fp);
        return;
    }

    fread(&CabecalhoLocal->status, sizeof(char), 1, fp); //Leio as informações do cabecalho
    fread(&CabecalhoLocal->topo, sizeof(long long int), 1, fp);
    fread(&CabecalhoLocal->proxByteOffset, sizeof(long long int), 1, fp);
    fread(&CabecalhoLocal->nroRegArq, sizeof(int), 1, fp);
    fread(&CabecalhoLocal->nroRegRem, sizeof(int), 1, fp);
    
    if(CabecalhoLocal->status == '0'){ //Testo o status do arquivo que estou lendo
        printf("Falha no processamento do arquivo.\n");
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
        printf("Falha no processamento do arquivo.\n");
        fclose(fp);
        return;
    }

    fseek(fp, 0, SEEK_SET);
    CabecalhoLocal->status = '0'; //A partir de agora vou editar meu arquivo, então aviso que pode haver falha na integridade do arquivo de dados
    fwrite(&CabecalhoLocal->status, sizeof(char), 1, fp);

    indexDados** IndiceLocal;
    if((IndiceLocal = (indexDados**) malloc ((CabecalhoLocal->nroRegArq)*sizeof(indexDados*))) == NULL){ //Aloco o tamanho exato
        printf("Falha no processamento do arquivo.\n");
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
            printf("Falha no processamento do arquivo.\n");
            return;
        }
    }else{
        fread(&indexCab, sizeof(char), 1, fpIndex); //Le as informacoes do cabecalho
        if(indexCab == '0'){
            printf("Falha no processamento do arquivo.\n");
            return;
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

    CabArvB * CabecalhoArvB = NULL;
    FILE* fpArvB = NULL;
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
        
        antecipa_id(); //Essa função le a linha do buffer, se tiver um ID, escrevo no buffer uma busca por ID de um campo só, se não tiver ID devolvo a linha inteira para o buffer stdin
        //A função antecipa se terá um id, e adpata a entrada para que a busca da função 3,5,8,9 consigam interpretar e buscar corretamente

        scanf("%d", &numeroCampos);
        for(int j = 0; j < numeroCampos; j++){ // loop em que cada iteracao sera um filtro diferente a ser aplicado aos registros
            if(j == 0){ // caso seja o primeiro filtro, aplica filtro ao conjunto de registros completo

                newRegistro = filtra(newRegistro, &numeroResultados, 1, RegistrodeOffsets, 5, IndiceLocal, fp, numeroResultadosFixo, ByteSets, tamanho_real, CabecalhoArvB, fpArvB);

            }else{  // caso nao seja o primeiro, filtra o conjunto ja filtrado na iteracao anterior, alocando novamente o novo e dando free no anterior
                regDados** tempReg = newRegistro;
                int tempNum = numeroResultados;
                newRegistro = filtra(newRegistro, &numeroResultados, 2, RegistrodeOffsets, 5, IndiceLocal, fp, numeroResultadosFixo, ByteSets, tamanho_real, CabecalhoArvB, fpArvB);
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
                    printf("Falha no processamento do arquivo.\n");
                    return;
                }
                if((RegistroLista = (listaDados*) malloc (sizeof(listaDados))) == NULL){ //Aloco um novo elemento de minha lista ordenada de arquivos removidos
                    printf("Falha no processamento do arquivo.\n");
                    return;
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
                        printf("Falha no processamento do arquivo.\n");
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
        printf("Falha no processamento do arquivo.\n");
        return;
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

void function6(){ //insere um registro  
    char arquivo1[30]; //Arquivo de entrada ".bin"
    scanf("%s", arquivo1);
    char arquivo2[30]; //Arquivo de indice ".bin"
    scanf("%s", arquivo2);
    FILE *fp;
    FILE *fpIndex;
    regCabecalho* CabecalhoLocal;
    
    if (((fp = fopen(arquivo1, "rb+"))) == NULL){ //Abre o binario para leitura e escrita
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    if((CabecalhoLocal = (regCabecalho*) malloc (sizeof(regCabecalho))) == NULL){ //Aloca o cabecalho
        printf("Falha no processamento do arquivo.\n");
        return;
    }
    fread(&CabecalhoLocal->status, sizeof(char), 1, fp); //Le as informacoes do cabecalho
    fread(&CabecalhoLocal->topo, sizeof(long long int), 1, fp);
    fread(&CabecalhoLocal->proxByteOffset, sizeof(long long int), 1, fp);
    fread(&CabecalhoLocal->nroRegArq, sizeof(int), 1, fp);
    fread(&CabecalhoLocal->nroRegRem, sizeof(int), 1, fp);
    
    if(CabecalhoLocal->status == '0'){
        printf("Falha no processamento do arquivo.\n");
        fclose(fp);
        return;
    }

    indexDados** IndiceLocal;
    if((IndiceLocal = (indexDados**) malloc ((CabecalhoLocal->nroRegArq)*sizeof(indexDados*))) == NULL){ //Aloca o tamanho exato do indice
        printf("Falha no processamento do arquivo.\n");
        fclose(fp);//
        return;
    }
    char indexCab;

    if (((fpIndex = fopen(arquivo2, "rb+"))) == NULL){ //Abre o indice para leitura e escrita
        //entra no if se falhar em abrir o arquivo, ou seja, arquivo de indice nao existe
        int tamanho_real = CabecalhoLocal->nroRegArq+CabecalhoLocal->nroRegRem; // salva o numero de registros 

        constroi_indice_sem_registro(IndiceLocal, tamanho_real, fp); //constroi o indice
 
        if (((fpIndex = fopen(arquivo2, "wb"))) == NULL){ //Abre o indice para leitura e escrita
            printf("Falha no processamento do arquivo.\n");
            return;
        }
    }else{
        fread(&indexCab, sizeof(char), 1, fpIndex); //Le as informacoes do cabecalho
        if(indexCab == '0'){
            printf("Falha no processamento do arquivo.\n");
            return;
        }
        ler_index(IndiceLocal, CabecalhoLocal->nroRegArq, fpIndex); // preenche o indiceLocal
    }

    indexCab = '0';
    fseek(fpIndex, 0, SEEK_SET);
    fwrite(&indexCab, sizeof(char), 1, fpIndex); //define o bit de status para 0 e escreve ele no arquivo de indice

    listaDados** listaOrdenada;
    if((listaOrdenada = (listaDados**) malloc (((CabecalhoLocal->nroRegRem))*sizeof(listaDados*))) == NULL){ //aloca a lista ordenada de registros removidos
        printf("Falha no processamento do arquivo.\n");
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
                if(posicao-1!=-1){
                    byteinicial = listaOrdenada[posicao-1]->byteoffset;
                }
            }
    
        }
        free(jogador->nomeJogador); // registro ja foi escrito onde devia, é necessario free antes da proxima iteracao do loop
        free(jogador->nacionalidade);
        free(jogador->nomeClube);
        free(jogador);
    }

    if(!escrever_dados_indice(IndiceLocal, CabecalhoLocal->nroRegArq, fpIndex)){ //escreve os dados no indice
        printf("Falha no processamento do arquivo.\n");
        return;
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

void function7(){ //Le os dados de um arquivo ".bin" e cria uma ArvB para esse binário lido
    char arquivo1[30]; //Arquivo de entrada ".bin"
    scanf("%s", arquivo1);
    char arquivo2[30]; //Arquivo de saida ArvB ".bin"
    scanf("%s", arquivo2);
    FILE *fp; //Ponteiro para o controle do arquivo de dados
    FILE *fpArvB; //Ponteiro para o controle da ArvB
    if (((fp = fopen(arquivo1, "rb+"))) == NULL){ //Abro o binário para leitura
        printf("Falha no processamento do arquivo.\n");
        return;
    }
    
    regCabecalho* CabecalhoLocal;
    if((CabecalhoLocal = (regCabecalho*) malloc (sizeof(regCabecalho))) == NULL){ //Aloco o cabecalho
        printf("Falha no processamento do arquivo.\n");
        fclose(fp);
        return;
    }

    fread(&CabecalhoLocal->status, sizeof(char), 1, fp); //Leio as informações do cabecalho
    fread(&CabecalhoLocal->topo, sizeof(long long int), 1, fp);
    fread(&CabecalhoLocal->proxByteOffset, sizeof(long long int), 1, fp);
    fread(&CabecalhoLocal->nroRegArq, sizeof(int), 1, fp);
    fread(&CabecalhoLocal->nroRegRem, sizeof(int), 1, fp);
    
    if(CabecalhoLocal->status == '0'){ //Checo se há algum erro no meu arquivo de dados.
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    if(CabecalhoLocal->nroRegArq == 0){ //Checo se existem registros no meu arquivo de dados.
        printf("Registro inexistente.\n\n");
        return;
    }

    int tamanho_real = CabecalhoLocal->nroRegArq+CabecalhoLocal->nroRegRem; //O tamanho que preciso realmente buscar são os ativos + os removidos
 
    CabArvB * CabecalhoArvB; //Cabeçalho da ArvB
    CabecalhoArvB = (CabArvB*) malloc (sizeof(CabArvB));
    if(CabecalhoArvB == NULL){
        return;
    }

    if (((fpArvB = fopen(arquivo2, "wb+"))) == NULL){ //Abro o binário para leitura
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    CabecalhoArvB->status = '0'; //Escrevo que a ArvB ainda não está pronta
    fwrite(&CabecalhoArvB, sizeof(char), 1, fpArvB);
    CabecalhoArvB->noRaiz = -1; //Inicio sem nenhuma raiz
    CabecalhoArvB->proxRRN = 0; //O próximo é naturalmente o primeiro
    CabecalhoArvB->nroChaves = 0; //Inicio sem nenhum nó

    criar_arvoreB(tamanho_real, fp, CabecalhoArvB, fpArvB); //Construo meu indice usando o que tiver no registro
    fclose(fp);

    escreverCabArvB(CabecalhoArvB, fpArvB); //Escrevo o cabeçalho na memória

    fclose(fpArvB);
    free(CabecalhoArvB);
    free(CabecalhoLocal);
    binarioNaTela(arquivo2);
}

void pesquisa_ArvB(int function){ //Modularização das funções 8 e 9
    char arquivo1[30]; //Arquivo de entrada ".bin"
    scanf("%s", arquivo1);
    char arquivo2[30]; //Arquivo de arvoreB ".bin"
    scanf("%s", arquivo2);
    FILE *fp;
    FILE *fpArvB;
    regCabecalho* CabecalhoLocal;
    
    if (((fp = fopen(arquivo1, "rb"))) == NULL){ //Abre o binario para leitura
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    if((CabecalhoLocal = (regCabecalho*) malloc (sizeof(regCabecalho))) == NULL){ //Aloca o cabecalho
        printf("Falha no processamento do arquivo.\n");
        return;
    }
    fread(&CabecalhoLocal->status, sizeof(char), 1, fp); //Le as informacoes do cabecalho
    fread(&CabecalhoLocal->topo, sizeof(long long int), 1, fp);
    fread(&CabecalhoLocal->proxByteOffset, sizeof(long long int), 1, fp);
    fread(&CabecalhoLocal->nroRegArq, sizeof(int), 1, fp);
    fread(&CabecalhoLocal->nroRegRem, sizeof(int), 1, fp);
    
    if(CabecalhoLocal->status == '0'){  //checa o status do cabecalho
        printf("Falha no processamento do arquivo.\n");
        fclose(fp);
        return;
    }

    if(CabecalhoLocal->nroRegArq == 0){ //Checo se existem registros no meu arquivo de dados.
        printf("Registro inexistente.\n\n");
        return;
    }

    CabArvB * CabecalhoArvB;    //cria e aloca o arquivo de arvoreB
    CabecalhoArvB = (CabArvB*) malloc (sizeof(CabArvB));
    if(CabecalhoArvB == NULL){
        return;
    }

    int tamanho_real = CabecalhoLocal->nroRegArq+CabecalhoLocal->nroRegRem; // salva o numero de registros

    if (((fpArvB = fopen(arquivo2, "rb+"))) == NULL){ //Abre o indice para leitura
        //entra no if se falhar em abrir o arquivo, ou seja, arquivo de indice nao existe 

        if (((fpArvB = fopen(arquivo2, "wb+"))) == NULL){ //Abro a arvoreB para escrita. Crio e escrevo ela
            printf("Falha no processamento do arquivo.\n");
            return;
        }
        criar_arvoreB(tamanho_real, fp, CabecalhoArvB, fpArvB);

    }else{  //Le as informacoes do cabecalho
        fread(&CabecalhoArvB->status, sizeof(char), 1, fpArvB); 
        if(CabecalhoArvB->status == '0'){   //checa o status da arvoreB
            printf("Falha no processamento do arquivo.\n");
            return;
        }
        fread(&CabecalhoArvB->noRaiz, sizeof(int), 1, fpArvB);
        fread(&CabecalhoArvB->proxRRN, sizeof(int), 1, fpArvB);
        fread(&CabecalhoArvB->nroChaves, sizeof(int), 1, fpArvB);
    }

    CabecalhoArvB->status = '0';     //define o bit de status para 0 e escreve ele no arquivo de indice
    fseek(fpArvB, 0, SEEK_SET);
    fwrite(&CabecalhoArvB->status, sizeof(char), 1, fpArvB);
    
    long long int *ByteSets;
    ByteSets = (long long int*) malloc(CabecalhoLocal->nroRegArq * sizeof(long long int));  //vetor de byteoffsets
    if(ByteSets == NULL) {
        printf("Falha no processamento do arquivo.\n");
        fclose(fp);
        return;
    }

    indexDados** IndiceLocal = NULL;
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

        antecipa_id();  //checa se ha o campo id na busca

        if(function == 8){  //se for a funcao 8, a busca so usa um campo
            numeroCampos = 1;
        }else{  //se for a funcao 9, a busca usa n campos
            scanf("%d", &numeroCampos);
        }

        for(int j = 0; j < numeroCampos; j++){ // loop em que cada iteracao sera uma filtragem diferente a ser aplicado aos registros
            if(j == 0){ // caso seja o primeiro filtro, aplica filtro ao conjunto de registros completo

                newRegistro = filtra(newRegistro, &numeroResultados, 1, RegistrodeOffsets, function, IndiceLocal, fp, numeroResultadosFixo, ByteSets, tamanho_real, CabecalhoArvB, fpArvB);

            }else{  // caso nao seja o primeiro, filtra o conjunto ja filtrado na iteracao anterior, alocando novamente o novo e dando free no anterior
                regDados** tempReg = newRegistro;
                int tempNum = numeroResultados;
                newRegistro = filtra(newRegistro, &numeroResultados, 2, RegistrodeOffsets, function, IndiceLocal, fp, numeroResultadosFixo, ByteSets, tamanho_real, CabecalhoArvB, fpArvB);
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

        if(function==8){    //SE FOR FUNCAO 8, A BUSCA É EM CAPS LOCK
            printf("BUSCA %d\n\n", i+1);
        }else{
            printf("Busca %d\n\n", i+1);
        }

        if(numeroResultados == 0){  //busca nao encontrou nenhum registro compativel
            printf("Registro inexistente.\n\n");
        }else{
            for(int j = 0; j < numeroResultados; j++){ //imprimi todos os resultados
               imprimirjogador(newRegistro[j]);
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

    escreverCabArvB(CabecalhoArvB, fpArvB); //escreve o cabecalho da arvoreB

    fseek(fp, 0, SEEK_SET);
    CabecalhoLocal->status = '1'; // escreve todas as mudancas do cabecalho dos registros
    fwrite(&CabecalhoLocal->status, sizeof(char), 1, fp);
    fwrite(&CabecalhoLocal->topo, sizeof(long long int), 1, fp);
    fwrite(&CabecalhoLocal->proxByteOffset, sizeof(long long int), 1, fp);
    fwrite(&CabecalhoLocal->nroRegArq, sizeof(int), 1, fp);
    fwrite(&CabecalhoLocal->nroRegRem, sizeof(int), 1, fp);

    free(ByteSets); //frees e fcloses
    fclose(fp);
    fclose(fpArvB);
    free(CabecalhoArvB);
    free(CabecalhoLocal);
}

void function10(){ //insere um registro no arquivo de dados e na ArvB
    char arquivo1[30]; //Arquivo de entrada ".bin"
    scanf("%s", arquivo1);
    char arquivo2[30]; //Arquivo de indice ArvB ".bin"
    scanf("%s", arquivo2);
    FILE *fp; //Ponteiro do Registro
    FILE *fpArvB; //Ponteiro da ArvB
    regCabecalho* CabecalhoLocal;

    if(strcmp(arquivo2, "naoexiste.bin") == 0){ //Caso em que não devo gerar a árvore
        printf("Falha no processamento do arquivo.\n");
        return;
    }
    
    if (((fp = fopen(arquivo1, "rb+"))) == NULL){ //Abre o binario para leitura e escrita
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    if((CabecalhoLocal = (regCabecalho*) malloc (sizeof(regCabecalho))) == NULL){ //Aloca o cabecalho
        printf("Falha no processamento do arquivo.\n");
        return;
    }
    fread(&CabecalhoLocal->status, sizeof(char), 1, fp); //Le as informacoes do cabecalho
    fread(&CabecalhoLocal->topo, sizeof(long long int), 1, fp);
    fread(&CabecalhoLocal->proxByteOffset, sizeof(long long int), 1, fp);
    fread(&CabecalhoLocal->nroRegArq, sizeof(int), 1, fp);
    fread(&CabecalhoLocal->nroRegRem, sizeof(int), 1, fp);
    
    if(CabecalhoLocal->status == '0'){ //Testo o status do arquivo que estou lendo
        printf("Falha no processamento do arquivo.\n");
        fclose(fp);
        return;
    }

    CabArvB * CabecalhoArvB; //Cabeçalho da ArvB
    CabecalhoArvB = (CabArvB*) malloc (sizeof(CabArvB));
    if(CabecalhoArvB == NULL){
        return;
    }

    if (((fpArvB = fopen(arquivo2, "rb+"))) == NULL){ //Abro a ArvB para leitura e escrita
        //entra no if se falhar em abrir o arquivo, ou seja, arquivo de indice nao existe
        int tamanho_real = CabecalhoLocal->nroRegArq+CabecalhoLocal->nroRegRem; // salva o numero de registros 

        if (((fpArvB = fopen(arquivo2, "wb+"))) == NULL){ //Abro o binário para escrita
            printf("Falha no processamento do arquivo.\n");
            return;
        }

        CabecalhoArvB->noRaiz = -1; //Inicio sem nenhuma raiz
        CabecalhoArvB->proxRRN = 0; //O próximo é naturalmente o primeiro
        CabecalhoArvB->nroChaves = 0; //Inicio sem nenhum nó

        criar_arvoreB(tamanho_real, fp, CabecalhoArvB, fpArvB); //Construo minha ArvB

    }else{
        fread(&CabecalhoArvB->status, sizeof(char), 1, fpArvB); //Le as informacoes da ArvB
        if(CabecalhoArvB->status == '0'){ //Testo o status da ArvB que estou lendo
            printf("Falha no processamento do arquivo.\n");
            return;
        }
        fread(&CabecalhoArvB->noRaiz, sizeof(int), 1, fpArvB);
        fread(&CabecalhoArvB->proxRRN, sizeof(int), 1, fpArvB);
        fread(&CabecalhoArvB->nroChaves, sizeof(int), 1, fpArvB);
    }

    CabecalhoArvB->status = '0';
    fseek(fpArvB, 0, SEEK_SET);
    fwrite(&CabecalhoArvB->status, sizeof(char), 1, fpArvB); //define o bit de status para 0 e escreve ele no arquivo de indice

    listaDados** listaOrdenada;
    if((listaOrdenada = (listaDados**) malloc (((CabecalhoLocal->nroRegRem))*sizeof(listaDados*))) == NULL){ //aloca a lista ordenada de registros removidos
        printf("Falha no processamento do arquivo.\n");
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
            indice->byteoffset = localdainsercao; // inicializa o ultimo campo desse indice
            inserir_chave_ArvB(indice->id,  indice->byteoffset, CabecalhoArvB, fpArvB); //Insere uma chave na ArvB
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
                indice->byteoffset = localdainsercao; // inicializa o ultimo campo desse indice
                inserir_chave_ArvB(indice->id,  indice->byteoffset, CabecalhoArvB, fpArvB); //Insere uma chave na ArvB
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
                indice->byteoffset = byteoffsetdainsercao; // inicializa o ultimo campo desse indice
                inserir_chave_ArvB(indice->id,  indice->byteoffset, CabecalhoArvB, fpArvB); //Insere uma chave na ArvB
            }

            free(listaOrdenada[posicaodeinsercao]); // free no elemento que foi removido da lista ordenada (sobreescrito)
            for(int j = posicaodeinsercao; j < posicao - 1; j++){ // desloca a lista ordenada de elementos removidos para refletir seu novo encadeamento
                listaOrdenada[j] = listaOrdenada[j+1];
            }
            posicao--; // lista ordenada de elementos removidos tem um elemento a menos
            if(posicaodeinsercao == posicao){ // se o elemento removido for o ultimo, altera o byteinicial
                if(posicao-1!=-1){
                    byteinicial = listaOrdenada[posicao-1]->byteoffset;
                }
            }
    
        }
        free(jogador->nomeJogador); // registro ja foi escrito onde devia, é necessario free antes da proxima iteracao do loop
        free(jogador->nacionalidade);
        free(jogador->nomeClube);
        free(indice);
        free(jogador);
    }

    escreverCabArvB(CabecalhoArvB, fpArvB); //Escreve o CabArvB no disco

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
    fclose(fpArvB);
    free(CabecalhoArvB);
    free(CabecalhoLocal);

    binarioNaTela(arquivo1); // output 
    binarioNaTela(arquivo2);
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
        pesquisa_ArvB(8);
        break;
    case 9:
        pesquisa_ArvB(9);
        break;
    case 10:
        function10();
        break;
    default:
        break;
    }

    return 0;
}