#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NTHREADS 2
#define TAMVETOR 10000

int vetor[TAMVETOR];

void * tarefaThread1(void *arg){
    int i;

    for(i = 0; i < TAMVETOR/2; i++){
        vetor[i]++;
    }

    pthread_exit(NULL);
}

void * tarefaThread2(void *arg){
    int i;

    for(i = TAMVETOR/2; i < TAMVETOR; i++){
        vetor[i]++;
    }

    pthread_exit(NULL);
}

int main(){
    pthread_t tid[NTHREADS];
    int i, varControle = 0;

    for(i = 0; i < TAMVETOR; i++){
        vetor[i] = 0;
    }

    for(i = 1; i <= NTHREADS; i++){
        if(i == 1){
            if(pthread_create(&tid[i-1], NULL, tarefaThread1, NULL)){
                printf("Erro -- pthread_create\n");
            }
        }
        else{
            if(pthread_create(&tid[i-1], NULL, tarefaThread2, NULL)){
                printf("Erro -- pthread_create\n");
            }
        }
    }

    for(i = 0; i < NTHREADS; i++){
        if(pthread_join(tid[i], NULL)){
            printf("Erro -- pthread_create\n");
        }
    }

    for(i = 0; i < TAMVETOR; i++){
        if(vetor[i] != 1){
            varControle = 1;
            break;
        }
    }

    if(varControle != 0){
        printf("Erro. Os valores finais do vetor sao diferentes.\n");
    }
    else{
        printf("Os valores finais do vetor estao corretos.\n");
    }

}