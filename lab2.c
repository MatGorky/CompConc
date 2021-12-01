/* Disciplina: Computacao Concorrente */
/* Prof.: Silvana Rossetto */
/* DRE: 119043032    Nome: Matheus Moura Gorchinsky*/
/* Módulo 1 - Laboratório: 1 */

#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<time.h> //apenas para ter números mais aleatórios
#include "timer.h"

float *matriz1; 
float *matriz2; 
float *resultado;
float *resultado_conc;
int nthreads; //numero de threads

typedef struct{
   int id; //identificador do elemento que a thread ira processar
   int dim; //dimensao das estruturas de entrada
} tArgs;


//funcao que as threads executarao
void * tarefa(void *arg) {
   tArgs *args = (tArgs*) arg;

   //mesma lógica do algoritmo clássico, mas cada threads tem responsabilidades intercaladas pelas linhas da matriz
   for(int i = args->id;i < args->dim; i+=nthreads){
      for(int j = 0; j < args->dim; j++){
         for(int k = 0; k < args->dim; k++){
            resultado_conc[i * args->dim + j] += matriz1[i * args->dim + k] * matriz2[k * args->dim + j];
         }
      }
   }
   pthread_exit(NULL);
}

//as matrizes são globais
void * mult_seq(int dim){
   /*O algoritmo clássico de multiplicação de matrizes, basta fazer o passo a passo iterativamente
    cada posição da matriz resultante será a soma da multiplicação de cada elemento da linha respectiva da matriz 1 com cada elemento da coluna respectiva da matriz 2
    isto é, resultado i,j = (matriz1 i,1 * matriz2 1,j) + (matriz1 i,2 * matriz2 2,j) + ... + (matriz1 i,n * matriz2 n,j)
    então precisaremos de um loop extra para incrementar a variável deste somatório.*/
   for(int i = 0;i < dim; i++){
      for(int j = 0; j < dim; j++){
         for(int k = 0; k < dim; k++){
            resultado[i * dim + j] += matriz1[i * dim + k] * matriz2[k * dim + j];
         }
      }
   }
}

void * verifica(int dim){
   for(int i = 0;i < dim; i++){
      for(int j = 0; j < dim; j++){
         if(resultado[i * dim + j] != resultado_conc[i * dim + j]){
            printf("Mas os resultados estão incorretos\n");
            return;
         }
      }
   }
   printf("E os resultados estão corretos\n");
}
//fluxo principal
int main(int argc, char* argv[]) {
   int dim; //dimensao da matriz de entrada
   pthread_t *tid; //identificadores das threads no sistema
   tArgs *args; //identificadores locais das threads e dimensao
   double inicio, fim, delta;
   
   srand(time(NULL));
   //leitura e avaliacao dos parametros de entrada
   if(argc<3) {
      printf("Digite: %s <dimensao da matriz> <numero de threads>\n", argv[0]);
      return 1;
   }
   dim = atoi(argv[1]);
   nthreads = atoi(argv[2]);
   if (nthreads > dim) nthreads=dim;
   
   //alocacao de memoria para as estruturas de dados
   matriz1 = (float *) malloc(sizeof(float) * dim * dim);
   if (matriz1 == NULL) {printf("ERRO--malloc\n"); return 2;}
   matriz2 = (float *) malloc(sizeof(float) * dim * dim);
   if (matriz1 == NULL) {printf("ERRO--malloc\n"); return 2;}
   resultado = (float *) malloc(sizeof(float) * dim * dim);
   if (matriz1 == NULL) {printf("ERRO--malloc\n"); return 2;}
   resultado_conc = (float *) malloc(sizeof(float) * dim * dim);
   if (matriz1 == NULL) {printf("ERRO--malloc\n"); return 2;}

   
   //inicializacao das estruturas de dados de entrada e saida
   for(int i=0; i<dim; i++) {
      for(int j=0; j<dim; j++){
         matriz1[i * dim + j] = (float)(rand()%1000)/10;
         matriz2[i * dim + j] = (float)(rand()%1000)/10;
         resultado[i * dim + j] = 0;
         resultado_conc[i * dim + j] = 0;
      }
   }
   
   GET_TIME(inicio);
   mult_seq(dim);
   GET_TIME(fim);
   delta = fim - inicio;
   
   printf("Tempo multiplicacao sequencial:%lf\n", delta);

   
   //por existir um overhead do uso de threads, acho mais justo contar o tempo total do preparo delas
   GET_TIME(inicio);
   //alocacao das estruturas
   tid = (pthread_t*) malloc(sizeof(pthread_t)*nthreads);
   if(tid==NULL) {puts("ERRO--malloc"); return 2;}
   args = (tArgs*) malloc(sizeof(tArgs)*nthreads);
   if(args==NULL) {puts("ERRO--malloc"); return 2;}
   //criacao das threads
   for(int i=0; i<nthreads; i++) {
      (args+i)->id = i;
      (args+i)->dim = dim;
      if(pthread_create(tid+i, NULL, tarefa, (void*) (args+i))){
         puts("ERRO--pthread_create"); return 3;
      }
   } 
   //espera pelo termino da threads
   for(int i=0; i<nthreads; i++) {
      pthread_join(*(tid+i), NULL);
   }
   GET_TIME(fim)   
   delta = fim - inicio;
   printf("Tempo multiplicacao concorrente:%lf\n", delta);
   verifica(dim);
   
   //liberacao da memoria
   
   free(matriz1);
   free(matriz2);
   free(resultado);
   free(resultado_conc);
   free(args);
   free(tid);
   
   return 0;
}
