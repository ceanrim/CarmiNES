/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Carmine Foggia $
   ======================================================================== */
#include "memory.h"
#include "main.h"
#include "cpu.h"
namespace CPU
{
    unsigned CurrentCycle;
    unsigned char A, X, Y, S, P;
    unsigned short PC, PCTemp;
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
                    } break;
                    case 0x4C: //JMP XXXX
                    {
                        if(CPU::InstructionCycle == 1)
                        {
                            Memory::AddressBus = Memory::Read(PCTemp);
                            CPU::PCTemp++;
                            CPU::InstructionCycle++;
                        }
                        else if(CPU::InstructionCycle == 2)
                        {
                            Memory::AddressBus |= (Memory::Read(PCTemp) << 8);
                            CPU::PCTemp = Memory::AddressBus;
                            CPU::InstructionCycle = 0;
                        }
                        break;
                    }
                    case 0x6C: //JMP (XXXX)
                    {
                        if(CPU::InstructionCycle == 1)
                        {
                            Memory::AddressBus = Memory::Read(PCTemp);
                            PCTemp++;
                            InstructionCycle++;
                        }
                        else if(CPU::InstructionCycle == 2)
                        {
                            Memory::AddressBus |= (Memory::Read(PCTemp) << 8);
                            InstructionCycle++;
                        }
                        else if(CPU::InstructionCycle == 3)
                        {
                            Memory::temp = Memory::AddressBus;
                            Memory::AddressBus &= Memory::Read(Memory::temp);
                            InstructionCycle++;
                        }
                        else if(CPU::InstructionCycle == 4)
                        {
                            Memory::AddressBus |=
                                (Memory::Read(Memory::temp+1) << 8);
                            PCTemp = Memory::AddressBus;
                            InstructionCycle = 0;
                        }
                        break;
                    }
                    case 0x24: //BIT xx
                    {
                        unsigned char temp;
                        Memory::Read((unsigned short)0, &CPU::InstructionCycle,
                                     ADDR_ZERO_PAGE, &temp);
                        if(InstructionCycle == 0)
                        {
                            unsigned char a = CPU::A & temp;
                            if(a & 0b10000000)
                            {
                                CPU::P |= 0b10000000;
                            }
                            else
                            {
                                CPU::P &= 0b01111111;
                            }
                            if(a & 0b01000000)
                            {
                                CPU::P |= 0b01000000;
                            }
                            else
                            {
                                CPU::P &= 0b10111111;
                            }
                            if(!a)
                            {
                                CPU::P |= 0b00000010;
                            }
                            else
                            {
                                CPU::P &= 0b11111101;
                            }
                        }
                        break;
                    }
                    case 0x2c: //BIT xxxx
                    {
                        unsigned char temp;
                        Memory::Read((unsigned short)0, &CPU::InstructionCycle,
                                     ADDR_ABSOLUTE, &temp);
                        if(InstructionCycle == 0)
                        {
                            unsigned char a = CPU::A & temp;
                            if(a & 0b10000000)
                            {
                                CPU::P |= 0b10000000;
                            }
                            else
                            {
                                CPU::P &= 0b01111111;
                            }
                            if(a & 0b01000000)
                            {
                                CPU::P |= 0b01000000;
                            }
                            else
                            {
                                CPU::P &= 0b10111111;
                            }
                            if(!a)
                            {
                                CPU::P |= 0b00000010;
                            }
                            else
                            {
                                CPU::P &= 0b11111101;
                            }
                        }
                        break;
                    }
                    case 0x84: //STY xx
                    case 0x8C: //STY xxxx
                    case 0x94: //STY xx+X
                    {
                        unsigned char addrMode = Memory::ConversionTable
                            [(((unsigned char)(FuncCurrentInstruction &
                                               (unsigned char) 0b00011100)) >> 2)];
                        Memory::Write(CPU::Y, &CPU::InstructionCycle, addrMode);
                        break;
                    }
                    case 0xC0: //CPY #xx
                    case 0xC4: //CPY xx
                    case 0xCC: //CPY xxxx
                    {
                        CMP(&CPU::Y, Memory::ConversionTable
                            [(((unsigned char)(FuncCurrentInstruction &
                                               (unsigned char) 0b00011100)) >> 2)]);
                        break;
                    }
                    case 0xE0: //CPX #xx
                    case 0xE4: //CPX xx
                    case 0xEC: //CPX xxxx
                    {
                        CMP(&CPU::X, Memory::ConversionTable
                            [(((unsigned char)(FuncCurrentInstruction &
                                               (unsigned char)0b00011100)) >> 2)]);
                        break;
                    }
                    case 0xA8: //TAY
                    {
                        Y = A;
                        InstructionCycle = 0;
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
                        break;
                    }
                    case 0x98: //TYA
                    {
                        A = Y;
                        InstructionCycle = 0;
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
                        break;
                    }
                    case 0x90: //BCC
                    {
                        Branch(1, true);
                        break;
                    }
                    case 0xB0: //BCS
                    {
                        Branch(1, false);
                        break;
                    }
                    case 0xD0: //BNE
                    {
                        Branch(2, true);
                        break;
                    }
                    case 0xF0: //BEQ
                    {
                        Branch(2, false);
                        break;
                    }
                    case 0x10: //BPL
                    {
                        Branch(128, true);
                        break;
                    }
                    case 0x30: //BMI
                    {
                        Branch(128, false);
                        break;
                    }
                    case 0x50: //BVC
                    {
                        Branch(64, true);
                        break;
                    }
                    case 0x70: //BVS
                    {
                        Branch(64, false);
                        break;
                    }
                    default:
                    {
                        CPU::InstructionCycle = 0;
                        break;
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
                            Memory::Read(PCTemp);
                            PCTemp++;
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
                        CMP(&CPU::A, (unsigned char)(FuncCurrentInstruction &
                                                     (unsigned char)0b00011100));
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
                switch(FuncCurrentInstruction)
                {
                    case 0xA2: //LDX #xx
                    case 0xA6: //LDX xx
                    case 0xAE: //LDX xxxx
                    case 0xB6: //LDX xx+Y
                    case 0xBE: //LDX xxxx+Y
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
                    case 0x86: //STX xx
                    case 0x8E: //STX xxxx
                    case 0x96: //STX xx+Y
                    {
                        unsigned char addrMode = Memory::ConversionTable
                            [(((unsigned char)(FuncCurrentInstruction &
                                               (unsigned char) 0b00011100)) >> 2)];
                        if(addrMode == ADDR_ZERO_PAGE_X)
                        {
                            addrMode = ADDR_ZERO_PAGE_Y;
                        }
                        Memory::Write(CPU::X, &CPU::InstructionCycle,
                                      addrMode);
                        break;
                    }
                    case 0xAA: //TAX
                    {
                        X = A;
                        InstructionCycle = 0;
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
                        break;
                    }
                    case 0x8A: //TXA
                    {
                        A = X;
                        InstructionCycle = 0;
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
                        break;
                    }
                    case 0xBA: //TSX
                    {
                        X = S;
                        InstructionCycle = 0;
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
                        break;
                    }
                    case 0x9A: //TXS
                    {
                        S = X;
                        InstructionCycle = 0;
                        break;
                    }
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
    void CMP(unsigned char *Register, unsigned char addrMode)
    {
        unsigned char temp;
        Memory::Read((unsigned short)0, &CPU::InstructionCycle,
                     addrMode,
                     &temp);
        if(CPU::InstructionCycle == 0)
        {
            if(((*Register)-(temp)) & 128)
            {
                CPU::P |= 128;
            }
            else
            {
                CPU::P &= 127;
            }
            if(*Register == temp)
            {
                CPU::P |= 2;
            }
            else
            {
                CPU::P &= 0b11111101;
            }
            if(temp <= *Register)
            {
                CPU::P |= 1;
            }
            else
            {
                CPU::P &= 0b11111110;
            }
        }
    }
    void Branch(unsigned char flag, bool clear)
    {
        if(InstructionCycle == 1)
        {
            Memory::temp = Memory::Read(CPU::PCTemp);
            CPU::PCTemp++;
            if(clear)
            {
                if(!(P & flag))
                {
                    InstructionCycle++;
                }
                else
                {
                    InstructionCycle = 0;
                }
            }
            else
            {
                if(P & flag)
                {
                    InstructionCycle++;
                }
                else
                {
                    InstructionCycle = 0;
                }
            }
        }
        else if(InstructionCycle == 2)
        {
            Memory::Read(CPU::PCTemp);
            if((CPU::PCTemp >> 8) !=
               ((CPU::PCTemp + (char)Memory::temp) >> 8))
            {
                InstructionCycle++;
            }
            else
            {
                InstructionCycle = 0;
            }
            CPU::PCTemp += (char)Memory::temp;
        }
        else if(InstructionCycle == 3)
        {
            Memory::Read(CPU::PCTemp - 256);
            InstructionCycle = 0;
        }
    }
    void Reset()
    {
        PC = Memory::Read(0xFFFC);
        PC |= (((unsigned short)(Memory::Read(0xFFFD))) << 8);
        PCTemp = PC;
        CurrentCycle = 7;
        InstructionCycle = 0;
        P = 0x34;
        A = 0;
        X = 0;
        Y = 0;
        S = 0xfd;
    }
}
