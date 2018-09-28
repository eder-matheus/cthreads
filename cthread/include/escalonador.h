#include "../include/support.h"
#include "../include/cdata.h"

PFILA2 __aptos_prio_0;
PFILA2 __aptos_prio_1;
PFILA2 __aptos_prio_2;
PFILA2 __executando;
PFILA2 __bloqueados;

int inicializaFilas();

int insereEmApto(TCB_t *thread);

int insereEmExecutando(TCB_t *thread);

int insereEmBloqueado(TCB_t *thread);

int removeDeExecutando();

int removeDeApto();

int removeDeBloqueado();

TCB_t* retornaExecutando();

TCB_t* retornaApto();

TCB_t* retornaBloqueado();

int alternaEntreAptos(TCB_t *thread, int prioAntiga);

_Bool executandoLivre();

TCB_t* buscaThread(int tid, _Bool *erro, int *emApto);

int atualizaPrioridade(int tid, int nova_prio);