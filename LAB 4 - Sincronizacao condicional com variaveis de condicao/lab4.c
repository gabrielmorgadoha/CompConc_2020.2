/* Disciplina: Computacao Concorrente */
/* Prof.: Silvana Rossetto */
/* Aluno: Gabriel Morgado */
/* Codigo: Laboratorio 4 - Sincronizacao condicional com variaveis de condicao */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NTHREADS 4

// variaveis globais 
int controleT1 = 0, controleT2T3 = 0; // variavel para controlar a inicializacao das threads 1, 2 e 3
pthread_mutex_t T_mutex;
pthread_cond_t doisTres_cond, um_cond;

// Thread 4 
void *frase1 (void *arg){

    pthread_mutex_lock(&T_mutex);
    printf("Seja bem-vindo!\n");
    controleT2T3++; // conta que a thread terminou para a checagem da liberacao das threads 2 e 3
    pthread_cond_broadcast(&doisTres_cond); // caso bloqueadas libera thread 2 e thread 3
    pthread_mutex_unlock(&T_mutex);

    pthread_exit(NULL);
}

// Thread 3 
void *frase2 (void *arg){

    pthread_mutex_lock(&T_mutex);
    if(controleT2T3 < 1){ 
        pthread_cond_wait(&doisTres_cond, &T_mutex); // caso a thread 4 ainda nao tenha finalizado faz a thread 3 esperar a finalizacao da thread 4
    }
    printf("Sente-se por favor.\n");
    controleT1++; // conta que a thread terminou para a checagem da liberacao da thread 1
    if(controleT1 == 2){
        pthread_cond_signal(&um_cond); // manda o sinal para liberar a thread 1
    }
    pthread_mutex_unlock(&T_mutex);

    pthread_exit(NULL);
}

// Thread 2
void *frase3 (void *arg){
    
    pthread_mutex_lock(&T_mutex);
    if(controleT2T3 < 1){ 
        pthread_cond_wait(&doisTres_cond, &T_mutex); // caso a thread 4 ainda nao tenha finalizado faz a thread 2 esperar a finalizacao da thread 4
    }
    printf("Fique a vontade.\n");
    controleT1++; // conta que a thread terminou para a checagem da liberacao da thread 1
    if(controleT1 == 2){
        pthread_cond_signal(&um_cond); // manda o sinal para liberar a thread 1
    }
    pthread_mutex_unlock(&T_mutex);

    pthread_exit(NULL);
}

// Thread 1
void *frase4 (void *arg){

    pthread_mutex_lock(&T_mutex);
    if(controleT1 < 2){ 
        pthread_cond_wait(&um_cond, &T_mutex); // caso as threads 2 e 3 ainda nao tenha finalizado faz a thread 1 esperar a finalizacao delas
    }
    pthread_mutex_unlock(&T_mutex);

    printf("Volte sempre!\n");
    pthread_exit(NULL);
}


// funcao principal
int main(int argc, char *argv[]) {
    int i; 
    pthread_t threads[NTHREADS];

    // faz a inicializacao do mutex (lock de exclusao mutua) e a variavel de condicao 
    pthread_mutex_init(&T_mutex, NULL);
    pthread_cond_init (&doisTres_cond, NULL);
    pthread_cond_init (&um_cond, NULL);

    // realiza a criacao das threads
    pthread_create(&threads[0], NULL, frase4, NULL);
    pthread_create(&threads[1], NULL, frase3, NULL);
    pthread_create(&threads[2], NULL, frase2, NULL);
    pthread_create(&threads[3], NULL, frase1, NULL);

    // faz o programa esperar o termino de todas as threads antes de avancar
    for (i = 0; i < NTHREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    // libera a memoria alocada pelo mutex e pela variavel de condicao
    pthread_mutex_destroy(&T_mutex);
    pthread_cond_destroy(&doisTres_cond);
    pthread_cond_destroy(&um_cond);
    return 0;
}