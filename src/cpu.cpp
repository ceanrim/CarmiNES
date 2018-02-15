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
                    } break;
                    case 0b00100000: //AND
                    {
                    } break;
                    case 0b01000000: //EOR
                    {
                    } break;
                    case 0b01100000: //ADC
                    {
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
                    } break;
                    case 0b11000000: //CMP
                    {
                    } break;
                    case 0b11100000: //SBC
                    {
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
