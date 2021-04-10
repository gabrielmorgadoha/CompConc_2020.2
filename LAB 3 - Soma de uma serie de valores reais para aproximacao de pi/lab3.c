/* Disciplina: Computacao Concorrente */
/* Prof.: Silvana Rossetto */
/* Aluno: Gabriel Morgado */
/* Codigo: Laboratório 3 - Soma de uma serie de valores reais para aproximacao de pi */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include "timer.h"

long long int N;
int nthreads;
double *vetor;

// funcao para soma dos elementos de forma concorrente
void * tarefa(void *arg){
    long int i;
    double *somaThread;
    somaThread = (double *)malloc(sizeof(double));
    for(i = (long int) arg; i < N; i += nthreads){
        *somaThread += vetor[i];
    }
    pthread_exit((void *) somaThread);
}

int main(int argc, char *argv[]){
    int negativo = 0;
    long int i;
    double *retornoThread, tempoSequencial, tempoConcorrente, inicio, fim, j = 1, somaSequencial = 0, somaConcorrente = 0;
    pthread_t *tid;

    // recebe da linha de comando o tamanho do vetor e quantas threads devem ser utilizadas 
    if(argc < 3){
        fprintf(stderr, "Digite: %s <dimensao do vetor> <numero de threads>\n", argv[0]);
        return 1;
    }
    N = atol(argv[1]);
    nthreads = atoi(argv[2]);
    
    // aloca memoria para o vetor
    vetor = (double *)malloc(sizeof(double) * N);
    if(vetor == NULL){
        fprintf(stderr, "Erro no malloc do vetor.");
        return 2;
    }

    // preenche os elementos do vetor com os valores da serie
    for(i = 0; i < N; i++){
        vetor[i] = (1/j);
        j += 2;
        if(negativo == 1){ // inverte o sinal do numero posicao sim, posicao nao
            vetor[i] *= -1;
        }
        negativo = (negativo == 0) ? 1 : 0;
    }

    // executa a soma da serie de forma sequencial
    GET_TIME(inicio);
    for(i = 0; i < N; i++){
        somaSequencial += vetor[i];
    }
    somaSequencial *= 4;
    GET_TIME(fim);
    tempoSequencial = fim - inicio;
	// printa o tempo que leva para executar a soma de forma sequencial
    // printf("Tempo sequencial: %lf\n", tempoSequencial);

    
    // aloca memoria para o identificador das threads
    GET_TIME(inicio);
    tid = (pthread_t *)malloc(sizeof(pthread_t) * nthreads);
    if(tid == NULL){
        fprintf(stderr, "Erro no malloc do identificador das threads.");
        return 2;
    }

    // cria as threads a serem utilizadas pelo programa
    for(i = 0; i < nthreads; i++){
        if(pthread_create(tid + i, NULL, tarefa, (void *) i)){
            fprintf(stderr, "Erro em pthread_create\n");
            return 3;
        }
    }

    // faz o programa esperar o termino de todas as threads antes de finalizar
    for(i = 0; i < nthreads; i++){
        if(pthread_join(*(tid + i), (void **) &retornoThread)){
            fprintf(stderr, "Erro em pthread_join\n");
            return 3;
        }
        somaConcorrente += *retornoThread;
    }
    somaConcorrente *= 4;
    GET_TIME(fim);
    tempoConcorrente = fim - inicio;
	// printa o tempo que leva para executar a soma de forma concorrente
    // printf("Tempo concorrente: %lf\n", tempoConcorrente);

    // faz o calculo do ganho de desempenho entre a versao sequencial e concorrente
    printf("Ganho de desempenho = %lf\n\n", tempoSequencial / tempoConcorrente);

    // printa o valor de pi para checar qual mais se aproxima
    printf("Valor de pi: %.15lf\n", M_PI);
    printf("Soma sequencial: %.15lf\n", somaSequencial);
    printf("Soma concorrente: %.15lf\n", somaConcorrente);
    (M_PI - somaSequencial > M_PI - somaConcorrente) ? puts("Soma concorrente se aproxima mais rapidamente de pi.") : puts("Soma sequencial se aproxima mais rapidamente de pi.");

    // libera a memoria alocada
    free(tid);
    free(vetor);

    return 0;
}