#include  <stdio.h>
#include  <stdlib.h>
#include  <math.h>
#include "../include/support.h"
#include "../include/cthread.h"

#define   MAX_SIZE  200
#define   MAX_THR   10

int vetor[MAX_SIZE];
int  inc = 0;
int contThread = 1;
int contCyield = 1;
int contTermino = 1;

void *func(void *arg){
	contThread++;
   while ( inc < MAX_SIZE ) {
       vetor[inc] = (int)arg;
       inc++;
       if ( (inc % 20) == 0 ) {
           contCyield++;
           if(contCyield > 10)
           	contCyield = 1;
           cyield();
       }
       else
           continue;
   }

   contTermino++;
   return (NULL);
}


int main(int argc, char *argv[]) {
    int i, pid[MAX_THR];
    csetprio(0, 0);
  
    for (i = 0; i < MAX_THR; i++) {
        pid[i] = ccreate(func, (void *)('A'+i), 1);
       if ( pid[i] == -1) {
          printf("ERRO: criação de thread!\n");
          exit(-1);
       }
     }

    for (i = 0; i < MAX_THR; i++){
      	 cjoin(pid[i]);
     }

    for (i = 0; i < MAX_SIZE; i++) {    
        if ( (i % 20) == 0 )
           printf("\n");
        printf("%c", (char)vetor[i]);
    }
      
    printf("\nConcluido vetor de letras... com inc em %d\n", inc);
    exit(0);
}