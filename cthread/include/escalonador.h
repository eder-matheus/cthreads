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