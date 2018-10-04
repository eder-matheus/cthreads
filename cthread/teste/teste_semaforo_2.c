#include "../include/support.h"
#include "../include/cthread.h"
#include <stdio.h>

static void* func1(csem_t *sem) {
	if(cwait(sem) == 0) {
		printf("func1 está rodando\n");
	}
	else
		printf("Erro na função cwait\n");
	return;
}

static void* func2(csem_t *sem) {
	cwait(sem);
	printf("func2 está rodando\n");
	return;
}

static void* func3(csem_t *sem) {
	cwait(sem);
	printf("func3, a última função, está rodando\n");
	return;
}

static void* func4(csem_t *sem) {
	cwait(sem);
	printf("func4, finalmente, está rodando\n");
	return;
}


int main () {
	csem_t sem;

	printf("Iniciando main\n");
	
	if(csem_init(&sem, 2) < 0)
		printf("Erro na inicializacão de semáforo\n");

	int tid1 = ccreate(&func1, &sem, 0);

	int tid2 = ccreate(&func2, &sem, 0);

	int tid3 = ccreate(&func3, &sem, 1);

	printf("func3 nao executou por falta de recurso e main voltou\n");

	int tid4 = ccreate(&func4, &sem, 0);

	csignal(&sem);

	printf("Main voltou a executar\n");

	csignal(&sem);

	printf("Main terminou!\n");
	

	return 0;
}
