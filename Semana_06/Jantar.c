#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<time.h>
#include<pthread.h>
#include<semaphore.h>

#define NFILO 5
#define NHASHI NFILO

sem_t hashi[NHASHI];
sem_t pegar;
int rodadas = 0;
int refeicoes[NFILO];

void * filosofo(void * parametro){
    int indiceFilo = *(int *)parametro;
    refeicoes[indiceFilo] = 0;
    for(int i=0; i<rodadas; i++){
        sem_wait(&pegar);
        sem_wait(&hashi[indiceFilo]);
	    sem_wait(&hashi[(indiceFilo+1)%NFILO]);
        sem_post(&pegar);

	    filoCome(indiceFilo);
        printf("O filósofo %d parou de comer!\n", indiceFilo);
        sem_post(&hashi[(indiceFilo+1)%NFILO]);
	    sem_post(&hashi[indiceFilo]);
	    
        filoPensa(indiceFilo);
    }

}

void filoCome(int indiceFilo){
    printf("O filosofo %d está comendo!\n", indiceFilo);
    refeicoes[indiceFilo]++;
    int fome = rand()%5;
    sleep(fome);
}
void filoPensa(int indiceFilo){
    printf("O filósofo %d está alimentado e pensando!\n", indiceFilo);
    int sono = rand()%5;
    sleep(sono);
}

int main(void){
    pthread_t nThreads[NFILO];
    int indice[NFILO], i;
    do{
        printf("Por favor, informe a quantidade de vezes que cada filósofo deve se alimentar:\n");
        scanf("%d", &rodadas);
    } while(rodadas < 1 || rodadas > 20);
    srand(time(NULL));

    for(i=0; i<NFILO; i++){
        sem_init(&hashi[i], 0, 1);
    }
    sem_init(&pegar, 0, NFILO-1);

    for(i=0;i<NFILO;i++){
		indice[i]=i;
		pthread_create(&nThreads[i],NULL,filosofo,(void *)&indice[i]);
	}
    for(i=0;i<5;i++){
		pthread_join(nThreads[i],NULL);
    }
    for(i=0; i<NFILO ;i++)
        sem_destroy(&hashi[i]);
    
    sem_destroy(&pegar);
    printf("\n\n\n");
    for(i=0; i<NFILO; i++){
        printf("O filósofo %d se alimentou %d vez(es)!\n", i, rodadas);
    }
    printf("\n\n\t\tFim da execução\n\n\t\tOu seja, não houve deadlock!!!!!!!!\n\n\n");
}