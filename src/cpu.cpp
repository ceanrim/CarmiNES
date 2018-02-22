/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Carmine Foggia $
   ======================================================================== */
#include "memory.h"
#include "main.h"
namespace CPU
{
    unsigned CurrentCycle;
    unsigned char A, X, Y, S, P;
    unsigned short PC;
    unsigned char InstructionCycle;
    unsigned char CurrentInstruction;
    void RunCycle(unsigned char FuncCurrentInstruction, unsigned char FuncInstructionCycle)
    {
        switch(FuncCurrentInstruction & 0b00000011)
        {
            case 0: //BIT JMP STY LDY CPY CPX BPL BMI BVC BVS BCC BCS BNE BEQ BRK
                    //JSR RTI RTS PHP PLP PHA PLA DEY TAY INY INX CLC SEC CLI SEI
                    //TYA CLV CLD SED
            {
                switch(FuncCurrentInstruction)
                {
                    case 0x18: //CLC
                    {
                        P &= 0b11111110;
                        CPU::InstructionCycle = 0;
                    } break;
                    case 0x38: //SEC
                    {
                        P |= 1;
                        CPU::InstructionCycle = 0;
                    } break;
                    case 0x58: //CLI
                    {
                        P &= 0b11111011;
                        CPU::InstructionCycle = 0;
                    } break;
                    case 0x78: //SEI
                    {
                        P |= 4;
                        CPU::InstructionCycle = 0;
                    } break;
                    case 0xB8: //CLV
                    {
                        P &= 0b10111111;
                        CPU::InstructionCycle = 0;
                    } break;
                    case 0xD8: //CLD
                    {
                        P &= 0b11110111;
                        CPU::InstructionCycle = 0;
                    } break;
                    case 0xF8: //SED
                    {
                        P |= 8;
                        CPU::InstructionCycle = 0;
                    } break;
                    case 0xA0: case 0xA4: case 0xAC: case 0xB4: case 0xBC: //LDY
                    {
                        unsigned char addrMode = Memory::ConversionTable
                            [(((unsigned char)(FuncCurrentInstruction &
                                               (unsigned char)0b00011100)) >> 2)];
                        Memory::Read((unsigned short)0, &CPU::InstructionCycle,
                                     addrMode,
                                     &CPU::Y);
                        if(CPU::InstructionCycle == 0)
                        {
                            if(CPU::Y & 128)
                            {
                                CPU::P |= 128;
                            }
                            else
                            {
                                CPU::P &= 127;
                            }
                            if(CPU::Y == 0)
                            {
                                CPU::P |= 2;
                            }
                            else
                            {
                                CPU::P &= 0b11111101;
                            }
                        }
                    }
                }
            } break;
            case 1: //ORA AND EOR ADC STA LDA CMP SBC
            {
                switch(FuncCurrentInstruction & 0b11100000)
                {
                    case 0b00000000: //ORA
                    {
                        unsigned char temp;
                        Memory::Read((unsigned short)0, &CPU::InstructionCycle,
                                     (unsigned char)(FuncCurrentInstruction &
                                                     (unsigned char)0b00011100),
                                     &temp);
                        if(CPU::InstructionCycle == 0)
                        {
                            CPU::A |= temp;
                        }
                        if(CPU::InstructionCycle == 0)
                        {
                            if(CPU::A & 128)
                            {
                                CPU::P |= 128;
                            }
                            else
                            {
                                CPU::P &= 127;
                            }
                            if(CPU::A == 0)
                            {
                                CPU::P |= 2;
                            }
                            else
                            {
                                CPU::P &= 0b11111101;
                            }
                        }
                    } break;
                    case 0b00100000: //AND
                    {
                        unsigned char temp;
                        Memory::Read((unsigned short)0, &CPU::InstructionCycle,
                                     (unsigned char)(FuncCurrentInstruction &
                                                     (unsigned char)0b00011100),
                                     &temp);
                        if(CPU::InstructionCycle == 0)
                        {
                            CPU::A &= temp;
                            if(CPU::A & 128)
                            {
                                CPU::P |= 128;
                            }
                            else
                            {
                                CPU::P &= 127;
                            }
                            if(CPU::A == 0)
                            {
                                CPU::P |= 2;
                            }
                            else
                            {
                                CPU::P &= 0b11111101;
                            }
                        }
                    } break;
                    case 0b01000000: //EOR
                    {
                        unsigned char temp;
                        Memory::Read((unsigned short)0, &CPU::InstructionCycle,
                                     (unsigned char)(FuncCurrentInstruction &
                                                     (unsigned char)0b00011100),
                                     &temp);
                        if(CPU::InstructionCycle == 0)
                        {
                            CPU::A ^= temp;
                            if(CPU::A & 128)
                            {
                                CPU::P |= 128;
                            }
                            else
                            {
                                CPU::P &= 127;
                            }
                            if(CPU::A == 0)
                            {
                                CPU::P |= 2;
                            }
                            else
                            {
                                CPU::P &= 0b11111101;
                            }
                        }
                    } break;
                    case 0b01100000: //ADC
                    {
                        unsigned char temp;
                        Memory::Read((unsigned short)0, &CPU::InstructionCycle,
                                     (unsigned char)(FuncCurrentInstruction &
                                                     (unsigned char)0b00011100),
                                     &temp);
                        if(CPU::InstructionCycle == 0)
                        {
                            bool Carry = false;
                            bool Overflow = false;
                            if(((unsigned short)CPU::A
                                + (unsigned short)temp
                                + (unsigned short)(CPU::P & 1)) >= 256)
                            {
                                Carry = true;
                            }
                            if((CPU::A^(CPU::A+temp+(CPU::P & 1))) &
                               (temp^(CPU::A+temp+(CPU::P & 1))) &
                               0x80)
                            {
                                Overflow = true;
                            }
                            CPU::A += temp;
                            CPU::A += (CPU::P & 1);
                            if(Carry)
                            {
                                CPU::P |= 1;
                            }
                            else
                            {
                                CPU::P &= 0b11111110;
                            }
                            if(Overflow)
                            {
                                CPU::P |= 64;
                            }
                            else
                            {
                                CPU::P &= 0b10111111;
                            }
                            if(CPU::A == 0)
                            {
                                CPU::P |= 2;
                            }
                            else
                            {
                                CPU::P &= 0b11111101;
                            }
                            if(CPU::A & 128)
                            {
                                CPU::P |= 128;
                            }
                            else
                            {
                                CPU::P &= 127;
                            }
                        }
                    } break;
                    case 0b10000000: //STA, 2 cycles NOP if called with immediate
                                     //addressing mode
                    {
                        if(FuncCurrentInstruction == 0x89)
                        {
                            Memory::Read(PC);
                            PC++;
                            CPU::InstructionCycle = 0;
                        }
                        else
                        {
                            Memory::Write(CPU::A,
                                          &CPU::InstructionCycle,
                                          (unsigned char)
                                          (FuncCurrentInstruction &
                                           (unsigned char)0b00011100));
                        }
                    } break;
                    case 0b10100000: //LDA
                    {                     
                        Memory::Read((unsigned short)0, &CPU::InstructionCycle,
                                     (unsigned char)(FuncCurrentInstruction &
                                                     (unsigned char)0b00011100),
                                     &CPU::A);
                        if(CPU::InstructionCycle == 0)
                        {
                            if(CPU::A & 128)
                            {
                                CPU::P |= 128;
                            }
                            else
                            {
                                CPU::P &= 127;
                            }
                            if(CPU::A == 0)
                            {
                                CPU::P |= 2;
                            }
                            else
                            {
                                CPU::P &= 0b11111101;
                            }
                        }
                    } break;
                    case 0b11000000: //CMP
                    {
                        unsigned char temp;
                        Memory::Read((unsigned short)0, &CPU::InstructionCycle,
                                     (unsigned char)(FuncCurrentInstruction &
                                                     (unsigned char)0b00011100),
                                     &temp);
                        if(CPU::InstructionCycle == 0)
                        {
                            if(((CPU::A)-(temp)) & 128)
                            {
                                CPU::P |= 128;
                            }
                            else
                            {
                                CPU::P &= 127;
                            }
                            if(CPU::A == temp)
                            {
                                CPU::P |= 2;
                            }
                            else
                            {
                                CPU::P &= 0b11111101;
                            }
                            if(temp <= CPU::A)
                            {
                                CPU::P |= 1;
                            }
                            else
                            {
                                CPU::P &= 0b11111110;
                            }
                        }
                    } break;
                    case 0b11100000: //SBC
                    {
                        
                        unsigned char temp;
                        Memory::Read((unsigned short)0, &CPU::InstructionCycle,
                                     (unsigned char)(FuncCurrentInstruction &
                                                     (unsigned char)0b00011100),
                                     &temp);
                        if(CPU::InstructionCycle == 0)
                        {
                            bool Carry = false;
                            bool Overflow = false;
                            if((unsigned short)((unsigned short)CPU::A
                                                + (unsigned short)(~temp & 255)
                                                + (unsigned short)(CPU::P & 1))
                               >= 256)
                            {
                                Carry = true;
                            }
                            if((CPU::A^(CPU::A+~temp+(CPU::P & 1))) &
                               (~temp^(CPU::A+~temp+(CPU::P & 1))) &
                               0x80)
                            {
                                Overflow = true;
                            }
                            CPU::A += ~temp;
                            CPU::A += (CPU::P & 1);
                            if(Carry)
                            {
                                CPU::P |= 1;
                            }
                            else
                            {
                                CPU::P &= 0b11111110;
                            }
                            if(Overflow)
                            {
                                CPU::P |= 64;
                            }
                            else
                            {
                                CPU::P &= 0b10111111;
                            }
                            if(CPU::A == 0)
                            {
                                CPU::P |= 2;
                            }
                            else
                            {
                                CPU::P &= 0b11111101;
                            }
                            if(CPU::A & 128)
                            {
                                CPU::P |= 128;
                            }
                            else
                            {
                                CPU::P &= 127;
                            }
                        }
                    } break;
                }
            } break;
            case 2: //ASL ROL LSR ROR STX LDX DEC INC
            {
                switch(FuncCurrentInstruction & 0b11100000)
                {
                    case 0b10100000: //LDX
                    {
                        unsigned char addrMode = Memory::ConversionTable
                            [(((unsigned char)(FuncCurrentInstruction &
                                               (unsigned char)0b00011100)) >> 2)];
                        if(addrMode == ADDR_ZERO_PAGE_X)
                        {
                            addrMode = ADDR_ZERO_PAGE_Y;
                        }
                        if(addrMode == ADDR_ABSOLUTE_X)
                        {
                            addrMode = ADDR_ABSOLUTE_Y;
                        }
                        Memory::Read((unsigned short)0, &CPU::InstructionCycle,
                                     addrMode,
                                     &CPU::X);
                        if(CPU::InstructionCycle == 0)
                        {
                            if(CPU::X & 128)
                            {
                                CPU::P |= 128;
                            }
                            else
                            {
                                CPU::P &= 127;
                            }
                            if(CPU::X == 0)
                            {
                                CPU::P |= 2;
                            }
                            else
                            {
                                CPU::P &= 0b11111101;
                            }
                        }
                    } break;
                    default:
                    {
                        return;
                    }
                }
            } break;
            case 3:
            {
            } break;
            default: //This is not supposed to happen
            {
                return;
            }
        }
        return; //TODO
    }
    void Reset()
    {
        PC = Memory::Read(0xFFFC);
        PC |= (((unsigned short)(Memory::Read(0xFFFD))) << 8);
        CurrentCycle = 7;
        InstructionCycle = 0;
        P = 34;
        A = 0;
        X = 0;
        Y = 0;
        S = 0xfd;
    }
}
