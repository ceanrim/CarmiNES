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
    unsigned char A, X, Y, S, P; //The CPU's registers
    unsigned short PC;
    unsigned char CurrentInstruction; //The instruction we're running
private:
    unsigned short AddressBus;
    unsigned char temp;
    unsigned char addrMode;
public:
    void Run();
    void Reset();
    void CMP(unsigned char*, unsigned char);
    void Branch(unsigned char, bool);
    void UpdateFlags(unsigned char);
    void ASL(unsigned char*);
    void LSR(unsigned char*);
    void ROL(unsigned char*);
    void ROR(unsigned char*);
    void INC(unsigned char*);
    void DEC(unsigned char*);
    void SLO(unsigned char*);
    void NOP(unsigned char);
    void Push(unsigned char);
    void Pull(unsigned char *);
};
#endif
