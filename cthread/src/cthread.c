#include "support.h"
#include "cthread.h"
#include "cdata.h"
#include "ucontext.h"
#include "escalonador.h"

#define __NUMBER_OF_ARGS 1

_Bool __filas_inicializadas = 0;
_Bool __tcb_main_inicializado = 0;

static ucontext_t contexto_main;

int __tid = 1;

int ccreate (void* (*start)(void*), void *arg, int prio) {
	printf("Criando thread\n");
	if (!__filas_inicializadas) {
		inicializaFilas();
		__filas_inicializadas = 1;
	}

	if (!__tcb_main_inicializado) {
		// inicializaMain();
		TCB_t *mainThread = (TCB_t*) malloc(sizeof(TCB_t));
		mainThread->tid = 0;
		mainThread->state = PROCST_CRIACAO; // Vai direto pro apto
		mainThread->prio = 1;
		mainThread->context = contexto_main;
		mainThread->data = NULL;

		insereEmExecutando(mainThread);
		__tcb_main_inicializado = 1;
	}

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
	tcb->state = PROCST_CRIACAO; // Vai direto pro apto
	tcb->prio = prio;
	tcb->context = contexto;
	tcb->data = NULL;
	__tid += 1;

	
	erro = escalonaThread(tcb);

	if (!erro) {
		return tcb->tid;
	}

	printf("Erro ao adcionar thread\n");

	return -1;
}

// --------------------------------------------------------------------------------------------------- //

int csetprio(int tid, int prio) {
	TCB_t *thread;
	TCB_t *em_apto;
	_Bool erro = 1;
	int emApto = -1;
	int sucesso;

	// thread encontrada sempre estará em executando
	thread = buscaThread(tid, &erro, &emApto);

	if (!erro) {
		if (thread->state != PROCST_EXEC) {
			printf("Erro em csetprio: tentativa de modificar a prio de outra thread\n");;
			return -1;
		}

		thread->prio = prio; // nova prioridade definida
		em_apto = retornaApto();
		
		if (em_apto->prio < thread->prio) { // Comparador é <, pois para valores 0 é maior prio e 3 é menor prio
			// retira thread da exec, e coloca a thread em apto
			int removeErro;
			int insereErro;
			removeErro = removeDeExecutando();
			
			if (!removeErro) {
				insereErro = insereEmExecutando(em_apto);
				removeDeApto();
				if (!insereErro) {
					sucesso = insereEmApto(thread);
				}
			}
			
			if (swapcontext(&thread->context, &em_apto->context) == -1) {
				printf("Erro ao trocar os contextos\n");
				return -1;
			}
		}
	} else {
		sucesso = -1;
	}

	return sucesso;
}

// --------------------------------------------------------------------------------------------------- //

int cyield(void) {
	// Remove thread atual de executando
	TCB_t *thread_atual;
	TCB_t *thread_apta;
	_Bool sucesso;

	// Busca a thread que está executando e a retira de execucao
	thread_atual = retornaExecutando();
	removeDeExecutando();

	// Busca a proxima thread apta e a retira da fila de aptos
	thread_apta = retornaApto();
	removeDeApto();
		
	// Insere em apto a thread que foi retirada de execucao
	insereEmApto(thread_atual);
	sucesso = insereEmExecutando(thread_apta);
	if (swapcontext(&thread_atual->context, &thread_apta->context) == -1) {
		printf("Erro ao trocar os contextos em cyield\n");
		sucesso = -1;
	}

	return sucesso;
}

// --------------------------------------------------------------------------------------------------- //

int cjoin(int tid) {
	printf("Iniciando cjoin\n");
	TCB_t *thread_esperada;
	TCB_t *thread_atual;
	TCB_t *thread_apta;
	_Bool erro;
	int emApto;

	thread_atual = retornaExecutando();
	printf("Pegou thread atual: %d\n", thread_atual->tid);


	ucontext_t *contexto_fim_de_thread = (ucontext_t *) malloc(sizeof(ucontext_t));

	if (getcontext(contexto_fim_de_thread) == -1) {
		printf("getcontext para fim de thread falhou\n");
		return -1;
	}

	printf("Busca thread para esperar\n");
	thread_esperada = buscaThread(tid, &erro, &emApto);
	printf("Encontrou thread %d\n", thread_esperada->tid);
	
	makecontext(thread_esperada->context.uc_link, (void (*) (void))sincronizaTermino, __NUMBER_OF_ARGS, thread_atual->tid);	

	// bloqueia thread atual
	removeDeExecutando();
	insereEmBloqueado(thread_atual);

	// coloca a proxima thread apta em execucao
	thread_apta = retornaApto();
	printf("Thread %d vai pra exec\n", thread_apta->tid);
	removeDeApto();
	insereEmExecutando(thread_apta);
	if (swapcontext(&thread_atual->context, &thread_apta->context)) {
		printf("Erro ao trocar os contextos em cjoin\n");
		return -1;
	}

	return 0;
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

static void* func2(void) {
	printf("func2: started ->\n");
	printf("func2: swapcontext(&uctx_func2, &uctx_func1)\n");
	//cyield();
	printf("f2: swapcontext\n");
	printf("func2: returning\n");
}


static void* func1(void) {
	int i;
	int erro;
	printf("func1: started\n");
	printf("func1: swapcontext(&uctx_func1, &uctx_func2)\n");
	//erro = csetprio(1, 2);
	int tid2 = ccreate(&func2, (void*)&i, 0);
	cjoin(tid2);
	printf("f1: swapcontext\n");
	printf("func1: returning\n");
	return;
}

int main () {
	char name[60];

	cidentify(name, 60);
	printf("%s\n", name);

	int i;

	int tid = ccreate(&func1, (void*)&i, 0);
	//int tid2 = ccreate(&func2, (void*)&i, 0);


	// TCB_t *thread;
	// thread = retornaApto();


	// removeDeApto();
	// insereEmExecutando(thread);
	// swapcontext(&contexto_main, &thread->context);

	printf("Main terminou!\n");
	return 0;
}