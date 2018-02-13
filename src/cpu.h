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
    extern unsigned short PC; //The program counter
    extern char InstructionCycle; //The cycle we are in in the current instruction;
                           //-1 if we've just completed one
    extern unsigned char CurrentInstruction; //The instruction we're running
    extern void RunCycle(unsigned char, char);
}
#endif
