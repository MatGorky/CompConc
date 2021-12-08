/* Disciplina: Computacao Concorrente */
/* Prof.: Silvana Rossetto */
/* DRE: 119043032    Nome: Matheus Moura Gorchinsky*/
/* Módulo 1 - Laboratório: 1 */

#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include "timer.h"

long long int dim; //dimensao do vetor de entrada
long long int nthreads; //numero de threads
double *vetor; //vetor de entrada com dimensao dim
double limInf;
double limSup;

//fluxo das threads
void * tarefa(void * arg) {
   long long int id = (long long int) arg; //identificador da thread
   long long int *valoresEncontradosLocais; //valores encontrados localmente na thread
   valoresEncontradosLocais = (long long int*) malloc(sizeof(long long int));
   if(valoresEncontradosLocais==NULL) {exit(1);}
   *valoresEncontradosLocais = 0;
   long long int tamBloco = dim/nthreads;  //tamanho do bloco de cada thread 
   long long int ini = id * tamBloco; //elemento inicial do bloco da thread
   long long int fim; //elemento final(nao processado) do bloco da thread
   if(id == nthreads-1) fim = dim;
   else fim = ini + tamBloco; //trata o resto se houver
   //procura os valores entre os elementos do bloco da thread
   for(long long int i=ini; i<fim; i++){
      if(vetor[i] > limInf && vetor[i] < limSup){
         *valoresEncontradosLocais += 1; //valores 
      }
   }
   //retorna o resultado da soma local
   pthread_exit((void *) valoresEncontradosLocais); 
}

//fluxo principal
int main(int argc, char *argv[]) {
   long long int encontradosSeq= 0; //soma sequencial
   long long int encontradosConc= 0; //soma concorrente
   double ini, fim; //tomada de tempo
   pthread_t *tid; //identificadores das threads no sistema
   long long int *retorno; //valor de retorno das threads

   //recebe e valida os parametros de entrada (dimensao do vetor, numero de threads)
   if(argc < 3) {
       fprintf(stderr, "Digite: %s <dimensao do vetor> <numero threads>\n", argv[0]);
       return 1; 
   }
   dim = atoll(argv[1]);
   nthreads = atoll(argv[2]);
   //aloca o vetor de entrada
   vetor = (double *) malloc(sizeof(double)*dim);
   if(vetor == NULL) {
      fprintf(stderr, "ERRO--malloc\n");
      return 2;
   }
   //preenche o vetor de entrada
   for(long long int i=0; i<dim; i++)
      vetor[i] = ((double)rand()/(double)(RAND_MAX)) * (double)(rand()%10000); //o valores aleatórios vão estar entre 0 e 10000
  
   printf("Informe respectivamente os limiares inferiores e superiores\n");
   scanf("%lf %lf", &limInf, &limSup);

   
   printf("\n\n");
   //valores encontrados dentro do limiar sequencialmente
   GET_TIME(ini);
   for(long long int i=0; i<dim; i++){
    if(vetor[i] > limInf && vetor[i] < limSup){ 
       encontradosSeq += 1;
    }
   }  
   GET_TIME(fim);
   printf("Tempo sequencial:  %lf\n", fim-ini);

   //valores encontrados dentro do limiar com uso de concorrência
   GET_TIME(ini);
   tid = (pthread_t *) malloc(sizeof(pthread_t) * nthreads);
   if(tid==NULL) {
      fprintf(stderr, "ERRO--malloc\n");
      return 2;
   }
   //criar as threads
   for(long long int i=0; i<nthreads; i++) {
      if(pthread_create(tid+i, NULL, tarefa, (void*) i)){
         fprintf(stderr, "ERRO--pthread_create\n");
         return 3;
      }
   }
   //aguardar o termino das threads
   for(long int i=0; i<nthreads; i++) {
      if(pthread_join(*(tid+i), (void**) &retorno)){
         fprintf(stderr, "ERRO--pthread_create\n");
         return 3;
      }
      //somando o retorno das threads
      encontradosConc += *retorno;
   
   }
   GET_TIME(fim);
   printf("Tempo concorrente:  %lf\n", fim-ini);

   //exibir os resultados
   printf("Valores encontrados seq:  %lld\n", encontradosSeq);
   printf("Valores encontrados conc: %lld\n", encontradosConc);
   if(encontradosSeq == encontradosConc){
      printf("valores corretos");
   }
   else{
      printf("valores errados");
   }
   //libera as areas de memoria alocadas
   free(vetor);
   free(tid);

   return 0;
}
