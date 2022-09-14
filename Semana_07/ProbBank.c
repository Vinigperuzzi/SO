#include<stdio.h>
#include<stdlib.h>
#include<time.h>

#define percentage 0.35//índice do valor máximo em relação ao recurso admissível no inicial.
#define limite 9//Valor de recursos a ser gerado.

int main (void) {

    srand(time(NULL));
    int process, resources, i, j, k=0, valor, valorPerc;
    printf("Por favor, informe a quantidade de quantidade de processos e recursos:");
    scanf("%d", &process);
    scanf("%d", &resources);
    getchar();

    int recNeeded[process][resources],
    recAllocated[process][resources],
    existents[resources], disponible[resources],
    allocated[resources],
    processado[process],
    ordem[process],
    certo,
    fim = 0,
    impossivel,
    modo = 0;

    for (i=0; i<process; i++){
        processado[i] = 0;
    }

    for (i=0; i<resources; i++){
        valor = (rand()%limite) + 1;
        existents[i] = valor;
        disponible[i] = valor;
        allocated[i] = 0;
    }
    for (i=0; i<process; i++){
        for (j=0; j<resources; j++){
            valor = rand()%existents[j];
            recNeeded[i][j] = valor;
            recAllocated[i][j] = 0;
        }
    }

    //------------------------Parte da impressão e atualização------------------------------------------
    int time = 0;
    while (fim < process){
        system ("cls || clear");
        printf("Recursos Necessários:\n");
        for (i=0; i<process; i++){
            for (j=0; j<resources; j++){
                printf("%d ", recNeeded[i][j]);
            }
            printf("\n");
        }
        printf("\nRecursos Alocados:\n");
        for (i=0; i<process; i++){
            for (j=0; j<resources; j++){
                printf("%d ", recAllocated[i][j]);
            }
            printf("\n");
        }

        printf("\nAlocados: ");
        for (i=0; i<resources; i++){
            printf("%d ", allocated[i]);
        }
        printf("\nDisponíveis: ");
        for (i=0; i<resources; i++){
            printf("%d ", disponible[i]);
        }
        printf("\nExistentes: ");
        for (i=0; i<resources; i++){
            printf("%d ", existents[i]);
        }
        
        if(time == 0){
            for (i=0; i<process; i++){
                for (j=0; j<resources; j++){
                    if (disponible[j] == 0){
                        valor = 0;
                    } else {
                        valor = (int)rand()%disponible[j];
                    }
                    valorPerc = (int)valor * percentage;
                    recAllocated[i][j] = valorPerc;
                    allocated[j] += valorPerc;
                    disponible[j] -= valorPerc;
                }
            }
            time++;
        } else {
            impossivel = 0;
            for (i=0; i<process; i++){
                certo = 0;
                for(j=0; j<resources; j++){
                    if (recNeeded[i][j] <= disponible[j] && processado[i] == 0){
                        certo++;
                    }
                }
                if (certo == resources && processado[i] == 0){
                    printf("\n\nExecutando o processo %d", i);
                    fim++;
                    ordem[k] = i;
                    k++;
                    processado[i] = 1;
                    for (j=0; j<resources; j++){
                        disponible[j] += recNeeded[i][j];
                        allocated[j] -= recNeeded[i][j];
                        recNeeded[i][j] = 0;
                        recAllocated[i][j] = 0;
                    }
                } else {
                    impossivel++;
                }
            }
            if (impossivel == process){
                fim = process;
                modo = 1;
            }
        }
        printf("\n\nPressione enter para prosseguir!\n\n");
        getchar();
    }

    if (modo == 0){    
        printf("\n\nEstá seguro!!\nOrdem correta de rodar os processos:\n");
        for(i=0; i<process; i++){
            printf("%d ", ordem[i]);
            if (i + 1 != process){
                printf(" -->  ");
            }
        }
        printf("\n\n");
    } else {
        printf("\n\nNão está seguro!!\n\n");
    }
}

