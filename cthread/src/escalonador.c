#include "escalonador.h"
#include "support.h"

int inicializaFilas() {
	int retorno = 0;
	retorno += CreateFila2(&__aptos_prio_0);
	retorno += CreateFila2(&__aptos_prio_1);
	retorno += CreateFila2(&__aptos_prio_2);
	retorno += CreateFila2(&__executando);
	retorno += CreateFila2(&__bloqueados);

	return retorno;
}

// ----------------------------------------------------------------------------------- //

int insereEmApto(TCB_t *thread) {
	int prioridade = thread->prio;
	int success;
	printf ("Thread prio: %d", thread->prio);
	if (prioridade == 0) {
		success = AppendFila2(&__aptos_prio_0, thread);
	} else if (prioridade == 1) {
		success = AppendFila2(&__aptos_prio_1, thread);
	} else if (prioridade == 2) {
		success = AppendFila2(&__aptos_prio_2, thread);
	} else{ 
		return -1;
	}

	return success;
}

// ----------------------------------------------------------------------------------- //

int insereEmExecutando(TCB_t *thread) {
	int success;

	success = AppendFila2(&__executando, thread);
	return success;
}

// ----------------------------------------------------------------------------------- //

int insereEmBloqueado(TCB_t *thread) {
	int success;

	success = AppendFila2(&__bloqueados, thread);
	return success;
}

// ----------------------------------------------------------------------------------- //

int removeDeExecutando() {
	int estado_executando;
	int estado_iterador;
	printf("Definindo iterador \n");
	estado_iterador = FirstFila2(&__executando);
	printf("Definiu iterador \n");
	if (estado_iterador == 0) {
		printf("Retirando de exec \n");
		estado_executando = DeleteAtIteratorFila2(&__executando);
		printf("Retirou de exec \n");
		estado_iterador = FirstFila2(&__executando);
	}
	printf("Estado exec: %d \n", estado_executando);

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

int removeDeBloqueado() {
	int estado_bloqueado;
	int estado_iterador;

	estado_iterador = FirstFila2(&__bloqueados);
	if (estado_iterador == 0) {
		estado_bloqueado = DeleteAtIteratorFila2(&__bloqueados);
	}

	if (estado_bloqueado == 0 || estado_bloqueado == DELITER_VAZIA) {
		return 0;
	}

	return -1;
}

// ----------------------------------------------------------------------------------- //

TCB_t* retornaExecutando() {
	TCB_t *thread;
	TCB_t* ptr_invalido = NULL;
	int erro;
	printf("Procurando em execucao\n");
	erro = FirstFila2(&__executando);
	printf("Terminou de procurar\n");
	if (!erro) {
		thread = GetAtIteratorFila2(&__executando);
	}
	else {
		printf("Retornando ptr invalido\n");
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
		thread = GetAtIteratorFila2(&__aptos_prio_0);
		return thread;
	}

	estado_iterador = FirstFila2(&__aptos_prio_2);
	if (estado_iterador == 0) { // fila nao vazia
		thread = GetAtIteratorFila2(&__aptos_prio_0);
		return thread;
	}

	return ptr_invalido;
}

// ----------------------------------------------------------------------------------- //

TCB_t* retornaBloqueado() {
	TCB_t *thread;
	
	FirstFila2(&__bloqueados);
	thread = GetAtIteratorFila2(&__bloqueados);

	return thread;
}

// ----------------------------------------------------------------------------------- //

int alternaEntreAptos(TCB_t *thread, int prioAnterior) {
	int success;
	int remove;

	// Encontra a thread na sua fila atual
	if (prioAnterior == 0) {
		FirstFila2(&__aptos_prio_0);
		_Bool encontrou = 0;
		TCB_t *threadAtual;

		while (!encontrou) {
			threadAtual = GetAtIteratorFila2(&__aptos_prio_0);
			if (thread->tid == threadAtual->tid) {
				encontrou = 1;
			} else {
				NextFila2(&__aptos_prio_0);
			}
		}
	} else if (prioAnterior == 1) {
		FirstFila2(&__aptos_prio_1);
		_Bool encontrou = 0;
		TCB_t *threadAtual;
		
		while (!encontrou) {
			threadAtual = GetAtIteratorFila2(&__aptos_prio_1);
			if (thread->tid == threadAtual->tid) {
				encontrou = 1;
			} else {
				NextFila2(&__aptos_prio_1);
			}
		}
	} else if (prioAnterior == 2) {
		FirstFila2(&__aptos_prio_2);
		_Bool encontrou = 0;
		TCB_t *threadAtual;
		
		while (!encontrou) {
			threadAtual = GetAtIteratorFila2(&__aptos_prio_2);
			if (thread->tid == threadAtual->tid) {
				encontrou = 1;
			} else {
				NextFila2(&__aptos_prio_2);
			}
		}
	} else {
		return -1;
	}
	// Aqui, a thread a ser movida entre as filas já foi encontrada
	// e o iterador da fila aponta pra ela
	if (prioAnterior == 0) {
		remove = DeleteAtIteratorFila2(&__aptos_prio_0);
	} else if (prioAnterior == 1) {
		remove = DeleteAtIteratorFila2(&__aptos_prio_1);
	} else {
		remove = DeleteAtIteratorFila2(&__aptos_prio_2);
	}
	
	// Insere a thread na sua nova fila, de acordo com sua prioridade
	if (remove == 0 || remove == DELITER_VAZIA) {
		success = insereEmApto(thread);
	}

	return success;
}

// ----------------------------------------------------------------------------------- //

_Bool executandoLivre() {
	if (FirstFila2(&__executando) != 0) {
		return 1;
	}

	return 0;
}

// ----------------------------------------------------------------------------------- //

TCB_t* buscaThread(int tid, _Bool *erro, int *emApto) {
	// procura em executando
	int estado_iterador;
	TCB_t *tcb_temp;
	TCB_t *ptr_invalido = NULL;

	*erro = 0;
	*emApto = -1;

	estado_iterador = FirstFila2(&__executando);
	if (estado_iterador == 0) {
		tcb_temp = GetAtIteratorFila2(&__executando);
		if (tcb_temp->tid == tid) {
			return tcb_temp;
		}
	}

	estado_iterador = FirstFila2(&__bloqueados);
	if (estado_iterador == 0) {
		tcb_temp = GetAtIteratorFila2(&__bloqueados);
		if (tcb_temp->tid == tid) {
			return tcb_temp;
		}
	}

	estado_iterador = FirstFila2(&__aptos_prio_0);
	if (estado_iterador == 0) {
		tcb_temp = GetAtIteratorFila2(&__aptos_prio_0);
		if (tcb_temp->tid == tid) {
			*emApto = 0;
			return tcb_temp;
		}
	}

	estado_iterador = FirstFila2(&__aptos_prio_1);
	if (estado_iterador == 0) {
		tcb_temp = GetAtIteratorFila2(&__aptos_prio_1);
		if (tcb_temp->tid == tid) {
			*emApto = 1;
			return tcb_temp;
		}
	}

	estado_iterador = FirstFila2(&__aptos_prio_2);
	if (estado_iterador == 0) {
		tcb_temp = GetAtIteratorFila2(&__aptos_prio_2);
		if (tcb_temp->tid == tid) {
			*emApto = 2;
			return tcb_temp;
		}
	}

	*erro = 1;
	return ptr_invalido;
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