#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

typedef struct infoThread{
    int ini;
    int end;
    int *vetor;
} info;

int soma = 0;
info parametro;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER ;

void *somaValor(void * argumento){
    pthread_mutex_lock(&mutex);
    info param = parametro;
    pthread_mutex_unlock(&mutex);
    for (int i=param.ini; i<param.end; i++){
        pthread_mutex_lock(&mutex);
        soma += param.vetor[i];
        pthread_mutex_unlock(&mutex);
    }
    printf("Thread %d~%d: soma = %d\n\n", parametro.ini, parametro.end, soma);
    pthread_exit(NULL);
}


int main (int argc, char *argv[]){

    int nThreads, n, nXThreads;
    nThreads = atoi(argv[2]);
    n = atoi(argv[1]);
    int i, erro;
    pthread_t vetorThread[nThreads];
    pthread_mutex_init(&mutex, NULL);

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
        pthread_mutex_lock(&mutex);
        parametro.ini = i * nXThreads;
        if (i+1 != nThreads){
            parametro.end = parametro.ini + nXThreads;
        } else {
            parametro.end = parametro.ini + nXThreads + (n - (nThreads * nXThreads));
        }
        pthread_mutex_unlock(&mutex);
        erro = pthread_create(&vetorThread[i], NULL, (void *)somaValor, NULL);
            if (erro){
                printf("ERRO; pthread_create() devolveu o erro %d\n", erro);
                exit(-1);
            }
    }
    for (i=0; i<nThreads; i++){
        pthread_join(vetorThread[i], NULL);
    }
    free(parametro.vetor);

    printf("O resultado é: %d.\n", soma);
    return 0;
}