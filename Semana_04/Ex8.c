#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#define TAM 20
#define NTHREAD 4
#define METADE 10
#define QUARTO 5
int matriz[TAM][TAM];

void *monothread(void *indice){
    int index = *(int *)indice, i, j;
    int *valorMaior = malloc(sizeof(int));
    *valorMaior = -1;
    for (i=index; i < TAM; i++){
        for (j=0; j < TAM; j++){
            if (matriz[i][j] > *valorMaior){
                *valorMaior = matriz[i][j];
            }
        }
    }
    return (void *) valorMaior;
}

void *dualthread(void *indice){
    int index = *(int *)indice, i, j;
    int *valorMaior = malloc(sizeof(int));
    *valorMaior = -1;
    index = index * METADE;
    for (i=index; i < index + METADE; i++){
        for (j=0; j < TAM; j++){
            if (matriz[i][j] > *valorMaior){
                *valorMaior = matriz[i][j];
            }
        }
    }
    return (void *) valorMaior;
}

void *quadthread(void *indice){
    int index = *(int *)indice, i, j;
    int *valorMaior = malloc(sizeof(int));
    *valorMaior = -1;
    index = index * QUARTO;
    for (i=index; i < index + QUARTO; i++){
        for (j=0; j < TAM; j++){
            if (matriz[i][j] > *valorMaior){
                *valorMaior = matriz[i][j];
            }
        }
    }
    return (void *) valorMaior;
}

int main (void){
    pthread_t vetorThreads[NTHREAD];
    clock_t start, end;
    srand(time(NULL));
    double etMono, etDual, etQuad;
    int i, j, erro, vetorParaThre[NTHREAD], valor;
    int *valorMa[NTHREAD];

    for (i=0; i < TAM; i++){
        for (j=0; j < TAM; j++){
            valor = rand()%100;
            matriz[i][j] = valor;
        }
    }

    start = clock();
    for (i=0; i<1; i++){
        vetorParaThre[i] = i;
        erro = pthread_create(&vetorThreads[i], NULL, (void *)monothread, (void *)&vetorParaThre[i]);
        if (erro){
            printf("ERRO; pthread_create() devolveu o erro %d\n", erro);
            exit(-1);
        }
    }
    for (i=0; i<1; i++){
        pthread_join(vetorThreads[i], (void **) &valorMa[i]);
    }
    end = clock();
    etMono = (double)(end - start)/CLOCKS_PER_SEC;
    printf("O maior valor encontrado pela função monothread foi: %d\n", *valorMa[0]);
    for (i=0; i<1; i++){
            free(valorMa[i]);
        }

    start = clock();
    for (i=0; i<2; i++){
        vetorParaThre[i] = i;
        erro = pthread_create(&vetorThreads[i], NULL, (void *)dualthread, (void *)&vetorParaThre[i]);
        if (erro){
            printf("ERRO; pthread_create() devolveu o erro %d\n", erro);
            exit(-1);
        }
    }
    for (i=0; i<2; i++){
        pthread_join(vetorThreads[i], (void **) &valorMa[i]);
    }
    end = clock();
    if (*valorMa[0] >= *valorMa[1]){
        valor = *valorMa[0];
    }else{
        valor = *valorMa[1];
    }
    etDual = (double)(end - start)/CLOCKS_PER_SEC;
    printf("O maior valor encontrado pela função dualthread foi: %d\n", valor);
    for (i=0; i<2; i++){
            free(valorMa[i]);
        }

    start = clock();
    for (i=0; i<4; i++){
        vetorParaThre[i] = i;
        erro = pthread_create(&vetorThreads[i], NULL, (void *)quadthread, (void *)&vetorParaThre[i]);
        if (erro){
            printf("ERRO; pthread_create() devolveu o erro %d\n", erro);
            exit(-1);
        }
    }
    for (i=0; i<4; i++){
        pthread_join(vetorThreads[i], (void **) &valorMa[i]);
    }
    end = clock();
    valor = 0;
    for (i=0; i<4; i++){
        if (*valorMa[i] > valor){
            valor = *valorMa[i];
        }
    }
    printf("O maior valor encontrado pela função quadthread foi: %d\n", valor);
    etQuad = (double)(end - start)/CLOCKS_PER_SEC;
    for (i=0; i<NTHREAD; i++){
            free(valorMa[i]);
        }

    printf("\n\nPor fim. Os tempos de execução foram:\n\n\t\t%lf para o monothread\n\t\t%lf para 2 thread\n\t\t%lf para o 4 thread\n\n", etMono, etDual, etQuad);


    return 0;
}