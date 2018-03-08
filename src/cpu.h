#if !defined(CPU_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Carmine Foggia $
   ======================================================================== */
#define CPU_H
namespace CPU
{
    extern unsigned CurrentCycle; //The cycles elapsed since the start of the frame
    extern unsigned char A, X, Y, S, P; //The CPU's registers
    extern unsigned short PC, PCTemp; //The program counter
    extern unsigned char InstructionCycle;
                           //The cycle we are in in the current instruction;
                           //0 if we've just completed one
    extern unsigned char CurrentInstruction; //The instruction we're running
    extern unsigned short AddressBus;
    extern unsigned char temp;
    extern void RunCycle(unsigned char, unsigned char);
    extern void Reset();
    extern void CMP(unsigned char*, unsigned char);
    extern void Branch(unsigned char, bool);
}
#endif
