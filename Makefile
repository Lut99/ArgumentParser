##### CONSTANTS #####

# Compilers
GXX=g++
GXX_ARGS=-std=c++17 -O2 -Wall -Wextra

# Folders
SRC	=src
LIB	=$(SRC)/lib
INCL=$(LIB)/include
BIN	=bin
OBJ	=$(BIN)/obj
TEST=tests/

# Required files
AST_SOURCE = $(shell find $(LIB)/AST -name '*.cpp')
AST = $(AST_SOURCE:$(LIB)/%.cpp=$(OBJ)/%.o)
PARSER = $(OBJ)/ADLParser.o $(OBJ)/SymbolStack.o $(OBJ)/ADLTokenizer.o $(OBJ)/ADLExceptions.o $(AST)
TOKENIZER = $(OBJ)/ADLTokenizer.o $(OBJ)/ADLExceptions.o

# Prepare the list of includes and use that to extend the list of directories we need to make
INCLUDE_DIRS = $(shell find $(INCL) -type d)
INCLUDE = $(INCLUDE_DIRS:%=-I%)
DIRS = $(BIN) $(OBJ) $(INCLUDE_DIRS:$(INCL)/%=$(OBJ)/%)



##### INPUT #####
ifdef DEBUG
GXX_ARGS += -g -DDEBUG
endif



##### PHONY RULES #####
.PHONY: default test_tokenizer all dirs clean
default: all

all: test_tokenizer

clean:
	find $(OBJ) -name "*.o" -type f -delete
	rm -f $(BIN)/*.out



##### DIRECTORY RULES #####

$(BIN):
	mkdir -p $@
$(OBJ):
	mkdir -p $@
$(OBJ)/AST:
	mkdir -p $@
$(OBJ)/AST/toplevel:
	mkdir -p $@
$(OBJ)/AST/values:
	mkdir -p $@
$(OBJ)/AST/configs:
	mkdir -p $@
$(OBJ)/AST/types:
	mkdir -p $@
dirs: $(DIRS)



##### COMPILE RULES #####

$(OBJ)/%.o: $(LIB)/%.cpp | dirs
	$(GXX) $(GXX_ARGS) $(INCLUDE) -o $@ -c $<
# $(OBJ)/AST/%.o: $(LIB)/AST/%.cpp | dirs
# 	$(GXX) $(GXX_ARGS) $(INCLUDE) -o $@ -c $<
# $(OBJ)/AST/values/%.o: $(LIB)/AST/values/%.cpp | dirs
# 	$(GXX) $(GXX_ARGS) $(INCLUDE) -o $@ -c $<



##### TEST RULES #####

# Test Tokenizer
$(OBJ)/test_tokenizer.o: $(TEST)/test_tokenizer.cpp | dirs
	$(GXX) $(GXX_ARGS) $(INCLUDE) -o $@ -c $<
$(BIN)/test_tokenizer.out: $(OBJ)/test_tokenizer.o $(TOKENIZER) | dirs
	$(GXX) $(GXX_ARGS) -o $@ $^
test_tokenizer: $(BIN)/test_tokenizer.out

# Test Parser
$(OBJ)/test_parser.o: $(TEST)/test_parser.cpp | dirs
	$(GXX) $(GXX_ARGS) $(INCLUDE) -o $@ -c $<
$(BIN)/test_parser.out: $(OBJ)/test_parser.o $(PARSER) | dirs
	$(GXX) $(GXX_ARGS) -o $@ $< $(PARSER)
test_parser: $(BIN)/test_parser.out
