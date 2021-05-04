/* Disciplina: Computacao Concorrente */
/* Prof.: Silvana Rossetto */
/* Aluno: Gabriel Morgado */
/* Codigo: Laboratório 5 - Problemas classicos de concorrencia usando locks e variaveis de condicao (barreira) */

// bibliotecas utilizadas
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// variaveis globais a serem utilizadas no programa
int nthreads, *vetor, *vetorAux, bloqueadas = 0;
pthread_mutex_t x_mutex;
pthread_cond_t x_cond;

// funcao barreira para bloquear o acesso ate todas as threads chegarem neste ponto
void barreira(){
    int i;
    pthread_mutex_lock(&x_mutex); // bloqueia acesso durante secao critica
    if(bloqueadas == (nthreads - 1)){
        for(i = 0; i < nthreads; i++){
            vetor[i] = vetor[i] + vetorAux[i]; // faz a soma em questao
        }
        pthread_cond_broadcast(&x_cond);
        bloqueadas = 0;
    }
    else{ // bloqueia as threads ate a chegada da ultima
        bloqueadas++;
        pthread_cond_wait(&x_cond, &x_mutex);
    }
    pthread_mutex_unlock(&x_mutex); // libera acesso
}

// tarefa a ser executada pelas threads
void *tarefa(void *arg){
    int i;
    long int id = (long int) arg;

    for(i = 1; i <= nthreads; i *= 2){ // avanca i em termos de 2^n
        if(id - i > -1){
            vetorAux[id] = vetor[id - i]; // salva o valor no auxiliar
        }
        else{
            vetorAux[id] = 0;
        }
        barreira(nthreads); // vai para a barreira esperar ou liberar as outras threads
    }

    pthread_exit(NULL);
}

// funcao principal do programa
int main(int argc, char *argv[]){
    long int i, negativo;
    pthread_t *tid;

    // recebe da linha de comando o numero de threads
    if(argc < 2){
        fprintf(stderr, "Digite: %s <numero de threads>\n", argv[0]);
        return 1;
    }
    nthreads = atoi(argv[1]);

    // inicializa o lock de exclusao mutua e a variavel de condicao
    pthread_mutex_init(&x_mutex, NULL);
    pthread_cond_init(&x_cond, NULL);

    // inicializa o vetor a ser calculado e seu auxiliar
    vetor = (int *)malloc(sizeof(int) * nthreads);
    vetorAux = (int *)malloc(sizeof(int) * nthreads);
    if(vetor == NULL || vetorAux == NULL){
        fprintf(stderr, "Erro no malloc do vetor.");
        return 2;
    }

    // pega numeros pseudoaleatorios para preencher o vetor
    srand(time(NULL));
    printf("Vetor construido: [");
    for(i = 0; i < nthreads; i++){
        negativo = rand() % 2;  // define se o numero vai ser negativo
        vetor[i] = rand() % 50; // pega um numero de 0 ate 49 | edite para alterar o valor maximo
        if(negativo != 0){
            vetor[i] *= -1;
        }
        if(i != 0){ // formatacao da impressao no terminal
            printf(", %d", vetor[i]);
        }
        else{
            printf("%d", vetor[i]);
        }
    }
    puts("]");

    // calculo sequencial para comparacao de resultado
    int vetorSequencial[nthreads];
    vetorSequencial[0] = vetor[0];
    for(i = 1; i < nthreads; i++){
        vetorSequencial[i] = (vetor[i] + vetorSequencial[i - 1]);
    }
    printf("Vetor sequencial: [");
    for(i = 0; i < nthreads; i++){
        if(i != 0){
            printf(", %d", vetorSequencial[i]);
        }
        else{
            printf("%d", vetorSequencial[i]);
        }
    }
    puts("]");

    // aloca memoria para o identificador das threads
    tid = (pthread_t *)malloc(sizeof(pthread_t) * nthreads);
    if(tid == NULL){
        fprintf(stderr, "Erro no malloc do identificador das threads.");
        return 2;
    }

    // cria as threads a serem utilizadas pelo programa
    for(i = 0; i < nthreads; i++){
        if(pthread_create(tid + i, NULL, tarefa, (void *)i)){
            fprintf(stderr, "Erro em pthread_create.\n");
            return 3;
        }
    }

    // faz o programa esperar o termino de todas as threads antes de finalizar
    for(i = 0; i < nthreads; i++){
        if(pthread_join(*(tid + i), NULL)){
            fprintf(stderr, "Erro em pthread_join.\n");
            return 3;
        }
    }

    // exibe o resultado do calculo concorrente
    printf("Vetor final: [");
    for(i = 0; i < nthreads; i++){
        if(i != 0){
            printf(", %d", vetor[i]);
        }
        else{
            printf("%d", vetor[i]);
        }
    }
    puts("]");

    for(i = 0; i < nthreads; i++){
        if(vetor[i] != vetorSequencial[i]){
            puts("O codigo deu errado! Os vetores sao diferentes.");
            break;
        }
        if(i == nthreads - 1){
            puts("Deu tudo certo. Os vetores sao iguais.");
        }
    }

    // libera a memoria alocada para os vetores e para o lock e condicao
    free(vetor);
    free(vetorAux);
    pthread_mutex_destroy(&x_mutex);
    pthread_cond_destroy(&x_cond);
    return 0;
}