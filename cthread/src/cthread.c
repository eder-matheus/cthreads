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
	if (!__filas_inicializadas) {
		inicializaFilas();
		__filas_inicializadas = 1;
	}

	if (!__tcb_main_inicializado) {
		// inicializaMain();
		TCB_t *mainThread = (TCB_t*) malloc(sizeof(TCB_t));
		mainThread->tid = 0;
		mainThread->state = PROCST_CRIACAO; // Vai direto pro apto
		mainThread->prio = 2;
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
	int sucesso;

	// thread encontrada sempre estará em executando
	thread = retornaExecutando();
	
	if (thread != NULL) {
		if (thread->state != PROCST_EXEC) {
			printf("Erro em csetprio: tentativa de modificar a prio de outra thread\n");
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
		printf("Erro interno em csetprio: thread a ter prioridade atualizada nao encontrada\n");
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
	TCB_t *thread_esperada;
	TCB_t *thread_atual;
	TCB_t *thread_apta;
	_Bool erro;
	int emApto;

	_Bool thread_ja_eperada = buscaThreadEsperada(tid);

	if (thread_ja_eperada == 1) {
		printf("Erro em cjoin: thread %d já é esperada por outra thread\n", tid);
		return -1;
	}

	thread_atual = retornaExecutando();

	ucontext_t *contexto_fim_de_thread = (ucontext_t *) malloc(sizeof(ucontext_t));

	if (getcontext(contexto_fim_de_thread) == -1) {
		printf("getcontext para fim de thread falhou\n");
		return -1;
	}

	thread_esperada = buscaThread(tid, &erro, &emApto);
	
	if (erro) {
		printf("Thread de tid %d nao encontrada!\n", tid);
		return -1;
	}

	makecontext(thread_esperada->context.uc_link, (void (*) (void))sincronizaTermino, __NUMBER_OF_ARGS, thread_atual->tid);	

	// bloqueia thread atual
	removeDeExecutando();
	insereEmBloqueado(thread_atual);

	// coloca a proxima thread apta em execucao
	thread_apta = retornaApto();
	removeDeApto();
	insereEmExecutando(thread_apta);

	insereEmThreadsEsperadas(&thread_esperada->tid);

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

static void* func5(void) {
	printf("-----func5: started ->\n");
	printf("-----func5: cjoin\n");
	cjoin(1);
	printf("-----f5: finishing\n");
	printf("-----func5: returning\n");
}

static void* func4(void) {
	printf("----func4: started ->\n");
	printf("----func4: set prio\n");
	csetprio(4, 0);
	printf("----f4: finishing\n");
	printf("----func4: returning\n");
}

static void* func3(void) {
	printf("---func3: started ->\n");
	printf("---func3: set prio\n");
	csetprio(3,2);
	printf("---f3: finishing\n");
	printf("---func3: returning\n");
}

static void* func2(void) {
	printf("--func2: started ->\n");
	printf("--func2: cyield\n");
	cyield();
	printf("--f2: finishing\n");
	printf("--func2: returning\n");
}


static void* func1(void) {
	int i;
	int erro;
	printf("-func1: started\n");
	printf("-func1: blocked waiting for f2\n");
	//erro = csetprio(1, 2);
	int tid2 = ccreate(&func2, (void*)&i, 1);
	int tid3 = ccreate(&func3, (void*)&i, 0);
	int tid4 = ccreate(&func4, (void*)&i, 2);
	int tid5 = ccreate(&func5, (void*)&i, 0);
	cjoin(tid2);
	printf("-f1: return to f1\n");
	printf("-func1: returning\n");
	return;
}

int main () {
	char name[60];

	cidentify(name, 60);
	printf("%s\n", name);

	int i;

	printf("Iniciando main\n");

	int tid = ccreate(&func1, (void*)&i, 0);
	printf("Retorna para main: bloqueando main após crirar thread 1\n");
	printf("main: cjoin\n");
	cjoin(4);

	printf("Main terminou!\n");
	return 0;
}