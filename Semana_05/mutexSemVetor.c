#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

typedef struct infoThread{
    int ini;
    int end;
    int lock;
    int *vetor;
} info;

int soma = 0;
info parametro;

pthread_mutex_t mutexSoma = PTHREAD_MUTEX_INITIALIZER ;
pthread_mutex_t mutexI = PTHREAD_MUTEX_INITIALIZER ;


void *somaValor(void * argumento){
    int ini = parametro.ini;
    int end = parametro.end;
    parametro.lock = 0;
    for (int i=ini; i<end; i++){
        pthread_mutex_lock(&mutexSoma);
        soma += parametro.vetor[i];
        pthread_mutex_unlock(&mutexSoma);
    }
    //printf("Thread %d~%d: soma = %d\n\n", ini, end, soma);
    pthread_exit(NULL);
}


int main (int argc, char *argv[]){

    clock_t start, final;
    start = clock();
    int nThreads, n, nXThreads;
    nThreads = atoi(argv[2]);
    n = atoi(argv[1]);
    int i, erro;
    parametro.lock = 0;
    pthread_t vetorThread[nThreads];
    pthread_mutex_init(&mutexSoma, NULL);
    pthread_mutex_init(&mutexI, NULL);

    for(i=0; i<nThreads; i++){
        if ((parametro.vetor = (int *)malloc(n * sizeof(int))) == NULL) {
		    printf("Não foi possível alocar memória\n");
		    exit(1);
	    }
    }
    for (i=0; i<n; i++){
        parametro.vetor[i] = i + 1;
    }

    nXThreads = (int)n/nThreads;

    for(i=0; i<nThreads; i++){
        if (parametro.lock == 0){
            parametro.ini = i * nXThreads;
            if (i+1 != nThreads){
                parametro.end = parametro.ini + nXThreads;
            } else {
                parametro.end = parametro.ini + nXThreads + (n - (nThreads * nXThreads));
            }
            parametro.lock = 1;
            erro = pthread_create(&vetorThread[i], NULL, (void *)somaValor, NULL);
            if (erro){
                printf("ERRO; pthread_create() devolveu o erro %d\n", erro);
                exit(-1);
            }
        } else{
            i--;
        }
    }
    for (i=0; i<nThreads; i++){
        pthread_join(vetorThread[i], NULL);
    }
    free(parametro.vetor);
    pthread_mutex_destroy(&mutexSoma);

    printf("O resultado é: %d.\n", soma);
    final = clock();
    printf("Tempo de execução: %f\n\n", (double)(final - start)/CLOCKS_PER_SEC);
    return 0;
}