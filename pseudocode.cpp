Program:
    Input: source code string

    Step 1: Lexer / Tokenizer
        - scan input string character by character
        - identify tokens: NUM, VAR, operators (+, -, *, /, %, ^), parentheses, assignment (=), semicolon
        - return a list/vector of tokens

    Step 2: Parser → AST
        - Input: vector of tokens
        - Output: AST (Abstract Syntax Tree)
        - Parse expressions with precedence rules:
            - Handle parentheses
            - Handle operators: +, -, *, /, %, ^ 
            - Handle assignment statements
            - Build Node objects:
                - Node type: NUM, VAR, OPERATOR, ASSIGN, PROGRAM
                - left, right children for binary operators
                - children vector for sequences/statements
        - Return root of AST

    Step 3: Symbol Table Collection
        - Input: AST root
        - Output: symbol table (map: variable name → index)
        - Traverse AST recursively:
            - For each VAR or ASSIGN node:
                - if variable not in table, add with next available index
        - Allocate data vector of size = symbol table size

    Step 4: Code Generation
        - Input: AST root, symbol table
        - Output: Instruction vector (IR)
        - For each node in AST recursively:
            - if NUM: generate LOAD instruction to a register
            - if VAR: generate LOAD_VAR instruction
            - if OPERATOR: generate instructions for left and right children
                - then generate ADD/SUB/MUL/DIV/MOD instruction combining registers
            - if ASSIGN: generate code to store result into variable (symbol table index)
        - Assign target register (usually R0) for final expression result

    Step 5: Processor / VM
        - Input: instruction vector, data vector
        - Execute instructions sequentially:
            - LOAD: store constant into register
            - LOAD_VAR: load variable from data vector into register
            - STORE: store register into variable in data vector
            - ADD/SUB/MUL/DIV/MOD: operate on registers
        - Final result stored in R0

    Step 6: Output
        - Print final result in R0
        - Optionally print symbol table and data vector
