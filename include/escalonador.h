#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "support.h"
#include "cdata.h"

#define STACK_SIZE 131072
#define __NUMBER_OF_ARGS 1

FILA2 __aptos_prio_0;
FILA2 __aptos_prio_1;
FILA2 __aptos_prio_2;
FILA2 __executando;
FILA2 __bloqueados;
FILA2 __threads_esperadas;

int inicializaEscalonador();

int inicializaFilas();

int inicializaMain();

int insereEmApto(TCB_t *thread);

int insereEmExecutando(TCB_t *thread);

int insereEmBloqueado(TCB_t *thread);

int insereEmThreadsEsperadas(int *tid);

int removeDeExecutando();

int removeDeApto();

int removeDeBloqueado(TCB_t *thread);

int removeDeThreadsEsperadas(int tid);

TCB_t* retornaExecutando();

TCB_t* retornaApto();

TCB_t* retornaBloqueado(int tid);

// int alternaEntreAptos(TCB_t *thread, int prioAntiga);

_Bool executandoLivre();

TCB_t* buscaThread(int tid, _Bool *erro, int *fila);

_Bool buscaThreadEsperada(int tid);

int atualizaPrioridade(int tid, int nova_prio);

int escalonaThread(TCB_t *thread);

int finalizaThread();

void proximaThread();

int sincronizaTermino(int tid);

TCB_t* buscaMaiorPrioridade(PFILA2 fila);