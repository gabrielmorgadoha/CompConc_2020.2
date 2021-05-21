// classe implementando o método leitores e escritores na versao de temperatura com sensores e atuadores
public class SensoresAtuadores{
    private int i, nSensores, nAtuadores, sensorEsperando, tamanhoVetor;
    private Medicoes[] vetor;
   
    // construtor
    public SensoresAtuadores(){
        this.nSensores = 0;
        this.nAtuadores = 0;
        this.sensorEsperando = 0;
        this.vetor = new Medicoes[60];
        this.tamanhoVetor = vetor.length;
        for(i = 0; i < tamanhoVetor; i++){
            vetor[i] = new Medicoes();
        }
    }

    // metodo que retorna o tamanho do vetor (quantidade de elementos)
    public int getTamanhoVetor(){
        return this.tamanhoVetor;
    }

    // metodo que retorna a posicao de um elemento no vetor
    public synchronized Medicoes getPosicaoVetor(int pos) {
        return this.vetor[pos % tamanhoVetor];
    }

    // metodo para escrevever no vetor um valor passado como parametro
    public synchronized void EscreveVetor(Medicoes i, int in){
        this.vetor[in].temperatura = i.temperatura;
        this.vetor[in].idSensor = i.idSensor;
        this.vetor[in].idLeitura = i.idLeitura;
    }

    // entrada para sensores (escritores)
    public synchronized void EntraSensor(int id){
        try{
            this.sensorEsperando++;
            while((this.nAtuadores > 0) || (this.nSensores > 0)){ // se nao houverem atuadores lendo ou sensores escrevendo
                /* System.out.println("Sensor  (" + id + ") bloqueado."); // print para log de execucao */
                wait(); // bloqueia pela condicao logica da aplicacao 
            }
            this.nSensores++; // registra que ha um sensor escrevendo
            this.sensorEsperando--;
            /* System.out.println("Sensor  (" + id + ") escrevendo."); // print para log de execucao */
        } catch(InterruptedException e){}
    }
    
    // saida para sensores (escritores)
    public synchronized void SaiSensor(int id){
        this.nSensores--; // registra que o sensor saiu
        /* System.out.println("Sensor  (" + id + ") saindo."); // print para log de execucao */
        notifyAll(); // libera sensores e atuadores que tenham sido bloqueados
    }

    // entrada para atuadores (leitores)
    public synchronized void EntraAtuador(int id){
        try{   
            while(this.nSensores > 0 || this.sensorEsperando > 0){ // se nao houverem sensores escrevendo ou sensores esperando (prioridade para escrita)
                /* System.out.println("Atuador (" + id + ") bloqueado."); // print para log de execucao */
                wait(); // bloqueia pela condicao logica da aplicacao 
            }
            this.nAtuadores++; // registra que ha mais um atuador lendo
            /* System.out.println("Atuador (" + id + ") lendo."); // print para log de execucao */
        }catch(InterruptedException e){}
    }

    // saida para atuadores (leitores)
    public synchronized void SaiAtuador(int id){
        this.nAtuadores--; // registra que um atuador saiu
        if(this.nAtuadores == 0){
            this.notify(); // libera sensor caso haja algum bloqueado
        }
        /* System.out.println("Atuador (" + id + ") saindo."); // print para log de execucao */
    }
}