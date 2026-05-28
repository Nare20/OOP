CXX      = g++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra -Iinclude

COMMON = utils.cpp tokenizer.cpp lexer.cpp parser.cpp \
         SymbolTable.cpp Memory.cpp CPU.cpp Loader.cpp \
         VM.cpp compiler_core.cpp instruction.cpp Executor.cpp

compiler: $(COMMON) mainCompiler.cpp
	$(CXX) $(CXXFLAGS) $(COMMON) mainCompiler.cpp -o compiler

executor: $(COMMON) mainExec.cpp
	$(CXX) $(CXXFLAGS) $(COMMON) mainExec.cpp -o executor

all: compiler executor

clean:
	rm -f compiler executor *.exe *.o

.PHONY: all clean