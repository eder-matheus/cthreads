#include <stdio.h>
#include <string.h>
#include "../include/support.h"
#include "../include/cthread.h"
#include "../include/cdata.h"
#include "ucontext.h"
#include "../src/escalonador.c"


int ccreate (void* (*start)(void*), void *arg, int prio) {
	if (prio < 0 || prio > 2) {
		return -1;
	}

	static ucontext_t contexto;
	char stack[16384];

	if (getcontext(&contexto) == -1) {
		printf("getcontext falhou\n");
	}

	contexto.uc_stack.ss_sp = stack;
	contexto.uc_stack.ss_size = sizeof(stack);
	contexto.uc_link = &contexto;
	//makecontext(&contexto, (start), 0);

	struct TCB_t *tcb;
	
	
	return 0;
}

int csetprio(int tid, int prio) {
	int success;

	success = atualizaPrioridade(tid, prio);
	return success;
}

int cyield(void) {
	return -1;
}

int cjoin(int tid) {
	return -1;
}

int csem_init(csem_t *sem, int count) {
	return -1;
}

int cwait(csem_t *sem) {
	return -1;
}

int csignal(csem_t *sem) {
	return -1;
}

int cidentify (char *name, int size) {
	strncpy (name, "Eder Matheus Rodrigues Monteiro e Guilherme Girotto Sartori", size);
	return 0;
}