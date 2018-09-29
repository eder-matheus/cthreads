#include "support.h"
#include "cthread.h"
#include "cdata.h"
#include "ucontext.h"
#include "escalonador.h"

static ucontext_t uctx_main;

#define __NUMBER_OF_ARGS 1
// static void trampoline(int cb, int arg)
// {
// 	void *(*real_cb)(void *) = (void *(*)(void *)) cb;
// 	void *real_arg = arg;
// 	real_cb(real_arg);
// }

int ccreate (void* (*start)(void*), void *arg, int prio) {
	TCB_t *threadExec;

	if (prio < 0 || prio > 2) {
		return -1;
	}

	static ucontext_t contexto;

	if (getcontext(&contexto) == -1) {
		printf("getcontext falhou\n");
	}

	printf("Inicializando contexto\n");
	contexto.uc_stack.ss_sp = (char*) malloc(STACK_SIZE * sizeof(char));
	contexto.uc_stack.ss_size = STACK_SIZE;
	contexto.uc_link = &uctx_main;
	makecontext(&contexto, (void (*) (void))start, __NUMBER_OF_ARGS, arg);

	printf("Inicializando tcb\n");
	TCB_t *tcb = malloc(sizeof(TCB_t));
	tcb->tid = Random2();
	tcb->state = PROCST_CRIACAO; // Está sendo criado
	tcb->prio = prio;
	tcb->context = contexto;
	tcb->data = NULL;

	printf("-----tid da thread: %d", tcb->tid);

	printf("Inserindo tcb no escalonador\n");
	threadExec = retornaExecutando();
	printf("Retorna thread do executando\n");
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
			printf("Inserindo em apto\n");
			if (insereEmApto(tcb) != 0) {
				printf("Erro ao inserir em apto\n");
				return -1;
			}
		}
	} else {
		printf("Inserindo em apto\n");
		if (insereEmApto(tcb) != 0) {
			printf("Erro ao inserir em apto\n");
			return -1;
		}
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

static void func1(void) {
	printf("func1: started\n");
	printf("func1: swapcontext(&uctx_func1, &uctx_func2)\n");
	// if (swapcontext(&uctx_func1, &uctx_func2) == -1)
	printf("swapcontext");
	printf("func1: returning\n");
}

static int func2(int i) {
	printf("func2: started -> %d\n", i);
	printf("func2: swapcontext(&uctx_func2, &uctx_func1)\n");
	// if (swapcontext(&uctx_func2, &uctx_func1) == -1)
	printf("swapcontext");
	printf("func2: returning\n");
	return i;
}

int main () {
	inicializaFilas();
	printf("Terminou de inicializar as filas\n");
	char name[60];

	cidentify(name, 60);
	printf("%s\n", name);

	int i = 40000;
	char func1_stack[16384];
	char func2_stack[16384];

	int tid = ccreate(func1, 0, 0);

	TCB_t *thread;

	thread = retornaApto();
	printf("tid: %d", thread->tid);

	swapcontext(&uctx_main, &thread->context);
}