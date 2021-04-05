/* Disciplina: Computacao Concorrente */
/* Prof.: Silvana Rossetto */
/* Aluno: Gabriel Morgado */
/* Codigo: Laboratório 2 - Multiplicacao de matrizes em versao concorrente */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "timer.h"

float *matrizA;
float *matrizB;
float *matrizResposta;
int nthreads;

typedef struct{
    int id;
    int dim;
} tArgs;

void * tarefa(void *arg){
    int i, j, k;
    tArgs *args = (tArgs *) arg;
    for(i = args->id; i < args->dim; i += nthreads){
        for(j = 0; j < args->dim; j++){
            for(k = 0; k < args->dim; k++){
                matrizResposta[i * (args->dim) + j] += (matrizA[(i * (args->dim)) + k] * matrizB[(k * (args->dim)) + j]);
            }
        }
    }
    pthread_exit(NULL);
}

void mostraMatriz(float *matriz, int dim){
    int i;
    for(i = 0; i < dim * dim; i++){
        if(i != 0 && i % dim == 0){
            puts("");
        }
        printf("%.lf ", matriz[i]);
    }
    puts("");
}

int main(int argc, char* argv[]){
    int dim, i, j;
    double inicio, fim, delta, total = 0;
    pthread_t *tid;
    tArgs *args;

    GET_TIME(inicio);
    if(argc < 3){
        printf("Digite %s <dimensao da matriz> <numero de threads>\n", argv[0]);
        return 1;
    }
    dim = atoi(argv[1]);
    nthreads = atoi(argv[2]);
    if(dim < nthreads){
        nthreads = dim;
    }

    matrizA = (float *)malloc(sizeof(float) * dim * dim);
    matrizB = (float *)malloc(sizeof(float) * dim * dim);
    matrizResposta = (float *)malloc(sizeof(float) * dim * dim);
    if(matrizA == NULL || matrizB == NULL || matrizResposta == NULL){
        puts("Erro na alocação de memória para matriz.");
        return 2;
    }

    //inicializa as matrizes
    for(i = 0; i < dim; i++){
        for(j = 0; j < dim; j++){
            matrizA[i * dim + j] = 1;
            matrizB[i * dim + j] = 1;
            matrizResposta[i * dim + j] = 0;
        }
    }
    GET_TIME(fim);
    delta = fim - inicio;
    total += delta;
    printf("Tempo de inicializacao: %lf\n", delta);

    GET_TIME(inicio);
    tid = (pthread_t *)malloc(sizeof(pthread_t) * nthreads);
    args = (tArgs *)malloc(sizeof(tArgs) * nthreads);
    if(tid == NULL || args == NULL){
        puts("Erro na alocação de memória para as estruturas.");
        return 2;
    }

    for(i = 0; i < nthreads; i++){
        (args + i)->id = i;
        (args + i)->dim = dim;
        if(pthread_create(tid + i, NULL, tarefa, (void *)(args + i))){
            puts("Erro ao criar as threads.");
            return 3;
        }
    }

    for(i = 0; i < nthreads; i++){
        pthread_join(*(tid + i), NULL);
    }
    GET_TIME(fim);
    delta = fim - inicio;
    total += delta;
    printf("Tempo de multiplicacao: %lf\n", delta);

    // puts("MatrizA:");
    // mostraMatriz(matrizA, dim);

    // puts("MatrizB:");
    // mostraMatriz(matrizB, dim);

    // puts("Resposta da multiplicacao:");
    // mostraMatriz(matrizResposta, dim);

    GET_TIME(inicio);
    free(matrizA);
    free(matrizB);
    free(matrizResposta);
    free(args);
    free(tid);
    GET_TIME(fim);
    delta = fim - inicio;
    total += delta;
    printf("Tempo de finalizacao: %lf\n", delta);
    printf("Tempo total: %lf\n", total);

    return 0;
}