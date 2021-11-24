/* Disciplina: Computacao Concorrente */
/* Prof.: Silvana Rossetto */
/* DRE: 119043032    Nome: Matheus Moura Gorchinsky*/
/* Módulo 1 - Laboratório: 1 */

#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h>
#define TAM 10000
#define NTHREADS  2 //total de threads a serem criadas

//cria a estrutura de dados para armazenar os argumentos da thread
typedef struct {
   int *array; //ponteiro para apontar para o array
   int startIndex;
} t_Args;

//funcao executada pelas threads
void *ElementSquared (void *arg) {
  t_Args *args = (t_Args *) arg;

  //a ideia é que cada thread trabalhe com uma parta do array, como serão 2 threads, cada uma irá ficar com cada metade            
  for(int i = 0; i < TAM/NTHREADS; i++){ //essa lógica funciona com mais threads e outros tamanhos, contanto que a divisão entre tamanho e nthreads seja inteira
      args->array[i + args->startIndex] *= args->array[i + args->startIndex];
  }
  free(arg); //aqui pode liberar a alocacao feita na main

  pthread_exit(NULL);
}

//funcao principal do programa
int main() {
  pthread_t tid_sistema[NTHREADS]; //identificadores das threads no sistema
  int thread; //variavel auxiliar
  int elementos[TAM];

  for(int i = 0; i < TAM; i++){ //inicializando o array
      elementos[i] = i+1; 
  }

  t_Args *arg; //receberá os argumentos para a thread

  for(thread=0; thread<NTHREADS; thread++) {
    printf("--Aloca e preenche argumentos para thread %d\n", thread);
    arg = malloc(sizeof(t_Args));
    if (arg == NULL) {
      printf("--ERRO: malloc()\n"); exit(-1);
    }
    arg->array = elementos; //o arg->array irá apontar para o elementos
    arg->startIndex = thread * TAM/NTHREADS; //cada thread começará de uma posição diferente, a primeira começará do 0, e a segunda do 5000 nesse caso.
    
    printf("--Cria a thread %d\n", thread);
    if (pthread_create(&tid_sistema[thread], NULL, ElementSquared, (void*) arg)) {
      printf("--ERRO: pthread_create()\n"); exit(-1);
    }
  }
  
  //--espera todas as threads terminarem
  for (thread=0; thread<NTHREADS; thread++) {
    if (pthread_join(tid_sistema[thread], NULL)) {
         printf("--ERRO: pthread_join() \n"); exit(-1); 
    }
  }

  for(int i = 0; i < TAM; i++){
    if(elementos[i] != ((i+1) * (i+1))){
        printf("ERRO, VALOR ERRADO");
        return 0;
    }
    printf("elemento[%d] : %d\n", i, elementos[i]);
  }

  printf("--Thread principal terminou\n");
  pthread_exit(NULL);
}