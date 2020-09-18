GXX=g++
GXX_ARGS=-std=c++17 -O2 -Wall -Wextra

SRC=src
BIN=bin
OBJ=$(BIN)/obj
TEST=tests/

INCL=-I$(SRC)

### INPUT ###
ifdef DEBUG
GXX_ARGS += -g
endif

### PHONY RULES ###
.PHONY: default tests all dirs clean
default: all

all: tests

clean:
	rm -f $(OBJ)/*.o
	rm -f $(BIN)/*.out

tests: $(BIN)/test_argparser.out
	$(info Running tests...)
	$(info )

	$(BIN)/test_argparser.out

### DIRECTORY RULES ###

$(BIN):
	mkdir -p $@
$(OBJ):
	mkdir -p $@
dirs: $(BIN) $(OBJ)

### COMPILE RULES ###

$(OBJ)/test_argparser.o: $(TEST)/test_argparser.cpp | dirs
	$(GXX) $(GXX_ARGS) $(INCL) -o $@ -c $<

$(BIN)/test_argparser.out: $(OBJ)/test_argparser.o | dirs
	$(GXX) $(GXX_ARGS) $(INCL) -o $@ $(OBJ)/test_argparser.o
