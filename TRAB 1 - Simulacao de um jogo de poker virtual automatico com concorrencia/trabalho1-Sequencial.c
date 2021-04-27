/* Disciplina: Computacao Concorrente */
/* Prof.: Silvana Rossetto */
/* Alunos: Gabriel Morgado e Thiago Campbell */
/* Codigo: Trabalho de Implementacao 1 - Simulacao de um jogo de poker virtual automatico com concorrencia - versao sequencial para comparacao de resultados */

// bibliotecas necessarias
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include "timer.h"

// estrutura para os dados da carta
typedef struct _carta{
	int numero, naipe;
} carta;

// estrutura para um jogador | define quais cartas ele possui, sua pontuacao e qual tipo de combinacao ele atingiu
typedef struct _jogador{
	carta cartas[5];
    long int pontuacao;
    char *combinacao;
} jogador;

// variaveis globais do codigo
int combinacoes[10], jogadores = 0;
long int nJogadores;
char vetorNaipes[4][7] = {"paus", "ouros", "copas", "espadas"};
char nomeCombinacoes[10][20] = {"High Card", "Par", "Dois Pares", "Trinca", "Sequencia", "Flush", "Full House", "Quadra", "Straight Flush", "Royal Straight Flush"};
jogador *vetorJogadores;

// funcao para ordenar as cartas que um jogador possui | facilita na hora de checar qual combinacao o jogador conseguiu
void ordenaMao(int pos){
    int i, j;
    carta aux;
    for(i = 0; i < 5; i++){
        for(j = i + 1; j < 5; j++){
            if(vetorJogadores[pos].cartas[j].numero < vetorJogadores[pos].cartas[i].numero){ // se a carta da frente for menor que a de tras realiza a troca
                aux.numero = vetorJogadores[pos].cartas[i].numero;
                vetorJogadores[pos].cartas[i].numero = vetorJogadores[pos].cartas[j].numero;
                vetorJogadores[pos].cartas[j].numero = aux.numero;
                aux.naipe = vetorJogadores[pos].cartas[i].naipe;
                vetorJogadores[pos].cartas[i].naipe = vetorJogadores[pos].cartas[j].naipe;
                vetorJogadores[pos].cartas[j].naipe = aux.naipe;
            }
        }
    }
}

