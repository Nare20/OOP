# Project Architecture

This file groups the project files by logical compiler architecture layers and subsystems. Files remain in `src/` to avoid breaking includes; use this as a roadmap for a later physical refactor.

## Frontend (Lexer / Parser / AST)
- tokenizer.hpp, tokenizer.cpp
- parser.hpp, parser.cpp
- ASTNode.hpp, ASTNode.cpp
- BlockNode.hpp, IfNode.hpp, WhileNode.hpp, ReturnNode.hpp, FuncNode.hpp

## Midend (IR / Bytecode Generator)
- IR.hpp
- IRGenerator.hpp, IRGenerator.cpp
- bytecode.hpp, bytecode.cpp
- BytecodeGenerator.hpp, BytecodeGenerator.cpp

## Backend (Bytecode)
- bytecode.hpp, bytecode.cpp

## Runtime / VM
- VM.hpp, VM.cpp

## Tools / Semantic Analysis / Allocation
- SymbolTable.hpp, SymbolTable.cpp
- RegisterAllocator.hpp, RegisterAllocator.cpp
- Compiler.hpp, Compiler.cpp

## CLI / Entry points
- mainCompiler.cpp
- mainVM.cpp

## Notes
- This is intentionally non-destructive. If you'd like, I can perform a physical refactor (move files into subfolders and update include paths). That will require updating many include directives and rebuilding — tell me when you want me to proceed.
