/* 
 * test_vetor.c: realiza a criação de 10 threads, cada uma delas escreve uma
 * sequencia de 20 letras iguais e passa a vez para outra thread. Repete até
 * preencher um vetor de 250 caracteres.
 */

#include	"../include/support.h"
#include	"../include/cthread.h"
#include	<stdio.h>
#include	<stdlib.h>

#define ALTA 0
#define MEDIA 1
#define BAIXA 2

#define		MAX_SIZE	609
#define		MAX_THR		10

int vetor[MAX_SIZE];
int  inc = 0;

void *func(void *arg){
  printf("Exec func\n");

   while ( inc < MAX_SIZE ) {
       vetor[inc] = (int)arg;
       inc++;
       if ( (inc % 20) == 0 )
           cyield();
       else
           continue;
   }
   printf("Saindo de func\n");
   return NULL;
}


int main(int argc, char *argv[]) {
    int i, pid[MAX_THR];

  
    csetprio(0, MEDIA);

    for (i = 0; i < MAX_THR; i++) {
        pid[i] = ccreate(func, (void *)('A'+i), MEDIA);
        printf("Thread %d criada\n", i+1);
       if ( pid[i] == -1) {
          printf("ERRO: criação de thread!\n");
          exit(-1);
       }
     }

    for (i = 0; i < MAX_THR; i++) 
         cjoin(pid[i]);

    for (i = 0; i < MAX_SIZE; i++) {    
        if ( (i % 20) == 0 )
           printf("\n");
        printf("%c", (char)vetor[i]);
    }
      
    printf("\nConcluido vetor de letras...\n");
    exit(0);
}

