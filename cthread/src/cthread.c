#include <stdio.h>
#include <string.h>
#include "../include/support.h"
#include "../include/cthread.h"
#include "../include/cdata.h"
#include "../src/escalonador.c"


int ccreate (void* (*start)(void*), void *arg, int prio) {
	if (prio < 0 || prio > 2) {
		return -1;
	}

	int i = 0;
	i = inicializaFilas();
	return i;
}

int csetprio(int tid, int prio) {

	return -1;
}

int cyield(void) {
	return -1;
}

int cjoin(int tid) {
	return -1;
}

int csem_init(csem_t *sem, int count) {
	return -1;
}

int cwait(csem_t *sem) {
	return -1;
}

int csignal(csem_t *sem) {
	return -1;
}

int cidentify (char *name, int size) {
	strncpy (name, "Eder Matheus Rodrigues Monteiro e Guilherme Girotto Sartori", size);
	return 0;
}