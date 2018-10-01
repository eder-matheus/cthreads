#include "support.h"
#include "cthread.h"
#include "cdata.h"
#include "ucontext.h"
#include "escalonador.h"

#define __NUMBER_OF_ARGS 1
int __tid = 1;
// static void trampoline(int cb, int arg)
// {
// 	void *(*real_cb)(void *) = (void *(*)(void *)) cb;
// 	void *real_arg = arg;
// 	real_cb(real_arg);
// }

int ccreate (void* (*start)(void*), void *arg, int prio) {
	int erro;
	ucontext_t *contexto_fim_de_thread = (ucontext_t *) malloc(sizeof(ucontext_t));

	if (prio < 0 || prio > 2) {
		return -1;
	}

	static ucontext_t contexto;

	if (getcontext(&contexto) == -1) {
		printf("getcontext falhou\n");
	}

	if (getcontext(contexto_fim_de_thread) == -1) {
		printf("getcontext para fim de thread falhou\n");
		return -1;
	}

//	printf("Inicializando contexto para fim de thread\n");
	contexto_fim_de_thread->uc_stack.ss_sp = (char*) malloc(STACK_SIZE * sizeof(char));
	contexto_fim_de_thread->uc_stack.ss_size = STACK_SIZE;
	contexto_fim_de_thread->uc_link = NULL;
	makecontext(contexto_fim_de_thread, (void (*) (void))finalizaThread, 0);	

//	printf("Inicializando contexto\n");
	contexto.uc_stack.ss_sp = (char*) malloc(STACK_SIZE * sizeof(char));
	contexto.uc_stack.ss_size = STACK_SIZE;
	contexto.uc_link = contexto_fim_de_thread;
	makecontext(&contexto, (void (*) (void))start, __NUMBER_OF_ARGS, (void *)arg);

//	printf("Inicializando tcb\n");
	TCB_t *tcb = (TCB_t*) malloc(sizeof(TCB_t));
	tcb->tid = __tid;
	tcb->state = PROCST_APTO; // Vai direto pro apto
	tcb->prio = prio;
	tcb->context = contexto;
	tcb->data = NULL;
	__tid += 1;

	printf("Inserindo tcb no escalonador\n");

	
	erro = escalonaThread(tcb);

	if (!erro) {
		return tcb->tid;
	}

	printf("Erro ao adcionar thread\n");

	return -1;
}

// --------------------------------------------------------------------------------------------------- //

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
		FirstFila2(&__executando);
		emExecucao = GetAtIteratorFila2(&__executando);
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

// --------------------------------------------------------------------------------------------------- //

int cyield(void) {
	// Remove thread atual de executando
	TCB_t *threadAtual;
	TCB_t *threadParaExec;
	_Bool sucesso;

	// Busca a thread que está executando e a retira de execucao
	threadAtual = retornaExecutando();
	removeDeExecutando();

	// Busca a proxima thread apta e a retira da fila de aptos
	threadParaExec = retornaApto();
	removeDeApto();
	
	// Insere em apto a thread que foi retirada de execucao
	insereEmApto(threadAtual);
	sucesso = insereEmExecutando(threadParaExec);
	
	if (swapcontext(&threadAtual->context, &threadParaExec->context) == -1){
		printf("Erro ao trocar os contextos em cyield\n");
		sucesso = 0;
	}

	if (!sucesso)
		return -1;

	return 0;
}

// --------------------------------------------------------------------------------------------------- //

int cjoin(int tid) {
	return -1;
}

// --------------------------------------------------------------------------------------------------- //

int csem_init(csem_t *sem, int count) {
	return -1;
}

// --------------------------------------------------------------------------------------------------- //

int cwait(csem_t *sem) {
	return -1;
}

// --------------------------------------------------------------------------------------------------- //

int csignal(csem_t *sem) {
	return -1;
}

// --------------------------------------------------------------------------------------------------- //

int cidentify (char *name, int size) {
	strncpy (name, "Eder Matheus Rodrigues Monteiro e Guilherme Girotto Sartori\0", size);
	return 0;
}

// --------------------------------------------------------------------------------------------------- //

// static void* func1(void) {
// 	printf("func1: started\n");
// 	printf("func1: swapcontext(&uctx_func1, &uctx_func2)\n");
// 	cyield();
// 	printf("f1: swapcontext\n");
// 	printf("func1: returning\n");
// 	return;
// }

// static void* func2(void) {
// 	printf("func2: started ->\n");
// 	printf("func2: swapcontext(&uctx_func2, &uctx_func1)\n");
// 	cyield();
// 	printf("f2: swapcontext\n");
// 	printf("func2: returning\n");
// }

int main () {
	inicializaFilas();
	printf("Terminou de inicializar as filas\n");
	// char name[60];

	// cidentify(name, 60);
	// printf("%s\n", name);

	// int i;

	// int tid = ccreate(&func1, (void*)&i, 0);
	// int tid2 = ccreate(&func2, (void*)&i, 0);

	// cjoin(tid);
	// cjoin(tid2);

	// TCB_t *thread;
	// thread = retornaApto();


	// removeDeApto();
	// insereEmExecutando(thread);
	// swapcontext(&uctx_main, &thread->context);

	// printf("Main terminou!\n");
	return 0;
}