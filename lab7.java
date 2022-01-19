/*Matheus Moura Gorchinsky
DRE: 119043032*/

//classe de recurso compartilhado
class Shared {
  static int soma = 0;

  public static synchronized void add(int value) {   //sincronizando o recurso compartilhado, impedidos condições de corrida.
     soma += value;
  }

  public static synchronized int get() { 
      return soma; 
  }
  
}

//clase que implementa runnable, 
class Adder implements Runnable {
   private int tid;
   private int soma = 0;//variável local de soma

   public Adder(int i) { 
      this.tid = i; 
   }
   
   //tarefa executada pela thread
   public void run() {

      for(int i = 0; i < (Main.ARR/Main.N) + 1; i++){ //cada thread será responsável por tamanho do array/número de threads tarefas, esse +1 força a pegarmos o próximo resultado inteiro
      //basicamente funciona como uma função teto caso a divisão não tenha resto 0.
      //mas caso tenha resto 0, a thread com id 0 é a única que passa do if na sua última iteração
        if((tid -1) + i * Main.N < Main.ARR){// cada thread vai cuidar de um valor múltiplo de seu id e claro, se esse múltiplo não for maior que o array
            //balanceamento de carga garantido pois todas a threads irão executar a mesma quantidade de iterações, mas dependendo do resultado da disisão
            //algumas passam pelo if na última iteração e outras não.
            Shared.add(Main.array[(tid -1) + i * Main.N]);
        }
      }
   }
}

//classe do metodo main
class Main {
   static final int N = 10; //número de thredas
   static final int ARR = 1000; //número de casas do array
   static int[] array = new int[ARR];
   public static int resultado = 0;
   public static void main (String[] args) {
      //reserva espaço para um vetor de threads
      Thread[] threads = new Thread[N];
      
      //preencher array
      for(int i = 0; i < ARR;i++){
        array[i] = i+1;
      }

      //transformar runnable em thread.
      for (int i=0; i<threads.length; i++) {
         System.out.println("--Cria a thread " + i);
         threads[i] = new Thread(new Adder(i+1));
      }

      //Inciando as threads
      for (int i=0; i<threads.length; i++) {
         threads[i].start();
      }

      //Esperando término e somando
      for (int i=0; i<threads.length; i++) {
            try { threads[i].join();} 
            catch (InterruptedException e) { return; }
      } 

      System.out.println("Terminou, soma: " + Shared.get()); 
   }
}