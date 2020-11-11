GXX=g++
GXX_ARGS=-std=c++17 -O2 -Wall -Wextra

SRC=src
LIB=$(SRC)/lib
BIN=bin
OBJ=$(BIN)/obj
TEST=tests/

INCL=-I$(LIB)/include

### INPUT ###
ifdef DEBUG
GXX_ARGS += -g
endif

### PHONY RULES ###
.PHONY: default test_tokenizer all dirs clean
default: all

all: test_tokenizer

clean:
	rm -f $(OBJ)/*.o
	rm -f $(BIN)/*.out

### DIRECTORY RULES ###

$(BIN):
	mkdir -p $@
$(OBJ):
	mkdir -p $@
dirs: $(BIN) $(OBJ)

### COMPILE RULES ###

$(OBJ)/test_tokenizer.o: $(TEST)/test_tokenizer.cpp | dirs
	$(GXX) $(GXX_ARGS) $(INCL) -o $@ -c $<
$(OBJ)/ADLTokenizer.o: $(LIB)/ADLTokenizer.cpp | dirs
	$(GXX) $(GXX_ARGS) $(INCL) -o $@ -c $<
$(BIN)/test_tokenizer.out: $(OBJ)/test_tokenizer.o $(OBJ)/ADLTokenizer.o | dirs
	$(GXX) $(GXX_ARGS) -o $@ $^
test_tokenizer: $(BIN)/test_tokenizer.out
