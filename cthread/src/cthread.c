#include "support.h"
#include "cthread.h"
#include "cdata.h"
#include "ucontext.h"
#include "escalonador.h"

#define __NUMBER_OF_ARGS 1
static void trampoline(int cb, int arg)
{
	void *(*real_cb)(void *) = (void *(*)(void *)) cb;
	void *real_arg = arg;
	real_cb(real_arg);
}

int ccreate (void* (*start)(void*), void *arg, int prio) {
	TCB_t *threadExec;

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
	makecontext(&contexto, start, __NUMBER_OF_ARGS, arg);

	TCB_t *tcb = malloc(sizeof(TCB_t));
	tcb->tid = Random2();
	tcb->state = PROCST_CRIACAO; // Está sendo criado
	tcb->prio = prio;
	tcb->context = contexto;
	tcb->data = NULL;

	threadExec = retornaExecutando();
	if (threadExec != NULL) {
		if (tcb->prio < threadExec->prio) {
			removeDeExecutando();
			insereEmExecutando(tcb);
			insereEmApto(threadExec);
			if (swapcontext(&threadExec->context, &tcb->context) == -1) {
				printf("Erro ao trocar os contextos\n");
				return -1;
			}
		} else {
			if (insereEmApto(tcb) != 0) {
				printf("Erro ao inserir em apto\n");
				return -1;
			}
		}
	} else {
		printf("Erro ao retornar a thread em execucao!!!\n");
		return -1;
	}
	
	return tcb->tid;
}

int csetprio(int tid, int prio) {
	TCB_t *thread, *emExecucao;
	_Bool erro = 1;
	int emApto = -1;
	int prioAnterior;
	int sucesso;

	thread = buscaThread(tid, &erro, &emApto);
	prioAnterior = thread->prio;

	if (!erro) {
		thread->prio = prio; // nova prioridade definida
		FirstFila2(__executando);
		emExecucao = GetAtIteratorFila2(__executando);
		if (thread->prio < emExecucao->prio) { // Comparador é <, pois para valores 0 é maior prio e 3 é menor prio
			// retira "emExecucao" do exec, e coloca a thread
			int removeErro;
			int insereErro;
			removeErro = removeDeExecutando();
			
			if (!removeErro) {
				insereErro = insereEmExecutando(thread);
				if (!insereErro) {
					sucesso = insereEmApto(thread);
				}
			}
			
			if (swapcontext(&emExecucao->context, &thread->context) == -1) {
				printf("Erro ao trocar os contextos\n");
				return -1;
			}
		} else if(prioAnterior != thread->prio && emApto != -1) {
			// move a thread para a fila de aptos de acordo com a prioridade, caso nao esteja no bloqueado
			sucesso = alternaEntreAptos(thread, prioAnterior);
		}
	} else {
		sucesso = -1;
	}

	return sucesso;
}

int cyield(void) {
	// Remove thread atual de executando
	TCB_t *threadAtual;
	TCB_t *threadParaExec;
	int sucesso;

	threadAtual = retornaExecutando();
	removeDeExecutando();
	insereEmApto(threadAtual);

	threadParaExec = retornaApto();
	sucesso = insereEmExecutando(threadParaExec);

	if (swapcontext(&threadAtual->context, &threadParaExec->context) == -1){
		printf("Erro ao trocar os contextos\n");
		sucesso = 0;
	}

	if (!sucesso)
		return -1;

	return sucesso;
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
	strncpy (name, "Eder Matheus Rodrigues Monteiro e Guilherme Girotto Sartori\0", size);
	return 0;
}

int main () {
	char *name[60];

	cidentify(&name, 60);
	printf("%s\n", name);
}