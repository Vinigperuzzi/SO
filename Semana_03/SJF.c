/*
                            ATENÇÃO
Esse programa tem por finalidade simular a política de escalonamento SJF de sistemas operacionais.
Por favor, note que o programa DEVE SER EXECUTADO PELO TERMINAL, pois ele exige que dois parâmetros de compilação
sejam passados. O primeiro parâmetro deve ser o nome do arquivo origem das informações, no caso, "tarefas.txt" e
o segundo deve ser o número de processadores, que pode ser qualquer valor.
*/
#define QTD 30 //Quantidade de processos possíveis em cada processador, mínimo igual a quantidade de linhas de "tarefas.txt"
#define NOMEPROC 5 //Quantidade de caracteres no nome dos processos

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct processos{
    char nome[NOMEPROC];
    int tempo;
} processo;

typedef struct processadores{
    int processo[QTD];
    int tempoini[QTD];
    int tempofim[QTD];
    int tMin;
    int ultPos;
} CPU;

int pegaLinhas(char nomearq[]);
void learq(int linhas, processo process[], char nomearq[]);
void bubbleSort(int linhas, processo process[]);
void escalona(int nCPU, int linhas, CPU processador[], processo process[]);
void escrevearq(int nCPU, int linhas, CPU processador[], processo process[]);


int main(int argc, char *argv[]){

    int linhas, nCPU;
    nCPU = atoi(argv[2]);

    if (nCPU <= 0){
        printf("\n\t\t\tERRO!!!\nForam passados zero ou menos processadores por parâmetro no comando da execução do programa.\n\n");
        exit(1);
    }
    
    CPU processador[nCPU];
    char nomearq[12];

    strcpy(nomearq, argv[1]);

    linhas = pegaLinhas(nomearq);

    processo process[linhas];

    learq(linhas, process, nomearq);
    bubbleSort(linhas, process);
    escalona(nCPU, linhas, processador, process);
    escrevearq(nCPU, linhas, processador, process);

    printf("\n\nPrograma executado com sucesso, por favor, procure o arquivo chamado \"saida.txt\" no diretório em que o programa foi executado.\n\n");

    return 0;
}

void learq(int linhas, processo process[linhas], char nomearq[12]){

    int i;

    FILE * arq = NULL;

    if((arq = fopen(nomearq, "r")) == NULL){
        printf("Não foi possível abrir o arquivo\n\n");
        exit(1);
    }

    for (i=0; i<linhas; i++){
        fscanf(arq, "%s", process[i].nome);
        fscanf(arq, "%d", &process[i].tempo);
    }

    fclose(arq);
    return;
}

void bubbleSort(int linhas, processo process[linhas]){

    int i, j;
    processo temp;

    for (i=0; i<linhas-1; i++){
        for(j=0; j<linhas - i - 1; j++){
            if (process[j].tempo > process[j+1].tempo){
                temp = process[j];
                process[j] = process[j+1];
                process[j+1] = temp;
            }
        }
    }
    return;
}

void escalona(int nCPU, int linhas, CPU processador[nCPU], processo process[linhas]){
    int i, j, menorVal, menorCPU, elemento=0;

    for (i=0; i < nCPU; i++){
        processador[i].tMin = 0;
        processador[i].ultPos = 0;
        for (j=0; j < linhas; j++){
            processador[i].tempoini[j] = 0;
            processador[i].tempofim[j] = 0;
        }
    }

    do{
        menorVal = 9999999;
        for (i=0; i < nCPU; i++){
            if (processador[i].tMin < menorVal){
                menorVal = processador[i].tMin;
                menorCPU = i;
            }
        }
        processador[menorCPU].processo[processador[menorCPU].ultPos] = elemento;//O campo processo recebe o índice do elemento do vetor ordenado.
        processador[menorCPU].tempoini[processador[menorCPU].ultPos] = processador[menorCPU].tempoini[processador[menorCPU].ultPos] + processador[menorCPU].tMin;//Tempo ini passa a ser a soma do próprio tempoini com o tempo mínimo que o processador tinha antes da atualização.
        processador[menorCPU].tempofim[processador[menorCPU].ultPos] = processador[menorCPU].tempoini[processador[menorCPU].ultPos] + process[elemento].tempo;//Tempofim passa a ser o tempoini atualizado mais o tempo para o processo.
        processador[menorCPU].tMin = processador[menorCPU].tMin + process[elemento].tempo;//O tempo mínimo do processador escolhido passa a ser a soma do próprio tempo mínimo com o tempo do processo.
        processador[menorCPU].ultPos++;
        elemento++;
    } while (elemento < linhas);
}

void escrevearq(int nCPU, int linhas, CPU processador[nCPU], processo process[linhas]){

    FILE * arq = NULL;
    int i, j;

    if((arq = fopen("saida.txt", "w")) == NULL){
        printf("Não foi possível gerar o arquivo de saída\n\n");
        exit(1);
    }

    for (i=0; i < nCPU; i++){
       fprintf(arq, "Processador_%d", i+1);
       for (j=0; processador[i].tempofim[j] != 0; j++){
        fprintf(arq, "\n%s;%d;%d", process[processador[i].processo[j]].nome, processador[i].tempoini[j], processador[i].tempofim[j]);
       }
       if(i+1 != nCPU)
           fprintf(arq, "\n\n");
    }
    fclose(arq);
}


int pegaLinhas(char nomearq[]){

    FILE *arq = NULL;
    char teste;
    int linhas;
    if((arq = fopen(nomearq, "r")) == NULL){
        printf("Não foi possível abrir o arquivo\n\n");
        exit(1);
    }

    linhas = 0;
    for(teste = getc(arq); teste != EOF; teste = getc(arq))
        if(teste == '\n')
            linhas++;
    fclose(arq);
    return linhas;
}