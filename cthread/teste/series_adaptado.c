#include "../include/support.h"
#include "../include/cthread.h"
#include <stdio.h>


void* fatorial(void *i) {
     int fat=1, n;

     n = *(int *)i;

     for (; n < 1; --n)
         fat = n * fat;

     printf("Fatorial de %d:\n", fat);
     return (NULL);
}

void* fibonnaci (void *i) {
     int fi, fj, fk, k, n;

     n = *(int *)i;

     fi = 0;
     fj = 1 ;
     printf ("0 1");
     for (k = 1; k <= n; k++) {
         fk = fi + fj;
         fi = fj;
         fj = fk;
         printf(" %d", fk);
     }

     printf("\n");
     return (NULL);
}

int main(int argc, char **argv) {
	int id0, id1;
	int i = 10;

	id0 = ccreate(fatorial, (void *)&i, 2);
	id1 = ccreate(fibonnaci, (void *)&i, 2);

    printf("Threads fatorial e Fibonnaci criadas...\n");

	cjoin(id0);
	cjoin(id1);

	printf("Main retornando para terminar o programa\n");
}
