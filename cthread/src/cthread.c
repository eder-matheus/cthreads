#include "../include/support.h"
#include "../include/cthread.h"
#include "../include/cdata.h"
#include "ucontext.h"
#include "../include/escalonador.h"

int __tid = 1;

int ccreate (void* (*start)(void*), void *arg, int prio) {
	if (inicializaEscalonador() != 0) {
		printf("Erro ao inicializar escalonador\n");
		return -1;
	}

	int erro;
	ucontext_t *contexto_fim_de_thread = (ucontext_t *) malloc(sizeof(ucontext_t));

	if (prio < 0 || prio > 2) {
		printf("Erro em ccreate: prioridade invalida\n");
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

	// Inicializando contexto para fim de thread
	contexto_fim_de_thread->uc_stack.ss_sp = (char*) malloc(STACK_SIZE * sizeof(char));
	contexto_fim_de_thread->uc_stack.ss_size = STACK_SIZE;
	contexto_fim_de_thread->uc_link = NULL;
	makecontext(contexto_fim_de_thread, (void (*) (void))finalizaThread, 0);	

	// Inicializando contexto para thread criada
	contexto.uc_stack.ss_sp = (char*) malloc(STACK_SIZE * sizeof(char));
	contexto.uc_stack.ss_size = STACK_SIZE;
	contexto.uc_link = contexto_fim_de_thread;
	makecontext(&contexto, (void (*) (void))start, __NUMBER_OF_ARGS, (void *)arg);

	// Inicializando tcb para thread criada
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
	if (inicializaEscalonador() != 0) {
		printf("Erro ao inicializar escalonador\n");
		return -1;
	}

	TCB_t *thread;
	TCB_t *em_apto;
	int sucesso;

	// thread encontrada sempre estará em executando
	thread = retornaExecutando();
	
	if (thread != NULL) {
		if (thread->state != PROCST_EXEC) {
			printf("Erro em csetprio: thread encontrada nao esta em exec\n");
			return -1;
		}

		thread->prio = prio; // nova prioridade definida
		em_apto = retornaApto();

		if (em_apto == NULL) {
			return 0;
		}
		
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
	if (inicializaEscalonador() != 0) {
		printf("Erro ao inicializar escalonador\n");
		return -1;
	}

	// Remove thread atual de executando
	TCB_t *thread_atual;
	TCB_t *thread_apta;
	_Bool sucesso;

	// Busca a thread que está executando e a retira de execucao
	thread_atual = retornaExecutando();
	if (thread_atual == NULL) {
		printf("Erro: nao ha thread em execucao\n");
		return -1;
	}
	removeDeExecutando();

	// Insere em apto a thread que foi retirada de execucao
	insereEmApto(thread_atual);

	// Busca a proxima thread apta e a retira da fila de aptos
	thread_apta = retornaApto();
	removeDeApto();

	// Insere em execucao a proxima thread apta
	sucesso = insereEmExecutando(thread_apta);

	// printf(">>> Em cyield: remove thread %d de exec\n", thread_atual->tid);
	// printf(">>> Em cyield: insere thread %d em exec\n", thread_apta->tid);

	if (swapcontext(&thread_atual->context, &thread_apta->context) == -1) {
		printf("Erro ao trocar os contextos em cyield\n");
		sucesso = -1;
	}

	return sucesso;
}

// --------------------------------------------------------------------------------------------------- //

int cjoin(int tid) {
	if (inicializaEscalonador() != 0) {
		printf("Erro ao inicializar escalonador\n");
		return -1;
	}

	TCB_t *thread_esperada;
	TCB_t *thread_atual;
	TCB_t *thread_apta;
	_Bool erro;
	int emApto;

	// Busca, na lista de threads que estao sendo esperadas, a thread passada atualmente
	_Bool thread_ja_eperada = buscaThreadEsperada(tid);

	// Se a thread passada ja esta sendo esperada, termina a exec da cjoin
	if (thread_ja_eperada == 1) {
		printf("Erro em cjoin: thread %d já é esperada por outra thread\n", tid);
		return -1;
	}

	thread_atual = retornaExecutando();
	if (thread_atual == NULL) {
		printf("Erro: nao ha thread em execucao\n");
		return -1;
	}
	// Modifica contexto da thread esperada para sincronizar seu termino
	thread_esperada = buscaThread(tid, &erro, &emApto);
	if (thread_esperada == NULL) {
		printf("Erro: thread de tid %d nao encontrada!\n", tid);
		return -1;
	}

	makecontext(thread_esperada->context.uc_link, (void (*) (void))sincronizaTermino, __NUMBER_OF_ARGS, thread_atual->tid);	
	// bloqueia thread atual

	removeDeExecutando();
	insereEmBloqueado(thread_atual);

	// coloca a proxima thread apta em execucao
	thread_apta = retornaApto();
	if (thread_apta == NULL) {
		printf("Erro: nao ha thread em apto\n");
		return -1;
	}
	removeDeApto();
	insereEmExecutando(thread_apta);

	insereEmThreadsEsperadas(&thread_esperada->tid);

	if (swapcontext(&thread_atual->context, &thread_apta->context) == -1) {
		printf("Erro ao trocar os contextos em cjoin\n");
		return -1;
	}

	return 0;
}

// --------------------------------------------------------------------------------------------------- //

int csem_init(csem_t *sem, int count) {
	printf("Inicializando semáforo\n");
	int ret_log;

	sem->count = count;
	sem->fila = (PFILA2 ) malloc(sizeof(PFILA2));

	if(CreateFila2(sem->fila) == 0 && sem->count == count)
		ret_log = 0;	
	else ret_log = -1;
	
	return ret_log;
}

// --------------------------------------------------------------------------------------------------- //

int cwait(csem_t *sem) {
	TCB_t *thread_atual;
	TCB_t *thread_apta;
	int ret_log = 0;

	sem->count -= 1;
	if(sem->count < 0) {
		printf("recurso indisponível, thread será bloquada!\n");
		thread_atual = retornaExecutando();
		removeDeExecutando();
		AppendFila2(sem->fila, thread_atual);
		thread_apta = retornaApto();
		if(thread_apta == NULL) {
			printf("Nenhuma thread apta para a execução!\n");
			insereEmExecutando(thread_atual);
		}
		removeDeApto();
		insereEmBloqueado(thread_atual);
		insereEmExecutando(thread_apta);
		
		if (swapcontext(&thread_atual->context, &thread_apta->context) == -1) {
			printf("Erro ao trocar os contextos em cwait\n");
		}
		ret_log = -1;
	}


	return ret_log;
}

// --------------------------------------------------------------------------------------------------- //

int csignal(csem_t *sem) {
	TCB_t *thread_desbl;
	int ret_log = 0;

	sem->count += 1;
	if(sem->count <= 0) {
		printf("Semaforo aberto\n");

		thread_desbl = buscaMaiorPrioridade(sem->fila);
		
		if(thread_desbl == NULL) {
			printf("Nenhuma thread esperava pelo csignal\n");
			ret_log = -1;
		}
		
		else {
			DeleteAtIteratorFila2(sem->fila);
			removeDeBloqueado(thread_desbl);
			escalonaThread(thread_desbl);
		}
	}

	return ret_log;
}

// --------------------------------------------------------------------------------------------------- //

int cidentify (char *name, int size) {
	size = 60;
	strncpy (name, "Eder Matheus Rodrigues Monteiro e Guilherme Girotto Sartori\0", size);
	return 0;
}

// --------------------------------------------------------------------------------------------------- //