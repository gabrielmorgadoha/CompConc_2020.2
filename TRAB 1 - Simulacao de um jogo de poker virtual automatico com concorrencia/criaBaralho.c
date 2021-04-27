// bibliotecas necessarias
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// tamanho do baralho a ser criado
#define TAMBARALHO 100000000

int main(){
    // variaveis utilizadas para execucao da main
    int i, j, numero, naipe, cartasUsadas[5], baralho[52], aux = 0;
    FILE *file;

    // abre ou cria o arquivo de baralho
    file = fopen("baralho.txt", "w");
    if(file == NULL){ // aborta a execucao caso ocorra algum erro na abertura ou criacao do arquivo
        fprintf(stderr, "\nArquivo <baralho.txt> nao encontrado.\nPor favor compile e execute primeiro o arquivo <criaBaralho.c> encontrado na mesma pasta que esse codigo.\n\n");
        exit(EXIT_FAILURE);
    }

    puts("Iniciando a criacao do baralho."); // avisa que o processo de criacao comecou
    // iniciliza um vetor baralho auxiliar
    for(i = 0; i < 52; i++){
        baralho[i] = 0;
    }

    // faz o processo de criar cartas aleatoriamente e garantir que nao existirao cartas iguais em uma mesma linha
    srand(time(NULL));
    for(i = 0; i < TAMBARALHO; i++){
        if(i % 5 == 0 && i != 0){ // a cada 5 cartas faz uma nova linha e reinicia o processo de marcacao de cartas iguais
            fputs("\n", file);
            aux = 0;
            for(j = 0; j < 5; j++){ // reseta a marcacao de quais cartas foram usadas para o proximo jogador
                baralho[cartasUsadas[j]] = 0;
            }
        }
        numero = rand() % 13; // seleciona um numero aleatorio entre 0 e 12 para ser o valor da carta
        naipe = rand() % 4; // seleciona um numero aleatorio entre 0 e 3 para ser o naipe da carta

        while(baralho[(naipe * 13) + numero] == 1){ // caso seja uma carta que ja foi escolhida repete a selecao de numero e naipe
            numero = rand() % 13;
            naipe = rand() % 4;
        }
        baralho[(naipe * 13) + numero] = 1; // marca que a carta em questao ja foi escolhida
        cartasUsadas[aux] = (naipe * 13) + numero; // salva a posicao das cartas que foram maradas
        aux++;
        fprintf(file, "%d,%d;", numero + 1, naipe); // escreve no arquivo o valor real da carta e seu naipe, separados entre si por virgulas e por ponto e virgula com outras cartas
    }
    puts("O baralho foi criado."); // avisa que o processo de criacao terminou
    fclose(file); // fecha o arquivo do baralho

    return 0;
}