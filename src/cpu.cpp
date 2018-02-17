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
            case 0:
            {
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
                    case 0b10000000: //STA
                    {
                    } break;
                    case 0b10100000: //LDA: Loads the accumulator with the chosen value
                    {                             //For now we only support immediate
                                                  //Cycle sequence for immediate addressing mode:
                                                  //Cycle 0: Fetch opcode, increment PC
                                                  //Cycle 1: Fetch value, increment PC
                                                  //Cycle 0 of next instruction: Load value into accumulator
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
                                CPU::P |= 1;
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
            case 2:
            {
            } break;
            case 3:
            {
            } break;
            default: //This is not supposed to happen
            {
                Log("Error. Instruction & 0b00000011 greater than 3");
                return;
            }
        }
        return; //TODO
    }
}
