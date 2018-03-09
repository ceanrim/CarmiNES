#if !defined(CPU_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Carmine Foggia $
   ======================================================================== */
#define CPU_H
class CPUClass
{
public:
    unsigned CurrentCycle; //The cycles elapsed since the start of the frame
    unsigned char A, X, Y, S, P; //The CPU's registers
    unsigned short PC, PCTemp; //The program counter
    unsigned char InstructionCycle;
                           //The cycle we are in in the current instruction;
                           //0 if we've just completed one
    unsigned char CurrentInstruction; //The instruction we're running
private:
    unsigned short AddressBus;
    unsigned char temp;
public:
    void RunCycle(unsigned char, unsigned char);
    void Reset();
    void CMP(unsigned char*, unsigned char);
    void Branch(unsigned char, bool);
};
#endif
