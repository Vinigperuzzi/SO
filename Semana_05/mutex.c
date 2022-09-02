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

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *somaValor(void * param){
    info parametro = *(info *)param;
    for (int i=parametro.ini; i<parametro.end; i++){
        pthread_mutex_lock(&mutex);
        soma += parametro.vetor[i];
        pthread_mutex_unlock(&mutex);
    }
    pthread_exit(NULL);
}


int main (int argc, char *argv[]){

    int nThreads, n, nXThreads;
    n = atoi(argv[1]);
    nThreads = atoi(argv[2]);
    if(n < 100 || nThreads < 2){
        printf("\t\t\tATENÇÃO\n\nValores passados inválidos, por favor, passe um valor de pelo menos 100 para o vetor e uma quantidade de pelo menos 2 threads.\n\n");
        exit(-1);
    }
    int i, j, erro;
    pthread_t vetorThread[nThreads];
    info parametro[nThreads];
    pthread_mutex_init(&mutex, NULL);

    for(i=0; i<nThreads; i++){
        if ((parametro[i].vetor = (int *)malloc(n * sizeof(int))) == NULL) {
		    printf("Não foi possível alocar memória\n");
		    exit(1);
	    }
    }
    for (j=0; j<nThreads; j++){
        for (i=0; i<n; i++){
            parametro[j].vetor[i] = i + 1;
        }
    }

    nXThreads = (int)n/nThreads;

    for(i=0; i<nThreads; i++){
        parametro[i].ini = i * nXThreads;
        if (i+1 != nThreads){
            parametro[i].end = parametro[i].ini + nXThreads;
        } else {
            parametro[i].end = parametro[i].ini + nXThreads + (n - (nThreads * nXThreads));
        }
        erro = pthread_create(&vetorThread[i], NULL, (void *)somaValor, (void *)&parametro[i]);
            if (erro){
                printf("ERRO; pthread_create() devolveu o erro %d\n", erro);
                exit(-1);
            }
    }
    for (i=0; i<nThreads; i++){
        pthread_join(vetorThread[i], NULL);
    }
    for (i=0; i<nThreads; i++){
        free(parametro[i].vetor);
    }

    printf("O resultado é: %d.\n", soma);
    return 0;
}