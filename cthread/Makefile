#
# Makefile de EXEMPLO
#
# OBRIGATÓRIO ter uma regra "all" para geração da biblioteca e de uma
# regra "clean" para remover todos os objetos gerados.
#
# É NECESSARIO ADAPTAR ESSE ARQUIVO de makefile para suas necessidades.
#  1. Cuidado com a regra "clean" para não apagar o "support.o"
#
# OBSERVAR que as variáveis de ambiente consideram que o Makefile está no diretótio "cthread"
# 

CC=gcc
LINKER=ld
OBJ = cthread.o
LIB_DIR=./lib
INC_DIR= -I ./include
BIN_DIR=$(wildcard ./bin/*.o)
THR_DIR=$(wildcard ./src/cthread.c)
SCA_DIR=$(wildcard ./src/escalonador.c)
SCA_OBJ=$(patsubst %.c,%.o,$(SCA_DIR))
THR_OBJ=$(patsubst %.c,%.o,$(THR_DIR))

#all: clean $(SCA_OBJ) $(THR_OBJ) $(OBJ)
all: clean regra1

regra1: #dependências para a regra1
	$(CC) -Wall -m32 $(SCA_DIR) $(THR_DIR) $(INC_DIR) $(BIN_DIR) -o $(OBJ)

#$(OBJ): $(SCA_OBJ) $(THR_OBJ) #dependências para a regra1
#	$(LINKER) -o $(INC_DIR) $(BIN_DIR) $@ $^ -Wall -m32

./src/cthread.o: ./src/cthread.c #dependências para a regra2
	$(CC) -Wall -m32 $(INC_DIR) $(BIN_DIR) -o $@ $<

./src/escalonador.o: ./src/escalonador.c #dependências para a regra2
	$(CC) -Wall -m32 $(INC_DIR) $(BIN_DIR) -o $@ $<


#regran: #dependências para a regran
#	$(CC) -o $(BIN_DIR)/regran $(SRC_DIR)/regran.c -Wall -m32

clean:
	rm -rf $(LIB_DIR)/*.a ./src/*.o


