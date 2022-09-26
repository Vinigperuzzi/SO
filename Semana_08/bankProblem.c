#include<stdio.h>
#include<stdlib.h>
#include<time.h>

#define percentage 0.5//índice do valor máximo em relação ao recurso admissível no inicial.
#define limite 9//Valor de recursos a ser gerado.

typedef struct processos{
    int *procRecNec;
    int *procRecAloc;
} processo;

int **aloca(int **endereco, int n, int m);
void freeAloca(int **endereco, int n);
int* alocaVet(int *endereco, int m);
void interfaceHM(int **recNecessarios, int **recAlocados, int *existentesVet, int *disponiveisVet, int *alocadosVet, int processos, int recursos);
int bankProblem(int **recNecessarios, int **recAlocados, int *disponiveisVet, int processos, int recursos, int *pronto, int *ordem, int*alocadosVet, int *existentesVet);


int main (void) {

    srand(time(NULL));
    int processos, recursos, i, j, valor, valorPerc;
    printf("Por favor, informe a quantidade de processos e recursos, um de cada vez:\n");
    scanf("%d", &processos);
    scanf("%d", &recursos);
    getchar();

//-------------------------Criação e alocação dos vetores e matrizes------------------------------
    int  **recNecessarios, **recAlocados, *existentesVet, *disponiveisVet, *alocadosVet, *ordem, *pronto;

    recNecessarios = aloca(recNecessarios, processos, recursos);
    recAlocados = aloca(recAlocados, processos, recursos);
    existentesVet = alocaVet(existentesVet, recursos);
    disponiveisVet = alocaVet(disponiveisVet, recursos);
    alocadosVet = alocaVet(alocadosVet, recursos);    
    ordem = alocaVet(ordem, processos);
    pronto = alocaVet(pronto, processos);

//------------------------Atribuição e inicialização de valores-----------------------------------------------------------
    for (j=0; j<recursos; j++){
        valor = (rand()%limite) + 1;
        existentesVet[j] = valor;
        disponiveisVet[j] = valor;
        alocadosVet[j] = 0;
    }

    for (i=0; i<processos; i++){
        for (j=0; j<recursos; j++){
            valor = rand()%existentesVet[j];
            recNecessarios[i][j] = valor;
            recAlocados[i][j] = 0;
        }
    }

    interfaceHM(recNecessarios, recAlocados, existentesVet, disponiveisVet,
    alocadosVet, processos, recursos);

    printf("\n\nPressione enter para gerar as alocações randômicas!\n\n");
    getchar();

    for (i=0; i<processos; i++){
        for (j=0; j<recursos; j++){
            valor = rand()%disponiveisVet[j];
            valorPerc = valor * percentage;
            recAlocados[i][j] = valorPerc;
            disponiveisVet[j] -= valorPerc;
            alocadosVet[j] += valorPerc;
        }
        pronto[i] = 0;
    }

    interfaceHM(recNecessarios, recAlocados, existentesVet, disponiveisVet,
    alocadosVet, processos, recursos);

    printf("\n\nPressione enter para prosseguir!\n\n");
    getchar();


//--------------------------Lógica de seleção-------------------------------------------------------------------------------
    int modo = bankProblem(recNecessarios, recAlocados, disponiveisVet, processos, recursos, pronto, ordem, alocadosVet, existentesVet);

//-------------------------------Impressão final-------------------------------------------------------
    if (modo == 1){
        printf("\n\nO estado não é seguro!\n\n");
    } else {
        printf("\n\nO estado é seguro na seguinte sequência de processos:\n");
        for (i=0; i<processos; i++){
            printf("%d ", ordem[i]);
            if ((i+1) != processos){
                printf(" ---> ");
            } else {
                printf(".");
            }
        }
    }

    printf("\n\n\t\t\tFim da execução\n\n");

//--------------------Free nas alocações todas--------------------------------------------------------------------------
    freeAloca(recNecessarios, processos);
    freeAloca(recAlocados, processos);
    free(existentesVet);
    free(disponiveisVet);
    free(alocadosVet);
    free(ordem);
    free(pronto);
}

