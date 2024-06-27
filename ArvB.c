/*
Autores: 2024 - Maicon Chaves Marques - 14593530
Karl Cruz Altenhofen- 14585976
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "ArvB.h"

struct dadosI{
    int id;
    long long byteoffset;
};

typedef struct reg{
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
}regDados;

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

bool criar_arvoreB(int tamanho, FILE* fp, CabArvB * CabecalhoArvB, FILE* fpArvB){ //Percorre o disco e insere todos os elementos não removidos na ArvB
    int i = 0; //Indice que representa a quantidade de jogadores não removidos, lidos até o momento
    long long int Proxbyteoffset = 25; //Começo em 25 por considerar o tamanho do cabeçalho
    CabecalhoArvB->nroChaves = 0; //Se vou criar minha arvore, certamente ela ainda não tem nenhuma chave
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
                printf("Falha no processamento do arquivo.\n");
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
            inserir_chave_ArvB(ElementoArvB.id, ElementoArvB.byteoffset, CabecalhoArvB, fpArvB); //Insere chave por chave na ArvB
        }
    }

    return true;
}

void inserir_chave_ArvB(int id, long long byteoffset, CabArvB * CabecalhoArvB, FILE* fpArvB){ //Chamo a função que comanda a ArvB
    indexDados ElementoArvB;
    ElementoArvB.id = id;
    ElementoArvB.byteoffset = byteoffset;
    Driver_ArvB(ElementoArvB, CabecalhoArvB, fpArvB);
}

void insercao_ArvB(int NroPaginaAtual, int id_buscado, long long int ByteSetDoID, int* id_promovido, long long int* ByteSetPromovido, int* paginaFilha, int* promocao, CabArvB * CabecalhoArvB, FILE* fpArvB){
    if(NroPaginaAtual == -1){ //Checo se estou em um nó NULL
        *id_promovido = id_buscado; //Sempre insiro em uma folha então subo para cima do NULL
        *ByteSetPromovido = ByteSetDoID; //Retorno o byteset que diz respeito a chave que deve ser colocada
        *paginaFilha = -1; //Não tenho filhos em uma inserção em um nó folha
        *promocao = 1; //O status de promoção vai para 1
        return;
    }else{
        //Aqui vou procurar o lugar o qual devo inserir
        NoArvB* PaginaArvB; //Aloco a página que será olhada
        PaginaArvB = (NoArvB*) malloc (sizeof(NoArvB));
        if(PaginaArvB == NULL){
            exit(1);
        }
        ler_PaginaNoDisco(PaginaArvB, NroPaginaAtual, fpArvB); //Trago a página
        int posicao = 0;
        for(int i = 0; i < PaginaArvB->NroChaves; i++){ //Procuro em qual posição eu deveria inserir esse id
            if(PaginaArvB->C[i]>id_buscado){
                break;
            }
            posicao++;
        }
        if(posicao < 3){ //Caso em que todos os nós da minha pagina são menores, ai não posso checar a posicão para testar se encontrei o nó
            if(PaginaArvB->C[posicao] == id_buscado){ //Checo se eu encontrei o nó, não posso inserir 2 ids repetidos
                *promocao = -1;
                return;
            }
        }
        insercao_ArvB(PaginaArvB->P[posicao], id_buscado, ByteSetDoID, id_promovido, ByteSetPromovido, paginaFilha, promocao, CabecalhoArvB, fpArvB); //Chamada recursiva da função, só que indo para o RRN que seria ideal para essa chave
        if(*promocao == 0){ //Já que não tenho promoção, já inseri, basta retornar
            free(PaginaArvB);
            return;
        }else if(*promocao == 1 && (PaginaArvB->NroChaves < 3)){ //Tenho uma promoção mas não precisa de um split, cabe nessa página
            posicao = 0;
            for(int i = 0; i < PaginaArvB->NroChaves; i++){ //Procuro em qual posição eu deveria inserir esse id
                if(PaginaArvB->C[i]>*id_promovido){
                    break;
                }
                posicao++;
            }
            for(int i = 1; i >= posicao; i--){ //Desloco os elementos para frente, para que eu possa colocar o novo
                PaginaArvB->C[i+1]=PaginaArvB->C[i];
                PaginaArvB->Pr[i+1]=PaginaArvB->Pr[i];
                PaginaArvB->P[i+2] = PaginaArvB->P[i+1];
            }
            PaginaArvB->C[posicao] = *id_promovido; //Insiro na página o elemento que quero colocar
            PaginaArvB->Pr[posicao]= *ByteSetPromovido; //Insiro seu id
            PaginaArvB->P[posicao+1] = *paginaFilha; //atualizo a página filha nessa posição
            PaginaArvB->NroChaves++; //Tenho uma chava a mais
            *promocao = 0; //Já inseri, não preciso mais promover ninguém
            escrever_PaginaNoDisco(PaginaArvB, NroPaginaAtual, fpArvB); //Atualizo essa página no disco
            free(PaginaArvB);
        }else{
            //Esse é o caso em que ocorre um split (Exceto raiz)
            int ChavesSplit[4] = {-1, -1, -1, -1}; //Página de split, que contém todos os 3 nós + 1 split
            long long int ByteSetsSplit[4] = {-1, -1, -1, -1};
            int ProxSplit[5] = {-1, -1, -1, -1, -1};
            for(int i = 0; i < PaginaArvB->NroChaves; i++){ //Copio todos da pagina atual para a página de split
                ChavesSplit[i] = PaginaArvB->C[i];
                ByteSetsSplit[i] = PaginaArvB->Pr[i];
            }
            for(int i = 0; i <= PaginaArvB->NroChaves; i++){ //Copio todos da pagina atual para a página de split
                ProxSplit[i] = PaginaArvB->P[i];
            }
            posicao = 0;
            for(int i = 0; i < PaginaArvB->NroChaves; i++){ //Procuro em qual posição eu deveria inserir esse id
                if(PaginaArvB->C[i]>*id_promovido){
                    break;
                }
                posicao++;
            }
            for(int i = 2; i >= posicao; i--){ //Desloca todos para frente para montar a página de split
                ChavesSplit[i+1]=ChavesSplit[i];
                ByteSetsSplit[i+1]=ByteSetsSplit[i];
                ProxSplit[i+2] = ProxSplit[i+1];
            }
            ChavesSplit[posicao] = *id_promovido; //Insiro o novo elemento na página de split
            ByteSetsSplit[posicao] = *ByteSetPromovido; //Insiro o novo elemento na página de split
            ProxSplit[posicao+1] = *paginaFilha; //Insiro o novo elemento na página de split
            NoArvB* NovaPaginaArvB;
            NovaPaginaArvB = (NoArvB*) malloc (sizeof(NoArvB)); //Essa será a nova página criada
            if(NovaPaginaArvB == NULL){
                exit(1);
            }
            NovaPaginaArvB->NroChaves = 2; //Como temos grau 4, no máximo 3 chaves, com o split a da direita terá 2
            NovaPaginaArvB->alturaNo = PaginaArvB->alturaNo; //A altura das 2 páginas é a mesma
            split(PaginaArvB, NovaPaginaArvB, ChavesSplit, ByteSetsSplit, ProxSplit, id_promovido, ByteSetPromovido, paginaFilha, CabecalhoArvB); //Chamo o split passando a pagina de split e as outras 2 (antiga e nova)
            PaginaArvB->NroChaves = 1; //A antiga página tem apenas 1 chave agora
            escrever_PaginaNoDisco(PaginaArvB, NroPaginaAtual, fpArvB); //Atualizo a página no disco
            escrever_PaginaNoDisco(NovaPaginaArvB, CabecalhoArvB->proxRRN, fpArvB); //Atualizo a página no disco
            *promocao = 1; //Há uma promoção, um split
            CabecalhoArvB->proxRRN++; //Tenho uma nova chave adicionada
            free(PaginaArvB);
            free(NovaPaginaArvB);
        }
    }
}

void split(NoArvB* PaginaArvB, NoArvB* NovaPaginaArvB, int ChavesSplit[], long long int ByteSetsSplit[], int ProxSplit[], int* id_promovido, long long int* ByteSetPromovido, int* paginaFilha, CabArvB * CabecalhoArvB){
    int posicao_split = 1;
    for(int i = 0; i < PaginaArvB->NroChaves; i++){ //Reseto os valores dessa página
        PaginaArvB->C[i] = -1;
        PaginaArvB->Pr[i] = -1;
    }
    for(int i = 0; i < PaginaArvB->NroChaves+1; i++){ //Reseto os valores dessa página
        PaginaArvB->P[i] = -1;
    }
    for(int i = 0; i < 3; i++){ //Reseto os valores dessa página
        NovaPaginaArvB->C[i] = -1;
        NovaPaginaArvB->Pr[i] = -1;
    }
    for(int i = 0; i < 4; i++){ //Reseto os valores dessa página
        NovaPaginaArvB->P[i] = -1;
    }
    for(int i = 0; i < posicao_split; i ++){ //Copio os valores até a posição do split para a primeira página
        PaginaArvB->C[i] = ChavesSplit[i];
        PaginaArvB->Pr[i] = ByteSetsSplit[i];
        PaginaArvB->P[i] = ProxSplit[i];
    }
    PaginaArvB->P[1] = ProxSplit[posicao_split]; //Copio os valores até a posição do split para a primeira página

    *id_promovido = ChavesSplit [posicao_split]; //Essa sera a chave que vai receber a promoção
    *ByteSetPromovido = ByteSetsSplit [posicao_split]; //Esse sera o byteset que vai receber a promoção
    *paginaFilha = CabecalhoArvB->proxRRN; //A filha da direita é a nova página criada

    int j = posicao_split+1;
    int i = 0;
    while(i < NovaPaginaArvB->NroChaves){ //Copio os valores depois do split para a nova página
        NovaPaginaArvB->C[i] = ChavesSplit[j];
        NovaPaginaArvB->Pr[i] = ByteSetsSplit[j];
        NovaPaginaArvB->P[i] = ProxSplit[j];
        j++;
        i++;
    }
    NovaPaginaArvB->P[i] = ProxSplit[j]; //Copio os valores depois do split para a nova página
}

void Driver_ArvB(indexDados ElementoArvB, CabArvB * CabecalhoArvB, FILE* fpArvB){ //Função que comanda a ArvB, e controla o split na raiz
    int id_promovido = -1; //Inicio dizendo que não quero promover ninguém
    long long int ByteSetPromovido = -1; //Não tem nimguém sendo promovido, então seu byteoffset também não existe
    int paginaFilha = -1; //Também começo sem ter página filha
    int promocao = 0; //Variavel que represendo o sinal de promoção
    if(CabecalhoArvB->nroChaves == 0){ //Estou criando minha arvore, ainda não tenho nenhum nó
        CabecalhoArvB->noRaiz = 0; //O nó raiz sera colocado no RRN zero. 
        CabecalhoArvB->nroChaves = 0; //AInda não tenho chaves, mas terei
        CabecalhoArvB->proxRRN = 1; //O próximo RRN passa a ser 1
        NoArvB* PaginaArvB;
        PaginaArvB = (NoArvB*) malloc (sizeof(NoArvB)); //Aloco a ArvB
        if(PaginaArvB == NULL){
            exit(1);
        }
        PaginaArvB->alturaNo = 0; //A altura de um nó folha é zero.
        PaginaArvB->NroChaves = 1; //Passo a ter 1 chave
        for(int i = 0; i < 3; i++){ //Começo as variáveis da página com -1
            PaginaArvB->C[i] = -1;
            PaginaArvB->Pr[i] = -1;
        }
        for(int i = 0; i < 4; i++){
            PaginaArvB->P[i] = -1;
        }
        PaginaArvB->C[0] = ElementoArvB.id; //Chave inserida
        PaginaArvB->Pr[0] = ElementoArvB.byteoffset; //Byteoffset no disco
        CabecalhoArvB->nroChaves++; //Tenho uma chave a mais
        escrever_PaginaNoDisco(PaginaArvB, 0, fpArvB); //Escrevo no disco
        free(PaginaArvB); //libero memória
    }else{
        insercao_ArvB(CabecalhoArvB->noRaiz, ElementoArvB.id, ElementoArvB.byteoffset, &id_promovido, &ByteSetPromovido, &paginaFilha, &promocao, CabecalhoArvB, fpArvB); //Inserção na ArvB
        CabecalhoArvB->nroChaves++; //Adicionei uma chave
            if(promocao == 1){ //Aqui preciso fazer split na raiz;
            NoArvB* PaginaArvB;
            PaginaArvB = (NoArvB*) malloc (sizeof(NoArvB)); //Pagina da Nova raiz
            NoArvB* RaizArvB;
            RaizArvB = (NoArvB*) malloc (sizeof(NoArvB)); //Pagina da raiz antiga
            if(PaginaArvB == NULL){
                exit(1);
            }
            if(RaizArvB == NULL){
                exit(1);
            }
            ler_PaginaNoDisco(RaizArvB, CabecalhoArvB->noRaiz, fpArvB); //Leio a raiz atual
            PaginaArvB->alturaNo = RaizArvB->alturaNo+1; //A altura da nova é um acima da antiga
            PaginaArvB->NroChaves = 1; //Após o split tenho uma chave na raiz
            for(int i = 0; i < 3; i++){ //Limpo os valores da página
                PaginaArvB->C[i] = -1;
                PaginaArvB->Pr[i] = -1;
            }
            for(int i = 0; i < 4; i++){
                PaginaArvB->P[i] = -1;
            }
            PaginaArvB->C[0] = id_promovido; //Coloco o id que foi promovido
            PaginaArvB->Pr[0] = ByteSetPromovido; //Coloco o ByteSet que foi promovido
            PaginaArvB->P[0] = CabecalhoArvB->noRaiz; //Coloca a raiz antiga como filha da nova na esquerda
            PaginaArvB->P[1] = paginaFilha; //Coloco a página filha que foi promovida na direita
            CabecalhoArvB->noRaiz = CabecalhoArvB->proxRRN; //O novo nó raiz será inserido aqui
            CabecalhoArvB->proxRRN++; //Atualizo para o próximo
            escrever_PaginaNoDisco(PaginaArvB, CabecalhoArvB->noRaiz, fpArvB); //Escreve a página no disco
            free(PaginaArvB);
            free(RaizArvB);
        }  
    }
}

void ler_PaginaNoDisco(NoArvB* PaginaArvB, int RRN, FILE* fp){ //Le a página do disco
    fseek(fp, (RRN*60)+60, SEEK_SET);
    fread(&PaginaArvB->alturaNo, sizeof(int), 1, fp);
    fread(&PaginaArvB->NroChaves, sizeof(int), 1, fp); 
    fread(&PaginaArvB->C[0], sizeof(int), 1, fp); 
    fread(&PaginaArvB->Pr[0], sizeof(long long int), 1, fp); 
    fread(&PaginaArvB->C[1], sizeof(int), 1, fp); 
    fread(&PaginaArvB->Pr[1], sizeof(long long int), 1, fp); 
    fread(&PaginaArvB->C[2], sizeof(int), 1, fp); 
    fread(&PaginaArvB->Pr[2], sizeof(long long int), 1, fp); 
    fread(&PaginaArvB->P[0], sizeof(int), 1, fp); 
    fread(&PaginaArvB->P[1], sizeof(int), 1, fp); 
    fread(&PaginaArvB->P[2], sizeof(int), 1, fp); 
    fread(&PaginaArvB->P[3], sizeof(int), 1, fp);
}

void escrever_PaginaNoDisco(NoArvB* PaginaArvB, int RRN, FILE* fp){ //Escreve a página no disco
    fseek(fp, (RRN*60)+60, SEEK_SET);
    fwrite(&PaginaArvB->alturaNo, sizeof(int), 1, fp);
    fwrite(&PaginaArvB->NroChaves, sizeof(int), 1, fp); 
    fwrite(&PaginaArvB->C[0], sizeof(int), 1, fp); 
    fwrite(&PaginaArvB->Pr[0], sizeof(long long int), 1, fp); 
    fwrite(&PaginaArvB->C[1], sizeof(int), 1, fp); 
    fwrite(&PaginaArvB->Pr[1], sizeof(long long int), 1, fp); 
    fwrite(&PaginaArvB->C[2], sizeof(int), 1, fp); 
    fwrite(&PaginaArvB->Pr[2], sizeof(long long int), 1, fp); 
    fwrite(&PaginaArvB->P[0], sizeof(int), 1, fp); 
    fwrite(&PaginaArvB->P[1], sizeof(int), 1, fp); 
    fwrite(&PaginaArvB->P[2], sizeof(int), 1, fp); 
    fwrite(&PaginaArvB->P[3], sizeof(int), 1, fp);
}

void escreverCabArvB(CabArvB * CabecalhoArvB, FILE* fpArvB){ //Escrevo o cabeçalho no disco
    CabecalhoArvB->status = '1';
    fseek(fpArvB, 0, SEEK_SET);
    fwrite(&CabecalhoArvB->status, sizeof(char), 1, fpArvB);
    fwrite(&CabecalhoArvB->noRaiz, sizeof(int), 1, fpArvB);
    fwrite(&CabecalhoArvB->proxRRN, sizeof(int), 1, fpArvB);
    fwrite(&CabecalhoArvB->nroChaves, sizeof(int), 1, fpArvB);
    char cifrao = '$';
    for(int i = 0; i < 47; i++){
        fwrite(&cifrao, sizeof(char), 1, fpArvB);
    }
}

void imprimirPaginadeDisco(NoArvB* PaginaArvB){ //Imprime uma página de disco
    printf("AlturaNo: %d\n", PaginaArvB->alturaNo);
    printf("NroChaves: %d\n", PaginaArvB->NroChaves);
    printf("C1 %d\n", PaginaArvB->C[0]);
    printf("PR1 %lld\n", PaginaArvB->Pr[0]);
    printf("C2 %d\n", PaginaArvB->C[1]);
    printf("PR2 %lld\n", PaginaArvB->Pr[1]);
    printf("C3 %d\n", PaginaArvB->C[2]);
    printf("PR3 %lld\n", PaginaArvB->Pr[2]);
    for(int i = 0; i < 4; i++){
        printf("P%d %d ", i+1, PaginaArvB->P[i]);
    }
    printf("\n\n");
}

void imprimirPaginadeSplit(int C[], long long int Pr[], int P[]){ //Imprime uma página de split
    printf("C1 %d\n", C[0]);
    printf("PR1 %lld\n", Pr[0]);
    printf("C2 %d\n", C[1]);
    printf("PR2 %lld\n", Pr[1]);
    printf("C3 %d\n", C[2]);
    printf("PR3 %lld\n", Pr[2]);
    printf("C4 %d\n", C[3]);
    printf("PR4 %lld\n", Pr[3]);
    for(int i = 0; i < 5; i++){
        printf("P%d %d ", i+1, P[i]);
    }
    printf("\n\n");
}

long long int busca_id_ArvB(int id, int RRNno, FILE* fpArvB){
    if(RRNno == -1) //nao encontrou o registro com id
        return -1;
        
    NoArvB* PaginaArvB; //cria e aloca um no
    PaginaArvB = (NoArvB*) malloc (sizeof(NoArvB));
    if(PaginaArvB == NULL){
        exit(1);
    }

    ler_PaginaNoDisco(PaginaArvB, RRNno, fpArvB);   //preenche o no alocado com a pagina do RRN dado

    int ProxRRN;
    for(int pos = 0; pos < PaginaArvB->NroChaves; pos++){
        if(PaginaArvB->C[pos] == id){   //se encontrou o id, da free no no e retorna
            long long Pr = PaginaArvB->Pr[pos];
            free(PaginaArvB);
            return Pr;
        }
        else if(PaginaArvB->C[pos] > id){   //se nao encontrou o id, mas ja passou do valor de id, busca no RRN apontado
            ProxRRN = PaginaArvB->P[pos];
            free(PaginaArvB);
            return busca_id_ArvB(id, ProxRRN, fpArvB);
        }
    }
    ProxRRN = PaginaArvB->P[PaginaArvB->NroChaves]; //se nao encontrou o id ou valor maior que o id, busca no maior RRN apontado
    free(PaginaArvB);
    return busca_id_ArvB(id, ProxRRN, fpArvB);
}