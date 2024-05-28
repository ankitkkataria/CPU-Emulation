#include <stdio.h>
#include <stdlib.h>

using BYTE = unsigned char;
using WORD = unsigned short;
using u32 = unsigned int;


struct MEMORY {

    static constexpr u32 MAX_MEM = 1024 * 64; // Cause the CPU only has 64KB memory
    BYTE Data[MAX_MEM]; // This will be the memory we will be able to access through CPU
  
    void initialize() {
        for(u32 i = 0 ; i < MAX_MEM ; i++) {
            Data[i] = 0;
        }
    }


};

struct CPU
{

    WORD PC; // Program counter
    WORD SP; // Stack pointer 

    BYTE A, X, Y; // Accumulator, index registor X, index register Y.

    BYTE C : 1; // Carry flag
    BYTE Z : 1; // Zero flag
    BYTE I : 1; // Interrupt flag
    BYTE D : 1; // Decimal Mode flag
    BYTE B : 1; // Break flag
    BYTE V : 1; // Overflow flag
    BYTE N : 1; // Negative flag
    
    void reset(MEMORY &memory) {
        PC = 0xFFFC;
        SP = 0x0100;
        C = Z = I = D = B = V = N = 0;
        A = X = Y = 0;
        memory.initialize();
        printf("CPU Resetted");
    }

};


int main() {
    MEMORY memory;
    CPU cpu;
    cpu.reset(memory);    
}