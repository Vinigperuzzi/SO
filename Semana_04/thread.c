#include <stdio.h>
#include <unistd.h>//Para utilizar o sleep
#include <pthread.h>

void *Funcao(void *argumento){
    printf("[threadOlá Mundo!\n]");
    fflush(stdout);
}
int main(void){
    pthread_t id;
    pthread_create(&id, NULL, Funcao, NULL);
    printf("[main]Olá Mundo!\n");
    pthread_exit(NULL);
    return (0);
}