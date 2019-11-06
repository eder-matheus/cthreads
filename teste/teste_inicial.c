#include  <stdio.h>
#include  <stdlib.h>
#include  <math.h>
#include "../include/support.h"
#include "../include/cthread.h"

static void* func5(void) {
	printf("-----func5: started ->\n");
	printf("-----func5: cjoin\n");
	cjoin(1);
	printf("-----f5: finishing\n");
	printf("-----func5: returning\n");
	return (NULL);
}

static void* func4(void) {
	printf("----func4: started ->\n");
	printf("----func4: set prio\n");
	csetprio(4, 0);
	printf("----f4: finishing\n");
	printf("----func4: returning\n");
	return (NULL);
}

static void* func3(void) {
	printf("---func3: started ->\n");
	printf("---func3: set prio\n");
	csetprio(3,2);
	printf("---f3: finishing\n");
	printf("---func3: returning\n");
	return (NULL);
}

static void* func2(void) {
	printf("--func2: started ->\n");
	printf("--func2: cyield\n");
	cyield();
	printf("--f2: finishing\n");
	printf("--func2: returning\n");
	return (NULL);
}


static void* func1(void *i) {
	int erro;
	printf("-func1: started\n");
	printf("Param %d\n", i);
	i++;
	printf("-func1: blocked waiting for f2\n");
	//erro = csetprio(1, 2);
	int tid2 = ccreate(&func2, (void*)&i, 1);
	int tid3 = ccreate(&func3, (void*)&i, 0);
	int tid4 = ccreate(&func4, (void*)&i, 2);
	int tid5 = ccreate(&func5, (void*)&i, 0);
	cjoin(tid2);
	printf("Param %d\n", i);
	printf("-f1: return to f1\n");
	printf("-func1: returning\n");
	return (NULL);
}

int main () {
	char name[60];

	cidentify(name, 60);
	printf("%s\n", name);

	int i = 10;

	printf("Iniciando main\n");
	printf("cyield na main\n");
	cyield();
	printf("Retorna da cyield\n");

	printf("Cria t1\n");
	int tid = ccreate(&func1, (void*)i, 0);
	printf("Thread %d criada\n", tid);
	printf("Retorna para main: bloqueando main após crirar thread 1\n");
	printf("main: cjoin\n");
	cjoin(4);

	printf("Main terminou!\n");
	return 0;
}