// funcao para ver qual combinacao o jogador em questao conseguiu | usa a mao ordenada pra ver a combinacao adquirida e atribuir os pontos referentes a ela
void checaCombinacao(int pos){
    int i, j, seq = 0, flush = 0;

    strcpy(vetorJogadores[pos].combinacao, nomeCombinacoes[0]); //high card | atribui logo no inicio a menor combinacao possivel
    (vetorJogadores[pos].cartas[0].numero == 1) ? (vetorJogadores[pos].pontuacao = 14) : (vetorJogadores[pos].pontuacao = vetorJogadores[pos].cartas[4].numero);

    for(i = 0; i < 5; i++){ // avanca por todas as cartas da mao do jogador
        if(i < 4 && vetorJogadores[pos].cartas[i].numero == vetorJogadores[pos].cartas[i + 1].numero){ // combinacao possivel apenas nas primeiras quatro cartas da mao 
            if(i < 3 && vetorJogadores[pos].cartas[i].numero == vetorJogadores[pos].cartas[i + 2].numero){ // combinacao possivel apenas nas primeiras tres cartas da mao 
                if(i < 2 && vetorJogadores[pos].cartas[i].numero == vetorJogadores[pos].cartas[i + 3].numero){ // combinacao possivel apenas nas primeiras duas cartas da mao 
                    strcpy(vetorJogadores[pos].combinacao, nomeCombinacoes[7]); // quadra
                    (vetorJogadores[pos].cartas[i].numero == 1) ? (vetorJogadores[pos].pontuacao += 228) : (vetorJogadores[pos].pontuacao += (vetorJogadores[pos].cartas[i].numero * 2) + 200);
                    break;
                }
                else if(i == 0 && vetorJogadores[pos].cartas[i + 3].numero == vetorJogadores[pos].cartas[i + 4].numero){
                    strcpy(vetorJogadores[pos].combinacao, nomeCombinacoes[6]); // full house
                    (vetorJogadores[pos].cartas[i].numero == 1) ? (vetorJogadores[pos].pontuacao += 178) : (vetorJogadores[pos].pontuacao += (vetorJogadores[pos].cartas[i].numero * 2) + 150);
                    break;
                }
                else{
                    strcpy(vetorJogadores[pos].combinacao, nomeCombinacoes[3]); // trinca
                    (vetorJogadores[pos].cartas[i].numero == 1) ? (vetorJogadores[pos].pontuacao += 78) : (vetorJogadores[pos].pontuacao += (vetorJogadores[pos].cartas[i].numero * 2) + 50);
                    break;
                }
            }
            else if(i == 0 && vetorJogadores[pos].cartas[i + 2].numero == vetorJogadores[pos].cartas[i + 3].numero && vetorJogadores[pos].cartas[i + 2].numero ==  vetorJogadores[pos].cartas[i + 4].numero){
                strcpy(vetorJogadores[pos].combinacao, nomeCombinacoes[6]); // full house
                (vetorJogadores[pos].cartas[i].numero == 1) ? (vetorJogadores[pos].pontuacao += 178) : (vetorJogadores[pos].pontuacao += (vetorJogadores[pos].cartas[i].numero * 2) + 150);
                break;
            }
            else if(i == 0 && (vetorJogadores[pos].cartas[i + 2].numero == vetorJogadores[pos].cartas[i + 3].numero || vetorJogadores[pos].cartas[i + 3].numero == vetorJogadores[pos].cartas[i + 4].numero)){
                strcpy(vetorJogadores[pos].combinacao, nomeCombinacoes[2]); // dois pares
                (vetorJogadores[pos].cartas[i].numero == 1) ? (vetorJogadores[pos].pontuacao += 69) : (vetorJogadores[pos].pontuacao += (vetorJogadores[pos].cartas[i].numero * 2) + 40);
                break;
            }
            else if(i == 1 && vetorJogadores[pos].cartas[i + 2].numero == vetorJogadores[pos].cartas[i + 3].numero){
                strcpy(vetorJogadores[pos].combinacao, nomeCombinacoes[2]); //dois pares
                (vetorJogadores[pos].cartas[i].numero == 1) ? (vetorJogadores[pos].pontuacao += 69) : (vetorJogadores[pos].pontuacao += (vetorJogadores[pos].cartas[i].numero * 2) + 40);
                break;
            }
            else{ // caso nenhuma outra seja possivel
                strcpy(vetorJogadores[pos].combinacao, nomeCombinacoes[1]); // par
                (vetorJogadores[pos].cartas[i].numero == 1) ? (vetorJogadores[pos].pontuacao += 47) : (vetorJogadores[pos].pontuacao += (vetorJogadores[pos].cartas[i].numero * 2) + 20);
                break;
            }           
        }

        else if(i == 0){ // apenas possivel na checagem da primeira carta da mao
            for(j = 1; j < 5; j++){
                if(vetorJogadores[pos].cartas[j].numero != vetorJogadores[pos].cartas[i].numero + j){
                    seq = 1; // registra se nao e uma sequencia
                    break;
                }
            }
            for(j = 1; j < 5; j++){
                if(vetorJogadores[pos].cartas[i].naipe != vetorJogadores[pos].cartas[j].naipe){
                    flush = 1; // registra se nao e um flush
                    break;
                }
            }
            if(seq == 0){ // executa caso seja uma sequencia
                if(flush == 0){ // executa caso seja um flush
                    strcpy(vetorJogadores[pos].combinacao, nomeCombinacoes[8]); // straight flush
                    (vetorJogadores[pos].cartas[i].numero == 1) ? (vetorJogadores[pos].pontuacao += 328) : (vetorJogadores[pos].pontuacao += (vetorJogadores[pos].cartas[i].numero * 2) + 300);
                } 
                else {
                    strcpy(vetorJogadores[pos].combinacao, nomeCombinacoes[4]); // sequencia
                    (vetorJogadores[pos].cartas[i].numero == 1) ? (vetorJogadores[pos].pontuacao += 98) : (vetorJogadores[pos].pontuacao += (vetorJogadores[pos].cartas[i].numero * 2) + 70);
                }
            } 
            else if(vetorJogadores[pos].cartas[i].numero == 1 && vetorJogadores[pos].cartas[i+1].numero == 10 && vetorJogadores[pos].cartas[i+2].numero == 11 && vetorJogadores[pos].cartas[i+3].numero == 12 && vetorJogadores[pos].cartas[i+4].numero == 13){
                if(flush == 0){ // executa caso seja um flush
                    strcpy(vetorJogadores[pos].combinacao, nomeCombinacoes[9]); // royal straight flush
                    (vetorJogadores[pos].cartas[i].numero == 1) ? (vetorJogadores[pos].pontuacao += 528) : (vetorJogadores[pos].pontuacao += (vetorJogadores[pos].cartas[i].numero * 2) + 500);
                } 
                else {
                    strcpy(vetorJogadores[pos].combinacao, nomeCombinacoes[4]); // sequencia
                    (vetorJogadores[pos].cartas[i].numero == 1) ? (vetorJogadores[pos].pontuacao += 98) : (vetorJogadores[pos].pontuacao += (vetorJogadores[pos].cartas[i].numero * 2) + 70);
                }
            }
            else if(flush == 0){ // executa caso seja um flush
                strcpy(vetorJogadores[pos].combinacao, nomeCombinacoes[5]); // flush
                (vetorJogadores[pos].cartas[i].numero == 1) ? (vetorJogadores[pos].pontuacao += 128) : (vetorJogadores[pos].pontuacao += (vetorJogadores[pos].cartas[i].numero * 2) + 100);
            }
        }
    }
}

