/* Disciplina: Computacao Concorrente */
/* Prof.: Silvana Rossetto */ 
/* Alunos: Gabriel Morgado e Thiago Campbell */
/* Codigo: Trabalho de Implementacao 2 - Simulacao de um ambiente de monitoramento de temperatura */

// import para geracao de numeros aleatorios thread-safe
import java.util.concurrent.ThreadLocalRandom;

// classe que realiza os testes de corretude
class Testes{
    // testa se a temperatura passada esta correta
    public static void testaTemperatura(SensoresAtuadores monitor){
        int i;
        for(i = 0; i < monitor.getTamanhoVetor(); i++){ // verifica todas as posicoes do vetor
            if(monitor.getPosicaoVetor(i).temperatura < 31 && monitor.getPosicaoVetor(i).temperatura != 0){ // se forem menores que 31 e nao forem o valor inicial ha erro
                System.err.println(Trab2.COR_ROXA + "Temperatura lida menor que 31! Erro no vetor. :(" + Trab2.RESETA_COR);
                System.exit(-1); // aborta execucao do programa
            }
        }
    }

    // testa se o atuador nºx leu o sensor nºx
    public static void testaAtuadorNoSensorCerto(int idA, int temperatura, int idS){
        if(idA != idS){ // o atuador 0 precisa ler os valores registrados pelo sensor 0 (e assim em diante)
            System.err.println(Trab2.COR_ROXA + "Erro! O atuador " + idA + " leu o sensor " + idS + ". :(" + Trab2.RESETA_COR);
            System.exit(-2); // aborta execucao do programa
        }
        if(temperatura < 31){
            System.err.println(Trab2.COR_ROXA + "Erro! O atuador " + idA + " leu uma temperatura abaixo de 30°C. :(" + Trab2.RESETA_COR);
            System.exit(-2); // aborta execucao do programa
        }
        /* System.out.println("Atuador " + idA + " esta lendo " + temperatura + "°C do sensor " + idS + "."); // caso tudo certo mostra a temperatura e os atuadores e sensores pertinentes */
    }
}

// classe para o vetor com as informacoes da temperatura e do sensor que o leu
class Medicoes{
    int temperatura, idSensor, idLeitura;
}

// classe com a thread dos sensores (escritores)
class Sensores extends Thread{
    int identificadorSensor, identificadorLeitura;
    SensoresAtuadores monitor;

    // construtor
    Sensores(SensoresAtuadores monitor){
        this.monitor = monitor;
        this.identificadorLeitura = 0;
    }

    // metodo que os sensores irao executar
    public void run(){
        Medicoes auxiliar = new Medicoes(); // variavel auxiliar para registrar os valores
        this.identificadorSensor = (int)Thread.currentThread().getId() % Trab2.nSensores; // salva o id do sensor usando um metodo built-in de java
        try{ // try-catch padrao das threads
            while(true){ // mantem a execucao das threads em um loop infinito | tempo de execucao indeterminado
                this.monitor.EntraSensor(this.identificadorSensor); // chama a entrada do sensor x na fila
                auxiliar.temperatura = (int)ThreadLocalRandom.current().nextInt(25, 41); // temperatura medida e um numero randomico entre 25 e 40
                auxiliar.idSensor = identificadorSensor; // salva o id do sensor atual
                auxiliar.idLeitura = identificadorLeitura; // salva qual o numero da leitura que este sensor esta realizando | inicializado em 0
                if(auxiliar.temperatura > 30){ // caso a temperatura seja maior que 30°C escreve no vetor
                    this.monitor.EscreveVetor(auxiliar, ((identificadorSensor + (Trab2.nSensores * identificadorLeitura)) % this.monitor.getTamanhoVetor())); // escrita feita no padrao FIFO
                } 
                this.monitor.SaiSensor(this.identificadorSensor); // chama a saida do sensor x
                sleep(1000); // medicao da temperatura e feita a cada 1 segundo
                identificadorLeitura++; // incrementa a quantidade de vezes que o sensor checou a temperatura
            }
        }catch(InterruptedException e) {return;}
    }
}

// classe com a thread dos atuadores (leitores)
class Atuadores extends Thread{
    int identificadorAtuador;
    SensoresAtuadores monitor;

    // construtor
    Atuadores(SensoresAtuadores monitor){
        this.monitor = monitor;
    }

