#include "../include/escalonador.h"
#include "../include/support.h"

_Bool __filas_inicializadas = 0;
_Bool __tcb_main_inicializado = 0;
TCB_t *tcb_main = NULL;

int inicializaEscalonador() {
	int sucesso = 0;

	if (__filas_inicializadas && __tcb_main_inicializado)
		return sucesso;

	sucesso += inicializaFilas();
	sucesso += inicializaMain();

	return sucesso;
}

// ----------------------------------------------------------------------------------- //

int inicializaFilas() {
	if (__filas_inicializadas == 1) {
		return 0;
	}

	int retorno = 0;
	retorno += CreateFila2(&__aptos_prio_0);
	retorno += CreateFila2(&__aptos_prio_1);
	retorno += CreateFila2(&__aptos_prio_2);
	retorno += CreateFila2(&__executando);
	retorno += CreateFila2(&__bloqueados);
	retorno += CreateFila2(&__threads_esperadas);
	__filas_inicializadas = 1;

	return retorno;
}

// ----------------------------------------------------------------------------------- //

int inicializaMain() {
	if (__tcb_main_inicializado == 1) {
		return 0;
	}

	int sucesso = 0;
	tcb_main = (TCB_t*) malloc(sizeof(TCB_t));

	if (getcontext(&(tcb_main->context)) == 0) {
		tcb_main->tid = 0;
		tcb_main->state = PROCST_CRIACAO; // Vai direto pro apto
		tcb_main->prio = 2;
		tcb_main->context.uc_stack.ss_sp = (char*) malloc(STACK_SIZE * sizeof(char));
		tcb_main->context.uc_stack.ss_size = STACK_SIZE;
		tcb_main->context.uc_link = NULL;
		tcb_main->data = NULL;

		sucesso = insereEmExecutando(tcb_main);
		__tcb_main_inicializado = 1;
	} else {
		printf("getcontext error\n");
	}

	return sucesso;
}

// ----------------------------------------------------------------------------------- //

int insereEmApto(TCB_t *thread) {
	int prioridade = thread->prio;
	int sucesso;

	if (prioridade == 0) {
		sucesso = AppendFila2(&__aptos_prio_0, thread);
	} else if (prioridade == 1) {
		sucesso = AppendFila2(&__aptos_prio_1, thread);
	} else if (prioridade == 2) {
		sucesso = AppendFila2(&__aptos_prio_2, thread);
	} else{ 
		printf("Erro: prioridade da thread invalida\n");
		return -1;
	}

	thread->state = PROCST_APTO;

	return sucesso;
}

// ----------------------------------------------------------------------------------- //

int insereEmExecutando(TCB_t *thread) {
	int sucesso;

	sucesso = AppendFila2(&__executando, thread);
	thread->state = PROCST_EXEC;

	return sucesso;
}

// ----------------------------------------------------------------------------------- //

int insereEmBloqueado(TCB_t *thread) {
	int sucesso;

	sucesso = AppendFila2(&__bloqueados, thread);
	thread->state = PROCST_BLOQ;

	return sucesso;
}

// ----------------------------------------------------------------------------------- //

int insereEmThreadsEsperadas(int *tid) {
	int sucesso;

	sucesso = AppendFila2(&__threads_esperadas, tid);

	return sucesso;
}

// ----------------------------------------------------------------------------------- //

int removeDeExecutando() {
	int estado_executando;
	int estado_iterador;
	estado_iterador = FirstFila2(&__executando);
	if (estado_iterador == 0) {
		estado_executando = DeleteAtIteratorFila2(&__executando);
		estado_iterador = FirstFila2(&__executando);
	}

	if (estado_executando != 0 && estado_iterador == 0) {
		printf ("Erro ao retirar de executando: fila nao esta vazia\n");
		return -1;
	}

	return 0;
}

// ----------------------------------------------------------------------------------- //

