#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#define TAM 20

int main (void){
    double ETmono, ETdual, ETquad;
    clock_t start, end;
    srand(time(NULL));

    start = clock();
    monothread();
    pthread_join();
    end = clock();
    ETmono = (double)(end - start)/CLOCKS_PER_SEC;

    start = clock();
    dualthread();
    pthread_join();
    end = clock();
    ETdual = (double)(end - start)/CLOCKS_PER_SEC;

    start = clock();
    quadthread();
    pthread_join();
    end = clock();
    ETquad = (double)(end - start)/CLOCKS_PER_SEC;

    printf("Os tempos de execução foram:\n\n\t\t%lf para o monothread\n\t\t%lf para 2 thread\n\t\t%lf para o 4 thread", ETmono, ETdual, ETquad);


    return 0;
}

monothread(){}

dualthread(){}

quadthread(){}