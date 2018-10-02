#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "support.h"
#include "cdata.h"

#define STACK_SIZE 131072



FILA2 __aptos_prio_0;
FILA2 __aptos_prio_1;
FILA2 __aptos_prio_2;
FILA2 __executando;
FILA2 __bloqueados;

int inicializaFilas();

// int inicualizaMain();

int insereEmApto(TCB_t *thread);

int insereEmExecutando(TCB_t *thread);

int insereEmBloqueado(TCB_t *thread);

int removeDeExecutando();

int removeDeApto();

int removeDeBloqueado();

TCB_t* retornaExecutando();

TCB_t* retornaApto();

TCB_t* retornaBloqueado(int tid);

// int alternaEntreAptos(TCB_t *thread, int prioAntiga);

_Bool executandoLivre();

TCB_t* buscaThread(int tid, _Bool *erro, int *fila);

int atualizaPrioridade(int tid, int nova_prio);

int escalonaThread(TCB_t *thread);

int finalizaThread();

void proximaThread();

int sincronizaTermino(int tid);