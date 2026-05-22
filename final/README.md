Simple compiler + VM

Build:
  make

Compile a source file to bytecode:
  ./compiler sample.src -o sample.bc

Run the bytecode on the VM:
  ./vm sample.bc

Language features: integers, variables, assignments, expressions, relational/equality operators, if, while, functions, return, print builtin.

Example (sample.src):
  func main() {
    print(1 + 2 * 3);
  }

Architecture: see [src/ARCHITECTURE.md](src/ARCHITECTURE.md) for logical grouping of source files.