int removeDeApto() {
	int estado_apto;
	int estado_iterador;

	estado_iterador = FirstFila2(&__aptos_prio_0);
	if (estado_iterador == 0) { // fila nao vazia
		estado_apto = DeleteAtIteratorFila2(&__aptos_prio_0);
		if (estado_apto == 0 || estado_apto == DELITER_VAZIA) {
			return 0;
		}
		else {
			return -1;
		}
	}

	estado_iterador = FirstFila2(&__aptos_prio_1);
	if (estado_iterador == 0) { // fila nao vazia
		estado_apto = DeleteAtIteratorFila2(&__aptos_prio_1);
		if (estado_apto == 0 || estado_apto == DELITER_VAZIA) {
			return 0;
		}
		else {
			return -1;
		}
	}

	estado_iterador = FirstFila2(&__aptos_prio_2);
	if (estado_iterador == 0) { // fila nao vazia
		estado_apto = DeleteAtIteratorFila2(&__aptos_prio_2);
		if (estado_apto == 0 || estado_apto == DELITER_VAZIA) {
			return 0;
		}
		else {
			return -1;
		}
	}

	return -1;
}

// ----------------------------------------------------------------------------------- //

int removeDeBloqueado(TCB_t *thread) {
	int estado_remocao;
	int estado_iterador;
	TCB_t *thread_temp;

	estado_iterador = FirstFila2(&__bloqueados);
	while (estado_iterador == 0) {
		thread_temp = GetAtIteratorFila2(&__bloqueados);
		if (thread->tid == thread_temp->tid) {
			estado_remocao = DeleteAtIteratorFila2(&__bloqueados);
			if (estado_remocao == 0)
				return 0;
		}
		estado_iterador = NextFila2(&__bloqueados);
	}

	return -1;
}

// ----------------------------------------------------------------------------------- //

int removeDeThreadsEsperadas(int tid) {
	int estado_iterador;
	int *tid_temp;

	estado_iterador = FirstFila2(&__threads_esperadas);
	while (estado_iterador == 0) {
		tid_temp = GetAtIteratorFila2(&__threads_esperadas);
		if (tid == *tid_temp) {
			if (DeleteAtIteratorFila2(&__threads_esperadas) == 0) {
				return 0;
			}
		}
		estado_iterador = NextFila2(&__threads_esperadas);
	}

	return -1;
}

// ----------------------------------------------------------------------------------- //

TCB_t* retornaExecutando() {
	TCB_t *thread;
	TCB_t* ptr_invalido = NULL;
	int erro;
	erro = FirstFila2(&__executando);
	if (!erro) {
		thread = GetAtIteratorFila2(&__executando);
	}
	else {
		//printf("Retornando ptr invalido\n");
		return ptr_invalido;
	}
	return thread;
}

// ----------------------------------------------------------------------------------- //

TCB_t* retornaApto() {
	TCB_t *thread;
	TCB_t *ptr_invalido = NULL;
	int estado_iterador;

	estado_iterador = FirstFila2(&__aptos_prio_0);
	if (estado_iterador == 0) { // fila nao vazia
		thread = GetAtIteratorFila2(&__aptos_prio_0);
		return thread;
	}

	estado_iterador = FirstFila2(&__aptos_prio_1);
	if (estado_iterador == 0) { // fila nao vazia
		thread = GetAtIteratorFila2(&__aptos_prio_1);
		return thread;
	}

	estado_iterador = FirstFila2(&__aptos_prio_2);
	if (estado_iterador == 0) { // fila nao vazia
		thread = GetAtIteratorFila2(&__aptos_prio_2);
		return thread;
	}

	return ptr_invalido;
}

// ----------------------------------------------------------------------------------- //

TCB_t* retornaBloqueado(int tid) {
	TCB_t *thread = NULL;
	int estado_iterador;

	estado_iterador = FirstFila2(&__bloqueados);
	while (estado_iterador == 0) {
		thread = GetAtIteratorFila2(&__bloqueados);
		if (thread->tid == tid) {
			return thread;
		} else {
			thread = NULL;
		}
		estado_iterador = NextFila2(&__bloqueados);
	}

	return thread;
}

// ----------------------------------------------------------------------------------- //

_Bool executandoLivre() {
	if (FirstFila2(&__executando) != 0) {
		return 1;
	}

	return 0;
}

// ----------------------------------------------------------------------------------- //

