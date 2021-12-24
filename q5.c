/* 
Autor: Matheus Moura Gorchinsky
DRE: 119043032
*/

#include<pthread.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

#define N 1000


//variáveis globais 
int contador = 0;
int printei = 0; 

//variáveis para sincronização
pthread_mutex_t mutex;
pthread_cond_t cond_1, cond_2;


void *Thread1 (void * arg) {
  int *id = (int *) arg; 
  pthread_mutex_lock(&mutex);
  for(int i = 0; i < N; i++){
    contador++; //incrementando o contador(é a primeira linha para não printar o 0)
    //a variável global printei indica se a impressão já ocorreu ou não, serve para impedir de impremir o mesmo número duas vezes
    //e também, como a thread 2 não modifica o contador, printei ajuda a saber se a condição já foi satisfeita ou não
    while(!(contador % 100) && !printei){ //a condição é: Se contador for múltiplo de 100 e a impressão não tiver ocorrido
      pthread_cond_signal(&cond_2); //mandamos sinal para podermos continuar a thread 2
      pthread_cond_wait(&cond_1, &mutex); //esperamos recebermos o sinal da thread 1

    }//usando while por boas práticas

    printei = 0;//printei vira falso, pois ainda não printamos o próximo número
  }
  pthread_mutex_unlock(&mutex);
  pthread_exit(NULL);
}

void *Thread2 (void * arg) {
  int *id = (int *) arg;
  
  pthread_mutex_lock(&mutex);
  for(int i = 0; i < N/100; i++){ //a impressão só precisa acontecer N/100 vezes
    while(contador%100 || printei){ //Se o número for múltiplo ou já tivermos printado
      pthread_cond_wait(&cond_2, &mutex); //esperamos, isso evita que um número que não seja múltiplo de 100 seja impresso e 
      //também impede que um múltiplo de 100 que já foi impresso seja impresso de novo
    }
    printf("Contador : %d\n", contador); //imprimimos
    printei = 1; //printei vira verdadeiro, pois já imprimimos o múltiplo de 100
    pthread_cond_signal(&cond_1); //enviamos sinal para que possamos continuar a thread 1
   }
   pthread_mutex_unlock(&mutex);
   pthread_exit(NULL);
}

//funcao principal
int main(void) {
  //variáveis auxiliares
  int i;
 
  //identificadores das threads
  pthread_t tid[2];
  int *id[2];

  //aloca espaco para os IDs das threads
  for(int i=0; i<2;i++) {
    id[i] = malloc(sizeof(int));
    if(id[i] == NULL) exit(-1);
    *id[i] = i+1;
  }

  
  //inicializa variáveis de sincronização
  pthread_mutex_init(&mutex, NULL);
  pthread_cond_init(&cond_1, NULL);
  pthread_cond_init(&cond_2, NULL);

  //cria as threads 
  
  if(pthread_create(&tid[0], NULL, Thread1, (void *) id[0])) exit(-1);
  
  
  
  if(pthread_create(&tid[1], NULL, Thread2, (void *) id[1])) exit(-1);
  
  for(int i = 0; i<2; i++){
      pthread_join(tid[i], NULL);
  }

  pthread_exit(NULL);
  return 1;
}