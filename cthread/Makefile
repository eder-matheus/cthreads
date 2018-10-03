CC=gcc
LIB_DIR=./lib
INC_DIR=./include
BIN_DIR=./bin
SRC_DIR=./src

all: cthread libcthread

cthread: 
	$(CC) -Wall -m32 -c $(SRC_DIR)/cthread.c -o $(BIN_DIR)/cthread.o
	$(CC) -Wall -m32 -c $(SRC_DIR)/escalonador.c -o $(BIN_DIR)/escalonador.o

libcthread: 
	ar crs $(LIB_DIR)/libcthread.a $(BIN_DIR)/cthread.o $(BIN_DIR)/escalonador.o $(BIN_DIR)/support.o

clean:
	find $(BIN_DIR)/*.o ! -name 'support.o' -type f -exec rm -f {} +
	rm -rf $(LIB_DIR)/*.a $(SRC_DIR)/*~ $(INC_DIR)/*~ *~


