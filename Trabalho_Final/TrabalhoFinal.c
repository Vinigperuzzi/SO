#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <math.h>

typedef struct financista{
    int id;
    float especie;
    float subsidio;
    float taxas;
    float total;
    int atendido;
    int step;
} finan;

float bancoEspecie, bancoSubsidio, bancoTaxas;
int qtd, deadlock = 0, fimDeadlock = 0, ordemCount, exec=0, step=-1;
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
    float valor;
    valor = (int)(500001 + rand()%500000);
    bancoEspecie = valor;
    valor = (int)(100001 + rand()%100000);
    bancoSubsidio = valor;
    valor = (int)(30001 + rand()%30000);
    bancoTaxas = valor;
    bancoSubsidio /= 4;
    printf("\nSituação do banco: Espécie: %.2f, taxas: %.2f, subsídio: %.2f\n", bancoEspecie, bancoTaxas, bancoSubsidio);

    finan pessoas[qtd];

    for (int i = 0; i < qtd; i++){
        pessoas[i].id = i;
        pessoas[i].atendido = 0;
        pessoas[i].total = (int)(rand()%(int)bancoEspecie);
        float percentMin = 0.4*pessoas[i].total, percentMax = 0.3*pessoas[i].total;
        pessoas[i].especie = percentMin + rand()%(int)percentMax;
        float taxa = 0.05 * pessoas[i].total;
        taxa /= 2;
        pessoas[i].taxas = taxa + rand()%(int)taxa;
        int sub = 1 + rand()%3;
        float percentSubsidio = ((2.5 * pow(sub, 2)) - (2.5 * sub) + 5)/100; //Essa foi muito forte. Eu admito!!!
        pessoas[i].subsidio = pessoas[i].total * percentSubsidio;
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
    printf("\n\t\tSituação do banco: Espécie: %.2f, taxas: %.2f, subsídio: %.2f\n", bancoEspecie, bancoTaxas, bancoSubsidio);

    /*arq   printf("\n\n\n\n\t\tFim da execução\n\n\t\tApresentação dos resultados:\n\n");
            printf("\n\t\tSituação do banco: Espécie: %.2f, taxas: %.2f, subsídio: %.2f\n", bancoEspecie, bancoTaxas, bancoSubsidio);
    */

    if (ordemCount == 0){
        printf("\nO estado é inseguro, e nenhuma thread conseguiu acesso aos recursos!\n");
        //arq   printf("\nO estado é inseguro, e nenhuma thread conseguiu acesso aos recursos!\n");
    } else if (ordemCount < qtd){
        printf("\nO estado é inseguro, porém algumas threads conseguiram acesso aos recursos\n");
        //arq   printf("\nO estado é inseguro, porém algumas threads conseguiram acesso aos recursos\n");
    }
    if (ordemCount != 0){
        printf("\n\t\tOrdem segura de empréstimo dos recursos: Thread(momento)\n");
        //arq   printf("\n\t\tOrdem segura de empréstimo dos recursos: Thread(momento)\n");
        for (int i = 0; i<exec; i++){
            printf("%d(%d) ", ordem[i], pessoas[ordem[i]].step);
            //arq   printf("%d(%d) ", ordem[i], pessoas[ordem[i]].step);
            if (i + 1 != exec){
                printf("---> ");
                //arq   printf("---> ");
            }
        }
        printf("\n\n");
        printf("As seguintes threads executaram em conjunto:\n");
        //arq   printf("As seguintes threads executaram em conjunto:\n");
        for (int i=0; i<=pessoas[ordem[ordemCount-1]].step; i++){
            printf("Momento #%d: Threads: ", i);
            //arq   printf("Momento #%d: Threads: ", i);
            for (int j = 0; j<step; j++){
                if (pessoas[j].step == i){
                    printf("%d ", j);
                    //arq   printf("%d ", j);
                }
            }
            printf("\n");
            //arq   printf("\n");
        }
        printf("\n\n");
        //arq   printf("\n\n");
    }
    printf("\n\nEssa foi a presentação parcial dos resultados, para uma apresentação mais detalhada, procure pelo arquivo log.txt no diretório de execução do programa.\n\n\n");
    //arq   printf("\n\n\n\t\t\tFim do arquivo\n\n\n");

    free(ordem);
    return 0;
}


