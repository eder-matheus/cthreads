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

int retiraDeExecutando() {
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

int retiraDeApto() {
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

int retiraDeBloqueado() {
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

