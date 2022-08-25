#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#define TAM 20

int matrix[20][20];

void *monothread(void *indice){
    int index = *(int *)indice, i, j, valorMaior = -1;
    for (i=index; i < TAM; i++){
        for (j=0; j < TAM; j++){
            if (matrix[i][j] > valorMaior){
                valorMaior = matrix[i][j];
            }
        }
    }
    pthread_exit((void *)valorMaior);
}

void *dualthread(void *indice){
    int index = *(int *)indice, i, j, valor;
    index = index * 10;
    for (i=index; i < index + 10; i++){
        for (j=0; j < 20; j++){
            valor = rand()%99;
            matrix[i][j] = valor;
        }
    }
    return(0);
}

void *quadthread(void *indice){
    int index = *(int *)indice, i, j, valor;
    index = index * 5;
    for (i=index; i < index + 5; i++){
        for (j=0; j < 20; j++){
            valor = rand()%99;
            matrix[i][j] = valor;
        }
    }
    return(0);
}

int main (void){
    double ETmono, ETdual, ETquad;
    clock_t start, end;
    srand(time(NULL));
    pthread_t vetorThreads[4];
    int i, j, rc, vetor[4], valor;
    void *valorMa;

    for (i=0; i < TAM; i++){
        for (j=0; j < TAM; j++){
            valor = rand()%100;
            matrix[i][j] = valor;
        }
    }

    start = clock();
    for (i=0; i<1; i++){
        vetor[i] = i;
        rc = pthread_create(&vetorThreads[i], NULL, (void *)monothread, (void *)&vetor[i]);
        if (rc){
            printf("ERRO; pthread_create() devolveu o erro %d\n", rc);
            exit(-1);
        }
    }
    pthread_join(vetorThreads[0], (void **) &valorMa);
    end = clock();
    ETmono = (double)(end - start)/CLOCKS_PER_SEC;
    printf("%lf e %d\n", ETmono, valor);

    //printf("\n\nPor fim. Os tempos de execução foram:\n\n\t\t%lf para o monothread\n\t\t%lf para 2 thread\n\t\t%lf para o 4 thread\n\n", ETmono, ETdual, ETquad);


    return 0;
}