/* Aluno: Matheus Moura Gorchinsky
   DRE: 119043032 */
 


#include<pthread.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<time.h> //apenas para ter números mais aleatórios



//variaveis para sincronizacao
int nthreads = 10;
int contador = 0; //para a implementação da barreira, contador começa com 0, ao chegar ao valor de nthreads, significa que todas as outras threads já passaram por lá, poderemos então parar de aguardar

pthread_mutex_t mutex;
pthread_cond_t cond;
long int *valores;

void * soma (void * arg) {
  long int id = (long int) arg;
  long int *somatorio; //somatório local para o retorno
  somatorio = (long int*) malloc(sizeof(long int));
  if(somatorio==NULL) {exit(1);}
  *somatorio = 0;
  
  for(int i = 0; i < nthreads; i++){//loop principal
    for(int j = 0; j < nthreads; j++){ //loop do somatório
      *somatorio += valores[j]; //somando todos os valores do array
    }
    //primeira barreira, aguardando todas as threads terminarem de somar
    pthread_mutex_lock(&mutex);
    if(!contador){
      printf("\n\n\nIteração: %d\n\n\n",i);
    }
    contador++;
    if(contador < nthreads){
      printf("thread [%ld] aguardando as somas terminarem\n",id);
      pthread_cond_wait(&cond, & mutex);
    }else{
      printf("\n");
      contador = 0;
      pthread_cond_broadcast(&cond);
    }
    pthread_mutex_unlock(&mutex);

    
    valores[id] = rand()%10; //atribuindo valor aleatório na posição da thread
    
    pthread_mutex_lock(&mutex); //segunda barreira, aguardando todas as threads atualizarem seus respectivos arrays
    contador++;
    
    if(contador < nthreads){
      printf("thread [%ld] aguardando as atualizações terminarem, soma atual:%ld\n",id, *somatorio);
      pthread_cond_wait(&cond, & mutex);
    }else{
      printf("\n");
      contador = 0;
      pthread_cond_broadcast(&cond);
    }
    pthread_mutex_unlock(&mutex);

  }

  pthread_exit((void *) somatorio); 
}



//funcao principal
int main(int argc, char **argv) {
  
  
  //se valor for passado, atualizar nthread
  if(argc >= 2){
    nthreads = atoi(argv[1]);
  }

  long int *retorno;
  pthread_t tid[nthreads];
  int id[nthreads];
  int resultado[nthreads];
  
  valores = (long int*) malloc(sizeof(long int) * nthreads);

  srand(time(NULL));

  //inicializa as variaveis de sincronizacao
  pthread_mutex_init(&mutex, NULL);
  pthread_cond_init(&cond, NULL);


  for(int i = 0; i < nthreads; i++){
    valores[i] = rand()%10; //de 0 a 9
  }

  //cria as threads
  for(long int i=0; i<nthreads; i++) {
    if(pthread_create(&tid[i], NULL, soma, (void *) i)) exit(-1);
  }
  
  int flag_corretude = 1;
  for(long int i=0; i<nthreads; i++) {
    if(pthread_join(*(tid+i), (void**) &retorno)){
      fprintf(stderr, "ERRO--pthread_create\n");
      return 3;
    }
    resultado[i] = *retorno;
    if(i > 0){
      if(resultado[i] != resultado[i-1]){ //se houver um valor inconsistente, o houve erro no resultado
        flag_corretude = 0;
      }
    }
  }

  if(flag_corretude){
    printf("\nTudo certo\n");
  }else{
    printf("\nValores inconsistentes\n");
  }
      
  free(valores);
  return 0;
}
