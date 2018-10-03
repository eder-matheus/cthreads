CC=gcc
LIB_DIR=../lib

all: teste_inicial teste_vetores_adaptado series_adaptado teste_semaforo_1

teste_inicial: teste_inicial.c $(LIB_DIR)/libcthread.a
	$(CC) -o teste_inicial.o teste_inicial.c -L$(LIB_DIR) -lcthread -lm -m32

teste_vetores_adaptado: teste_vetores_adaptado.c $(LIB_DIR)/libcthread.a
	$(CC) -o teste_vetores_adaptado.o teste_vetores_adaptado.c -L$(LIB_DIR) -lcthread -lm -Wall -m32


series_adaptado: series_adaptado.c $(LIB_DIR)/libcthread.a
	$(CC) -o series_adaptado.o series_adaptado.c -L$(LIB_DIR) -lcthread -lm -Wall -m32

teste_semaforo_1: teste_semaforo_1.c $(LIB_DIR)/libcthread.a
	$(CC) -o teste_semaforo_1.o teste_semaforo_1.c -L$(LIB_DIR) -lcthread -lm -Wall -m32
	
clean:
	rm -rf *.o *~