//-------------------------------------------------------Corpo das threads--------------------------------------------

void *pedidoCredito(finan *pessoa){
    int modo;
    finan local = *(finan *)pessoa;
    float empresEspecie = local.total - local.especie - local.subsidio, empresTaxas = (local.total * 0.05) - local.taxas, empresSub = local.subsidio, entrada = local.especie;

    for(;;){
        pthread_barrier_wait(&sincroniza);
        pthread_barrier_wait(&sincronizaStep);
        if (step < qtd){
            if (local.atendido == 0){
                pthread_mutex_lock(&mutexBanker);
                modo = banker(&local);
                deadlock = 0;

                if(modo == 1){
                    //arq   printf("\nPessoa #%d pegou emprestado: %.2f em espécie, %.2f em taxas e %.2f de subsídio. Para pagar por um imóvel de %.2f, cuja entrada dada foi de %.2f.\n", local.id, (empresEspecie), (empresTaxas), (empresSub), local.total, entrada);
                    //arq   printf("\nnova situação do banco: Espécie: %.2f, taxas: %.2f, subsídio: %.2f\n", bancoEspecie, bancoTaxas, bancoSubsidio);
                    pthread_mutex_unlock(&mutexBanker);
                    local.atendido = 1;
                    local.step = step;
                    pthread_mutex_lock(&mutexOrdem);
                    ordem[ordemCount] = local.id;
                    ordemCount++;
                    pthread_mutex_unlock(&mutexOrdem);
                    printf("\nPessoa #%d conseguiu financiamento e está juntando o dinheiro para poder devolver.\n", local.id);
                    //arq   printf("\nPessoa #%d conseguiu financiamento e está juntando o dinheiro para poder devolver.\n", local.id);
                    sleep(5 + rand()%5);
                    pthread_mutex_lock(&mutexBanker);
                    bancoEspecie += local.especie;
                    bancoSubsidio += local.subsidio;
                    bancoTaxas += local.taxas;
                    printf("\n#%d: Dinheiro devolvido.\n", local.id);
                    //arq   printf("\n#%d: Dinheiro devolvido.\n", local.id);
                    //arq   printf("\nnova situação do banco: Espécie: %.2f, taxas: %.2f, subsídio: %.2f\n", bancoEspecie, bancoTaxas, bancoSubsidio);
                    pthread_mutex_unlock(&mutexBanker);
                    exec++;
                } else {
                    pthread_mutex_unlock(&mutexBanker);
                    //arq   printf("\n#%d: Não foi possível executar e está esperando que as outras acabem para uma nova tentativa", local.id);
                }
            deadlock = 0;
            } else {
                //arq   printf("\n#%d: Foi ser testada, porém já executou. Esta esperando as outras terminar para uma nova rodada de testes", local.id);
                deadlock = 0;
            }
        } else{
            //arq   printf("\n#%d: Já terminou a execução e espera para se juntar às outras na main", local.id);
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
                sleep(12);
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
    float taxas = local.total * 0.05;
    
    if (local.total - local.especie - local.subsidio <= bancoEspecie  //Total a emprestar (tirando o que a pessoa tem e o subsídio)
    && local.subsidio <= bancoSubsidio && taxas - local.taxas <= bancoTaxas){

        modo = 1;
        bancoSubsidio -= local.subsidio;
        float valorEmprestado = local.total - local.especie - local.subsidio;
        bancoEspecie -= valorEmprestado;
        local.especie += valorEmprestado;
        float taxasEmprestado = taxas - local.taxas;
        bancoTaxas -= taxasEmprestado;
        local.taxas += taxasEmprestado;
    } else {
        modo = 0;
    }

    *(finan *)pessoa = local;

    return modo;
}