/* Matheus Moura Gorchinsky
DRE: 119043032
*/

#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h>
#include <semaphore.h>

#define NTHREADS 5

// Variaveis globais
int contador = 0;      //variavel compartilhada entre as threads
sem_t em;     // semaforo para exclusao mutua entre as threads
sem_t condt1, condt5;     //semaforos para sincronizar a ordem de execucao das threads

//funcao executada pela thread 1
void *t1 () {
  sem_wait(&condt1); //espera t2,t3 e t4 terem sido executadas
  printf("Volte sempre!\n");
  pthread_exit(NULL);
}

//funcao executada pela thread 2
void *t2 () {
  sem_wait(&condt5); //espera T5 executar
  printf("Fique a vontade.\n");
  sem_wait(&em); //sessão crítica
  contador ++; //quando t2,t3 e t3 tiverem sido executadas, o contador será igual a 3, e a execução de t1 estará liberada
  if(contador == 3){
    sem_post(&condt1); //permite a execução de T1
  }
  sem_post(&em);
  
  pthread_exit(NULL);
}

//funcao executada pela thread 3
void *t3 () {
  sem_wait(&condt5); //espera T5 executar
  printf("Sente-se por favor.\n");
  sem_wait(&em); //sessão crítica
  contador ++; //quando t2,t3 e t3 tiverem sido executadas, o contador será igual a 3, e a execução de t1 estará liberada
  if(contador == 3){
    sem_post(&condt1); //permite a execução de T1
  }
  sem_post(&em);
  pthread_exit(NULL);
}

void *t4 () {
  sem_wait(&condt5); //espera T5 executar
  printf("Aceita um copo d'\u00E1gua\n");
  sem_wait(&em); //sessão crítica
  contador ++;
  if(contador == 3){ //quando t2,t3 e t3 tiverem sido executadas, o contador será igual a 3, e a execução de t1 estará liberada
    sem_post(&condt1); //permite a execução de T1
  }
  sem_post(&em);
  pthread_exit(NULL);
}

void *t5 () {
  
  printf("Seja bem vindo!\n");
  sem_post(&condt5); //incrementa o semáforo 3 vezes, permitindo que as threads 2,3 e 4 executem
  sem_post(&condt5); 
  sem_post(&condt5); 
}

//funcao principal
int main(int argc, char *argv[]) {
  pthread_t tid[NTHREADS];
  int *id[5], t;

  for (t=0; t<NTHREADS; t++) {
    if ((id[t] = malloc(sizeof(int))) == NULL) {
       pthread_exit(NULL); return 1;
    }
    *id[t] = t+1;
  }

  //inicia os semaforos
  sem_init(&em, 0, 1); //para exclusão mútua, começa com 1, pois o primeiro wait encontrado não irá impedir a execução, apenas um próximo
  sem_init(&condt1, 0, 0); //para condicional, começa com 0, pois o primeiro wait encontrado só será liberado após um post
  sem_init(&condt5, 0, 0);

  //cria as tres threads
  
  if (pthread_create(&tid[4], NULL, t5, NULL)) { printf("--ERRO: pthread_create()\n"); exit(-1); }
  if (pthread_create(&tid[1], NULL, t2, NULL)) { printf("--ERRO: pthread_create()\n"); exit(-1); }
  if (pthread_create(&tid[2], NULL, t3, NULL)) { printf("--ERRO: pthread_create()\n"); exit(-1); }
  if (pthread_create(&tid[3], NULL, t4, NULL)) { printf("--ERRO: pthread_create()\n"); exit(-1); }
  if (pthread_create(&tid[0], NULL, t1, NULL)) { printf("--ERRO: pthread_create()\n"); exit(-1); }

  //--espera todas as threads terminarem
  for (t=0; t<NTHREADS; t++) {
    if (pthread_join(tid[t], NULL)) {
         printf("--ERRO: pthread_join() \n"); exit(-1); 
    } 
    free(id[t]);
  } 
  pthread_exit(NULL);
}
