# MAKEFILE
#   by Tim Müller

### PHONY RULES ###
.PHONY: argparsetype parsetest dependencytest default all clean dirs
default: all

all: argparsetype parsetest dependencytest

clean:
	rm -rf bin/*.out

bin:
	mkdir -p $@
dirs: bin


### ARGPARSETYPE ###
bin/argparsetype.out: argparsetype.cpp | dirs
	g++ -g -std=c++17 -O2 -Wall -Wextra -o $@ $^
argparsetype: bin/argparsetype.out

### PARSETEST ###
bin/parsetest.out: parsetest.cpp | dirs
	g++ -g -std=c++17 -O2 -Wall -Wextra -o $@ $^
parsetest: bin/parsetest.out

### DEPENDENCYTEST ###
bin/dependencytest.out: dependencytest.cpp | dirs
	g++ -g -std=c++17 -O2 -Wall -Wextra -o $@ $^
dependencytest: bin/dependencytest.out
