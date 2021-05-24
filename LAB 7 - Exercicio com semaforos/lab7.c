/* Disciplina: Computacao Concorrente */
/* Prof.: Silvana Rossetto */
/* Aluno: Gabriel Morgado */
/* Codigo: Laboratorio 7 - Exercicio com semaforos */

// bibliotecas utilizadas no codigo
#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h>
#include <semaphore.h>

// numero de threads a serem utilizadas
#define NTHREADS 4

// variaveis globais
int liberaT1 = 0; // variavel compartilhada para definir a liberacao da thread 1
sem_t condT2T3, condT1, em; // semaforos para sincronizar a ordem de execucao das threads

// thread 4
void *frase1 (void *args) {
    printf("Seja bem-vindo!\n");
    sem_post(&condT2T3); // libera a execucao das threads 2 e 3
    pthread_exit(NULL);
}

// thread 3
void *frase2 (void *args) {
    sem_wait(&condT2T3); // espera a thread 4 executar
    sem_post(&condT2T3); // se a thread 3 acontecer como segunda mantem o criterio para executar a 2
    printf("Fique a vontade.\n");
    sem_wait(&em); // inicio da exclusao mutua para variavel de liberacao da thread 1 
    if(liberaT1 < 1){
        liberaT1++; // secao critica
    }
    else{
        sem_post(&condT1); // libera thread 1 caso a thread 2 ja tenha executado
    }
    sem_post(&em); // saida da secao critica
    pthread_exit(NULL);
}

// thread 2
void *frase3 (void *args) {
    sem_wait(&condT2T3); // espera a thread 4 executar
    sem_post(&condT2T3); // se a thread 2 acontecer como segunda mantem o criterio para executar a 3
    printf("Sente-se por favor.\n");
    sem_wait(&em); // inicio da exclusao mutua para variavel de liberacao da thread 1
    if(liberaT1 < 1){
        liberaT1++; // secao critica
    }
    else{
        sem_post(&condT1); // libera thread 1 caso a thread 3 ja tenha executado
    }
    sem_post(&em); // saida da secao critica
    pthread_exit(NULL);
}

// thread 1
void *frase4 (void *args) {
    sem_wait(&condT1); // espera as threads 2 e 3 executarem
    printf("Volte sempre!\n");
    pthread_exit(NULL);
}

// funcao principal
int main(int argc, char *argv[]){
    int i;
    pthread_t tid[NTHREADS];

    // faz a inicializacao dos semaforos
    sem_init(&condT2T3, 0, 0); // condicao das threads 2 e 3
    sem_init(&condT1, 0, 0); // condicao para a thread 1 acontecer depois da thread 2
    sem_init(&em, 0, 1); // semaforo de exclusao mutua

    // realiza a criacao das threads
    if(pthread_create(&tid[0], NULL, frase1, NULL)){ 
        printf("Erro na criacao da thread 4\n"); 
        exit(-1);
    }
    if(pthread_create(&tid[1], NULL, frase2, NULL)){ 
        printf("Erro na criacao da thread 3\n"); 
        exit(-1);
    }
    if(pthread_create(&tid[2], NULL, frase3, NULL)){
        printf("Erro na criacao da thread 2\n"); 
        exit(-1);
    }
    if(pthread_create(&tid[3], NULL, frase4, NULL)){
        printf("Erro na criacao da thread 1\n"); 
        exit(-1);
    }

    // faz o programa esperar o termino de todas as threads antes de avancar
    for(i = 0; i < NTHREADS; i++) {
        if(pthread_join(tid[i], NULL)) {
                printf("Erro em pthread_join\n"); 
                exit(-1); 
        } 
    }

    pthread_exit(NULL);
}
