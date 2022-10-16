#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <math.h>

typedef struct financista{
    int id;
    int especie;
    float subsidio;
    int taxas;
    int total;
    int atendido;
    int step;
} finan;

int bancoEspecie, bancoSubsidio, bancoTaxas, qtd, deadlock = 0, fimDeadlock = 0, ordemCount, exec=0, step=-1;
int *ordem;

pthread_mutex_t mutexBanker = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexOrdem = PTHREAD_MUTEX_INITIALIZER;
pthread_barrier_t sincroniza;
pthread_barrier_t sincronizaStep;

void *pedidoCredito(finan *pessoa);
int banker(finan *pessoa);
void *testaDeadlock(void);
void *incrementaStep(void);

//-------------------------------------------------------Corpo da main-----------------------------------------------

int main (void) {
    srand(time(NULL));
    system("clear || cls");
    do {
        printf("Indique a quantidade de pessoas que farão o pedido de financiamento: ");
        scanf("%d", &qtd);
        if (qtd <= 0)
            printf("\n\nVocê deve informar uma quantidade maior do que zero para a quantidade de pessoas!\n");
    } while (qtd <=0);
    pthread_barrier_init(&sincroniza, NULL, qtd+1);
    pthread_barrier_init(&sincronizaStep, NULL, qtd+1);
    
    if ((ordem = (int *)malloc(qtd * sizeof(int))) == NULL) {
		printf("Não foi possível alocar memória\n");
		exit(1);
	}
    ordemCount = 0;

//------------------------------------Início das instâciações e definições de valores--------------------------------
    int valor;
    valor = 500001 + rand()%500000;
    bancoEspecie = valor;
    valor = 100001 + rand()%100000;
    bancoSubsidio = valor;
    valor = 30001 + rand()%30000;
    bancoTaxas = valor;

    finan pessoas[qtd];

    for (int i = 0; i < qtd; i++){
        pessoas[i].id = i;
        pessoas[i].atendido = 0;
        pessoas[i].total = rand()%bancoEspecie;
        int percentMin = 0.4*pessoas[i].total, percentMax = 0.3*pessoas[i].total;
        pessoas[i].especie = percentMin + rand()%percentMax;
        int taxa = 0.05 * pessoas[i].total;
        taxa /= 2;
        pessoas[i].taxas = taxa + rand()%taxa;
        int sub = 1 + rand()%3;
        pessoas[i].subsidio = ((2.5 * pow(sub, 2)) - (2.5 * sub) + 5)/100; //Essa foi muito forte. Eu admito!!!
        pessoas[i].step = -1;
    }

    pthread_t financiamentos[qtd];
    pthread_t deadlock;
    pthread_t incrementa;

//---------------------------------------Chamada e espera das threads-------------------------------------------------

    for (int i= 0; i < qtd; i++){
        int erro = pthread_create(&financiamentos[i], NULL, (void *)pedidoCredito, (void *)&pessoas[i]);
                if (erro){
                    printf("ERRO; pthread_create() devolveu o erro %d\n", erro);
                    exit(-1);
                }
    }

    int erro = pthread_create(&incrementa, NULL, (void *)incrementaStep, NULL);
                if (erro){
                    printf("ERRO; pthread_create() devolveu o erro %d\n", erro);
                    exit(-1);
                }

    erro = pthread_create(&deadlock, NULL, (void *)testaDeadlock, NULL);
                if (erro){
                    printf("ERRO; pthread_create() devolveu o erro %d\n", erro);
                    exit(-1);
                }


    for (int i=0; i<qtd; i++){
            pthread_join(financiamentos[i], NULL);
        }
    pthread_join(incrementa, NULL);

    printf("\n\n\t\tEsperando pelo detector de deadlock encerrar sua análise.");
    printf("\n");
    fimDeadlock = 1;
    pthread_join(deadlock, NULL);
    
    pthread_mutex_destroy(&mutexBanker);
    pthread_mutex_destroy(&mutexOrdem);
    pthread_barrier_destroy(&sincroniza);

    printf("\n\n\n\n\t\tFim da execução\n\n\t\tApresentação dos resultados:\n\n");

    if (ordemCount == 0){
        printf("\nO estado é inseguro, e nenhuma thread conseguiu acesso aos recursos!\n");
    } else if (ordemCount < qtd){
        printf("\nO estado é inseguro, porém algumas threads conseguiram acesso aos recursos\n");
    }
    if (ordemCount != 0){
        printf("\n\t\tOrdem segura de empréstimo dos recursos:\n");
        for (int i = 0; i<exec; i++){
            printf("%d(%d) ", ordem[i], pessoas[ordem[i]].step);
            if (i + 1 != exec){
                printf("---> ");
            }
        }
        printf("\n\n");
        printf("As seguintes threads executaram em conjunto:\n");
        for (int i=0; i<=pessoas[ordem[ordemCount-1]].step; i++){
            printf("Momento #%d: Threads: ", i);
            for (int j = 0; j<step; j++){
                if (pessoas[j].step == i){
                    printf("%d ", j);
                }
            }
            printf("\n");
        }
        printf("\n\n");
    }
    printf("\n\nEssa foi a presentação parcial dos resultados, para uma apresentação mais detalhada, procure pelo arquivo log.txt no diretório de execução do programa.\n\n\n");
    return 0;
}


