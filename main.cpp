#include <stdio.h>
#include <stdlib.h>

using BYTE = unsigned char;
using WORD = unsigned short;
using u32 = unsigned int;

struct MEMORY
{

    static constexpr u32 MAX_MEM = 1024 * 64; // Cause the CPU only has 64KB memory
    BYTE Data[MAX_MEM];                       // This will be the memory we will be able to access through CPU

    void initialize()
    {
        for (u32 i = 0; i < MAX_MEM; i++)
        {
            Data[i] = 0;
        }
    }

    // Overriding [] operator so we can directly read a byte just using a MEMORY object without using memoryObj.Data[Address] rather we can just do memoryObj[Address];
    BYTE operator[](u32 Address) const
    {
        // Assert here Adress is < MAX_MEM
        return Data[Address];
    }

    // Overriding write to memory the one above is for reading  
    BYTE& operator[](u32 Address) 
    {
        // Assert here Adress is < MAX_MEM
        return Data[Address];
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

    void reset(MEMORY &memory)
    {
        PC = 0xFFFC;
        SP = 0x0100;
        C = Z = I = D = B = V = N = 0;
        A = X = Y = 0;
        memory.initialize();
        printf("CPU Resetted\n");
    }

    BYTE fetchByte(u32 &numCycles, MEMORY &memory)
    {
        BYTE data = memory.Data[PC];
        PC++;
        numCycles--;
        return data;
    }

    // Opcodes
    static constexpr BYTE INSTRUCTION_LDA_IM = 0xA9;

    void execute(u32 numCycles, MEMORY &memory)
    {
        while (numCycles > 0)
        {
            BYTE Instruction = fetchByte(numCycles, memory);
            switch (Instruction)
            {
            case INSTRUCTION_LDA_IM:
            {
                BYTE Value = fetchByte(numCycles, memory);
                // These down below are some rules while setting the accumulator
                A = Value;
                Z = (A == 0);
                N = (A & 0b10000000) > 0;

            }
            break;

            default: 
            {
                printf("Instruction not handled %d", Instruction);
            }
                break;
            }
        }
    }
};

int main()
{
    MEMORY memory;
    CPU cpu;
    cpu.reset(memory);
    // Hardcoding the instruction opcode and value in the PC location from which the processor starts reading.
    memory[0xFFFC] = CPU::INSTRUCTION_LDA_IM;
    memory[0xFFFD] = 0x42;
    printf("Accumulator value before executing the LDA instruction is %x\n", cpu.A);
    cpu.execute(2, memory);
    printf("Accumulator value after executing the LDA instruction is %x\n", cpu.A);
}