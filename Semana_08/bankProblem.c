#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<pthread.h>
#include<semaphore.h>
#include<unistd.h>

#define percentage 0.3//índice do valor máximo em relação ao recurso admissível no inicial.
#define limite 9//Valor de recursos a ser gerado.

typedef struct strprocessos{
    int indice;
    int *procRecNec;
    int *procRecAloc;
} strProcesso;

sem_t ciclo;
sem_t espera;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int esperando = 0;

int **aloca(int **endereco, int n, int m);
void freeAloca(int **endereco, int n);
int* alocaVet(int *endereco, int m);
void interfaceHM(int **recNecessarios, int **recAlocados, int *existentesVet, int *disponiveisVet, int *alocadosVet, int processos, int recursos);
int bankProblem(int **recNecessarios, int **recAlocados, int *disponiveisVet, int processos, int recursos, int *pronto, int *ordem, int*alocadosVet, int *existentesVet);

int *dispGlobais;


void *processa(strProcesso *parametro){
    strProcesso local = *(strProcesso*)parametro;
    int i; 
    int n = sizeof(local.procRecAloc)/sizeof(int);

    for(i=0; i<n; i++){
        dispGlobais[i] -= (local.procRecNec[i] - local.procRecAloc[i]);
    }
    pthread_mutex_unlock(&mutex);
    int tempo = rand()%10;
    tempo += 5;
    printf("\nExecutando o %dº processo por %d segundos!\n", local.indice, tempo);
    sleep(tempo);

    pthread_mutex_lock(&mutex);
    for(i=0; i<n; i++){
        dispGlobais[i] += local.procRecNec[i];
    }
    pthread_mutex_unlock(&mutex);
    
    sem_wait(&espera);
    if (esperando == 1){
        sem_post(&ciclo);
    }
    sem_post(&espera);

    printf("\nFim do processo %d\n", local.indice);

    pthread_exit(NULL);
}


int main (void) {

    srand(time(NULL));
    sem_init(&ciclo, 0, 0);
    sem_init(&espera, 0, 1);
    pthread_mutex_init(&mutex,NULL);
    int processos, recursos, i, j, valor, valorPerc;
    printf("Por favor, informe a quantidade de processos e recursos, um de cada vez:\n");
    scanf("%d", &processos);
    scanf("%d", &recursos);
    getchar();

//-------------------------Cria o vetor de processos globais---------------------------------------------
    strProcesso procGlobais[processos];
    for (i=0; i<processos; i++){
        procGlobais[i].procRecNec = alocaVet(procGlobais[i].procRecNec, recursos);
        procGlobais[i].procRecAloc = alocaVet(procGlobais[i].procRecAloc, recursos);
    }

//-------------------------Criação e alocação dos vetores e matrizes------------------------------
    int  **recNecessarios, **recAlocados, *existentesVet, *disponiveisVet, *alocadosVet, *ordem, *pronto;

    recNecessarios = aloca(recNecessarios, processos, recursos);
    recAlocados = aloca(recAlocados, processos, recursos);
    existentesVet = alocaVet(existentesVet, recursos);
    disponiveisVet = alocaVet(disponiveisVet, recursos);
    alocadosVet = alocaVet(alocadosVet, recursos);    
    ordem = alocaVet(ordem, processos);
    pronto = alocaVet(pronto, processos);
    dispGlobais = alocaVet(dispGlobais, recursos);

//------------------------Atribuição e inicialização de valores-----------------------------------------------------------
    for (j=0; j<recursos; j++){
        valor = (rand()%limite) + 1;
        existentesVet[j] = valor;
        disponiveisVet[j] = valor;
        dispGlobais[j] = valor;
        alocadosVet[j] = 0;
    }

    for (i=0; i<processos; i++){
        for (j=0; j<recursos; j++){
            valor = rand()%existentesVet[j];
            recNecessarios[i][j] = valor;
            recAlocados[i][j] = 0;
            procGlobais[i].procRecNec[j] = valor;
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
            procGlobais[i].procRecAloc[j] = valorPerc;
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

    printf("\n\n\t\t\tFim da análise\n\n");
    if (modo != 1)
        printf("\n\n\t\t\tPressione enter para ir para a etapa de simulações");
    getchar();

//-----------------------------Início da lógica das threads------------------------------------------------------------
    pthread_t threadProc[processos];
    if (modo != 1){ 
        int permitido, erro;
        for(i=0; i<processos; i++){
            sem_wait(&espera);
            permitido = 0;
            pthread_mutex_lock(&mutex);
            for(j=0; j<recursos; j++){
                if(procGlobais[ordem[i]].procRecNec[j] - procGlobais[ordem[i]].procRecAloc[j] <= dispGlobais[j]){
                    permitido++;
                }
            }
            pthread_mutex_unlock(&mutex);
            if(permitido == recursos){
                pthread_mutex_lock(&mutex);
                esperando = 0;
                sem_post(&espera);
                procGlobais[i].indice = ordem[i];
                erro = pthread_create(&threadProc[i], NULL, (void *)processa, (void *)&procGlobais[i]);
                if (erro){
                    printf("ERRO; pthread_create() devolveu o erro %d\n", erro);
                    exit(-1);
                }
            } else{
                esperando = 1;
                sem_post(&espera);
                i--;
                sem_wait(&ciclo);
                sem_wait(&ciclo);
            }
        }
        for (i=0; i<processos; i++){
            pthread_join(threadProc[i], NULL);
        }
    }

    printf("\n\n\n\t\t\tFim da execução.\n\n");

//--------------------Free nas alocações todas--------------------------------------------------------------------------
    freeAloca(recNecessarios, processos);
    freeAloca(recAlocados, processos);
    free(existentesVet);
    free(disponiveisVet);
    free(alocadosVet);
    free(ordem);
    free(pronto);
    free(dispGlobais);
    for (i=0; i<processos; i++){
        free(procGlobais[i].procRecAloc);
        free(procGlobais[i].procRecNec);
    }
    pthread_mutex_destroy(&mutex);
    sem_destroy(&ciclo);
    sem_destroy(&espera);
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
            
            printf("\n\nTestando o %dº processo.\n\nPressione enter para prosseguir!\n\n", i+1);
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
