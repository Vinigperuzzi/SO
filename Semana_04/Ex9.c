#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#define NTHREAD 2

void *jokenpo(void * argumento){
    int *hand = malloc(sizeof(int));

    *hand = rand()%3;

    return (void *) hand;
}


int main (void){
    pthread_t vetorThreads[NTHREAD];
    srand(time(NULL));
    int i, j, erro, rodadas, n1=0, n2=0;
    int * hand[NTHREAD];

    do{
        printf("Por favor, informe a quantidade de rodadas que serão jogadas (deve ser um valor positivo): ");
        scanf("%d", &rodadas);
    } while (rodadas < 1);

    for (j=0; j<rodadas; j++){
        printf("\n\nRodada %d\n", j+1);
        for (i=0; i<NTHREAD; i++){
            erro = pthread_create(&vetorThreads[i], NULL, (void *)jokenpo, NULL);
            if (erro){
                printf("ERRO; pthread_create() devolveu o erro %d\n", erro);
                exit(-1);
            }
        }
        for (i=0; i<NTHREAD; i++){
            pthread_join(vetorThreads[i], (void **) &hand[i]);
            printf("Jogador %d: ", i+1);
            if (*hand[i] == 0){
                printf("Pedra.\n");
            } else if (*hand[i] == 1){
                printf("Papel.\n");
            } else{
                printf("Tesoura.\n");
            }
        }

        switch(*hand[0]){
        case 0:
            if (*hand[1] == 1)
                n2++;
            if (*hand[1] == 2)
                n1++;
            break;
        case 1:
            if (*hand[1] == 0)
                n1++;
            if (*hand[1] == 2)
                n2++;
            break;
        case 2:
            if (*hand[1] == 0)
                n2++;
            if (*hand[1] == 1)
                n1++;
        default:
            break;
        }
        for (i=0; i<NTHREAD; i++){
            free(hand[i]);
        }
    }

    printf("\n\nO resultado final da partida foi:\n\nDe um total de %d rodadas, o jogador 1 ganhou: %d vezes e o jogador 2 ganhou %d vezes.\n\n", rodadas, n1, n2);
    
    if (n1 == n2){
        printf("Portanto acabou em empate!!\n");
        } else if (n1 > n2){
            printf("Portanto o jogador 1 venceu!! Parabéns jogador 1!!!\n");
            } else{
                printf("Portanto o jogador 2 venceu!! Parabéns jogador 2!!!\n");
                }

    return 0;
}