//---------------------Outras funções---------------------------------------------------------------------------------------

int** aloca(int **endereco, int n, int m){
    int i;
    endereco = (int**)malloc(sizeof(int *) * n);
        for(i=0; i<n; i++){
            endereco[i] = (int *)malloc(sizeof (int) * m);
        }
    return endereco;
}

void freeAloca(int **endereco, int n){
    int i;
    for(i=0; i<n; i++){
        free(endereco[i]);
    }
    free(endereco);
}

int* alocaVet(int *endereco, int m){
    endereco = (int*)malloc(sizeof(int) * m);
    return endereco;
}

void interfaceHM(int **recNecessarios, int **recAlocados,
int *existentesVet, int *disponiveisVet, int *alocadosVet,
int processos, int recursos){

/*interfaceHM(recNecessarios, recAlocados, existentesVet, disponiveisVet,
alocadosVet, processos, recursos);*/

    int i, j;
    system ("cls || clear");

    printf("Recursos Necessários:\n");
    for (i=0; i<processos; i++){
        for (j=0; j<recursos; j++){
            printf("%d ", recNecessarios[i][j]);
        }
        printf("\n");
    }

    printf("\nRecursos Alocados:\n");
    for (i=0; i<processos; i++){
        for (j=0; j<recursos; j++){
            printf("%d ", recAlocados[i][j]);
        }
        printf("\n");
    }

    printf("\nAlocados: \t");
    for (i=0; i<recursos; i++){
        printf("%d ", alocadosVet[i]);
    }
    printf("\nDisponíveis: \t");
    for (i=0; i<recursos; i++){
        printf("%d ", disponiveisVet[i]);
    }
    printf("\nExistentes: \t");
    for (i=0; i<recursos; i++){
        printf("%d ", existentesVet[i]);
    }

    printf("\n\n\n");
}

int bankProblem(int **recNecessarios, int **recAlocados, int *disponiveisVet, int processos, int recursos, int *pronto, int *ordem, int*alocadosVet, int *existentesVet){
    int fim=0, escolha, fimInseguro=0, modo, i, j, k=0;
    while (fim < processos){
        modo = 0;
        for(i=0; i<processos; i++){
            fimInseguro++;
            escolha = 0;
            for(j=0; j<recursos; j++){
                if (pronto[i] == 0 && (recNecessarios[i][j] - recAlocados[i][j]) <= disponiveisVet[j]){
                    escolha++;
                }
            }
            if (escolha == recursos){
                ordem[k++] = i;
                for (j=0; j<recursos; j++){
                    recNecessarios[i][j] = 0;
                    disponiveisVet[j] += recAlocados[i][j];
                    alocadosVet[j] -= recAlocados[i][j];
                    recAlocados[i][j] = 0;
                }
                fim++;
                fimInseguro--;
                pronto[i] = 1;
            }
            if(fimInseguro == processos && fim < processos){
                fim = processos;
                modo = 1;
            }
            interfaceHM(recNecessarios, recAlocados, existentesVet, disponiveisVet,
            alocadosVet, processos, recursos);
            
            printf("\n\nTestanto o %dº processo.\n\nPressione enter para prosseguir!\n\n", i+1);
            getchar();
        }
        i = -1;
        fimInseguro = 0;
    }
    return modo;
}


//Considerações
/*Apesar de no enunciado da questão dizer que o máximo de recursos pode varias em uma faixa de 0 a 9,
parando para pensar a respeito, não faz sentido que seja 0, pois caso seja 0, significa que o recurso
nunca está disponível, e se não estã disponível, é o mesmo que não tê-lo. Ou seja, é só perda de
eficiência ter um programa que contemple a possibilidade de um recurso com máximo zero. Além disso,
pode surgir todo o tipo de problema quando for feita a divisão para achar o resto no rand() por um
valor com 0, dessa forma, optei por tirar essa possibilidade e manter a margem de variação de 1 a 9.*/
