GXX=g++
GXX_ARGS=-std=c++17 -O2 -Wall -Wextra

SRC=src
LIB=$(SRC)/lib
BIN=bin
OBJ=$(BIN)/obj
TEST=tests/

INCL=-I$(LIB)/include

PARSER = $(OBJ)/ADLParser.o $(OBJ)/SymbolStack.o $(OBJ)/ADLTokenizer.o $(OBJ)/ADLTree.o $(OBJ)/ADLExceptions.o

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

$(OBJ)/ADLExceptions.o: $(LIB)/ADLExceptions.cpp | dirs
	$(GXX) $(GXX_ARGS) $(INCL) -o $@ -c $<
$(OBJ)/ADLTokenizer.o: $(LIB)/ADLTokenizer.cpp | dirs
	$(GXX) $(GXX_ARGS) $(INCL) -o $@ -c $<
$(OBJ)/ADLParser.o: $(LIB)/ADLParser.cpp | dirs
	$(GXX) $(GXX_ARGS) $(INCL) -o $@ -c $<
$(OBJ)/ADLTree.o: $(LIB)/ADLTree.cpp | dirs
	$(GXX) $(GXX_ARGS) $(INCL) -o $@ -c $<
$(OBJ)/SymbolStack.o: $(LIB)/SymbolStack.cpp | dirs
	$(GXX) $(GXX_ARGS) $(INCL) -o $@ -c $<

$(OBJ)/test_tokenizer.o: $(TEST)/test_tokenizer.cpp | dirs
	$(GXX) $(GXX_ARGS) $(INCL) -o $@ -c $<
$(BIN)/test_tokenizer.out: $(OBJ)/test_tokenizer.o $(OBJ)/ADLTokenizer.o | dirs
	$(GXX) $(GXX_ARGS) -o $@ $^
test_tokenizer: $(BIN)/test_tokenizer.out

$(OBJ)/test_parser.o: $(TEST)/test_parser.cpp | dirs
	$(GXX) $(GXX_ARGS) $(INCL) -o $@ -c $<
$(BIN)/test_parser.out: $(OBJ)/test_parser.o $(PARSER) | dirs
	$(GXX) $(GXX_ARGS) -o $@ $< $(PARSER)
test_parser: $(BIN)/test_parser.out
