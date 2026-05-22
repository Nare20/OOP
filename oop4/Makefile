CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra
SRC = src
OBJ = build

SRCS := $(wildcard $(SRC)/*.cpp)
OBJS := $(patsubst $(SRC)/%.cpp,$(OBJ)/%.o,$(SRCS))

all: compiler vm

$(OBJ):
	mkdir -p $(OBJ)

$(OBJ)/%.o: $(SRC)/%.cpp | $(OBJ)
	$(CXX) $(CXXFLAGS) -I$(SRC) -c $< -o $@

compiler: $(OBJ)/mainCompiler.o $(OBJ)/tokenizer.o $(OBJ)/parser.o $(OBJ)/ASTNode.o $(OBJ)/IRGenerator.o $(OBJ)/BytecodeGenerator.o $(OBJ)/bytecode.o
	$(CXX) $(CXXFLAGS) -o compiler $^

vm: $(OBJ)/mainVM.o $(OBJ)/VM.o $(OBJ)/bytecode.o
	$(CXX) $(CXXFLAGS) -o vm $^

clean:
	rm -rf $(OBJ) compiler vm *.bc

.PHONY: all clean
