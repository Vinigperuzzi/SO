#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#define TAM 20

int main (void){
    srand(time(NULL));
    int valor, i, j, matriz[TAM][TAM];

    for(i=0; i<TAM; i++){
        for (j=0; j<TAM; j++){
            valor = rand()%99;
            matriz[i][j] = valor;
        }
    } 

    for (i=0; i<TAM; i++){
        for (j=0; j<TAM; j++){
            printf("%2.1d ", matriz[i][j]);
        }
        printf("\n");
    }
    return 0;
}