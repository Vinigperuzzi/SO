#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<unistd.h>
#include<pthread.h>
#include<semaphore.h>

#define TAM 20
#define nThreads 4
#define infinite 9999999

int recursoDisp = 100, recursoTotal = 100, processos[TAM][2], fim = 0;
sem_t mutexVet;

void *emprestimo(void * indice){
    int i, menor, iMenor;
    while (fim < 20){
        menor = infinite;
        sem_wait(&mutexVet);
        for (i=0; i<TAM; i++){
            if (processos[i][0] < menor && processos[i][1] == 1 && fim < 20){
                menor = processos[i][0];
                iMenor = i;
            }
        }
        sem_post(&mutexVet);
        if (menor != infinite && processos[iMenor][0] <= recursoDisp && fim < 20){
            sem_wait(&mutexVet);
            recursoDisp -= processos[iMenor][0];
            processos[iMenor][1] = 0;
            sem_post(&mutexVet);
            printf("\nConsumindo %d do recurso.", processos[iMenor][0]);
            sleep(rand()%10);
            sem_wait(&mutexVet);
            recursoDisp += processos[iMenor][0];
            fim++;
            sem_post(&mutexVet);
        }else if (menor != infinite && processos[iMenor][0] > recursoTotal && fim < 20){
            sem_wait(&mutexVet);
            processos[iMenor][1] = 0;
            fim++;
            sem_post(&mutexVet);
            printf("\nNão é possível conceder esse recurso, ele exige mais recursos do que o limite.");
        }
    }
    pthread_exit(NULL);
}

int main (void){

    pthread_t threads[nThreads];
    int i, erro;
    srand(time(NULL));
    sem_init(&mutexVet, 0, 1);

    for (i=0; i<TAM; i++){
        processos[i][0] = rand()%30;
        processos[i][1] = 1;
    }
    processos[18][0] = 130;
    for (i=0; i<nThreads; i++){
        erro = pthread_create(&threads[i], NULL, (void *)emprestimo, NULL);
        if (erro){
            printf("ERRO; pthread_create() devolveu o erro %d\n", erro);
            exit(-1);
        }
    }

    for (i=0; i<nThreads; i++){
        pthread_join(threads[i], NULL);
    }
    sem_destroy(&mutexVet);

    return 0;
}