    // metodo que os atuadores irao executar
    public void run(){
        int i, j, pos, out, aux, vezesLidas, checaAmarelo, checaVermelho;
        float media;
        Medicoes elemento;
        this.identificadorAtuador = (int)Thread.currentThread().getId() % Trab2.nSensores; // salva o id do atuador usando um metodo built-in de java
        pos = this.identificadorAtuador; // a posicao inicial da leitura e o id do atuador
        try{ // try-catch padrao das threads
            while(true){ // mantem a execucao das threads em um loop infinito | tempo de execucao indeterminado 
                media = 0; // redefine os valores das variaveis a cada iteracao da thread
                vezesLidas = 0;
                checaAmarelo = 0;
                checaVermelho = 0; 
                this.monitor.EntraAtuador(identificadorAtuador); // chama a entrada do atuador x na fila
                elemento = this.monitor.getPosicaoVetor(pos); // elemento inicial a ser lido
                if(elemento.temperatura > 0){ // caso a temperatura ja tenha sido escrita
                    out = pos; // auxiliar e igual a posicao
                    Testes.testaAtuadorNoSensorCerto(this.identificadorAtuador, elemento.temperatura, elemento.idSensor); // chama o teste para verificar a corretude da leitura
                    for(i = 0; i < 15; i++){ // le os ultimos 15 numeros daquele sensor
                        aux = this.monitor.getPosicaoVetor(out).temperatura;
                        if(aux > 0){
                            media += aux; // calculo da media da temperatura
                            vezesLidas++;
                        }  
                        if(aux > 35 && i < 5){ // se as ultimas 5 temperaturas sao maiores que 35
                            checaVermelho++;
                        }
                        if(aux > 35){ // se as ultimas 15 temperaturas sao maiores que 35
                            checaAmarelo++;
                        }
                        for(j = 0; j < Trab2.nSensores; j++){ // verifica a proxima posicao a ser lida
                            out--; // le "voltando" o vetor
                            if(out < 0){ // caso a posicao seja menor que zero retorna ao final do vetor
                                out = this.monitor.getTamanhoVetor() - 1;
                            }
                        }
                        pos += (Trab2.nSensores); // incrementa a posicao a ser lida pelo numero de sensores/atuadores | avanca um numero balanceado de casas
                    }
                    if(checaVermelho > 4){ // emite o alerta vermelho caso necessário e exibe a media das leituras na tela 
                        System.out.printf(Trab2.COR_VERMELHA + "Alerta vermelho." + Trab2.RESETA_COR + " Media do sensor " + identificadorAtuador + ": %.2f°C\n", (media/vezesLidas));
                    }
                    else if(checaAmarelo > 4){ // emite o alerta amarelo caso necessário e exibe a media das leituras na tela 
                        System.out.printf(Trab2.COR_AMARELA + "Alerta amarelo." + Trab2.RESETA_COR + " Media do sensor " + identificadorAtuador + ": %.2f°C\n", (media/vezesLidas));
                    }
                    else{ // emite o alerta normal e exibe a media das leituras na tela 
                        System.out.printf(Trab2.COR_VERDE + "Alerta normal." + Trab2.RESETA_COR + " Media do sensor " + identificadorAtuador + ": %.2f°C\n", (media/vezesLidas));
                    }
                }
                this.monitor.SaiAtuador(identificadorAtuador); // chama a saida do atuador x
                sleep(2000); // atuador le os valores do sensor a cada 2 segundo
            }
        }catch(InterruptedException e) {return;}
    }
}

// classe com a main
class Trab2{

    //variaveis "globais" usadas | as Strings sao utilizadas para colorir o texto impresso no terminal
    static int nSensores;
    static Thread[] vetorSensores, vetorAtuadores;
    public static final String RESETA_COR = "\u001B[0m";
    public static final String COR_VERMELHA = "\u001B[31m";
    public static final String COR_VERDE = "\u001B[32m";
    public static final String COR_AMARELA = "\u001B[33m";
    public static final String COR_ROXA = "\u001B[35m";
    
    // metodo principal do codigo
    public static void main(String[] args){
        int i;
        SensoresAtuadores monitor = new SensoresAtuadores();
        
        // le o numero de sensores do terminal | aborta a execucao caso nao receba o parametro desejado
        if(args.length < 1){
            System.out.println("Digite: java Trab2 <numero de sensores>");
            return;
        }
        nSensores = Integer.parseInt(args[0]);

        // existe um atuador para cada sensor
        vetorSensores = new Thread[nSensores];
        vetorAtuadores = new Thread[nSensores];

        // cria, inicializa e inicia a chamada dos atuadores
        for(i = 0; i < nSensores; i++){
            vetorAtuadores[i] = new Atuadores(monitor);
            vetorAtuadores[i].start();
        }

        // cria, inicializa e inicia a chamada dos sensores
        for(i = 0; i < nSensores; i++){
            vetorSensores[i] = new Sensores(monitor);
            vetorSensores[i].start();
        }

        // faz o teste que temperaturas se salvas no vetor sao maiores que 30
        while(true){
            try{
                Testes.testaTemperatura(monitor);
                Thread.sleep(3000); // espera 3 segundos entre repeticoes do metodo
            }catch(InterruptedException e){return;}
        }
    }
}