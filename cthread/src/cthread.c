#include <stdio.h>
#include <string.h>
#include "../include/support.h"
#include "../include/cthread.h"
#include "../include/cdata.h"
#include "ucontext.h"
#include "../src/escalonador.c"

// static void trampoline(int cb, int arg)
// {
// 	void *(*real_cb)(void *) = (void *(*)(void *)) cb;
// 	void *real_arg = arg;
// 	real_cb(real_arg);
// }

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
	//makecontext(&contexto, trampoline, 2, (int) start, (int) arg);

	struct TCB_t *tcb;
	
	
	return 0;
}

int csetprio(int tid, int prio) {
	TCB_t *thread, *emExecucao;
	_Bool erro = 1;
	_Bool emApto = -1;
	int i;
	int prioAnterior;

	thread = buscaThread(tid, &erro, &emApto);
	prioAnterior = thread->prio;

	if (!erro) {
		thread->prio = prio; // nova prioridade definida
		i = FirstFila2(__executando);
		emExecucao = GetAtIteratorFila2(__executando);
		if (thread->prio > emExecucao->prio) {
			// retira "emExecucao" do exec, e coloca a thread

		} else if(prioAnterior != thread->prio && emApto != -1) {
			// move a thread para a fila de aptos de acordo com a prioridade, caso nao esteja no bloqueado
			if
		}
	} else {
		return -1;
	}

	return 0;
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