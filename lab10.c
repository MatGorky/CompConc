/* 
Matheus Moura Gorchinsky
119043032
*/

#include<pthread.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include <semaphore.h>


#define TAM 5 //tamanho do buffer
//quantidades padrões, mas podem ser modificados na entrada do programa
int p = 2; //qtde de threads produtoras
int c = 4; //qtde de threads consumidoras

sem_t emp;     // semáforo para exclusao mutua entre as threads produtoras
sem_t emc;     // semáforo para exclusao mutua entre as threads consumidoras
sem_t condc ;     //semáforo para sincronizar produtoras com consumidoras, associado a condição das consumidoras já terem esvaziado o buffer
sem_t condp ;   //semáforo para sincronizar produtoras com consumidoras, associado a condição das produtoras terem preenchido o buffer

sem_t slog; //esse semáforo existe apenas para garantir a atomicidade dos printfs, para a geração do log
//não afetando a execução do programa(além de aumentar o overhead)

//variaveis do problema
int Buffer[TAM]; //espaco de dados compartilhados
int count=0, in=0, out=0; //variaveis de estado

//inicializa o buffer
void IniciaBuffer(int n) {
  int i;
  for(i=0; i<n; i++)
    Buffer[i] = 0;
}

//imprime o buffer
void ImprimeBuffer(int n) {
  int i;
  sem_wait(&slog);
  printf("pc.verificaBuffer(\"");
  for(i=0; i<n; i++){
    printf("%d ", Buffer[i]);
  }  
  printf("\")\n");
  sem_post(&slog);
}

//Preenche o buffer ou bloqueia a thread caso o Buffer não esteja vazio
void Insere (int item, int id) {
   
   sem_wait(&emp); //exclusão mútua entre produtoras apenas, não queremos que duas ou mais produtoras passem por aqui concorrentemente
   
   sem_wait(&slog);
   printf("pc.produtorQuerInserir(%d)\n", id);
   sem_post(&slog);
   
   if(count > 0) { //se o buffer não estiver vazio
     
     sem_wait(&slog);
     printf("pc.produtorBloqueou(%d)\n", id);
     sem_post(&slog);
     
     sem_post(&condp); //libera as consumidoras
     sem_wait(&condc); //espera pelas consumidoras consumirem o array, devido a semáforos funcionarem por incrementos e decrementos, não há problema de corrida aqui
     //pois mesmo que que o valor de contador seja alterado após a entrada no if, um post da consumidora irá liberar este wait
    
     sem_wait(&slog);
     printf("pc.produtorDesbloqueou(%d)\n", id);
     sem_post(&slog);
   
   }
   
   for(int i = 0; i< TAM; i++){ //preenchendo o buffer
    Buffer[in] = item;
    in = (in + 1)%TAM;
   }
   //após preencher atualizamos o contador(neste caso, isso é melhor que incrementar o contador durante o preenchimento, pois evita que consumidoras sejam executadas
   count = TAM;

   sem_wait(&slog);
   printf("pc.produtorInseriu(%d)\n", id);
   sem_post(&slog);
   
   ImprimeBuffer(TAM);
   sem_post(&emp);  //exclusão entre as produtoras acaba também
   //sem_post(&condp); //libera as consumidoras
}

//retira um elemento no Buffer ou bloqueia a thread caso o Buffer esteja vazio
int Retira (int id) {
   int item;
   sem_wait(&emc);//exclusão mútua entre consumidoras apenas, não queremos que duas ou mais consumidoras passem por aqui concorrentemente
   
   sem_wait(&slog);
   printf("pc.consumidorQuerConsumir(%d)\n", id);
   sem_post(&slog);

   if(count == 0) { //buffer já foi esvaziado

     sem_wait(&slog);
     printf("pc.consumidorBloqueou(%d)\n", id);
     sem_post(&slog);

     sem_post(&condc); //então podemos liberar a produtora
     sem_wait(&condp); //esperamos a produtora preencher o buffer
     
     sem_wait(&slog);
     printf("pc.consumidorDesbloqueou(%d)\n", id);
     sem_post(&slog);

   }
  
   item = Buffer[out];
   Buffer[out] = 0;
   out = (out + 1)%TAM;
   count--;
   
   sem_wait(&slog);
   printf("pc.consumidorConsumiu(%d,%d)\n", id, item);
   sem_post(&slog);

   ImprimeBuffer(TAM);
   sem_post(&emc); //sessão crítica das consumidoras acaba aqui
   
   return item;
}

//thread produtora
void * produtor(void * arg) {
  int *id = (int *) arg;
  sem_wait(&slog);
  printf("pc.produtorCriado(%d)\n", *id);
  sem_post(&slog);
  while(1) {
    //produzindo o item
    Insere(*id, *id);
    sleep(1);
  } 
  free(arg);
  pthread_exit(NULL);
}

//thread consumidora
void * consumidor(void * arg) {
  int *id = (int *) arg;
  int item;
  sem_wait(&slog);
  printf("pc.consumidorCriado(%d)\n", *id);
  sem_post(&slog);
  while(1) {
    item = Retira(*id);
    sleep(1); //faz o processamento do item 
  } 
  free(arg);
  pthread_exit(NULL);
}


//funcao principal
int main(int argc, char *argv[]) {
  //variaveis auxiliares
  int i;
  
  if(argc < 3){
    printf("#Argumentos insuficientes, utilizando quantidades padrões");
  }
  else{
    c = atoi(argv[1]);
    p = atoi(argv[2]);
    
  }

  //identificadores das threads
  pthread_t tid[p+c];
  int *id[p+c];

  //aloca espaco para os IDs das threads
  for(i=0; i<p+c;i++) {
    id[i] = malloc(sizeof(int));
    if(id[i] == NULL) exit(-1);
    *id[i] = i+1;
  }
  //inicia os semaforos
  sem_init(&emp, 0, 1); //para exclusão mútua, começa com 1, pois o primeiro wait encontrado não irá impedir a execução, apenas um próximo
  sem_init(&condc, 0, 0); //para condicional, começa com 0, pois o primeiro wait encontrado só será liberado após um post
  sem_init(&emc, 0, 1); //para exclusão mútua, começa com 1, pois o primeiro wait encontrado não irá impedir a execução, apenas um próximo
  sem_init(&condp, 0, 0); //para condicional, começa com 0, pois o primeiro wait encontrado só será liberado após um post
  sem_init(&slog, 0, 1); //para o log
  
  //inicializa o Buffer
  IniciaBuffer(TAM);  


  //cria as threads produtoras
  for(i=0; i<p; i++) {
    if(pthread_create(&tid[i], NULL, produtor, (void *) id[i])) exit(-1);
  } 
  
  //cria as threads consumidoras
  for(i=0; i<c; i++) {
    if(pthread_create(&tid[i+p], NULL, consumidor, (void *) id[i+p])) exit(-1);
  } 

  pthread_exit(NULL);
  return 1;
}
