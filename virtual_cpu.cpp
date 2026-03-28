#include <iostream>
#include <vector>

using namespace std;

enum class OpCode {
    LOADI,
    LOAD,
    STORE,
    MOVE,
    ADD,
    SUB,
    MUL,
    DIV,
    HALT
};

struct Instruction {
    OpCode op;
    int a;
    int b;
};

struct CPU {
    int R[8]{0};
    int PC = 0;

    vector<Instruction> code;
    vector<int> data;

    CPU(const vector<Instruction>& c, const vector<int>& d)
        : code(c), data(d) {}

    void execute() {
        while (PC < (int)code.size()) {
            Instruction ins = code[PC++];

            switch (ins.op) {
                case OpCode::LOADI:
                    R[ins.a] = ins.b;
                    break;

                case OpCode::LOAD:
                    R[ins.a] = data[ins.b];
                    break;

                case OpCode::STORE:
                    data[ins.b] = R[ins.a];
                    break;

                case OpCode::MOVE:
                    R[ins.a] = R[ins.b];
                    break;

                case OpCode::ADD:
                    R[ins.a] = R[ins.a] + R[ins.b];
                    break;

                case OpCode::SUB:
                    R[ins.a] = R[ins.a] - R[ins.b];
                    break;

                case OpCode::MUL:
                    R[ins.a] = R[ins.a] * R[ins.b];
                    break;

                case OpCode::DIV:
                    R[ins.a] = R[ins.a] / R[ins.b];
                    break;

                case OpCode::HALT:
                    return;
            }
        }
    }
};

int main() {
    
    vector<int> data(2, 0);

    vector<Instruction> code = {
        {OpCode::LOADI, 0, 5},
        {OpCode::MOVE,  1, 0},
        {OpCode::LOADI, 0, 2},
        {OpCode::MOVE,  2, 0},
        {OpCode::LOADI, 0, 3},
        {OpCode::MUL,   2, 0},
        {OpCode::MOVE,  0, 1},
        {OpCode::ADD,   0, 2},
        {OpCode::HALT,  0, 0}
    };

    CPU cpu(code, data);
    cpu.execute();

    cout << "R0 result = " << cpu.R[0] << endl;
}