// funcao para contar quantas vezes cada combinacao aconteceu entre os jogadores
void contaCombinacoes(int pos){
    jogadores++; // conta o numero de jogadores processados para checagem posterior

    // compara a combinacao do jogador com a lista de combinacoes para contagem
    if(strcmp(vetorJogadores[pos].combinacao, nomeCombinacoes[0]) == 0){  
        combinacoes[0]++;
    }
    else if(strcmp(vetorJogadores[pos].combinacao, nomeCombinacoes[1]) == 0){
        combinacoes[1]++;
    }
    else if(strcmp(vetorJogadores[pos].combinacao, nomeCombinacoes[2]) == 0){
        combinacoes[2]++;
    }
    else if(strcmp(vetorJogadores[pos].combinacao, nomeCombinacoes[3]) == 0){
        combinacoes[3]++;
    }
    else if(strcmp(vetorJogadores[pos].combinacao, nomeCombinacoes[4]) == 0){
        combinacoes[4]++;
    }
    else if(strcmp(vetorJogadores[pos].combinacao, nomeCombinacoes[5]) == 0){
        combinacoes[5]++;
    }
    else if(strcmp(vetorJogadores[pos].combinacao, nomeCombinacoes[6]) == 0){
        combinacoes[6]++;
    }
    else if(strcmp(vetorJogadores[pos].combinacao, nomeCombinacoes[7]) == 0){
        combinacoes[7]++;
    }
    else if(strcmp(vetorJogadores[pos].combinacao, nomeCombinacoes[8]) == 0){
        combinacoes[8]++;
    }
    else if(strcmp(vetorJogadores[pos].combinacao, nomeCombinacoes[9]) == 0){
        combinacoes[9]++;
    }
}

// funcao para definir quem venceu o jogo vendo qual jogador possui mais pontos
int checaGanhador(){
    int i, posGanhador = 0; // estabelece inicialmente que o primeiro jogador e o ganhador

    for(i = 1; i < nJogadores; i++){ // checa caso algum jogador tenha mais pontos que aquele atualmente selecionado como ganhador realiza a substituicao
        if(vetorJogadores[i].pontuacao > vetorJogadores[posGanhador].pontuacao){ 
            posGanhador = i;
        }
    }

    return posGanhador; // retorna o numero do ganhador como uma posicao para o vetor de jogadores
}

