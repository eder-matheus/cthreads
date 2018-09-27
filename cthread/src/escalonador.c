#include "../include/escalonador.h"
#include "../include/support.h"

int inicializaFilas() {
	int retorno = 0;
	retorno += CreateFila2(__aptos_prio_0);
	retorno += CreateFila2(__aptos_prio_1);
	retorno += CreateFila2(__aptos_prio_2);
	retorno += CreateFila2(__executando);
	retorno += CreateFila2(__bloqueados);

	return retorno;
}

// ----------------------------------------------------------------------------------- //

int insereEmApto(TCB_t *thread) {
	int prioridade = thread->prio;
	int success;

	if (prioridade == 0) {
		success = AppendFila2(__aptos_prio_0, thread);
	} else if (prioridade == 1) {
		success = AppendFila2(__aptos_prio_1, thread);
	} else if (prioridade == 2) {
		success = AppendFila2(__aptos_prio_2, thread);
	} else{ 
		return -1;
	}

	return 0;
}

// ----------------------------------------------------------------------------------- //

int insereEmExecutando(TCB_t *thread) {
	int success;

	success = AppendFila2(__executando, thread);
	return success;
}

// ----------------------------------------------------------------------------------- //

int insereEmBloqueado(TCB_t *thread) {
	int success;

	success = AppendFila2(__bloqueados, thread);
	return success;
}

// ----------------------------------------------------------------------------------- //

int removeDeExecutando() {
	int estado_executando;
	int estado_iterador;

	estado_iterador = FirstFila2(__executando);
	if (estado_iterador == 0) {
		estado_executando = DeleteAtIteratorFila2(__executando);
	}

	if (estado_executando != DELITER_VAZIA) {
		printf ("Erro ao retirar de executando: fila nao esta vazia\n");
	}

	return 0;
}

// ----------------------------------------------------------------------------------- //

int removeDeApto() {
	int estado_apto;
	int estado_iterador;

	estado_iterador = FirstFila2(__aptos_prio_0);
	if (estado_iterador == 0) { // fila nao vazia
		estado_apto = DeleteAtIteratorFila2(__aptos_prio_0);
		if (estado_apto == 0 || estado_apto == DELITER_VAZIA) {
			return 0;
		}
		else {
			return -1;
		}
	}

	estado_iterador = FirstFila2(__aptos_prio_1);
	if (estado_iterador == 0) { // fila nao vazia
		estado_apto = DeleteAtIteratorFila2(__aptos_prio_1);
		if (estado_apto == 0 || estado_apto == DELITER_VAZIA) {
			return 0;
		}
		else {
			return -1;
		}
	}

	estado_iterador = FirstFila2(__aptos_prio_2);
	if (estado_iterador == 0) { // fila nao vazia
		estado_apto = DeleteAtIteratorFila2(__aptos_prio_2);
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

	estado_iterador = FirstFila2(__bloqueados);
	if (estado_iterador == 0) {
		estado_bloqueado = DeleteAtIteratorFila2(__bloqueados);
	}

	if (estado_bloqueado == 0 || estado_bloqueado == DELITER_VAZIA) {
		return 0;
	}

	return -1;
}

// ----------------------------------------------------------------------------------- //

int alternaEntreAptos(TCB_t *thread, int prioAntiga) {
	if (prioAntiga == 0) {
		int i = FirstFila2(__aptos_prio_0);
		_Bool encontrou = 0;
		TCB_t *threadAtual;
		while (!encontrou) {
			threadAtual = GetAtIteratorFila2(__aptos_prio_0);
			if (thread->tid == threadAtual->tid) {
				encontrou = 1;
			} else {
				i = NextFila2(__aptos_prio_0);
			}
		}
	} else if (prioAntiga == 1) {
		_Bool encontrou = 0;
		while (!encontrou) {
			
		}
	} else if (prioAntiga == 2) {
		_Bool encontrou = 0;
		while (!encontrou) {
			
		}
	} else {
		return -1;
	}

}

// ----------------------------------------------------------------------------------- //

_Bool executandoLivre() {
	if (FirstFila2(__executando) != 0) {
		return 1;
	}

	return 0;
}

// ----------------------------------------------------------------------------------- //

TCB_t* buscaThread(int tid, _Bool *erro, int *emApto) {
	// procura em executando
	int estado_iterador;
	TCB_t *tcb_temp;

	*erro = 0;
	*emApto = -1;

	estado_iterador = FirstFila2(__executando);
	if (estado_iterador == 0) {
		tcb_temp = GetAtIteratorFila2(__executando);
		if (tcb_temp->tid == tid) {
			return tcb_temp;
		}
	}

	estado_iterador = FirstFila2(__bloqueados);
	if (estado_iterador == 0) {
		tcb_temp = GetAtIteratorFila2(__bloqueados);
		if (tcb_temp->tid == tid) {
			return tcb_temp;
		}
	}

	estado_iterador = FirstFila2(__aptos_prio_0);
	if (estado_iterador == 0) {
		tcb_temp = GetAtIteratorFila2(__aptos_prio_0);
		if (tcb_temp->tid == tid) {
			*emApto = 0;
			return tcb_temp;
		}
	}

	estado_iterador = FirstFila2(__aptos_prio_1);
	if (estado_iterador == 0) {
		tcb_temp = GetAtIteratorFila2(__aptos_prio_1);
		if (tcb_temp->tid == tid) {
			*emApto = 1;
			return tcb_temp;
		}
	}

	estado_iterador = FirstFila2(__aptos_prio_2);
	if (estado_iterador == 0) {
		tcb_temp = GetAtIteratorFila2(__aptos_prio_2);
		if (tcb_temp->tid == tid) {
			*emApto = 2;
			return tcb_temp;
		}
	}

	*erro = 1;
}

// ----------------------------------------------------------------------------------- //

int atualizaPrioridade(int tid, int nova_prio) {
	int estado_iterador;
	TCB_t *tcb_temp;
	estado_iterador = FirstFila2(__executando);
	if (estado_iterador == 0) {
		tcb_temp = GetAtIteratorFila2(__executando);
		if (tcb_temp->tid == tid) {
			tcb_temp->prio = nova_prio;
			return 0;
		}
	}

	estado_iterador = FirstFila2(__bloqueados);
	if (estado_iterador == 0) {
		tcb_temp = GetAtIteratorFila2(__bloqueados);
		if (tcb_temp->tid == tid) {
			tcb_temp->prio = nova_prio;
			return 0;
		}
	}

	estado_iterador = FirstFila2(__aptos_prio_0);
	if (estado_iterador == 0) {
		tcb_temp = GetAtIteratorFila2(__aptos_prio_0);
		if (tcb_temp->tid == tid) {
			tcb_temp->prio = nova_prio;
			return 0;
		}
	}

	estado_iterador = FirstFila2(__aptos_prio_1);
	if (estado_iterador == 0) {
		tcb_temp = GetAtIteratorFila2(__aptos_prio_1);
		if (tcb_temp->tid == tid) {
			tcb_temp->prio = nova_prio;
			return 0;
		}
	}

	estado_iterador = FirstFila2(__aptos_prio_2);
	if (estado_iterador == 0) {
		tcb_temp = GetAtIteratorFila2(__aptos_prio_2);
		if (tcb_temp->tid == tid) {
			tcb_temp->prio = nova_prio;
			return 0;
		}
	}

	return -1;
}