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