// funcao principal do codigo. Executa as partes concorrentes e sequenciais
int main(int argc, char *argv[]){
    // variaveis utilizadas para execucao da main
    int i, ganhador, total = 0;
    double inicio, fim;
    FILE *file;

    // inicio da contagem de tempo de execucao do codigo
    GET_TIME(inicio);

    // abre o arquivo texto do baralho para definir as cinco cartas de cada jogador
    file = fopen("baralho.txt", "r");
    if(file == NULL){ // aborta a execucao caso o arquivo baralho.txt nao esteja na mesma pasta que o codigo ou ocorra algum erro em sua abertura
        fprintf(stderr, "\nArquivo <baralho.txt> nao encontrado.\nPor favor compile e execute primeiro o arquivo <criaBaralho.c> encontrado na mesma pasta que esse codigo.\n\n");
        exit(EXIT_FAILURE);
    }

    // recebe o numero de jogadores a serem usados no programa pela linha de codigo. Aborta a execucao caso nao receba os parametros esperados
    if(argc < 2){
        fprintf(stderr, "Digite: %s <numero de jogadores>\n", argv[0]);
        return 1;
    }
    nJogadores = atol(argv[1]);

    // aloca memoria para o vetor de jogadores baseado na quantidade escolhida na chamada do programa
    vetorJogadores = (jogador *)malloc(sizeof(jogador) * nJogadores);
    if(vetorJogadores == NULL){
        fprintf(stderr, "Erro no malloc do vetor.");
        return 2;
    } 
    // aloca memoria para o char combinacao de cada jogador | char usado para indicar o nome da combinacao feita com as cartas na mao
    for(i = 0; i < nJogadores; i++){
        vetorJogadores[i].combinacao = (char *)malloc(sizeof(char));
        if(vetorJogadores[i].combinacao == NULL){
            fprintf(stderr, "Erro no malloc do vetor da combinacao.");
            return 2;
        }
    }

    // inicializa o vetor combinacoes para futura soma
    for(i = 0; i < 10; i++){
        combinacoes[i] = 0;
    }

    // le o arquivo texto e preenche a mao de um jogador com os numeros nele encontrados. O primeiro valor e o numero da carta e o segundo o naipe. A sequencia se repete para as cinco cartas na mao do jogador
    for(i = 0; i < nJogadores; i ++){
        fscanf(file, "%d,%d;%d,%d;%d,%d;%d,%d;%d,%d;", &vetorJogadores[i].cartas[0].numero, &vetorJogadores[i].cartas[0].naipe, &vetorJogadores[i].cartas[1].numero, &vetorJogadores[i].cartas[1].naipe, &vetorJogadores[i].cartas[2].numero, &vetorJogadores[i].cartas[2].naipe, &vetorJogadores[i].cartas[3].numero, &vetorJogadores[i].cartas[3].naipe, &vetorJogadores[i].cartas[4].numero, &vetorJogadores[i].cartas[4].naipe);
    }

    // chama a funcao de ordenar a mao para cada um dos jogadores
    for(i = 0; i < nJogadores; i++){
        ordenaMao(i);
    }

    // chama a funcao contaCombinacoes para calcular quantas de cada combinacao foram encontradas
    for(i = 0; i < nJogadores; i++){
        checaCombinacao(i);
    }

    // chama a funcao checaGanhador para exibir qual jogador obteve a maior pontuacao e printa o resultado
    ganhador = checaGanhador();
    printf("O jogador %d ganhou com um(a) %s!\n", ganhador + 1, vetorJogadores[ganhador].combinacao);

    // chama a funcao contaCombinacoes para calcular quantas de cada combinacao foram encontradas
    for(i = 0; i < nJogadores; i++){
        contaCombinacoes(i);
    }
    // exibe o resultado encontrado pela funcao contaCombinacoes
    puts("");
    for(i = 0; i < 10; i++){
        printf("%s: %d\n", nomeCombinacoes[i], combinacoes[i]);
        total += combinacoes[i];
    }
    
    /* descomente para testar o funcionamento das partes do codigo

    // printa o numero jogadores processados e o total de combinacoes feitas | caso sejam iguais o processo foi contado corretamente
    // printf("jogadores: %d | total: %d\n", jogadores, total);

    // printa o valor das cartas e a combinacao adquirida pelo jogador
    for(i = 0; i < nJogadores; i++){
        for(j = 0; j < 5; j++){
            printf("%d ", vetorJogadores[i].cartas[j].numero);
            // (vetorJogadores[i].cartas[j].naipe == 0) ? puts("de Paus ") : ((vetorJogadores[i].cartas[j].naipe == 1) ? puts("de Ouros ") : 
            // ((vetorJogadores[i].cartas[j].naipe == 2) ? puts("de Copas ") : puts("de Espadas ")));
        }
        printf("- %ld - %s - (%d)\n", vetorJogadores[i].pontuacao, vetorJogadores[i].combinacao ,i + 1);
    }
    
    // printa o naipe das cartas
    puts("");
    for(i = 0; i < nJogadores; i++){
        for(j = 0; j < 5; j++){
            // (vetorJogadores[i].cartas[j].naipe == 0) ? printf("de Paus ") : ((vetorJogadores[i].cartas[j].naipe == 1) ? printf("de Ouros ") : 
            // ((vetorJogadores[i].cartas[j].naipe == 2) ? printf("de Copas ") : printf("de Espadas ")));
            printf("%s ", vetorNaipes[vetorJogadores[i].cartas[j].naipe]);
        }
        puts("");
    } 
    */

    // fecha o arquivo do baralho aberto | libera a memoria alocada para combinacoes e o vetor de jogadores
    fclose(file);
    for(i = 0; i < nJogadores; i++){
        free(vetorJogadores[i].combinacao);
    }
    free(vetorJogadores);

    //finaliza a contagem do tempo de execucao e exibe seu resultado
    GET_TIME(fim);
    printf("\nTempo Sequencial: %.15lf\n", fim - inicio);

    return 0;
}