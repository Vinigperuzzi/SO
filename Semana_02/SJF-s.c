/*
            Versão simplificada da mesma função (No sentido de que está mais incompleta)
                            ATENÇÃO
Esse programa tem por finalidade simular a política de escalonamento SJF de sistemas operacionais.
Por favor, note que o programa DEVE SER EXECUTADO PELO TERMINAL, pois ele exige que dois parâmetros de compilação
sejam passados. O primeiro parâmetro deve ser o nome do arquivo origem das informações, no caso, "tarefas.txt" e
o segundo deve ser o número de processadores, que pode ser qualquer valor.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct processos{
    char nome[4];
    int tempo;
} processo;

typedef struct processadores{
    int processo[9];
    int tempoini[9];
    int tempofim[9];
    int tMin;
    int ultPos;
} CPU;

void learq(processo process[], char nomearq[]);
void bubbleSort(processo process[]);
void escalona(CPU processador[], processo process[]);
void escrevearq(CPU processador[], processo process[]);



int linhas = 9, nCPU;//Linhas do arquivo de entrada e número de processadores

int main(int argc, char *argv[]){

    nCPU = atoi(argv[2]);
    CPU processador[nCPU];
    processo process[linhas];
    char nomearq[12];

    strcpy(nomearq, argv[1]);

    learq(process, nomearq);
    bubbleSort(process);
    escalona(processador, process);
    escrevearq(processador, process);

    printf("\n\nPrograma executado com sucesso, por favor, procure o arquivo chamado \"saida.txt\" no diretório em que o programa foi executado.\n\n");

    return 0;
}

void learq(processo process[linhas], char nomearq[12]){

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

void bubbleSort(processo process[linhas]){

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

void escalona(CPU processador[nCPU], processo process[linhas]){
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

void escrevearq(CPU processador[nCPU], processo process[linhas]){

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