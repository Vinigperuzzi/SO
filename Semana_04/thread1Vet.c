#include <stdio.h>
#include <unistd.h>//Para utilizar o sleep
#include <pthread.h>
#include <stdlib.h>
#define NUM_THREADS 5

void *printHello(void *threadid){
    int tid = *(int*)threadid;
    printf("Olá! Sou a thread #%d!\n", tid);
    pthread_exit(NULL);
}

int main (int argc, char *argv[]){
    pthread_t threads[NUM_THREADS];
    int rc, t, tVet[5];
    for(t=0; t<NUM_THREADS; t++){
        printf("main: criando thread %d\n", t);
        tVet[t] = t;
        rc = pthread_create(&threads[t], NULL, printHello, (void *)&tVet[t]);//Apesar de não fazer sentido, por causa do alto consumo de memória
        if(rc){
            printf("Erro\n");
            exit(-1);
        }
    }
    for (t=0; t<NUM_THREADS; t++){
        pthread_join(threads[t], NULL);
    }
}