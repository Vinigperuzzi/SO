#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#define TAM 20000
#define NTHREAD 4
#define METADE 10000
#define QUARTO 5000
int matriz[TAM][TAM];
void *monothread(void *indice){
    int index = *(int *)indice, i, j, valor;
    unsigned int seed = rand();
    for (i=index; i < TAM; i++){
        for (j=0; j < TAM; j++){
            //valor = rand_r(&seed)%100;
            matriz[i][j] = i;
        }
    }
    pthread_exit(NULL);
}

void *dualthread(void *indice){
    int index = *(int *)indice, i, j, valor;
    index = index * METADE;
    unsigned int seed = rand();
    for (i=index; i < index + METADE; i++){
        for (j=0; j < TAM; j++){
            //valor = rand_r(&seed)%100;
            matriz[i][j] = i;
        }
    }
    pthread_exit(NULL);
}

void *quadthread(void *indice){
    int index = *(int *)indice, i, j, valor;
    index = index * QUARTO;
    unsigned int seed = rand();
    for (i=index; i < index + QUARTO; i++){
        for (j=0; j < TAM; j++){
            //valor = rand_r(&seed)%100;
            matriz[i][j] = i;
        }
    }
    pthread_exit(NULL);
}

int main (void){
    pthread_t vetorThread[NTHREAD];
    clock_t start, end;
    double etMono, etDual, etQuad;
    srand(time(NULL));
    int i, j, vetTh[NTHREAD], erro;

    /*MonoThread*/
    printf("\n\n\t\t\tRodando a mono\n\n");
    start = clock();
    for (i=0; i<1; i++){
        vetTh[i] = i;
        erro = pthread_create(&vetorThread[i], NULL, (void *)monothread, (void *)&vetTh[i]);
        if (erro != 0){
            printf("Houve algum erro com a chamada da função monothread.\n\n");
            exit(-1);
        }
    }
    for (i=0; i<1; i++){
        pthread_join(vetorThread[i], NULL);
    }
    end = clock();
    etMono = (double)(end - start)/CLOCKS_PER_SEC;
    /*printf("\n\nEscrevedo matriz criada pelo monothread: \n");
    for(i=0; i<TAM; i++){
        for(j=0; j < TAM; j++){
            printf("%2.1d ", matriz[i][j]);
        }
        printf("\n");
    }*

    /*Dualthread*/
    printf("\n\n\t\t\tRodando a dual\n\n");
    start = clock();
    for (i=0; i<2; i++){
        vetTh[i] = i;
        erro = pthread_create(&vetorThread[i], NULL, (void *)dualthread, (void *)&vetTh[i]);
        if (erro != 0){
            printf("Houve algum erro com a chamada da função dualthread.\n\n");
            exit(-1);
        }
    }
    for (i=0; i<2; i++){
        pthread_join(vetorThread[i], NULL);
    }
    end = clock();
    etDual = (double)(end - start)/CLOCKS_PER_SEC;
    /*printf("\n\nEscrevedo matriz criada pelo dualthread: \n");
    for(i=0; i<TAM; i++){
        for(j=0; j < TAM; j++){
            printf("%2.1d ", matriz[i][j]);
        }
        printf("\n");
    }*/

    /*QuadThread*/
    printf("\n\n\t\t\tRodando a quad\n\n");
    start = clock();
    for (i=0; i<4; i++){
        vetTh[i] = i;
        erro = pthread_create(&vetorThread[i], NULL, (void *)quadthread, (void *)&vetTh[i]);
        if (erro != 0){
            printf("Houve algum erro com a chamada da função monothread.\n\n");
            exit(-1);
        }
    }
    for (i=0; i<4; i++){
        pthread_join(vetorThread[i], NULL);
    }
    end = clock();
    etQuad = (double)(end - start)/CLOCKS_PER_SEC;
    /*printf("\n\nEscrevedo matriz criada pelo quadthread: \n");
    for(i=0; i<TAM; i++){
        for(j=0; j < TAM; j++){
            printf("%2.1d ", matriz[i][j]);
        }
        printf("\n");
    }*/

    printf("\n\nPor fim. Os tempos de execução foram:\n\n\t\t%lf para o monothread\n\t\t%lf para o 2 thread\n\t\t%lf para o 4 thread\n\n", etMono, etDual, etQuad);
}