//-------------------------------------------------------Corpo das threads--------------------------------------------

void *pedidoCredito(finan *pessoa){
    int modo;
    finan local = *(finan *)pessoa;

    for(;;){
        pthread_barrier_wait(&sincroniza);
        pthread_barrier_wait(&sincronizaStep);
        if (step < qtd){
            if (local.atendido == 0){
                pthread_mutex_lock(&mutexBanker);
                modo = banker(&local);
                pthread_mutex_unlock(&mutexBanker);
                deadlock = 0;

                if(modo == 1){
                    local.atendido = 1;
                    local.step = step;
                    pthread_mutex_lock(&mutexOrdem);
                    ordem[ordemCount] = local.id;
                    ordemCount++;
                    pthread_mutex_unlock(&mutexOrdem);
                    printf("\nPessoa #%d juntando o dinheiro para poder devolver.\n", local.id);
                    sleep(5 + rand()%5);
                    pthread_mutex_lock(&mutexBanker);
                    bancoEspecie += local.especie;
                    bancoSubsidio += (local.subsidio * local.total);
                    bancoTaxas += local.taxas;
                    pthread_mutex_unlock(&mutexBanker);
                    printf("\n#%d: Dinheiro devolvido.\n", local.id);
                    //printf("\nSituação do banco: Espécie:%d, taxas: %d, subsídio: %.2f\n", bancoEspecie, bancoTaxas, bancoSubsidio);
                    exec++;
                } else {
                    printf("#%d Preso no else mais interno.\n", local.id);
                }
            deadlock = 0;
            } else {
                printf("#%d Preso no segundo else interno.\n", local.id);
                deadlock = 0;
            }
        } else{
            printf("#%d Preso no else mais externo.\n", local.id);
            deadlock = 0;
            *(finan *)pessoa = local;
            pthread_exit(NULL);
        }
    }
    *(finan *)pessoa = local;
    pthread_exit(NULL);
}

void *testaDeadlock(void){
    for(;;){
        if (fimDeadlock == 0){
            if (deadlock == 0){
                deadlock = 1;
                sleep(15);
            } else{
                printf("\n\n\t\tInfelizmente foi detectado um deadlock e o programa teve que ser abortado.\n\n");
                exit(-1);
            }
        } else{
            pthread_exit(NULL);
        }
    }
}

void *incrementaStep(void){
    
    while(step < qtd){
        pthread_barrier_wait(&sincroniza);
        step++;
        if (exec == qtd){
            step = qtd;
        }
        pthread_barrier_wait(&sincronizaStep);
    }

    pthread_exit(NULL);

}

//-----------------------------------------------------Corpo das funções auxiliares----------------------------------------------

int banker(finan *pessoa){
    int modo = 0;//1 pode ser atendido/0 não pode ser atendido
    finan local = *(finan *)pessoa;
    int imovelTotal = local.total;
    int subsidio = imovelTotal * local.subsidio;
    int taxas = imovelTotal * 0.05;
    
    if (imovelTotal - local.especie - subsidio <= bancoEspecie  //Total a emprestar (tirando o que a pessoa tem e o subsídio)
    && subsidio <= bancoSubsidio && taxas - local.taxas <= bancoTaxas){

        modo = 1;
        bancoSubsidio -= subsidio;
        int valorEmprestado = imovelTotal - local.especie - subsidio;
        bancoEspecie -= valorEmprestado;
        local.especie += valorEmprestado;
        int taxasEmprestado = taxas - local.taxas;
        bancoTaxas -= taxasEmprestado;
        local.taxas += taxasEmprestado;
    } else {
        modo = 0;
    }

    *(finan *)pessoa = local;

    return modo;
}