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

# Automatic file search
AST_SOURCE = $(shell find $(LIB)/AST -name '*.cpp')
AST = $(AST_SOURCE:$(LIB)/%.cpp=$(OBJ)/%.o)
TRAVERSALS_SOURCE = $(shell find $(LIB)/traversals -name '*.cpp')
TRAVERSALS = $(TRAVERSALS_SOURCE:$(LIB)/%.cpp=$(OBJ)/%.o)

# Dependencies for each stage of the compiler
TOKENIZER = $(OBJ)/ADLTokenizer.o $(OBJ)/ADLExceptions.o $(OBJ)/WarningTypes.o
PREPROCESSOR = $(OBJ)/ADLPreprocessor.o $(TOKENIZER)
PARSER = $(OBJ)/ADLParser.o $(OBJ)/SymbolStack.o $(OBJ)/ADLSymbolTable.o $(PREPROCESSOR) $(AST)
COMPILER = $(OBJ)/ADLCompiler.o $(PARSER) $(TRAVERSALS)

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
.PHONY: default compiler test_tokenizer test_parser bake_adl all dirs clean
default: all

all: test_tokenizer test_parser bake_adl

clean:
	-find $(OBJ) -name "*.o" -type f -delete
	-find $(OBJ) -type f ! -name "*.*" -delete
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
$(OBJ)/AST/identifiers:
	mkdir -p $@
$(OBJ)/adl:
	mkdir -p $@
$(OBJ)/traversals:
	mkdir -p $@
dirs: $(DIRS)



##### COMPILE RULES #####

# Rule to make the compiler itself
$(OBJ)/ADLCompiler.o: $(SRC)/ADLCompiler.cpp | dirs
	$(GXX) $(GXX_ARGS) $(INCLUDE) -o $@ -c $<
# Special rule for the preprocessor, which re-bakes the ADL system files
$(OBJ)/ADLPreprocessor.o: $(LIB)/ADLPreprocessor.cpp $(INCL)/adl/ADLBaked.hpp | dirs
	$(GXX) $(GXX_ARGS) $(INCLUDE) -o $@ -c $<
# General object rule
$(OBJ)/%.o: $(LIB)/%.cpp | dirs
	$(GXX) $(GXX_ARGS) $(INCLUDE) -o $@ -c $<

# Specialized rule for baking in the required system files
$(INCL)/adl/ADLBaked.hpp: $(LIB)/adl/bakery.sh $(INCL)/adl/ADLBaked_template.hpp $(ADL_SYSTEM)
	bash $^ $@
bake_adl: $(INCL)/adl/ADLBaked.hpp

# Rule to create the functional compiler!
$(BIN)/adlc: $(COMPILER) | dirs
	$(GXX) $(GXX_ARGS) -o $@ $^
compiler: $(BIN)/adlc



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
	$(GXX) $(GXX_ARGS) -o $@ $^
test_parser: $(BIN)/test_parser.out