TCB_t* buscaThread(int tid, _Bool *erro, int *fila) {
	// procura em executando
	int estado_iterador;
	TCB_t *tcb_temp;
	TCB_t *ptr_invalido = NULL;

	*erro = 0;
	*fila = -1;

	estado_iterador = FirstFila2(&__executando);
	if (estado_iterador == 0) {
		tcb_temp = GetAtIteratorFila2(&__executando);
		if (tcb_temp->tid == tid) {
			*fila = 4;
			return tcb_temp;
		}
	}

	// procura thread em bloqueados
	estado_iterador = FirstFila2(&__bloqueados);
	while (estado_iterador == 0) {
		tcb_temp = GetAtIteratorFila2(&__bloqueados);
		if (tcb_temp->tid == tid) {
			*fila = 3;
			return tcb_temp;
		}
		estado_iterador = NextFila2(&__bloqueados);
	}

	// procura thread em aptos 0
	estado_iterador = FirstFila2(&__aptos_prio_0);
	while (estado_iterador == 0) {
		tcb_temp = GetAtIteratorFila2(&__aptos_prio_0);
		if (tcb_temp->tid == tid) {
			*fila = 0;
			return tcb_temp;
		}
		estado_iterador = NextFila2(&__aptos_prio_0);
	}

	// procura thread em aptos 1
	estado_iterador = FirstFila2(&__aptos_prio_1);
	while (estado_iterador == 0) {
		tcb_temp = GetAtIteratorFila2(&__aptos_prio_1);
		if (tcb_temp->tid == tid) {
			*fila = 0;
			return tcb_temp;
		}
		estado_iterador = NextFila2(&__aptos_prio_1);
	}

	// procura thread em aptos 2
	estado_iterador = FirstFila2(&__aptos_prio_2);
	while (estado_iterador == 0) {
		tcb_temp = GetAtIteratorFila2(&__aptos_prio_2);
		if (tcb_temp->tid == tid) {
			*fila = 0;
			return tcb_temp;
		}
		estado_iterador = NextFila2(&__aptos_prio_2);
	}

	*erro = 1;
	return ptr_invalido;
}

// ----------------------------------------------------------------------------------- //

_Bool buscaThreadEsperada(int tid) {
	int estado_iterador;
	int *tid_temp;
	estado_iterador = FirstFila2(&__threads_esperadas);

	while (estado_iterador == 0) {
		tid_temp = GetAtIteratorFila2(&__threads_esperadas);
		if (*tid_temp == tid) {
			return 1;
		}
		estado_iterador = NextFila2(&__threads_esperadas);
	}

	return 0;
}

// ----------------------------------------------------------------------------------- //

int atualizaPrioridade(int tid, int nova_prio) {
	int estado_iterador;
	TCB_t *tcb_temp;
	estado_iterador = FirstFila2(&__executando);
	if (estado_iterador == 0) {
		tcb_temp = GetAtIteratorFila2(&__executando);
		if (tcb_temp->tid == tid) {
			tcb_temp->prio = nova_prio;
			return 0;
		}
	}

	estado_iterador = FirstFila2(&__bloqueados);
	if (estado_iterador == 0) {
		tcb_temp = GetAtIteratorFila2(&__bloqueados);
		if (tcb_temp->tid == tid) {
			tcb_temp->prio = nova_prio;
			return 0;
		}
	}

	estado_iterador = FirstFila2(&__aptos_prio_0);
	if (estado_iterador == 0) {
		tcb_temp = GetAtIteratorFila2(&__aptos_prio_0);
		if (tcb_temp->tid == tid) {
			tcb_temp->prio = nova_prio;
			return 0;
		}
	}

	estado_iterador = FirstFila2(&__aptos_prio_1);
	if (estado_iterador == 0) {
		tcb_temp = GetAtIteratorFila2(&__aptos_prio_1);
		if (tcb_temp->tid == tid) {
			tcb_temp->prio = nova_prio;
			return 0;
		}
	}

	estado_iterador = FirstFila2(&__aptos_prio_2);
	if (estado_iterador == 0) {
		tcb_temp = GetAtIteratorFila2(&__aptos_prio_2);
		if (tcb_temp->tid == tid) {
			tcb_temp->prio = nova_prio;
			return 0;
		}
	}

	return -1;
}

// --------------------------------------------------------------------------------------------------- //

