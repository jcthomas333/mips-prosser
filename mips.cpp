#include <iostream>
#include <bitset>
#include <vector>
#include <string>

using namespace std;

const int NUM_REG = 32;
const int NUM_MEM = 1024;

// Define instruction types
enum InstrType { R_TYPE, I_TYPE, J_TYPE, INVALID };

// Opcodes / funct codes
enum Instructions { ADD = 32, SUB = 34, AND = 36, OR = 37, SLT = 42,
                    LW = 35, SW = 43, BEQ = 4, BNE = 5, ADDI = 8 };

vector<int32_t> registers(NUM_REG, 0);
vector<int32_t> memory(NUM_MEM, 0);

struct Instruction {
    InstrType type = INVALID;
    int opcode = 0;
    int rs = 0, rt = 0, rd = 0;
    int imm = 0;
    int funct = 0;
};

// Convert hex string to binary
bitset<32> hexToBinary(const string &hex) {
    uint32_t val = stoi(hex, nullptr, 16);
    return bitset<32>(val);
}

// Decode binary into instruction
Instruction decodeInstruction(const bitset<32> &bits) {
    Instruction instr;
    int opcode = (bits.to_ulong() >> 26) & 0x3F;

    if (opcode == 0) { // R-type
        instr.type = R_TYPE;
        instr.rs = (bits.to_ulong() >> 21) & 0x1F;
        instr.rt = (bits.to_ulong() >> 16) & 0x1F;
        instr.rd = (bits.to_ulong() >> 11) & 0x1F;
        instr.funct = bits.to_ulong() & 0x3F;
    } else { // I-type
        instr.type = I_TYPE;
        instr.opcode = opcode;
        instr.rs = (bits.to_ulong() >> 21) & 0x1F;
        instr.rt = (bits.to_ulong() >> 16) & 0x1F;
        instr.imm = bits.to_ulong() & 0xFFFF;
    }
    return instr;
}

// Execution helpers
void ExAdd(int rs, int rt, int rd) { registers[rd] = registers[rs] + registers[rt]; }
void ExSub(int rs, int rt, int rd) { registers[rd] = registers[rs] - registers[rt]; }
void ExAnd(int rs, int rt, int rd) { registers[rd] = registers[rs] & registers[rt]; }
void ExOr(int rs, int rt, int rd)  { registers[rd] = registers[rs] | registers[rt]; }
void ExSlt(int rs, int rt, int rd) { registers[rd] = (registers[rs] < registers[rt]) ? 1 : 0; }
void ExAddi(int rs, int rt, int imm) { registers[rt] = registers[rs] + imm; }
void ExLw(int rs, int rt, int imm) {
    int addr = registers[rs] + imm;
    if (addr >= 0 && addr < NUM_MEM) registers[rt] = memory[addr];
}
void ExSw(int rs, int rt, int imm) {
    int addr = registers[rs] + imm;
    if (addr >= 0 && addr < NUM_MEM) memory[addr] = registers[rt];
}

// Execute one instruction
void execute(Instruction &instr, int &pc) {
    if (instr.type == R_TYPE) {
        switch (instr.funct) {
            case ADD: ExAdd(instr.rs, instr.rt, instr.rd); break;
            case SUB: ExSub(instr.rs, instr.rt, instr.rd); break;
            case AND: ExAnd(instr.rs, instr.rt, instr.rd); break;
            case OR:  ExOr(instr.rs, instr.rt, instr.rd); break;
            case SLT: ExSlt(instr.rs, instr.rt, instr.rd); break;
            default: cout << "Invalid R-type funct\n"; break;
        }
    } else if (instr.type == I_TYPE) {
        switch (instr.opcode) {
            case ADDI: ExAddi(instr.rs, instr.rt, instr.imm); break;
            case LW:   ExLw(instr.rs, instr.rt, instr.imm); break;
            case SW:   ExSw(instr.rs, instr.rt, instr.imm); break;
            default: cout << "Invalid I-type opcode\n"; break;
        }
    }
    pc++;
}

int main() {
    int pc = 0;
    string hex;
    vector<Instruction> program;

    cout << "Enter hex instructions (type 'end' to stop):\n";
    while (true) {
        cout << "> ";
        cin >> hex;
        if (hex == "end") break;
        bitset<32> bits = hexToBinary(hex);
        program.push_back(decodeInstruction(bits));
    }

    while (pc < program.size()) {
        execute(program[pc], pc);
    }

    for (int i = 0; i < NUM_REG; i++) {
        cout << "Register " << i << ": " << registers[i] << endl;
    }
}
