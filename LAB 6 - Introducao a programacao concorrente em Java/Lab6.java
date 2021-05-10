/* Disciplina: Computacao Concorrente */
/* Prof.: Silvana Rossetto */
/* Aluno: Gabriel Morgado */
/* Codigo: Laboratório 6 - Introducao a programacao concorrente em Java */

// classe com a tarefa que sera executada pelas threads
class Tarefa extends Thread{
    public void run(){
        int i;
        // pega o id da thread como ponto inicial no vetor e avanca o numero de threads a cada iteracao | divide a tarefa de forma balanceada
        for(i = (int)((Thread.currentThread().getId()) % Lab6.nThreads); i < Lab6.nElementos; i += Lab6.nThreads){
            Lab6.vetor[i]++; // soma 1 ao valor do elemento i do vetor
        }
    }
}

// classe com o metodo main
class Lab6{
    // declaracao das variaveis 
    static int nElementos;
    static int nThreads;
    static int[] vetor;

    public static void main(String[] args){
        int i;    
        Thread[] threads;

        // informa o usuario se nao recebeu a quantidade necessaria de argumentos
        if(args.length < 2){
            System.out.println("Digite: java Lab6 <tamanho do vetor> <numero de threads>");
            return;
        }
        nElementos = Integer.parseInt(args[0]); // primeiro argumento e o numero de elementos do vetor
        if(Integer.parseInt(args[1]) > nElementos){ // o segundo argumento e o numero de threads
            threads = new Thread[nElementos]; // o numero maximo de threads e o numero de elementos do vetor
        }
        else{
            threads = new Thread[Integer.parseInt(args[1])];
        }
        vetor = new int[nElementos];
        nThreads = threads.length; // define a quantidade de threads

        // inicializa o vetor com 0 em todas as posicoes
        System.out.print("Vetor inicial: [");
        for(i = 0; i < nElementos; i++){
            vetor[i] = 0;
            if(i != 0){ // condicional para formatacao do print
                System.out.print(", " + vetor[i]);
            }
            else{
                System.out.print(vetor[i]);
            }
        }
        System.out.println("]");
        
        // cria as threads | usa a classe que estendeu "Thread"
        for(i = 0; i < nThreads; i++){
            System.out.println("Cria a thread " + i);
            threads[i] = new Tarefa();
        }

        // inicializa as threads criadas
        for(i = 0; i < nThreads; i++){
            threads[i].start();
        }

        // faz o programa esperar o termino de todas as threads antes de prosseguir
        for(i = 0; i < nThreads; i++){
            try{
                threads[i].join();
            }
            catch(InterruptedException e){
                return;
            }
        }

        // printa o vetor apos as threads terminarem a tarefa
        System.out.print("Vetor threads: [");
        for(i = 0; i < nElementos; i++){
            if(i != 0){ // condicional para formatacao do print
                System.out.print(", " + vetor[i]);
            }
            else{
                System.out.print(vetor[i]);
            }
        }
        System.out.println("]");
    }
}