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
    BYTE &operator[](u32 Address)
    {
        // Assert here Adress is < MAX_MEM
        return Data[Address];
    }

    // Write a word (2 Bytes)
    void writeWord(WORD value, u32 address, u32 &numCycls) {  
        // Cause my system is little endian
        Data[address] = value & 0xFF;
        Data[address + 1] = (value >> 8);
        numCycls -= 2;
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

    // This is for reading instructions it moves the program counter
    BYTE fetchByte(u32 &numCycles, MEMORY &memory)
    {
        BYTE data = memory[PC];
        PC++;
        numCycles--;
        return data;
    }

    // This is for reading data it doesn't move the program counter
    BYTE readByte(u32 &numCycles, u32 Address, MEMORY &memory)
    {
        BYTE data = memory[Address];
        numCycles--;
        return data;
    }

     WORD fetchWord(u32 &numCycles, MEMORY &memory) // Basically reading 16 bits or 2 bytes
    {
        WORD data = memory[PC]; // Read one byte
        PC++;
        data = data | (memory[PC] << 8); // Read next byte (the reason for shifting the second byte left is cause my system is little endian so data is stored from least to most significant so second byte is the most significant byte and first one is least significant byte);
        PC++;
        numCycles -= 2; // Cause it took two cycles this time to read a word.
        // If your system is big endian you will have to swap the bytes here. 
        // if(PLATFORM_BIG_ENDIAN) 
        //      swapBytesInWord(data);
        return data;
    }

    // Opcodes
    static constexpr BYTE
        INSTRUCTION_LDA_IM = 0xA9,
        INSTRUCTION_LDA_ZP = 0xA5,
        INSTRUCTION_LDA_ZPX = 0xB5,
        INSTRUCTION_JSR = 0x20;


    // This is what you have to do in all the LDA instructions
    void LDASetStatus()
    {
        Z = (A == 0);
        N = (A & 0b10000000) > 0;
    }

    void execute(u32 numCycles, MEMORY &memory)
    {
        while (numCycles > 0)
        {
            BYTE Instruction = fetchByte(numCycles, memory);
          //  printf("Instruction fetched is %x", Instruction);
            switch (Instruction)
            {
            case INSTRUCTION_LDA_IM:
            {
                BYTE Value = fetchByte(numCycles, memory);
                // These down below are some rules while setting the accumulator
                A = Value;
                LDASetStatus();
            }
            break;

            case INSTRUCTION_LDA_ZP:
            {
                BYTE ZeroPageAddress = fetchByte(numCycles, memory);
                // These down below are some rules while setting the accumulator
                A = readByte(numCycles, ZeroPageAddress, memory);
                LDASetStatus();
            }
            break;

            case INSTRUCTION_LDA_ZPX:
            {
                BYTE ZeroPageAddress = fetchByte(numCycles, memory);
                ZeroPageAddress += X; // This is what we have to do in zeroPageX instructions whatever zero page address you get you must add the X register contents to it and that's how you get the final operand address that you have to put in the accumulator.
                numCycles--; // Incrementing the ZeroPageAddress takes one cycle
                // These down below are some rules while setting the accumulator
                A = readByte(numCycles, ZeroPageAddress, memory);
                LDASetStatus();
            }
            break;
 
            case INSTRUCTION_JSR:
            {
                WORD SubRoutineAddress = fetchWord(numCycles, memory);
                memory.writeWord(PC - 1, SP, numCycles);
                SP++;
                PC = SubRoutineAddress;
                numCycles--;
            }
            break;

            default:
            {
                printf("Instruction not handled %d\n ", Instruction);
                numCycles--;
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
    memory[0xFFFC] = CPU::INSTRUCTION_JSR;
    memory[0xFFFD] = 0x0042;
    memory[0xFFFE] = 0x0042;
    memory[0x4242] = CPU::INSTRUCTION_LDA_IM;
    memory[0x4243] = 0x95;
    printf("Accumulator value before executing the LDA instruction is %x\n", cpu.A);
    cpu.execute(8, memory);
    printf("Accumulator value after executing the LDA instruction is %x\n", cpu.A);
}