int escalonaThread(TCB_t *thread) {
	TCB_t *threadExec;

	threadExec = retornaExecutando();

	if (threadExec != NULL) { // existe thread em execucao
		// printf("Inserindo em exec");
		if (threadExec->prio < thread->prio) { // se a thread em execucao tiver prioridade menor do que a criada
			printf("Retirando thread de exec. Prio exec: %d; Prio newTh: %d\n", threadExec->prio, thread->prio);
			removeDeExecutando();
			insereEmExecutando(thread); // insere a nova thread em execucao
			insereEmApto(threadExec); // insere a antiga no apto
			if (swapcontext(&threadExec->context, &thread->context) == -1) { // troca o contexto para a thread nova
				printf("Erro ao trocar os contextos\n");
				return -1;
			}
			return 0;
		} else {
			if (insereEmApto(thread) != 0) {
				printf("Erro ao inserir em apto\n");
				return -1;
			}
		}
	} else {
		printf("Erro: nao ha thread em exec\n");
		return -1;		
	}

	return 0;
}

// --------------------------------------------------------------------------------------------------- //

int finalizaThread() {
	TCB_t *threadFinalizada;

	threadFinalizada = retornaExecutando();
	if (threadFinalizada == NULL) {
		printf("Erro em finalizaThread: nao encontrou thread em executando\n");
		return -1;
	}
	// Liberação das estruturas inicializadas para a criação da thread
	if (removeDeExecutando() != 0) {
		printf("Erro ao retirar thread finalizada de executando\n");
		return -1;
	}

	if (threadFinalizada->context.uc_stack.ss_sp != NULL)
		free(threadFinalizada->context.uc_stack.ss_sp);
	if (threadFinalizada->context.uc_link != NULL)
		free(threadFinalizada->context.uc_link);
	if (threadFinalizada != NULL)
		free(threadFinalizada);

	proximaThread();
	return 0;
}

// --------------------------------------------------------------------------------------------------- //

void proximaThread() {
	TCB_t *proxima_thread;
	int sucesso;

	proxima_thread = retornaApto();
	
	if (proxima_thread == NULL) {
		printf("Erro: nao ha mais threads em apto\n");
		return;
	}

	// remove a proxima thread a ser executada de apto
	removeDeApto();
	
	// e a insere em execucao
	sucesso = insereEmExecutando(proxima_thread);

	// executa a proxima thread "setando" seu contexto
	if (sucesso == 0)
		setcontext(&proxima_thread->context);
}

// --------------------------------------------------------------------------------------------------- //

int sincronizaTermino(int tid) {
	TCB_t *thread_exec;
	TCB_t *thread_bloq;
	int sucesso;

	// remove thread do bloqueado
	thread_bloq = retornaBloqueado(tid);
	if (thread_bloq == NULL) {
		printf("Erro: thread %d nao encontrada\n", tid);
		return -1;
	}
	removeDeBloqueado(thread_bloq);

	thread_exec = retornaExecutando();
	if (thread_exec == NULL) {
		printf("Erro: nao ha thread em execucao\n");
	}
	removeDeThreadsEsperadas(thread_exec->tid);
	
	// insere em apto
	insereEmApto(thread_bloq);

	sucesso = finalizaThread();
	if (sucesso == 0)
		proximaThread();
	
	return 0;
}

// --------------------------------------------------------------------------------------------------- //

TCB_t* buscaMaiorPrioridade(PFILA2 fila) {
	int i;
	int it_priori;
	int maior_prio = 3;
	int it_counter = 0;
	int estado_iterador;
	TCB_t *thread_desbl;

	estado_iterador = FirstFila2(fila);
		while (estado_iterador == 0) {
			thread_desbl = GetAtIteratorFila2(fila);
			if (thread_desbl->prio < maior_prio) {
				maior_prio = thread_desbl->prio;
				it_priori = it_counter;
			}
			estado_iterador = NextFila2(fila);
			it_counter++;
		}

		FirstFila2(fila);
		for(i = 0; i < it_priori; i++) {
			NextFila2(fila);
		}

		thread_desbl = GetAtIteratorFila2(fila);

		return thread_desbl;
}

// --------------------------------------------------------------------------------------------------- //