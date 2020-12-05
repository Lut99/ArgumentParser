##### CONSTANTS #####

# Compilers
GXX=g++
GXX_ARGS=-std=c++17 -O2 -Wall -Wextra

# Folders
SRC	=src
LIB	=$(SRC)/lib
ADLSYS=$(LIB)/adl
INCL=$(LIB)/include
BIN	=bin
OBJ	=$(BIN)/obj
TEST=tests/

# Required files
AST_SOURCE = $(shell find $(LIB)/AST -name '*.cpp')
AST = $(AST_SOURCE:$(LIB)/%.cpp=$(OBJ)/%.o)
TOKENIZER = $(OBJ)/ADLTokenizer.o $(OBJ)/ADLExceptions.o
PREPROCESSOR = $(INCL)/adl/ADLBaked.hpp $(OBJ)/ADLPreprocessor.o $(TOKENIZER)
PARSER = $(OBJ)/ADLParser.o $(OBJ)/SymbolStack.o $(PREPROCESSOR) $(AST)

# Prepare the list of includes and use that to extend the list of directories we need to make
INCLUDE_DIRS = $(shell find $(INCL) -type d)
INCLUDE = $(INCLUDE_DIRS:%=-I%)
DIRS = $(BIN) $(OBJ) $(INCLUDE_DIRS:$(INCL)/%=$(OBJ)/%)
ADL_SYSTEM = $(shell find $(ADLSYS) -name '*.adl')



##### INPUT #####
ifdef DEBUG
GXX_ARGS += -g -DDEBUG
endif



##### PHONY RULES #####
.PHONY: default test_tokenizer test_parser bake_adl all dirs clean
default: all

all: test_tokenizer test_parser bake_adl

clean:
	-find $(OBJ) -name "*.o" -type f -delete
	-rm -f $(BIN)/*.out
	-rm -f $(INCL)/adl/ADLBaked.hpp



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
$(OBJ)/adl:
	mkdir -p $@
dirs: $(DIRS)



##### COMPILE RULES #####

$(OBJ)/%.o: $(LIB)/%.cpp | dirs
	$(GXX) $(GXX_ARGS) $(INCLUDE) -o $@ -c $<

# Specialized rule for baking in the required system files
$(INCL)/adl/ADLBaked.hpp: $(LIB)/adl/bakery.sh $(INCL)/adl/ADLBaked_template.hpp $(ADL_SYSTEM)
	bash $^ $@
bake_adl: $(INCL)/adl/ADLBaked.hpp



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
